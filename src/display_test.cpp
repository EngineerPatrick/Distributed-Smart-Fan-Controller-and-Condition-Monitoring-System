#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include "char_framebuff.hpp"

#define DISPLAY0_ALIAS readings_display

static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(red_led), gpios);

int main(void) {
	DisplayErrors error_code = DisplayErrors::Ok;

	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static MonochromeDisplay readings_display{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};

	error_code = readings_display.screen_clear();

	if (error_code != DisplayErrors::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = readings_display.string_load("UPCOMING", 0, 2);

	if (error_code != DisplayErrors::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = readings_display.string_load("PROJECT!", 1, 2);

	if (error_code != DisplayErrors::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = readings_display.string_load("STAY TUNED:)", 3, 0);

	if (error_code != DisplayErrors::Ok) {
		gpio_pin_toggle_dt(&error_led);
	}

	error_code = readings_display.screen_print();

	if (error_code != DisplayErrors::Ok) {
		gpio_pin_toggle_dt(&error_led);
	}

	while (1) {}

	return 0;
}
