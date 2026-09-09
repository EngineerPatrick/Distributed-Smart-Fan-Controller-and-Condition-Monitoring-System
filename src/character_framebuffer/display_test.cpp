#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include "character_framebuffer.hpp"

#define DISPLAY0_ALIAS readings_display

int main(void) {
	character_framebuffer::ErrorCode error_code = character_framebuffer::ErrorCode::Ok;
	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(red_led), gpios);

	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static character_framebuffer::CharacterFramebuffer cb0{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};

	error_code = cb0.ram_clear();

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = cb0.ram_string_write("UPCOMING", 0 * 16, 2 * 10);

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = cb0.ram_string_write("PROJECT!", 1 * 16, 2 * 10);

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = cb0.ram_string_write("STAY TUNED:)", 3 * 16, 0 * 10);

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);
	}

	error_code = cb0.ram_flush();

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while(1) {}
	}

	error_code = cb0.ram_string_write("ERROR TRIGGERING", 0, 0);

	if (error_code != character_framebuffer::ErrorCode::ParamStringLength) {
		gpio_pin_toggle_dt(&error_led);

		while(1) {}
	}

	error_code = cb0.font_set(99);

	if (error_code != character_framebuffer::ErrorCode::ParamFontIndex) {
		gpio_pin_toggle_dt(&error_led);

		while(1) {}
	}

	while (1) {}

	return 0;
}
