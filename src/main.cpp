/*
*
*	Composition Root
*
*/

#include "grid_printer.hpp"
#include "display_ui.hpp"
#include "temperature_sensor.hpp"
#include "pwm.hpp"
#include "counter_capture.hpp"
#include "input_capture.hpp"
#include <cstdint>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/counter/stm32.h>

#define DISPLAY0_ALIAS readings_display
#define SENSOR0_ALIAS temp_sensor
#define FAN0_ALIAS fan_pwm

#define COUNTER_CAPTURE_ADDITIONAL_FLAGS (COUNTER_CAPTURE_STM32_PRESCALER_DIV1 | COUNTER_CAPTURE_STM32_FILTER_DTS_DIV2_N6)

DEFINE_TEMPERATURE_SENSOR(SENSOR0_ALIAS)

int main(void) {
	std::int16_t temp_c_x100 = 0;
	std::uint16_t speed_rpm = 0;

	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(error_led), gpios);
	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static grid_printer::GridPrinter mc0{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};

	if (mc0.error_get() != grid_printer::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	static temperature_sensor::TemperatureSensor bme{TEMPERATURE_SENSOR_DEVICE(SENSOR0_ALIAS)};

	if (bme.error_state_get().code != temperature_sensor::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	static pwm::PwmSignal control_fan{PWM_DT_SPEC_GET(DT_ALIAS(FAN0_ALIAS))};

	if (control_fan.error_state_get().code != pwm::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	static input_capture::InputCaptureSignal fan_tach{COUNTER_CAPTURE_TIMER(FAN0_ALIAS), COUNTER_CAPTURE_ADDITIONAL_FLAGS};

	if (fan_tach.error_get() != input_capture::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	if (display_ui::fixed_ui_print(mc0) != display_ui::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	if (control_fan.start(PERIOD_NS_FOR_25KHZ, 5000) != pwm::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	std::uint64_t fan_tach_period_ns = 0;

	while (1) {

		if (bme.temp_read(temp_c_x100) != temperature_sensor::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		if (fan_tach.period_ns_get(fan_tach_period_ns) != input_capture::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		speed_rpm = static_cast<std::uint16_t>(60000000000ULL / (fan_tach_period_ns * 2));

		if (display_ui::temp_value_print(mc0, (temp_c_x100 / 10)) != display_ui::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		if (display_ui::speed_value_print(mc0, speed_rpm) != display_ui::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}
	}

	while (1) {}

	return 0;
}
