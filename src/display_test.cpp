#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include "char_framebuff.hpp"

#define DISPLAY0_ALIAS readings_display

int main(void) {
	char_framebuff::ErrorCode error_code = char_framebuff::ErrorCode::Ok;
	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(red_led), gpios);

	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static char_framebuff::CharFramebuff cb0{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};

	error_code = cb0.screen_clear();

	if (error_code != char_framebuff::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = cb0.string_load("UPCOMING", 0, 2);

	if (error_code != char_framebuff::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = cb0.string_load("PROJECT!", 1, 2);

	if (error_code != char_framebuff::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = cb0.string_load("STAY TUNED:)", 3, 0);

	if (error_code != char_framebuff::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);
	}

	error_code = cb0.screen_print();

	if (error_code != char_framebuff::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while(1) {}
	}

	error_code = cb0.string_load("ERROR TRIGGETING", 0, 0);

	if (error_code != char_framebuff::ErrorCode::Param) {
		gpio_pin_toggle_dt(&error_led);

		while(1) {}
	}

	error_code = cb0.font_set(99);

	if (error_code != char_framebuff::ErrorCode::Param) {
		gpio_pin_toggle_dt(&error_led);

		while(1) {}
	}

	while (1) {}

	return 0;
}
