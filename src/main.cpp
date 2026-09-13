/*
*
*	Composition Root
*
*/

#include "monochrome_display.hpp"
#include "display_ui.hpp"
#include "temperature_sensor.hpp"
#include <cstdint>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#define DISPLAY0_ALIAS readings_display
#define SENSOR0_ALIAS temp_sensor

DEFINE_TEMPERATURE_SENSOR(SENSOR0_ALIAS)

int main(void) {
	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(error_led), gpios);
	std::int16_t temp_c_x100 = 0;
	display_ui::ErrorCode display_ui_error_code = display_ui::ErrorCode::Ok;
	temperature_sensor::ErrorState temperature_sensor_error_state = {temperature_sensor::ErrorCode::Ok, 0};

	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static monochrome_display::MonochromeDisplay mc0{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};
	static temperature_sensor::TemperatureSensor bme{TEMPERATURE_SENSOR_DEVICE(SENSOR0_ALIAS)};

	temperature_sensor_error_state = bme.error_state_get();

	if (temperature_sensor_error_state.code != temperature_sensor::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	display_ui_error_code = display_ui::fixed_ui_print(mc0);

	if (display_ui_error_code != display_ui::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	while (1) {

		temperature_sensor_error_state.code = bme.temp_read(temp_c_x100);

		if (temperature_sensor_error_state.code != temperature_sensor::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		display_ui_error_code = display_ui::temp_value_print(mc0, (temp_c_x100 / 10));

		if (display_ui_error_code != display_ui::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

	}

	while (1) {}

	return 0;
}
