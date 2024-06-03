/***************************************************************************/ /**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdio.h>

#include "common.h"
#include "tinyprobe/tp.h"


const osThreadAttr_t main_thread_attributes = {
    .name = "main",
    .attr_bits = 0,
    .cb_mem = 0,
    .cb_size = 0,
    .stack_mem = 0,
    .stack_size = 8192,
    .priority = osPriorityNormal,
    .tz_module = 0,
    .reserved = 0,
};
osThreadId_t main_thread_id = NULL;

void main_thread_func(void *arg);

/***************************************************************************/ /**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  main_thread_id = osThreadNew(main_thread_func, NULL, &main_thread_attributes);
  if (NULL == main_thread_id)
  {
    printf("Error creating main thread!\r\n");
  }
}

/***************************************************************************/ /**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
}

void main_thread_func(void *arg)
{
  (void)arg;
  sl_status_t status = SL_STATUS_OK;

  printf("\r\n---   WIUS FIRMWARE   ---\r\n");
  LOG_D("Started application thread!");

  leds_init();
  led_red_set(true);

  status = tp_init();
  if (status != SL_STATUS_OK)
  {
    LOG_E("TinyProbe initialization failed!");
    return;
  }

  led_red_set(false);
  led_green_set(true);

  tp_main_thread();

  LOG_D("Application thread exit");
}
