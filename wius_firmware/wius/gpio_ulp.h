/**
 * @file gpio_ulp.h
 *
 * @brief ULP GPIO module for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 10.04.2024
 *
 * @ingroup wius
 *
 */

#ifndef GPIO_ULP_H_
#define GPIO_ULP_H_

#include "common.h"

#include "sl_gpio_board.h"
#include "sl_si91x_peripheral_gpio.h"

/**
 * @brief GPIO output pin definition
 *
 * @param NUM: GPIO pin number
 *
 */
#define WIUS_GPIO_ULP_OUTPUT(NUM) { \
  .pin = NUM, \
  .dir = WIUS_GPIO_ULP_DIR_OUTPUT \
}

/**
 * @brief GPIO input pin definition
 *
 * @param NUM: GPIO pin number
 *
 */
#define WIUS_GPIO_ULP_INPUT(NUM) { \
  .pin = NUM, \
  .dir = WIUS_GPIO_ULP_DIR_INPUT \
}

/**
 * @brief GPIO directions enumeration
 *
 */
typedef enum wius_gpio_ulp_dir
{
  WIUS_GPIO_ULP_DIR_OUTPUT, /**< Output */
  WIUS_GPIO_ULP_DIR_INPUT   /**< Input */
} wius_gpio_ulp_dir_t;

/**
 * @brief GPIO interrupt trigger enumeration
 *
 */
typedef enum wius_gpio_ulp_interrupt
{
  WIUS_GPIO_ULP_INT_HIGH = SL_GPIO_INTERRUPT_HIGH,                                   /**< High level */
  WIUS_GPIO_ULP_INT_LOW = SL_GPIO_INTERRUPT_LOW,                                     /**< Low level */
  WIUS_GPIO_ULP_INT_RISING = SL_GPIO_INTERRUPT_RISE_EDGE,                            /**< Rising edge */
  WIUS_GPIO_ULP_INT_FALLING = SL_GPIO_INTERRUPT_FALL_EDGE,                           /**< Falling edge */
  WIUS_GPIO_ULP_INT_EDGE = SL_GPIO_INTERRUPT_RISE_EDGE | SL_GPIO_INTERRUPT_FALL_EDGE /**< Any edge */
} wius_gpio_ulp_interrupt_t;

/**
 * @brief GPIO pin definition structure
 *
 *
 * @note Can be generated using the @ref WIUS_GPIO_ULP_OUTPUT and @ref WIUS_GPIO_ULP_INPUT macros
 *
 */
typedef struct wius_gpio_ulp
{
  uint8_t pin;         /**< GPIO pin number */
  uint8_t pad;         /**< GPIO pad number */
  wius_gpio_ulp_dir_t dir; /**< GPIO direction */
} wius_gpio_ulp_t;

/**
 * @brief Initialize GPIO module
 *
 */
void wius_gpio_ulp_init(void);

/**
 * @brief Configure GPIO pin
 *
 * @param gpio: GPIO pin definition
 *
 * @note Must be called before using the pin
 * @note Must be called after @ref wius_gpio_ulp_init
 *
 */
void wius_gpio_ulp_pin_config(wius_gpio_ulp_t *gpio);

/**
 * @brief Attach interrupt to GPIO pin
 *
 * @param gpio: GPIO pin definition
 * @param trigger: Interrupt trigger (see @ref wius_gpio_ulp_interrupt_t)
 * @param function: Interrupt callback function
 *
 * @retval SL_STATUS_OK: Success
 * @retval SL_STATUS_INVALID_STATE: Pin is not an input or maximum number of interrupts reached
 *
 */
sl_status_t wius_gpio_ulp_pin_attach_interrupt(wius_gpio_ulp_t gpio, wius_gpio_ulp_interrupt_t trigger, void (*function)(void));

/**
 * @brief Set GPIO pin state
 *
 * @param gpio: GPIO pin definition
 * @param on: State to set (true = high, false = low)
 *
 */
void wius_gpio_ulp_pin_set(wius_gpio_ulp_t gpio, bool on);

/**
 * @brief Toggle GPIO pin state
 *
 * @param gpio: GPIO pin definition
 *
 */
void wius_gpio_ulp_pin_toggle(wius_gpio_ulp_t gpio);

/**
 * @brief Get GPIO pin state
 *
 * @param gpio: GPIO pin definition
 *
 * @return GPIO pin state (0 = low, 1 = high)
 *
 */
uint8_t wius_gpio_ulp_pin_get(wius_gpio_ulp_t gpio);

#endif /* GPIO_ULP_H_ */
