/**
 * @file spi.c
 *
 * @brief SPI module implementation for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 05.04.2024
 *
 * @ingroup wius
 *
 */

#include "spi.h"

#include "cmsis_os2.h"
#include "sl_si91x_gspi.h"
#include "sl_si91x_ssi.h"
#include "sl_gpio_board.h"

#include "sl_si91x_dma.h"
#include "rsi_gspi.h"
#include "clock_update.h"

#include "wius/gpio.h"

#if WIUS_SPI_EXT_CS0
wius_gpio_t cs0_pin = WIUS_GPIO_OUTPUT(WIUS_SPI_EXT_CS0);
#define WIUS_SPI_CS0_HIGH wius_gpio_pin_set(cs0_pin, true)
#define WIUS_SPI_CS0_LOW wius_gpio_pin_set(cs0_pin, false)
#else
#define WIUS_SPI_CS0_HIGH
#define WIUS_SPI_CS0_LOW
#endif

#if WIUS_SPI_EXT_CS1
wius_gpio_t cs1_pin = WIUS_GPIO_OUTPUT(WIUS_SPI_GPIO_EXT_CS1);
#define WIUS_SPI_CS1_HIGH wius_gpio_pin_set(cs1_pin, true)
#define WIUS_SPI_CS1_LOW wius_gpio_pin_set(cs1_pin, false)
#else
#define WIUS_SPI_CS1_HIGH
#define WIUS_SPI_CS1_LOW
#endif

// Timings
#define WIUS_SPI_SOC_PLL_MM_COUNT_LIMIT 0xA4    // Soc pll count limit
#define WIUS_SPI_INTF_PLL_500_CTRL_VALUE 0xD900 // Intf pll control value
#define WIUS_SPI_INTF_PLL_CLK 180000000         // Intf pll clock frequency
#define WIUS_SPI_SOC_PLL_REF_CLK 40000000       // Soc pll reference clock frequency
#define WIUS_SPI_SOC_PLL_CLK 20000000           // Soc pll clock frequency
#define WIUS_SPI_INTF_PLL_REF_CLK 40000000      // Intf pll reference clock frequency
#define WIUS_SPI_DIVISION_FACTOR 0              // Division factor

static sl_gspi_handle_t gspi_driver_handle = NULL;
static sl_ssi_handle_t ssi_driver_handle = NULL;
// extern osEventFlagsId_t event_flags;
osSemaphoreId_t spi0_sem;
osSemaphoreId_t spi1_sem;
static volatile bool spi0_transfer_complete = false;

static void gspi_callback_event(uint32_t event);
static void ssi_callback_event(uint32_t event);

sl_status_t wius_spi_init(wius_spi_inst_t instance)
{
  sl_status_t status;
  //	sl_gspi_status_t gspi_status;

  switch (instance)
  {
  case WIUS_SPI_INST_0:
    sl_gspi_clock_config_t gspi_clock_configuration;
    gspi_clock_configuration.soc_pll_mm_count_value = WIUS_SPI_SOC_PLL_MM_COUNT_LIMIT;
    gspi_clock_configuration.intf_pll_500_control_value = WIUS_SPI_INTF_PLL_500_CTRL_VALUE;
    gspi_clock_configuration.intf_pll_clock = WIUS_SPI_INTF_PLL_CLK;
    gspi_clock_configuration.intf_pll_reference_clock = WIUS_SPI_INTF_PLL_REF_CLK;
    gspi_clock_configuration.soc_pll_clock = WIUS_SPI_SOC_PLL_CLK;
    gspi_clock_configuration.soc_pll_reference_clock = WIUS_SPI_SOC_PLL_REF_CLK;
    gspi_clock_configuration.division_factor = WIUS_SPI_DIVISION_FACTOR;

    status = sl_si91x_gspi_configure_clock(&gspi_clock_configuration);
    if (status != SL_STATUS_OK)
    {
      LOG_E("Error configuring SPI clock: 0x%lx", status);
      return status;
    }

    status = sl_si91x_gspi_init(SL_GSPI_MASTER, &gspi_driver_handle);
    if (status != SL_STATUS_OK)
    {
      LOG_E("Error initializing SPI: 0x%lx", status);
      return status;
    }

    //  gspi_status = sl_si91x_gspi_get_status(gspi_driver_handle);
    //  printf("GSPI status is fetched successfully \n");
    //  printf("Busy: %d\n", gspi_status.busy);
    //  printf("Data_Lost: %d\n", gspi_status.data_lost);
    //  printf("Mode_Fault: %d\n", gspi_status.mode_fault);

    status = sl_si91x_gspi_set_configuration(gspi_driver_handle, NULL);
    if (status != SL_STATUS_OK)
    {
      LOG_E("Error setting SPI configuration: 0x%lx", status);
      return status;
    }

    status = sl_si91x_gspi_register_event_callback(gspi_driver_handle,
                                                   gspi_callback_event);
    if (status != SL_STATUS_OK)
    {
      LOG_E("Error registering SPI callback: 0x%lx", status);
      return status;
    }

    spi0_sem = osSemaphoreNew(1, 0, NULL);

    break;

  case WIUS_SPI_INST_1:
    sl_ssi_clock_config_t ssi_clock_configuration;
    ssi_clock_configuration.soc_pll_mm_count_value = WIUS_SPI_SOC_PLL_MM_COUNT_LIMIT;
    ssi_clock_configuration.intf_pll_500_control_value = WIUS_SPI_INTF_PLL_500_CTRL_VALUE;
    ssi_clock_configuration.intf_pll_clock = WIUS_SPI_INTF_PLL_CLK;
    ssi_clock_configuration.intf_pll_reference_clock = WIUS_SPI_INTF_PLL_REF_CLK;
    ssi_clock_configuration.soc_pll_clock = WIUS_SPI_SOC_PLL_CLK;
    ssi_clock_configuration.soc_pll_reference_clock = WIUS_SPI_SOC_PLL_REF_CLK;
    ssi_clock_configuration.division_factor = WIUS_SPI_DIVISION_FACTOR;

    status = sl_si91x_ssi_configure_clock(&ssi_clock_configuration);
    if (status != SL_STATUS_OK)
    {
      LOG_E("Error configuring SPI clock: 0x%lx", status);
      return status;
    }

    status = sl_si91x_ssi_init(SL_SSI_MASTER_ACTIVE, &ssi_driver_handle);
    if (status != SL_STATUS_OK)
    {
      LOG_E("Error initializing SPI: 0x%lx", status);
      return status;
    }

    status = sl_si91x_ssi_set_configuration(ssi_driver_handle, NULL, SSI_SLAVE_0);
    if (status != SL_STATUS_OK)
    {
      LOG_E("Error setting SPI configuration: 0x%lx", status);
      return status;
    }

    status = sl_si91x_ssi_register_event_callback(ssi_driver_handle, ssi_callback_event);
    if (status != SL_STATUS_OK)
    {
      LOG_E("Error registering SPI callback: 0x%lx", status);
      return status;
    }

    spi1_sem = osSemaphoreNew(1, 0, NULL);

    break;

  default:
    return SL_STATUS_INVALID_PARAMETER;
  }

#if WIUS_SPI_EXT_CS0
  wius_gpio_pin_config(&cs0_pin);
  WIUS_SPI_CS0_HIGH;
#endif
#if WIUS_SPI_EXT_CS1
  wius_gpio_pin_config(&cs1_pin);
  WIUS_SPI_CS1_HIGH;
#endif

  return status;
}

sl_gspi_handle_t wius_spi0_get_handle(void)
{
  return gspi_driver_handle;
}

sl_ssi_handle_t wius_spi1_get_handle(void)
{
  return ssi_driver_handle;
}

sl_status_t wius_spi_await(wius_spi_inst_t instance)
{
  // uint32_t inst_flags = (instance == WIUS_SPI_INST_0) ? FLAG_SPI_TF0_DONE : FLAG_SPI_TF1_DONE;
  // uint32_t flags = osEventFlagsWait(event_flags, inst_flags, 0, WIUS_SPI_RX_TIMEOUT);
  // if (!(flags & inst_flags))
  // {
  //   return SL_STATUS_TIMEOUT;
  // }

  if (instance == WIUS_SPI_INST_0)
    osSemaphoreAcquire(spi0_sem, osWaitForever);
  else
    osSemaphoreAcquire(spi1_sem, osWaitForever);

  return SL_STATUS_OK;
}

#define DMA_INSTANCE 0

static  GSPI_PIN gspi_clock    = { RTE_GSPI_MASTER_CLK_PORT ,RTE_GSPI_MASTER_CLK_PIN ,RTE_GSPI_MASTER_CLK_MUX ,RTE_GSPI_MASTER_CLK_PAD };
static  GSPI_PIN gspi_cs0       = { RTE_GSPI_MASTER_CS0_PORT ,RTE_GSPI_MASTER_CS0_PIN ,RTE_GSPI_MASTER_CS0_MUX ,RTE_GSPI_MASTER_CS0_PAD };
static  GSPI_PIN gspi_cs1       = { RTE_GSPI_MASTER_CS1_PORT ,RTE_GSPI_MASTER_CS1_PIN ,RTE_GSPI_MASTER_CS1_MUX ,RTE_GSPI_MASTER_CS1_PAD };
static  GSPI_PIN gspi_cs2       = { RTE_GSPI_MASTER_CS2_PORT ,RTE_GSPI_MASTER_CS2_PIN ,RTE_GSPI_MASTER_CS2_MUX ,RTE_GSPI_MASTER_CS2_PAD };
static  GSPI_PIN gspi_mosi     = { RTE_GSPI_MASTER_MOSI_PORT ,RTE_GSPI_MASTER_MOSI_PIN ,RTE_GSPI_MASTER_MOSI_MUX ,RTE_GSPI_MASTER_MOSI_PAD };
static  GSPI_PIN gspi_miso     = { RTE_GSPI_MASTER_MISO_PORT ,RTE_GSPI_MASTER_MISO_PIN ,RTE_GSPI_MASTER_MISO_MUX ,RTE_GSPI_MASTER_MISO_PAD };

static GSPI_FIFO_THRESHOLDS gspi_threshold = {TX_DMA_ARB_SIZE,RX_DMA_ARB_SIZE,RTE_FIFO_AFULL_THRLD,RTE_FIFO_AEMPTY_THRLD};

static GSPI_INFO          GSPI_MASTER_Info         = { 0U };
static GSPI_TRANSFER_INFO GSPI_MASTER_TransferInfo = { 0U };

void GSPI_MASTER_UDMA_Tx_Event (uint32_t event ,uint8_t dmaCh);
static GSPI_DMA GSPI_MASTER_UDMA_TX_CHNL = {
    {
        0,  //channelPrioHigh
        0,  //altStruct
        0,   //burstReq
        0,   //reqMask
        1,    //periphReq
        0,//DMA PERI ACK
        RTE_GSPI_MASTER_CHNL_UDMA_TX_CH    //dmaCh
    },
    RTE_GSPI_MASTER_CHNL_UDMA_TX_CH,
    GSPI_MASTER_UDMA_Tx_Event
};

void GSPI_MASTER_UDMA_Rx_Event (uint32_t event ,uint8_t dmaCh);
static GSPI_DMA GSPI_MASTER_UDMA_RX_CHNL = {
    {
        0,
        0,
        0,
        0,
        1,
        0,  //DMA ACK
        RTE_GSPI_MASTER_CHNL_UDMA_RX_CH
    },
    RTE_GSPI_MASTER_CHNL_UDMA_RX_CH,
    GSPI_MASTER_UDMA_Rx_Event
};

static const GSPI_RESOURCES gspi = {
    GSPI0,
    GSPI0_IRQn,
    &GSPI_MASTER_UDMA_RX_CHNL,
    &GSPI_MASTER_UDMA_TX_CHNL,
    &GSPI_MASTER_Info,
    &GSPI_MASTER_TransferInfo,
    {
      &gspi_clock,
      &gspi_cs0,
      &gspi_cs1,
      &gspi_cs2,
      &gspi_mosi,
      &gspi_miso, //pins end
    }, //pins end
    RTE_GSPI_CLOCK_SOURCE,//clock
    //thresholds
    &gspi_threshold,
    0
};

sl_status_t sl_si91x_gspi_transfer_data_cont(sl_gspi_handle_t gspi_handle, const void *data_out, void *data_in, uint32_t data_length)
{
//  // It calls the API to enable to slave gpio.
//  uint32_t gspi_status = ((sl_gspi_driver_t *)gspi_handle)->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
//  if (gspi_status != ARM_DRIVER_OK)
//  {
//    LOG_W("Error during slave GPIO setup");
//    return SL_STATUS_FAIL;
//  }

  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  uint16_t data_bits                 = 0;

  gspi.info->status.busy       = 1U;
  gspi.info->status.data_lost  = 0U;
  gspi.info->status.mode_fault = 0U;

  gspi.xfer->rx_buf = (uint8_t *)data_in;
  gspi.xfer->tx_buf = (uint8_t *)data_out;

  gspi.xfer->num    = data_length;
  gspi.xfer->rx_cnt = 0U;
  gspi.xfer->tx_cnt = 0U;

  // Enable Full duplex mode
  gspi.reg->GSPI_CONFIG1_b.SPI_FULL_DUPLEX_EN = 0x1;

  data_bits = GSPI0->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2;

  // Configure the FIFO thresholds
  gspi.reg->GSPI_FIFO_THRLD_b.FIFO_AEMPTY_THRLD = (unsigned int)(gspi.threshold->gspi_aempty_threshold & 0x0F);
  gspi.reg->GSPI_FIFO_THRLD_b.FIFO_AFULL_THRLD  = (unsigned int)(gspi.threshold->gspi_afull_threshold & 0x0F);


  control.transferType = UDMA_MODE_BASIC;
  control.nextBurst    = 0;
  if (data_length < 1024) {
    control.totalNumOfDMATrans = (unsigned int)((data_length - 1) & 0x0F);
  } else {
    control.totalNumOfDMATrans = 0x3FF;
  }
  control.rPower      = (unsigned int)(gspi.threshold->txdma_arb_size & 0x0F);
  control.srcProtCtrl = 0x0;
  control.dstProtCtrl = 0x0;
  if ((data_bits <= 8) && (data_bits != 0)) {
    //8-bit data frame
    control.srcSize = SRC_SIZE_8;
    control.srcInc  = SRC_INC_8;
    control.dstSize = DST_SIZE_8;
    control.dstInc  = DST_INC_NONE;
  } else {
    //16-bit data frame
    control.srcSize = SRC_SIZE_16;
    control.srcInc  = SRC_INC_16;
    control.dstSize = DST_SIZE_16;
    control.dstInc  = DST_INC_NONE;
  }

  sl_dma_xfer_t dma_transfer_tx = { 0 };
  uint32_t channel              = gspi.tx_dma->channel + 1;
  uint32_t channel_priority     = gspi.tx_dma->chnl_cfg.channelPrioHigh;
//  sl_dma_callback_t gspi_tx_callback;
//  //Initialize sl_dma callback structure
//  gspi_tx_callback.transfer_complete_cb = gspi_transfer_complete_callback;
//  gspi_tx_callback.error_cb             = gspi_error_callback;
  //Initialize sl_dma transfer structure
  dma_transfer_tx.src_addr       = (uint32_t *)((uint32_t)(gspi.xfer->tx_buf));
  dma_transfer_tx.dest_addr      = (uint32_t *)((uint32_t) & (gspi.reg->GSPI_WRITE_FIFO));
  dma_transfer_tx.src_inc        = control.srcInc;
  dma_transfer_tx.dst_inc        = control.dstInc;
  dma_transfer_tx.xfer_size      = control.dstSize;
  dma_transfer_tx.transfer_count = data_length;
  dma_transfer_tx.transfer_type  = SL_DMA_MEMORY_TO_PERIPHERAL;
  dma_transfer_tx.dma_mode       = control.transferType;
  dma_transfer_tx.signal         = (uint8_t)gspi.tx_dma->chnl_cfg.periAck;

//  //Allocate DMA channel for Tx
//  status = sl_si91x_dma_allocate_channel(DMA_INSTANCE, &channel, channel_priority);
//  if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
//      LOG_W("Error in TX DMA channel allocation");
//    return SL_STATUS_FAIL;
//  }
//  //Register transfer complete and error callback
//  if (sl_si91x_dma_register_callbacks(DMA_INSTANCE, channel, &gspi_tx_callback)) {
//      LOG_W("Error in TX DMA callback register");
//    return SL_STATUS_FAIL;
//  }
  //Configure the channel for DMA transfer
  if (sl_si91x_dma_transfer(DMA_INSTANCE, channel, &dma_transfer_tx)) {
      LOG_W("Error in TX DMA transfer");
    return SL_STATUS_FAIL;
  }

  GSPI0->GSPI_WRITE_DATA2_b.GSPI_MANUAL_WRITE_DATA2 = (unsigned int)(data_bits & 0x0F);
  gspi.reg->GSPI_CONFIG1_b.GSPI_MANUAL_WR          = 0x1; //write enable
  GSPI0->GSPI_WRITE_DATA2_b.USE_PREV_LENGTH         = 0x1;

  control.transferType = UDMA_MODE_BASIC;
  control.nextBurst    = 0;
  if (data_length < 1024) {
    control.totalNumOfDMATrans = (unsigned int)((data_length - 1) & 0x03FF);
  } else {
    control.totalNumOfDMATrans = 0x3FF;
  }
  control.rPower      = (unsigned int)(gspi.threshold->rxdma_arb_size & 0x0F);
  control.srcProtCtrl = 0x0;
  control.dstProtCtrl = 0x0;
  if ((data_bits <= 8) && (data_bits != 0)) {
    //8-bit data frame
    control.srcSize = SRC_SIZE_8;
    control.srcInc  = SRC_INC_NONE;
    control.dstSize = DST_SIZE_8;
    control.dstInc  = DST_INC_8;
  } else {
    //16-bit data frame
    control.srcSize = SRC_SIZE_16;
    control.srcInc  = SRC_INC_NONE;
    control.dstSize = DST_SIZE_16;
    control.dstInc  = DST_INC_16;
  }

  sl_dma_xfer_t dma_transfer_rx = { 0 };
  channel              = gspi.rx_dma->channel + 1;
  channel_priority     = gspi.rx_dma->chnl_cfg.channelPrioHigh;
//  sl_dma_callback_t gspi_rx_callback;
//  //Initialize sl_dma callback structure
//  gspi_rx_callback.transfer_complete_cb = gspi_transfer_complete_callback;
//  gspi_rx_callback.error_cb             = gspi_error_callback;
  //Initialize sl_dma transfer structure
  dma_transfer_rx.src_addr       = (uint32_t *)((uint32_t) & (gspi.reg->GSPI_READ_FIFO));
  dma_transfer_rx.dest_addr      = (uint32_t *)((uint32_t)(gspi.xfer->rx_buf));
  dma_transfer_rx.src_inc        = control.srcInc;
  dma_transfer_rx.dst_inc        = control.dstInc;
  dma_transfer_rx.xfer_size      = control.dstSize;
  dma_transfer_rx.transfer_count = data_length;
  dma_transfer_rx.transfer_type  = SL_DMA_PERIPHERAL_TO_MEMORY;
  dma_transfer_rx.dma_mode       = control.transferType;
  dma_transfer_rx.signal         = (uint8_t)gspi.rx_dma->chnl_cfg.periAck;

//  //Allocate DMA channel for Rx
//  status = sl_si91x_dma_allocate_channel(DMA_INSTANCE, &channel, channel_priority);
//  if (status && (status != SL_STATUS_DMA_CHANNEL_ALLOCATED)) {
//      LOG_W("Error in RX DMA channel allocation");
//    return SL_STATUS_FAIL;
//  }
//  //Register transfer complete and error callback
//  if (sl_si91x_dma_register_callbacks(DMA_INSTANCE, channel, &gspi_rx_callback)) {
//      LOG_W("Error in RX DMA callback register");
//    return SL_STATUS_FAIL;
//  }
  //Configure the channel for DMA transfer
  if (sl_si91x_dma_transfer(DMA_INSTANCE, channel, &dma_transfer_rx)) {
      LOG_W("Error in RX DMA transfer");
    return SL_STATUS_FAIL;
  }
  gspi.reg->GSPI_CONFIG1_b.GSPI_MANUAL_RD = 0x1;

  sl_si91x_dma_channel_enable(DMA_INSTANCE, gspi.rx_dma->channel + 1);
  sl_si91x_dma_channel_enable(DMA_INSTANCE, gspi.tx_dma->channel + 1);
  sl_si91x_dma_enable(DMA_INSTANCE);

  return SL_STATUS_OK;
}

sl_status_t wius_spi_xfer(wius_spi_inst_t instance, uint8_t *tx_buf, uint8_t *rx_buf, size_t len, bool wait)
{
  sl_status_t status = SL_STATUS_OK;

  switch (instance)
  {
  case WIUS_SPI_INST_0:
    sl_si91x_gspi_set_slave_number(GSPI_SLAVE_0);

    WIUS_SPI_CS0_LOW;

    spi0_transfer_complete = false;
    status = sl_si91x_gspi_transfer_data(gspi_driver_handle, tx_buf, rx_buf, len);
    if (SL_STATUS_OK != status)
    {
      WIUS_SPI_CS0_HIGH;
      return status;
    }

    WIUS_SPI_CS0_HIGH;

    break;

  case WIUS_SPI_INST_1:
    sl_si91x_ssi_set_slave_number(SSI_SLAVE_0);

    WIUS_SPI_CS1_LOW;

    status = sl_si91x_ssi_transfer_data(ssi_driver_handle, tx_buf, rx_buf, len);
    if (SL_STATUS_OK != status)
    {
      WIUS_SPI_CS1_HIGH;
      return status;
    }

    WIUS_SPI_CS1_HIGH;

    break;

  default:
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (wait)
    CHECK_STATUS(wius_spi_await(instance));

  return status;
}

sl_status_t wius_spi0_xfer_cont(uint8_t *tx_buf, uint8_t *rx_buf, size_t len)
{
  sl_si91x_gspi_set_slave_number(GSPI_SLAVE_0);

  WIUS_SPI_CS0_LOW;

  spi0_transfer_complete = false;
  sl_status_t status = sl_si91x_gspi_transfer_data_cont(gspi_driver_handle, tx_buf, rx_buf, len);

  WIUS_SPI_CS0_HIGH;

  return status;
}

static void gspi_callback_event(uint32_t event)
{
  switch (event)
  {
  case SL_GSPI_TRANSFER_COMPLETE:
//     osEventFlagsSet(event_flags, FLAG_SPI_TF0_DONE);
     osSemaphoreRelease(spi0_sem);
    break;
  case SL_GSPI_DATA_LOST:
    LOG_D("SPI0: Data lost");
    break;
  case SL_GSPI_MODE_FAULT:
    LOG_D("SPI0: Mode fault");
    break;
  }
}

static void ssi_callback_event(uint32_t event)
{
  switch (event)
  {
  case SSI_EVENT_TRANSFER_COMPLETE:
    // osEventFlagsSet(event_flags, FLAG_SPI_TF1_DONE);
    osSemaphoreRelease(spi1_sem);
    break;
  case SSI_EVENT_DATA_LOST:
    LOG_D("SPI1: Data lost");
    break;
  case SSI_EVENT_MODE_FAULT:
    LOG_D("SPI1: Mode fault");
    break;
  }
}
