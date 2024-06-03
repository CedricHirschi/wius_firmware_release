/**
 * @file mux.c
 *
 * @brief SPI mux implementation for the TinyProbe
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup tinyprobe
 *
 */

#include "mux.h"

#include "wius/gpio_ulp.h"
#include "wius/gpio_uulp.h"

wius_gpio_uulp_t extintmux_pin = WIUS_GPIO_UULP_OUTPUT(TP_MUX_GPIO_EXTINT);
wius_gpio_ulp_t afetxmux_pin = WIUS_GPIO_ULP_OUTPUT(TP_MUX_GPIO_AFETX);

void tp_mux_init(void)
{
    wius_gpio_ulp_init();
    wius_gpio_uulp_init();
    wius_gpio_ulp_pin_config(&afetxmux_pin);
    wius_gpio_uulp_pin_config(&extintmux_pin);
}

void tp_mux_select(tp_mux_t mux)
{
//    wius_gpio_ulp_pin_set(afetxmux_pin, mux & 0b01);
//    wius_gpio_uulp_pin_set(extintmux_pin, (mux & 0b10) >> 1);
  switch(mux)
  {
    case TP_MUX_PLL:
      wius_gpio_ulp_pin_set(afetxmux_pin, false);
      wius_gpio_uulp_pin_set(extintmux_pin, false);
      break;
    case TP_MUX_FPGA:
      wius_gpio_ulp_pin_set(afetxmux_pin, true);
      wius_gpio_uulp_pin_set(extintmux_pin, false);
      break;
    case TP_MUX_AFE:
      wius_gpio_ulp_pin_set(afetxmux_pin, false);
      wius_gpio_uulp_pin_set(extintmux_pin, true);
      break;
    case TP_MUX_TX:
      wius_gpio_ulp_pin_set(afetxmux_pin, true);
      wius_gpio_uulp_pin_set(extintmux_pin, true);
      break;
    default:
      break;
  }
}
