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

	error_code = cb0.string_load("UPCOMING", 0, 2);

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = cb0.string_load("PROJECT!", 1, 2);

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	error_code = cb0.string_load("STAY TUNED:)", 3, 0);

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);
	}

	error_code = cb0.ram_write();

	if (error_code != character_framebuffer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while(1) {}
	}

	error_code = cb0.string_load("ERROR TRIGGETING", 0, 0);

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
