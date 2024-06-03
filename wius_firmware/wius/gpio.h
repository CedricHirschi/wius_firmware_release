/**
 * @file gpio.h
 *
 * @brief GPIO module for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 05.04.2024
 *
 * @ingroup wius
 *
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "common.h"

#include "sl_gpio_board.h"
#include "sl_si91x_peripheral_gpio.h"

#define CONCAT_3(a, b, c) a ## b ## c

/**
 * @brief GPIO output pin definition
 *
 * @param NUM: GPIO pin number
 *
 */
#define WIUS_GPIO_OUTPUT(NUM) { \
  .pin = CONCAT_3(SL_SI91X_GPIO_, NUM, _PIN), \
  .port = CONCAT_3(SL_SI91X_GPIO_, NUM, _PORT), \
  .pad = CONCAT_3(SL_SI91X_GPIO_, NUM, _PAD), \
  .dir = WIUS_GPIO_DIR_OUTPUT \
}

/**
 * @brief GPIO input pin definition
 *
 * @param NUM: GPIO pin number
 *
 */
#define WIUS_GPIO_INPUT(NUM) { \
  .pin = CONCAT_3(SL_SI91X_GPIO_, NUM, _PIN), \
  .port = CONCAT_3(SL_SI91X_GPIO_, NUM, _PORT), \
  .pad = CONCAT_3(SL_SI91X_GPIO_, NUM, _PAD), \
  .dir = WIUS_GPIO_DIR_INPUT \
}

/**
 * @brief GPIO directions enumeration
 *
 */
typedef enum wius_gpio_dir
{
  WIUS_GPIO_DIR_OUTPUT, /**< Output */
  WIUS_GPIO_DIR_INPUT   /**< Input */
} wius_gpio_dir_t;

/**
 * @brief GPIO interrupt trigger enumeration
 *
 */
typedef enum wius_gpio_interrupt
{
  WIUS_GPIO_INT_HIGH = SL_GPIO_INTERRUPT_HIGH,                                        /**< High level */
  WIUS_GPIO_INT_LOW = SL_GPIO_INTERRUPT_LOW,                                          /**< Low level */
  WIUS_GPIO_INT_RISING = SL_GPIO_INTERRUPT_RISING_EDGE,                               /**< Rising edge */
  WIUS_GPIO_INT_FALLING = SL_GPIO_INTERRUPT_FALLING_EDGE,                             /**< Falling edge */
  WIUS_GPIO_INT_EDGE = SL_GPIO_INTERRUPT_RISING_EDGE | SL_GPIO_INTERRUPT_FALLING_EDGE /**< Any edge */
} wius_gpio_interrupt_t;

/**
 * @brief GPIO pin definition structure
 *
 *
 * @note Can be generated using the @ref WIUS_GPIO_OUTPUT and @ref WIUS_GPIO_INPUT macros
 *
 */
typedef struct wius_gpio
{
  uint8_t pin;         /**< GPIO pin number */
  sl_gpio_port_t port; /**< GPIO port number */
  uint8_t pad;         /**< GPIO pad number */
  wius_gpio_dir_t dir; /**< GPIO direction */
  uint8_t _port_pin;   /**< Internal use only (fix for port offset) */
} wius_gpio_t;

/**
 * @brief Initialize GPIO module
 *
 */
void wius_gpio_init(void);

/**
 * @brief Configure GPIO pin
 *
 * @param gpio: GPIO pin definition
 *
 * @note Must be called before using the pin
 * @note Must be called after @ref wius_gpio_init
 *
 */
void wius_gpio_pin_config(wius_gpio_t *gpio);

/**
 * @brief Attach interrupt to GPIO pin
 *
 * @param gpio: GPIO pin definition
 * @param trigger: Interrupt trigger (see @ref wius_gpio_interrupt_t)
 * @param function: Interrupt callback function
 *
 * @retval SL_STATUS_OK: Success
 * @retval SL_STATUS_INVALID_STATE: Pin is not an input or maximum number of interrupts reached
 *
 */
sl_status_t wius_gpio_pin_attach_interrupt(wius_gpio_t gpio, wius_gpio_interrupt_t trigger, void (*function)(void));

/**
 * @brief Set GPIO pin state
 *
 * @param gpio: GPIO pin definition
 * @param on: State to set (true = high, false = low)
 *
 */
void wius_gpio_pin_set(wius_gpio_t gpio, bool on);

/**
 * @brief Toggle GPIO pin state
 *
 * @param gpio: GPIO pin definition
 *
 */
void wius_gpio_pin_toggle(wius_gpio_t gpio);

/**
 * @brief Get GPIO pin state
 *
 * @param gpio: GPIO pin definition
 *
 * @return GPIO pin state (0 = low, 1 = high)
 *
 */
uint8_t wius_gpio_pin_get(wius_gpio_t gpio);

#endif /* GPIO_H_ */
