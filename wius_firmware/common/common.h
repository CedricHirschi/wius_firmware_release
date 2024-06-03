/**
 * @file common.h
 *
 * @brief Common code for the WIUS and TinyProbe firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup common
 *
 */

/**
 * @defgroup wius WIUS Firmware
 * @brief Firmware for the WIUS project. Controls the low-level hardware and provides a high-level interface for the user.
 *
 */

/**
 * @defgroup tinyprobe TinyProbe Firmware
 * @brief Firmware for the TinyProbe project. Controls the interface to the TinyProbe and provides a high-level interface for the user.
 *
 */

/**
 * @defgroup common Common
 * @brief Common code for the WIUS and TinyProbe firmware
 *
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "sl_status.h"
#include "os_tick.h"

#include "log.h"
#include "led.h"
#include "config.h"

#define TICKS_PER_SEC (OS_Tick_GetClock() / OS_Tick_GetInterval()) // Number of ticks per second

#define CHECK_STATUS(x)             \
    do                              \
    {                               \
        status = (x);               \
        if (SL_STATUS_OK != status) \
            return status;          \
    } while (0) // Check status and return if not OK

extern osEventFlagsId_t event_flags;

/**
 * @brief Initialize some common stuff
 *
 */
void common_init(void);

/**
 * @brief Delay for a given number of nanoseconds
 *
 * @param ns Number of nanoseconds to delay
 *
 * @note This function is not very accurate
 *
 */
void delay_ns(uint32_t ns);

/**
 * @brief Delay for a given number of milliseconds
 *
 * @param us Number of milliseconds to delay
 *
 */
void delay_ms(uint32_t ms);

/**
 * @brief Get the current time in milliseconds
 *
 * @return Current time in milliseconds
 *
 */
uint32_t time_ms(void);

#endif /* COMMON_H_ */
