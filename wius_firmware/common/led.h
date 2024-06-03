/**
 * @file led.h
 *
 * @brief LED module for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup common
 *
 */

#ifndef LED_H_
#define LED_H_

#include "common.h"

/**
 * @brief Initialize the onboard LEDs
 *
 */
void leds_init();

/**
 * @brief Set the red LED
 *
 * @param on Set to true to turn the LED on, false to turn it off
 *
 */
void led_red_set(bool on);

/**
 * @brief Set the green LED
 *
 * @param on Set to true to turn the LED on, false to turn it off
 *
 */
void led_green_set(bool on);

#endif /* LED_H_ */
