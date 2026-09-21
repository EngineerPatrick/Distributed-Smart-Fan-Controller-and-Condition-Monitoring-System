/*
*
*	Composition Root
*
*/

#include "monochrome_display.hpp"
#include "display_ui.hpp"
#include "temperature_sensor.hpp"
#include "pwm.hpp"
#include "input_capture.hpp"
#include <cstdint>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>

#define DISPLAY0_ALIAS readings_display
#define SENSOR0_ALIAS temp_sensor
#define FAN0_ALIAS fan_pwm

DEFINE_TEMPERATURE_SENSOR(SENSOR0_ALIAS)
DEFINE_INPUT_CAPTURE(FAN0_ALIAS)

int main(void) {
	std::int16_t temp_c_x100 = 0;
	std::uint16_t speed_rpm = 0;

	display_ui::ErrorCode display_ui_error_code = display_ui::ErrorCode::Ok;
	temperature_sensor::ErrorState temperature_sensor_error_state = {temperature_sensor::ErrorCode::Ok, 0};
	pwm::ErrorCode pwm_error_code = pwm::ErrorCode::Ok;
	input_capture::ErrorCode input_capture_error_code = input_capture::ErrorCode::Ok;

	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(error_led), gpios);
	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static monochrome_display::MonochromeDisplay mc0{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};
	static temperature_sensor::TemperatureSensor bme{TEMPERATURE_SENSOR_DEVICE(SENSOR0_ALIAS)};
	static pwm::PwmSignal control_fan{PWM_DT_SPEC_GET(DT_ALIAS(FAN0_ALIAS))};
	static input_capture::InputCaptureSignal fan_tach{INPUT_CAPTURE_TIMER(FAN0_ALIAS)};

	display_ui_error_code = display_ui::fixed_ui_print(mc0);

	if (display_ui_error_code != display_ui::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	pwm_error_code = control_fan.start(PERIOD_NS_FOR_25KHZ, PULSE_NS_FOR_HALF_DC);

	if (pwm_error_code != pwm::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	std::uint32_t capture_period_ns = 0;

	while (1) {

		temperature_sensor_error_state.code = bme.temp_read(temp_c_x100);

		if (temperature_sensor_error_state.code != temperature_sensor::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		input_capture_error_code = fan_tach.capture_period_ns_get(capture_period_ns);

		if (input_capture_error_code != input_capture::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		speed_rpm = static_cast<std::uint16_t>(60000000000UL / static_cast<std::uint64_t>(capture_period_ns) * 2);

		display_ui_error_code = display_ui::temp_value_print(mc0, (temp_c_x100 / 10));

		if (display_ui_error_code != display_ui::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		display_ui_error_code = display_ui::speed_value_print(mc0, speed_rpm);

		if (display_ui_error_code != display_ui::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

	}

	while (1) {}

	return 0;
}
