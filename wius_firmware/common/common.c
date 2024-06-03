/**
 * @file common.c
 *
 * @brief Common code implementation for the WIUS and TinyProbe firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup common
 *
 */

#include "common.h"

#include "cmsis_os2.h"

osEventFlagsId_t event_flags;

void common_init(void)
{
  event_flags = osEventFlagsNew(NULL);
}

void delay_ns(uint32_t ns)
{
  volatile int32_t timeout = (ns - 110000) * 119 / 1000 / 7;
//  volatile int32_t timeout = ns * 100 / 1000 / 7;

  do {
      timeout--;
  } while (timeout > 0);

//  delay_ms(2 + ns / 1000000);
}

void delay_ms(uint32_t ms)
{
  osDelay(ms * TICKS_PER_SEC / 1000);
}

uint32_t time_ms(void)
{
  return osKernelGetTickCount() * 1000 / TICKS_PER_SEC;
}
