/**
 * @file log.h
 *
 * @brief Logging module for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup common
 *
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#include "sys.h"
#include "cmsis_os2.h"
#include "os_tick.h"

#include "config.h"

#define LOG_LEVEL_DEBUG 2   // Log everything
#define LOG_LEVEL_WARNING 1 // Log warnings and errors
#define LOG_LEVEL_ERROR 0   // Log only errors

#ifdef WIUS_LOG_LEVEL
#define LOG_LEVEL WIUS_LOG_LEVEL
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_D(...) _LOG_PRINT("DEB", __VA_ARGS__) // Debug message
#else
#define LOG_D(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARNING
#define LOG_W(...) _LOG_PRINT("WRN", __VA_ARGS__) // Warning message
#else
#define LOG_W(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_E(...) _LOG_PRINT("ERR", __VA_ARGS__) // Error message
#else
#define LOG_E(...)
#endif

#define ASSERT(x, ...) _LOG_ASSERT(x, #x, ##__VA_ARGS__) // Assert macro

#define _LOG_ASSERT(x, x_str, ...)                                  \
    do                                                              \
    {                                                               \
        if (!(x))                                                   \
        {                                                           \
            printf("\n");                                           \
            LOG_E("Assertion failed: " x_str "\n\r\t" __VA_ARGS__); \
            while (true)                                            \
            {                                                       \
            }                                                       \
        }                                                           \
    } while (false)

#define _LOG_PRINT(level, ...)                                                \
    do                                                                        \
    {                                                                         \
        printf("[%s %7lu %27s]   ", level, osKernelGetTickCount(), __func__); \
        printf(__VA_ARGS__);                                                  \
        printf("\n\r");                                                       \
    } while (0)

#endif /* LOG_H_ */
