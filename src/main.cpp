/*
*
*	Composition Root
*
*/

#include "monochrome_display.hpp"
#include "ui_display.hpp"
#include "temperature_sensor.hpp"
#include <cstdint>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#define DISPLAY0_ALIAS readings_display
#define SENSOR0_ALIAS temp_sensor

SENSOR_DT_READ_IODEV(iodev, DT_ALIAS(temp_sensor), {SENSOR_CHAN_AMBIENT_TEMP, 0});
RTIO_DEFINE(ctx, 1, 1);

int main(void) {
	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(error_led), gpios);
	std::int32_t temp = 0;
	ui_display::ErrorCode ui_display_error_code = ui_display::ErrorCode::Ok;
	temperature_sensor::ErrorState temperature_sensor_error_state = {temperature_sensor::ErrorCode::Ok, 0};

	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static monochrome_display::MonochromeDisplay mc0{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};
	static temperature_sensor::TemperatureSensor bme{DEVICE_DT_GET(DT_ALIAS(SENSOR0_ALIAS)), &iodev, &ctx};

	temperature_sensor_error_state = bme.error_state_get();

	if (temperature_sensor_error_state.code != temperature_sensor::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	ui_display_error_code = ui_display::units_write(mc0);

	if (ui_display_error_code != ui_display::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	while (1) {

		temperature_sensor_error_state.code = bme.temp_read(temp);

		if (temperature_sensor_error_state.code != temperature_sensor::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		ui_display_error_code = ui_display::temp_write(mc0, temp);

		if (ui_display_error_code != ui_display::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

	}

	while (1) {}

	return 0;
}
