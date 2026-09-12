/*
*
*	Composition Root
*
*/

#include "monochrome_display.hpp"
#include "ui_display.hpp"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#define DISPLAY0_ALIAS readings_display

int main(void) {
	ui_display::ErrorCode error_code = ui_display::ErrorCode::Ok;
	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(error_led), gpios);
	uint32_t temp = 123;

	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static monochrome_display::MonochromeDisplay mc0{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};

	error_code = ui_display::units_write(mc0);

	if (error_code != ui_display::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = ui_display::temp_write(mc0, temp);

	if (error_code != ui_display::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	while (1) {}

	return 0;
}
