/**
 * @file power.c
 *
 * @brief Power management functions implementation for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 05.04.2024
 *
 * @ingroup wius
 *
 */

#include "power.h"

#include <sl_si91x_power_manager.h>

#define WIUS_POWER_FREQ_HIGH 180000000 /**< System clock in high performance mode (180MHz) */
#define WIUS_POWER_FREQ_LOW   32000000 /**< System clock in low power mode (32MHz) */

sl_status_t wius_power_init(void)
{
  sl_status_t status = SL_STATUS_OK;

  return status;
}

sl_status_t wius_power_m4_low(void)
{
  sl_status_t status = SL_STATUS_OK;

//  CHECK_STATUS(sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3));
//  CHECK_STATUS(sl_si91x_power_manager_set_clock_scaling(SL_SI91X_POWER_MANAGER_POWERSAVE));
//  if(0 != SysTick_Config(WIUS_POWER_FREQ_LOW / 1000))
//  {
//    return SL_STATUS_FAIL;
//  }

  return status;
}

sl_status_t wius_power_m4_high(void)
{
  sl_status_t status = SL_STATUS_OK;

//  CHECK_STATUS(sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4));
//  CHECK_STATUS(sl_si91x_power_manager_set_clock_scaling(SL_SI91X_POWER_MANAGER_PERFORMANCE));
//  if(0 != SysTick_Config(WIUS_POWER_FREQ_LOW / 1000))
//  {
//    return SL_STATUS_FAIL;
//  }

  return status;
}
