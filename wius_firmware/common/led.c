/**
 * @file led.c
 *
 * @brief LED module implementation for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup common
 *
 */

#include "wius/gpio.h"

// The LEDs are active low
// The LEDs are set to inputs to turn them off and low outputs to turn them on
wius_gpio_t led_r = WIUS_GPIO_INPUT(WIUS_LOG_GPIO_LED_RED);
wius_gpio_t led_g = WIUS_GPIO_INPUT(WIUS_LOG_GPIO_LED_GREEN);

#if WIUS_LOG_LEVEL >= LOG_LEVEL_DEBUG

void leds_init()
{
  wius_gpio_init();
  wius_gpio_pin_config(&led_r);
  wius_gpio_pin_config(&led_g);
}

void led_red_set(bool on)
{
  led_r.dir = on ? WIUS_GPIO_DIR_OUTPUT : WIUS_GPIO_DIR_INPUT;

  wius_gpio_pin_config(&led_r);
}

void led_green_set(bool on)
{
  led_g.dir = on ? WIUS_GPIO_DIR_OUTPUT : WIUS_GPIO_DIR_INPUT;

  wius_gpio_pin_config(&led_g);
}

#else

void leds_init()
{
}

void led_red_set(bool on)
{
}

void led_green_set(bool on)
{
}
#endif

