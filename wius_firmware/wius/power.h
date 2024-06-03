/**
 * @file power.h
 *
 * @brief Power management functions for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 05.04.2024
 *
 * @ingroup wius
 *
 */

#ifndef WIUS_POWER_H_
#define WIUS_POWER_H_

#include "common.h"

/**
 * @brief Initialize the power management module
 *
 * @return SL_STATUS_OK
 *
 * @note This function currently does nothing
 *
 */
sl_status_t wius_power_init(void);

/**
 * @brief Set the M4 SOC to low power mode
 *
 * @retval SL_STATUS_OK: Success
 * @retval SL_STATUS_FAIL: Any RSI function returned an error
 *
 */
sl_status_t wius_power_m4_low(void);

/**
 * @brief Set the M4 SOC to high power mode
 *
 * @retval SL_STATUS_OK: Success
 * @retval SL_STATUS_FAIL: Any RSI function returned an error
 *
 */
sl_status_t wius_power_m4_high(void);

#endif /* WIUS_POWER_H_ */
