/**
 * @file tp.c
 *
 * @brief High level FPGA control implementation for the TinyProbe
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup tinyprobe
 *
 */

#include "tp.h"

#include "tinyprobe/command.h"
#include "tinyprobe/mux.h"
#include "tinyprobe/fpga.h"
#include "tinyprobe/afe.h"
#include "tinyprobe/tx.h"
#include "tinyprobe/power.h"
#include "tinyprobe/buffer.h"
#include "wius/power.h"
#include "wius/wifi.h"
#include "wius/spi.h"
#include "wius/udp.h"
#include "wius/gpio_ulp.h"
#include "wius/gpio_uulp.h"

#define TEST_MODE 0 // For testing acquisition code without confirmation from TinyProbe

wius_gpio_uulp_t int_pin = WIUS_GPIO_UULP_INPUT(TP_GPIO_INT);
wius_gpio_ulp_t reset_pin = WIUS_GPIO_ULP_OUTPUT(TP_GPIO_RESET);

uint8_t wifi_rx_buffer[TP_WIFI_RX_BUFFER_SIZE] = {0};

extern osEventFlagsId_t event_flags;
osThreadId_t wifi_receive_thread_id;
osThreadAttr_t wifi_rx_thread_attr = {
    .name = "TP wifi receive",
    .stack_size = 1024,
    .priority = osPriorityLow,
};

// UDP socket over which communication happens
wius_udp_t tp_socket = {0};
char client_ip[16] = {0};
int client_port = 0;

// Buffer for storing acquired data
tp_buffer_t tp_buf;

// Variables for the command functions
bool enable_udp_replies = false;
uint16_t irq_shot_count = 0;
uint16_t n_packs_to_read = 0;

void _tp_thread_wifi_receive(void *argument);
void _tp_thread_transmit(void *argument);
void _tp_int_handler(void);
sl_status_t _tp_transmit_packages(void);

sl_status_t tp_init(void)
{
  sl_status_t status = SL_STATUS_OK;

  LOG_D("Initializing TinyProbe");

  // Initialize SPI
  status = wius_spi_init(WIUS_SPI_INST_0);
  if (SL_STATUS_OK != status)
  {
    LOG_E("Error initializing FPGA SPI: 0x%lx", status);
    return status;
  }

  delay_ms(1000);

  // Initialize GPIO
  wius_gpio_ulp_init();
  wius_gpio_uulp_init();
  wius_gpio_ulp_pin_config(&reset_pin);
  wius_gpio_uulp_pin_config(&int_pin);

  // Attach FPGA interrupt
  status = wius_gpio_uulp_pin_attach_interrupt(int_pin, WIUS_GPIO_ULP_INT_FALLING, _tp_int_handler);
  if (SL_STATUS_OK != status)
  {
    LOG_E("Error initializing INT pin callback: 0x%lx", status);
    return status;
  }

  // Initialize low speed configuration interfaces
  tp_mux_init();
  tp_power_init();

  // Reset the FPGA
  wius_gpio_ulp_pin_set(reset_pin, false);
  delay_ms(10);
  wius_gpio_ulp_pin_set(reset_pin, true);

  // Wait some time for PLL to settle
  delay_ms(10);

  LOG_D("Reset FPGA");

  // Select internal SPI slave module of the FPGA
  tp_mux_select(TP_MUX_FPGA);
  delay_ms(10);

#if !TEST_MODE
  // Initialize the FPGA
  status = tp_fpga_init();
  if (SL_STATUS_OK != status)
  {
    LOG_E("Error configuring FPGA: 0x%lx", status);
    return status;
  }

  LOG_D("Configured FPGA");
#endif

  // Enable power domains for TX chip
  tp_power_on();

  LOG_D("Enabled power domains");

#if !TEST_MODE
  // Reset AFE and TX chip by writing a value to the dedicated register
  CHECK_STATUS(tp_fpga_write_reg_safe(0x00000057, 10));
  // 10 ms delay
  delay_ms(10);
  // Remove reset signals and set TR_EN to 0
  CHECK_STATUS(tp_fpga_write_reg_safe(0x00000054, 10));

  // Turn on active control of the AFE clk (power downs in between shots)
  CHECK_STATUS(tp_fpga_write_reg_safe(0x4000ffff, 2));

  LOG_D("Reset AFE and TX");
#endif

  // Select TX
  tp_mux_select(TP_MUX_TX);
  delay_ms(10);

#if !TEST_MODE
  // TX chip setup //
  // Config TX chip
  CHECK_STATUS(tp_tx_init());

  // Turn on also active control of the TX BF clk
  // (power downs in between TR_EN wake ups)
//  CHECK_STATUS(tp_fpga_write_reg_safe(0x0000ffff, 2));
  tp_fpga_write_reg_safe(0x0000ffff, 2);

  LOG_D("Configured TX");
#endif

  // Switch the MUX to the AFE
  tp_mux_select(TP_MUX_AFE);
  delay_ms(10);

#if !TEST_MODE
  // AFE setup //
  // Config AFE
  CHECK_STATUS(tp_afe_init());
  CHECK_STATUS(tp_afe_test_pattern(HALF_ZEROS_HALF_ONES));

  // Set AFE Gain
  CHECK_STATUS(tp_afe_write_reg_dtgc_safe(0xB5, 0));

  LOG_D("Configured AFE");
#endif

  // Select the internal SPI slave module of the FPGA
  tp_mux_select(TP_MUX_FPGA);
  delay_ms(10);

#if !TEST_MODE
  //        // Enable AFE Fast Power down in between the shots
  //        // (controlled by waveform_gen)
  //        tp_fpga_write_reg_safe(0x00000050, 10);
  //        // Enable AFE Global Power down in between the shots
  //        // (controlled by waveform_gen)
  //        tp_fpga_write_reg_safe(0x00000044, 10);

  //            // Enable AFE Fast Power down and TR_EN signal duty cycling
  //            // AFE Global power down is manually disabled
  //            tp_fpga_write_reg_safe(0x00000010, 10);

  // Enable automatic AFE Fast Power down and TR_EN signal duty cycling
  // Enable AFE Global Power Down through pin
  tp_fpga_write_reg_safe(0x00000030, 10);
#endif

  // main_thread_id = osThreadNew(_tp_thread_main, NULL, &thread_attr);
  // if (main_thread_id == NULL)
  // {
  //   LOG_E("Error creating main thread");
  //   return SL_STATUS_FAIL;
  // }
  // LOG_D("Main thread started");

  LOG_D("Connecting to WiFi");
  // Connect to WiFi
  status = wius_wifi_init();
  while (SL_STATUS_OK != status)
  {
    LOG_W("Wi-Fi initialization failed with status 0x%lx, retrying in 1s...", status);
    wius_wifi_deinit();
    delay_ms(1000);
    status = wius_wifi_init();
  }
  LOG_D("Connected to WiFi");

  wifi_receive_thread_id = osThreadNew(_tp_thread_wifi_receive, NULL, &wifi_rx_thread_attr);
  if (wifi_receive_thread_id == NULL)
  {
    LOG_E("Error creating WiFi receive thread");
    return SL_STATUS_FAIL;
  }
  LOG_D("Wifi thread started");

  CHECK_STATUS(wius_wifi_set_performance_profile(WIUS_PERF_PROFILE_LOWPOWER));
  CHECK_STATUS(wius_power_m4_low());
  LOG_D("Low power mode activated");

  LOG_D("TinyProbe initialized");

  return status;
}

void tp_main_thread(void)
{
  sl_status_t status = SL_STATUS_OK;

  LOG_D("Started TinyProbe main thread");

  common_init();

  while (true)
  {
    // Wait for a command to be received
    if (!(osEventFlagsWait(event_flags, FLAG_CMD_RECEIVED, 0, osWaitForever) & FLAG_CMD_RECEIVED))
    {
      LOG_E("Error waiting for command received flags");
      continue;
    }

    led_red_set(true);

    // Execute the command
    status = tp_command_parse_and_execute(wifi_rx_buffer, TP_WIFI_RX_BUFFER_SIZE);
    if (SL_STATUS_OK != status)
    {
      LOG_E("Error executing command: 0x%lx", status);
    }

    // Set the command executed flag
    osEventFlagsSet(event_flags, FLAG_CMD_EXECUTED);

    led_red_set(false);
  }
}

void _tp_thread_wifi_receive(void *argument)
{
  // listens for incoming UDP packets
  (void)argument;
  sl_status_t status = SL_STATUS_OK;

  LOG_D("Started TinyProbe WiFi receive thread");

  wius_udp_init(&tp_socket);

  status = wius_udp_bind(&tp_socket, 0, TP_UDP_PORT);
  if (SL_STATUS_OK != status)
  {
    LOG_E("Error connecting UDP socket: 0x%lx", status);
    return;
  }

  ssize_t received_len = 0;

  while (true)
  {
    memset(wifi_rx_buffer, 0, TP_WIFI_RX_BUFFER_SIZE);

    status = wius_udp_receivefrom(&tp_socket, wifi_rx_buffer, TP_WIFI_RX_BUFFER_SIZE, &received_len,
                                  client_ip, sizeof(client_ip), &client_port, 0);
    if (SL_STATUS_OK != status)
    {
      LOG_E("Error receiving UDP packet: 0x%lx", status);
      continue;
    }

    LOG_D("Received UDP packet from %s:%d", client_ip, client_port);
    // LOG_D("Packet: '%s'", wifi_rx_buffer);

    // Parse the command
    if (tp_command_parse(wifi_rx_buffer, TP_WIFI_RX_BUFFER_SIZE) == NULL)
    {
      LOG_E("Invalid command, skipping");
      continue;
    }

    LOG_D("Valid command");

    // Set the command received flag
    osEventFlagsSet(event_flags, FLAG_CMD_RECEIVED);

    // Wait for the command to be executed
    if (!(osEventFlagsWait(event_flags, FLAG_CMD_EXECUTED, 0, osWaitForever) & FLAG_CMD_EXECUTED))
    {
      LOG_E("Error waiting for command executed flag");
      continue;
    }
  }
}

sl_status_t tp_ping(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args;
  (void)args_length;

  sl_status_t status = SL_STATUS_OK;

  // TODO: See if this is OK
  char reply[32] = {0};
  snprintf(reply, sizeof(reply), "TinyProbe %d", TP_PROBE_ID);
  CHECK_STATUS(wius_udp_sendto(&tp_socket, (const uint8_t *)reply, strlen(reply), client_ip, client_port));

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_en_replies(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;

  enable_udp_replies = *(bool *)args;

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_sw_mux(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;

  tp_mux_t mux = *(tp_mux_t *)args;
  tp_mux_select(mux);

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_write_spi(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args;
  (void)args_length;

  // TODO: Implement
  LOG_W("Not yet implemented");

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_write_fpga(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;
  sl_status_t status = SL_STATUS_OK;

  uint8_t fpga_reg_addr = *args;
  uint32_t fpga_reg_value = *(uint32_t *)(args + 1);

  CHECK_STATUS(tp_fpga_write_reg_safe(fpga_reg_value, fpga_reg_addr));

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_write_afe(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;
  sl_status_t status = SL_STATUS_OK;

  uint8_t dtgc_reg_flag = *args;
  uint8_t afe_reg_addr = *(args + 1);
  uint16_t afe_reg_value = *(uint16_t *)(args + 2);

  if (dtgc_reg_flag)
  {
    CHECK_STATUS(tp_afe_write_reg_dtgc(afe_reg_addr, afe_reg_value));
  }
  else
  {
    CHECK_STATUS(tp_afe_write_reg(afe_reg_addr, afe_reg_value));
  }

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_write_tx(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;
  sl_status_t status = SL_STATUS_OK;

  uint16_t tx_reg_addr = *(uint16_t *)args;
  uint32_t tx_reg_value = *(uint32_t *)(args + 2);

  CHECK_STATUS(tp_tx_write_reg(tx_reg_addr, tx_reg_value));

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_delay_ns(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;

  uint32_t delay = *(uint32_t *)args;
  delay_ns(delay);

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_sleep_ms(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;

  uint32_t delay = *(uint32_t *)args;
  delay_ms(delay);

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t tp_ctrl_pwr(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;

  tp_power_domain_t domain = *(tp_power_domain_t *)args;
  bool enabled = *(bool *)(args + 1);
  tp_power_set(domain, enabled);

  LOG_D("Done");

  return SL_STATUS_OK;
}

volatile bool fpga_ready = false;

sl_status_t tp_trigger_shot(uint8_t *args, uint16_t args_length)
{
  LOG_D("Executing");

  (void)args_length;
  sl_status_t status = SL_STATUS_OK;

  uint32_t n_shots = *(uint16_t *)args;
  n_packs_to_read = *(uint16_t *)(args + 2);

  uint8_t usdivten = *(uint8_t *)(args + 4);
  uint8_t usfifo = *(uint8_t *)(args + 5);

  LOG_D("Triggering %lu shots with %u packets to read", n_shots, n_packs_to_read);

  CHECK_STATUS(wius_power_m4_high());
  CHECK_STATUS(wius_wifi_set_performance_profile(WIUS_PERF_PROFILE_HIGHSPEED));
  LOG_D("High speed mode activated");

  uint32_t start_time = 0;
  uint32_t end_time = 0;

  CHECK_STATUS(tp_fpga_reset_multififo());
  CHECK_STATUS(tp_fpga_empty_tx());

  irq_shot_count = 0;

  start_time = time_ms();

  CHECK_STATUS(tp_fpga_send_start());

  for (uint32_t i = 0; i < n_shots; i++)
  {
// TODO: Is this handled correctly?
#if !TEST_MODE
//    osEventFlagsWait(event_flags, FLAG_FIFO_DATA_READY, 0, osWaitForever);
      while (!fpga_ready)
        {

        }
      fpga_ready = false;

//      delay_ns(200000);
//      delay_ns(usdivten * 10 * 1000);

      // TODO: DC-DC

//      delay_ns(usfifo * 1000);
      delay_ms(1);
#else
    delay_ms(1);
    irq_shot_count++;
#endif

    CHECK_STATUS(tp_fpga_en_read());
    // Wait 24 clock cycles of 10 MHz clock
    // It is worst case maximum time needed for the internal IP
    // To read the data from the Core FIFO and push it into the SPI TX buffer.
    delay_ns(2400);

    // TODO: Check implementation speed
    _tp_transmit_packages();

    CHECK_STATUS(tp_fpga_reset_multififo());
  }

  end_time = time_ms();

  LOG_D("Shot time:  %lu ms", end_time - start_time);
  LOG_D("Shot count: %u", irq_shot_count);

  CHECK_STATUS(wius_wifi_set_performance_profile(WIUS_PERF_PROFILE_LOWPOWER));
  CHECK_STATUS(wius_power_m4_low());
  LOG_D("Low power mode activated");

  LOG_D("Done");

  return SL_STATUS_OK;
}

sl_status_t _tp_transmit_packages(void)
{
  sl_status_t status = SL_STATUS_OK;

  tp_buffer_init(&tp_buf);

  tp_buffer_slot_t *slot_spi = tp_buffer_claim_writing(&tp_buf);
  if (NULL == slot_spi)
  {
    LOG_W("Timeout claiming initial buffer for write");
    return SL_STATUS_FAIL;
  }

  uint8_t tx_buf[TP_UDP_PACKET_SIZE + 2] = {0};
  tx_buf[0] = SP_RD_FIFO;
  tx_buf[1] = SPI_DUMMY_ADDR;

  //  CHECK_STATUS(tp_fpga_read_fifo(tx_dummy, slot_spi->data, TP_UDP_PACKET_SIZE + 2, false));
  status = wius_spi_xfer(WIUS_SPI_INST_0, tx_buf, slot_spi->data, TP_UDP_PACKET_SIZE + 2, false);
//  status = wius_spi_recv(WIUS_SPI_INST_0, slot_spi->data, TP_UDP_PACKET_SIZE + 2, false);
  if (SL_STATUS_OK != status)
  {
    LOG_E("Error starting initial SPI recv: 0x%04X", status);
    return status;
  }

  for (uint16_t i = 0; i < n_packs_to_read; i++)
  {
    CHECK_STATUS(wius_spi_await(WIUS_SPI_INST_0));
    // delay_ns(120000);

    slot_spi->length = TP_UDP_PACKET_SIZE + 2;

    tp_buffer_return(&tp_buf, slot_spi, false);

    if (i != n_packs_to_read - 1)
    {
      slot_spi = tp_buffer_claim_writing(&tp_buf);
      if (NULL == slot_spi)
      {
        LOG_E("Error claiming buffer for write");
        return SL_STATUS_FAIL;
      }

      // CHECK_STATUS(tp_fpga_read_fifo(tx_dummy, slot_spi->data, TP_UDP_PACKET_SIZE + 2, false));
//      status = wius_spi0_xfer_cont(tx_buf, slot_spi->data, TP_UDP_PACKET_SIZE + 2);
      status = wius_spi_xfer(WIUS_SPI_INST_0, tx_buf, slot_spi->data, TP_UDP_PACKET_SIZE + 2, false);
//      status = wius_spi_recv(WIUS_SPI_INST_0, slot_spi->data, TP_UDP_PACKET_SIZE + 2, false);
      if (SL_STATUS_OK != status)
      {
        LOG_E("Error starting SPI recv: 0x%04X", status);
        return status;
      }
    }

    tp_buffer_slot_t *slot_udp = tp_buffer_claim_reading(&tp_buf);
    if (NULL == slot_udp)
    {
      LOG_E("Error claiming buffer for read");
      return SL_STATUS_FAIL;
    }

    // Prepend the packet with the shot number
    memcpy(slot_udp->data, &i, 2);

    status = wius_udp_sendto(&tp_socket, slot_udp->data, TP_UDP_PACKET_SIZE + 2, client_ip, client_port);
    if (SL_STATUS_OK != status)
      LOG_W("Error transmitting packet");

    tp_buffer_return(&tp_buf, slot_udp, true);
  }

  LOG_D("Shot acquired");

  return status;
}

//sl_status_t _tp_transmit_packages(void)
//{
//  sl_status_t status = SL_STATUS_OK;
//
//  uint8_t tx_buf[TP_UDP_PACKET_SIZE + 2] = {0};
//  tx_buf[0] = SP_RD_FIFO;
//  tx_buf[1] = SPI_DUMMY_ADDR;
//
//  for (uint16_t i = 0; i < n_packs_to_read; i++)
//  {
//    status = wius_udp_sendto(&tp_socket, tx_buf, TP_UDP_PACKET_SIZE + 2, client_ip, client_port);
//    if (SL_STATUS_OK != status)
//      LOG_W("Error transmitting packet");
//    delay_ns(149000);
////    delay_ms(2);
//  }
//
//  return status;
//}

void _tp_int_handler(void)
{
//  osEventFlagsSet(event_flags, FLAG_FIFO_DATA_READY);
  irq_shot_count++;
  fpga_ready = true;
  // LOG_D("FPGA INT handler");
}
