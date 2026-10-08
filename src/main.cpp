/*
*
*	Composition Root
*
*/

#include "grid_printer.hpp"
#include "grid_printer_interface.hpp"

#include "readings_reporter.hpp"

#include "temperature_reader.hpp"
#include "temperature_reader_interface.hpp"

#include "pwm_generator.hpp"
#include "pwm_generator_interface.hpp"

#include "pulse_reader.hpp"
#include "pulse_reader_interface.hpp"

#include "fan_controller.hpp"

#include "fan_curve.hpp"

#include "control_thread.hpp"

#include <cstdint>
#include <array>

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
	static const struct gpio_dt_spec error_led = GPIO_DT_SPEC_GET(DT_ALIAS(error_led), gpios);
	gpio_pin_configure_dt(&error_led, GPIO_OUTPUT_ACTIVE);


	static grid_printer::DisplayGrid dashboard_grid{DEVICE_DT_GET(DT_ALIAS(DISPLAY0_ALIAS))};

	static temperature_reader::TemperatureSignal room_temperature{TEMPERATURE_SENSOR_DEVICE(SENSOR0_ALIAS)};

	static pwm_generator::PwmSignal fan_pwm{PWM_DT_SPEC_GET(DT_ALIAS(FAN0_ALIAS))};

	static pulse_reader::PulseSignal fan_tachometer{INPUT_CAPTURE_TIMER_DEVICE(FAN0_ALIAS), COUNTER_CAPTURE_ADDITIONAL_FLAGS};

	static fan_controller::FourWireFan arctic_p12_max{fan_pwm, fan_tachometer};

	static readings_reporter::Dashboard controller_state{dashboard_grid};

	std::array<fan_curve::Node, 10> sample_nodes{{
		{250, 500}, {280, 1300}, {300, 2000}, {301, 2000}, {302, 2000},
		{303, 2000}, {304, 2000}, {305, 2000}, {306, 2000}, {307, 2000}
	}};

	static fan_curve::FanCurve sample_curve{sample_nodes};

	static control_thread::ControlThread feedback_controller{arctic_p12_max, room_temperature, sample_curve};

	while (1) {}

	return 0;
}
