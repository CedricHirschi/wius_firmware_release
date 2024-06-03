/**
 * @file gpio.c
 *
 * @brief GPIO module implementation for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 05.04.2024
 *
 * @ingroup wius
 *
 */

#include "gpio.h"

#include "rsi_rom_clks.h"
#include "sl_si91x_gpio.h"
#include "sl_si91x_peripheral_gpio.h"
#include "sl_gpio_board.h"

#define MAX_PIN_INTERRUPTS 8

#define GENERATE_PIN_IRQ_HANDLER(IRQ_NUM)             \
	void PIN_IRQ##IRQ_NUM##_Handler(void)             \
	{                                                 \
		sl_gpio_clear_interrupts(PIN_INTR_##IRQ_NUM); \
		if (interrupt_functions[IRQ_NUM] != NULL)     \
			interrupt_functions[IRQ_NUM]();           \
	}

size_t active_interrupts = 0;
void (*interrupt_functions[MAX_PIN_INTERRUPTS])(void) = {NULL};

void wius_gpio_init(void)
{
	sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)M4CLK_GPIO);

	active_interrupts = 0;
	for (uint8_t i = 0; i < MAX_PIN_INTERRUPTS; i++)
	{
		NVIC_EnableIRQ(PININT0_NVIC_NAME + i);
	}
}

void wius_gpio_pin_config(wius_gpio_t *gpio)
{

	// EXPERIMENTAL: pin indexing error fix
	gpio->_port_pin = gpio->pin - gpio->port * MAX_GPIO_PORT_PIN;

	if (gpio->dir == WIUS_GPIO_DIR_OUTPUT)
	{
    sl_si91x_gpio_enable_pad_selection(gpio->pad);
    sl_si91x_gpio_enable_pad_receiver(gpio->pin);

		sl_gpio_set_pin_mode(gpio->port, gpio->_port_pin, EGPIO_PIN_MUX_MODE0, 0);
		sl_si91x_gpio_set_pin_direction(gpio->port, gpio->_port_pin, EGPIO_CONFIG_DIR_OUTPUT);
	}
	else
	{
    sl_si91x_gpio_enable_pad_selection(gpio->pad);
    sl_si91x_gpio_enable_pad_receiver(gpio->pin);

		sl_si91x_gpio_select_pad_driver_strength(gpio->pin, (sl_si91x_gpio_driver_strength_select_t)GPIO_TWO_MILLI_AMPS);
		sl_si91x_gpio_select_pad_driver_disable_state(gpio->pin, (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);

		sl_gpio_set_pin_mode(gpio->port, gpio->_port_pin, EGPIO_PIN_MUX_MODE0, 1);
		sl_si91x_gpio_set_pin_direction(gpio->port, gpio->_port_pin, EGPIO_CONFIG_DIR_INPUT);
	}
}

sl_status_t wius_gpio_pin_attach_interrupt(wius_gpio_t gpio, wius_gpio_interrupt_t trigger, void (*function)(void))
{
	if (gpio.dir != WIUS_GPIO_DIR_INPUT || active_interrupts > MAX_PIN_INTERRUPTS - 1)
		return SL_STATUS_INVALID_STATE;

	sl_gpio_configure_interrupt(gpio.port, gpio._port_pin, active_interrupts, (sl_gpio_interrupt_flag_t)trigger);

	interrupt_functions[active_interrupts] = function;

	NVIC_EnableIRQ(PININT0_NVIC_NAME + active_interrupts);
	NVIC_SetPriority(PININT0_NVIC_NAME + active_interrupts, 7);

	active_interrupts++;

	return SL_STATUS_OK;
}

void wius_gpio_pin_set(wius_gpio_t gpio, bool on)
{
	on ? sl_gpio_set_pin_output(gpio.port, gpio._port_pin) : sl_gpio_clear_pin_output(gpio.port, gpio._port_pin);
}

void wius_gpio_pin_toggle(wius_gpio_t gpio)
{
	wius_gpio_pin_set(gpio, !wius_gpio_pin_get(gpio));
}

uint8_t wius_gpio_pin_get(wius_gpio_t gpio)
{
	return sl_gpio_get_pin_input(gpio.port, gpio._port_pin);
}

GENERATE_PIN_IRQ_HANDLER(0)
GENERATE_PIN_IRQ_HANDLER(1)
GENERATE_PIN_IRQ_HANDLER(2)
GENERATE_PIN_IRQ_HANDLER(3)
GENERATE_PIN_IRQ_HANDLER(4)
GENERATE_PIN_IRQ_HANDLER(5)
GENERATE_PIN_IRQ_HANDLER(6)
GENERATE_PIN_IRQ_HANDLER(7)
