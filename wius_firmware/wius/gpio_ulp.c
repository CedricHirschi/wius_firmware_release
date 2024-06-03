/**
 * @file gpio_ulp.c
 *
 * @brief ULP GPIO module implementation for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 10.04.2024
 *
 * @ingroup wius
 *
 */

#include "gpio_ulp.h"

#include "rsi_rom_clks.h"
#include "sl_si91x_gpio.h"
#include "sl_si91x_peripheral_gpio.h"
#include "sl_gpio_board.h"

#define MAX_PIN_INTERRUPTS 8

size_t ulp_active_interrupts = 0;
void (*ulp_interrupt_functions[MAX_PIN_INTERRUPTS])(void) = {NULL};

void wius_gpio_ulp_init(void)
{
  sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);

  ulp_active_interrupts = 0;
  for (uint8_t i = 0; i < MAX_PIN_INTERRUPTS; i++)
  {
    NVIC_EnableIRQ(PININT0_NVIC_NAME + i);
  }
}

void wius_gpio_ulp_pin_config(wius_gpio_ulp_t *gpio)
{
  if (gpio->dir == WIUS_GPIO_ULP_DIR_OUTPUT)
  {
    sl_si91x_gpio_enable_ulp_pad_receiver(gpio->pin);

    sl_gpio_set_pin_mode(4, gpio->pin, _MODE0, 0);
    sl_si91x_gpio_set_pin_direction(4, gpio->pin, EGPIO_CONFIG_DIR_OUTPUT);
  }
  else
  {
    sl_si91x_gpio_enable_ulp_pad_receiver(gpio->pin);

    sl_si91x_gpio_select_ulp_pad_driver_strength(gpio->pin, (sl_si91x_gpio_driver_strength_select_t)GPIO_TWO_MILLI_AMPS);
    sl_si91x_gpio_select_ulp_pad_driver_disable_state(gpio->pin, (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);

    sl_gpio_set_pin_mode(4, gpio->pin, _MODE0, 0);
    sl_si91x_gpio_set_pin_direction(4, gpio->pin, EGPIO_CONFIG_DIR_INPUT);
  }
}

sl_status_t wius_gpio_ulp_pin_attach_interrupt(wius_gpio_ulp_t gpio, wius_gpio_ulp_interrupt_t trigger, void (*function)(void))
{
  if (gpio.dir != WIUS_GPIO_ULP_DIR_INPUT || ulp_active_interrupts > MAX_PIN_INTERRUPTS - 1)
    return SL_STATUS_INVALID_STATE;

  sl_si91x_gpio_configure_ulp_pin_interrupt(ulp_active_interrupts, (sl_si91x_gpio_interrupt_config_flag_t)trigger, gpio.pin);

  ulp_interrupt_functions[ulp_active_interrupts] = function;

  NVIC_EnableIRQ(ULP_PININT0_NVIC_NAME + ulp_active_interrupts);
  NVIC_SetPriority(ULP_PININT0_NVIC_NAME + ulp_active_interrupts, 7);

  ulp_active_interrupts++;

  return SL_STATUS_OK;
}

void wius_gpio_ulp_pin_set(wius_gpio_ulp_t gpio, bool on)
{
  on ? sl_gpio_set_pin_output(4, gpio.pin) : sl_gpio_clear_pin_output(4, gpio.pin);
}

void wius_gpio_ulp_pin_toggle(wius_gpio_ulp_t gpio)
{
  wius_gpio_ulp_pin_set(gpio, !wius_gpio_ulp_pin_get(gpio));
}

uint8_t wius_gpio_ulp_pin_get(wius_gpio_ulp_t gpio)
{
  return sl_gpio_get_pin_input(4, gpio.pin);
}

void ULP_PIN_IRQ_Handler(void)
{
  for (uint8_t i = 0; i < MAX_PIN_INTERRUPTS; i++)
  {
    if (sl_si91x_gpio_get_ulp_interrupt_status(i))
    {
      sl_si91x_gpio_clear_ulp_interrupt(i);

      if (ulp_interrupt_functions[i] != NULL)
      {
        ulp_interrupt_functions[i]();
      }
    }
  }
}
