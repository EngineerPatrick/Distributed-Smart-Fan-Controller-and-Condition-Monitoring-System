/*
*
*	Composition Root
*
*/

#include "grid_printer.hpp"
#include "grid_printer_interface.hpp"
#include "display_ui.hpp"
#include "temperature_reader.hpp"
#include "temperature_reader_interface.hpp"
#include "pwm_controller.hpp"
#include "pwm_controller_interface.hpp"
#include "input_capture_controller.hpp"
#include "input_capture_controller_interface.hpp"
#include "fan_controller.hpp"
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
#define SENSOR0_ALIAS room_temp_sensor
#define FAN0_ALIAS control_fan

#define COUNTER_CAPTURE_ADDITIONAL_FLAGS (COUNTER_CAPTURE_STM32_PRESCALER_DIV1 | COUNTER_CAPTURE_STM32_FILTER_DTS_DIV2_N6)

DEFINE_TEMPERATURE_SENSOR(SENSOR0_ALIAS)

int main(void) {
	int temp_c_x100 = 0;
	unsigned int speed_rpm = 0;

	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(error_led), gpios);
	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);

	static grid_printer::DisplayGrid readings_grid{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};

	if (readings_grid.error_state_get().code != grid_printer_interface::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	static temperature_reader::TemperatureSignal room_temp{TEMPERATURE_SENSOR_DEVICE(SENSOR0_ALIAS)};

	if (room_temp.error_state_get().code != temperature_reader_interface::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	static pwm_controller::PwmSignal fan_pwm{PWM_DT_SPEC_GET(DT_ALIAS(FAN0_ALIAS))};

	if (fan_pwm.error_state_get().code != pwm_controller_interface::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	static input_capture_controller::InputCaptureSignal fan_tach{INPUT_CAPTURE_TIMER_DEVICE(FAN0_ALIAS), COUNTER_CAPTURE_ADDITIONAL_FLAGS};

	if (fan_tach.error_state_get().code != input_capture_controller_interface::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	static fan_controller::FourWireFan arctic_p12_max{fan_pwm, fan_tach};

	if (arctic_p12_max.error_get() != fan_controller::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	if (display_ui::fixed_ui_print(readings_grid) != display_ui::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	if (arctic_p12_max.boot() != fan_controller::ErrorCode::Ok) {
		gpio_pin_toggle_dt(&error_led);

		while (1) {}
	}

	while (1) {

		if (room_temp.value_read(temp_c_x100) != temperature_reader_interface::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		if (arctic_p12_max.speed_measure(speed_rpm) != fan_controller::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		if (display_ui::temp_value_print(readings_grid, (temp_c_x100 / 10)) != display_ui::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}

		if (display_ui::speed_value_print(readings_grid, speed_rpm) != display_ui::ErrorCode::Ok) {
			gpio_pin_toggle_dt(&error_led);

			while (1) {}
		}
	}

	while (1) {}

	return 0;
}
