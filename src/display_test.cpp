#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include "display.hpp"

static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(red_led), gpios);

int main(void) {
	display_errors error_code = DISPLAY_ERR_OK;

	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static monochrome_display_controller readings_display_controller{DEVICE_DT_GET(DT_ALIAS(readings_display)), "readings_display"};

	error_code = readings_display_controller.display.screen_clear();

	if (error_code != DISPLAY_ERR_OK) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = readings_display_controller.display.string_print("UPCOMING", 0, 2);

	if (error_code != DISPLAY_ERR_OK) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = readings_display_controller.display.string_print("PROJECT!", 1, 2);

	if (error_code != DISPLAY_ERR_OK) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = readings_display_controller.display.string_print("STAY TUNED:)", 3, 0);

	if (error_code != DISPLAY_ERR_OK) {
		gpio_pin_toggle_dt(&error_led);
	}

	while (1) {}

	return 0;
}
