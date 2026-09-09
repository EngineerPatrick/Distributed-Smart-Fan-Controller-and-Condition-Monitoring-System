/*
*
*	Composition Root
*
*/

#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include "monochrome_display.hpp"

#define DISPLAY0_ALIAS readings_display

int main(void) {
	monochrome_display::ErrorCode error_code = monochrome_display::ErrorCode::Ok;
	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(red_led), gpios);

	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static monochrome_display::MonochromeDisplay mc0{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};

	error_code = mc0.screen_clear();

	if (error_code != monochrome_display::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = mc0.screen_string_print("UPCOMING", 0, 2);

	if (error_code != monochrome_display::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = mc0.screen_string_print("PROJECT!", 1, 2);

	if (error_code != monochrome_display::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = mc0.screen_string_print("STAY TUNED:)", 3, 0);

	if (error_code != monochrome_display::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);
	}

	while (1) {}

	return 0;
}
