/**
 * @file gpio_uulp.c
 *
 * @brief UULP GPIO module implementation for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 10.04.2024
 *
 * @ingroup wius
 *
 */

#include "gpio_uulp.h"

#include "rsi_rom_clks.h"
#include "sl_si91x_gpio.h"
#include "sl_si91x_peripheral_gpio.h"
#include "sl_gpio_board.h"

#define MAX_PIN_INTERRUPTS 4

size_t uulp_active_interrupts = 0;
void (*uulp_interrupt_functions[MAX_PIN_INTERRUPTS])(void) = {NULL};

void wius_gpio_uulp_init(void)
{
  sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);

  uulp_active_interrupts = 0;
  for (uint8_t i = 0; i < MAX_PIN_INTERRUPTS; i++)
  {
    NVIC_EnableIRQ(PININT0_NVIC_NAME + i);
  }
}

void wius_gpio_uulp_pin_config(wius_gpio_uulp_t *gpio)
{
  sl_si91x_gpio_select_uulp_npss_receiver(gpio->pin, GPIO_RECEIVER_EN);
  sl_si91x_gpio_set_uulp_npss_pin_mux(gpio->pin, NPSS_GPIO_PIN_MUX_MODE0);
  sl_si91x_gpio_set_uulp_npss_direction(gpio->pin, (sl_si91x_gpio_direction_t)gpio->dir);
}

sl_status_t wius_gpio_uulp_pin_attach_interrupt(wius_gpio_uulp_t gpio, wius_gpio_uulp_interrupt_t trigger, void (*function)(void))
{
  if (gpio.dir != WIUS_GPIO_UULP_DIR_INPUT || uulp_active_interrupts > MAX_PIN_INTERRUPTS - 1)
    return SL_STATUS_INVALID_STATE;

  uulp_pad_config_t config;
  config.gpio_padnum = gpio.pin;
  config.pad_select = 1;
  config.mode = 0;
  config.direction = 1;
  config.receiver = 1;
  sl_si91x_gpio_set_uulp_pad_configuration(&config);

  uulp_interrupt_functions[gpio.pin] = function;

  sl_si91x_gpio_configure_uulp_interrupt((sl_si91x_gpio_interrupt_config_flag_t)trigger, gpio.pin);

  NVIC_EnableIRQ(UULP_PININT_NVIC_NAME);
  NVIC_SetPriority(UULP_PININT_NVIC_NAME, 7);

  uulp_active_interrupts++;

  return SL_STATUS_OK;
}

void wius_gpio_uulp_pin_set(wius_gpio_uulp_t gpio, bool on)
{
  sl_si91x_gpio_set_uulp_npss_pin_value(gpio.pin, on);
}

void wius_gpio_uulp_pin_toggle(wius_gpio_uulp_t gpio)
{
  sl_si91x_gpio_toggle_uulp_npss_pin(gpio.pin);
}

uint8_t wius_gpio_uulp_pin_get(wius_gpio_uulp_t gpio)
{
  return sl_si91x_gpio_get_uulp_npss_pin(gpio.pin);
}

void UULP_PIN_IRQ_Handler(void)
{
  uint8_t status = sl_si91x_gpio_get_uulp_interrupt_status();

  for (uint8_t i = 0; i < MAX_PIN_INTERRUPTS; i++)
  {
    if (status & (1 << i))
    {
      sl_si91x_gpio_clear_uulp_interrupt(1 << i);

      if (uulp_interrupt_functions[i] != NULL)
      {
        uulp_interrupt_functions[i]();
      }
    }
  }
}
