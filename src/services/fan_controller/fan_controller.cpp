#include "fan_controller.hpp"
#include "pwm_controller_interface.hpp"
#include "input_capture_controller_interface.hpp"
#include <cstdint>

fan_controller::FourWireFan::FourWireFan(
	pwm_controller_interface::PwmSignalInterface& fan_pwm,
	input_capture_controller_interface::InputCaptureSignalInterface& fan_tachometer
) :
pwm{fan_pwm}, tachometer{fan_tachometer} {

	if (this->pwm.signal.error_state_get().code != pwm_controller_interface::ErrorCode::Ok) {
		this->error = fan_controller::ErrorCode::PwmUnready;
		return;
	}

	if (this->tachometer.signal.error_state_get().code != input_capture_controller_interface::ErrorCode::Ok) {
		this->error = fan_controller::ErrorCode::TachometerUnready;
		return;
	}

	this->system.fan_ready = true;
	this->error = fan_controller::ErrorCode::Ok;
}

fan_controller::ErrorCode fan_controller::FourWireFan::boot() {

	if (!this->system.fan_ready) {
		this->error = fan_controller::ErrorCode::FanUnready;
		return this->error;
	}

	if (this->system.fan_running) {

		if (this->stop() != fan_controller::ErrorCode::Ok) {
			return this->error;
		}
	}

	if (this->pwm.signal.start(PERIOD_NS_FOR_25KHZ, PERIOD_NS_FOR_25KHZ / 2) != pwm_controller_interface::ErrorCode::Ok) {
		this->error = fan_controller::ErrorCode::PwmStart;
		return this->error;
	}

	this->pwm.period_ns = PERIOD_NS_FOR_25KHZ;
	this->pwm.duty_cycle_x100 = PERIOD_NS_FOR_25KHZ / 2;

	if (this->tachometer.signal.capture_start() != input_capture_controller_interface::ErrorCode::Ok) {
		this->error = fan_controller::ErrorCode::TachometerReadingStart;
		return this->error;
	}

	this->system.fan_running = true;
	this->error = fan_controller::ErrorCode::Ok;
	return this->error;
}

fan_controller::ErrorCode fan_controller::FourWireFan::stop() {

	if (!this->system.fan_running) {
		this->error = fan_controller::ErrorCode::FanNotRunning;
		return this->error;
	}

	if (this->pwm.signal.stop() != pwm_controller_interface::ErrorCode::Ok) {
		this->error = fan_controller::ErrorCode::PwmStop;
		return this->error;
	}

	if (this->tachometer.signal.capture_stop() != input_capture_controller_interface::ErrorCode::Ok) {
		this->error = fan_controller::ErrorCode::TachometerReadingStop;
		return this->error;
	}

	this->system.fan_running = false;
	this->error = fan_controller::ErrorCode::Ok;
	return this->error;
}

fan_controller::ErrorCode fan_controller::FourWireFan::speed_measure(unsigned int& measured_speed_rpm) {
	std::uint64_t tachometer_period_ns = 0;

	if (!this->system.fan_running) {
		this->error = fan_controller::ErrorCode::FanNotRunning;
		return this->error;
	}

	if (this->tachometer.signal.capture_period_ns_get(tachometer_period_ns) != input_capture_controller_interface::ErrorCode::Ok) {
		this->error = fan_controller::ErrorCode::TachometerReadingCapture;
		return this->error;
	}

	this->tachometer.speed_rpm = static_cast<std::uint16_t>(60000000000ULL / (tachometer_period_ns * 2));
	measured_speed_rpm = this->tachometer.speed_rpm;

	this->error = fan_controller::ErrorCode::Ok;
	return this->error;
}

fan_controller::ErrorCode fan_controller::FourWireFan::duty_cycle_update(unsigned int duty_cycle_x100) {

	if (!this->system.fan_running) {
		this->error = fan_controller::ErrorCode::FanNotRunning;
		return this->error;
	}

	if (this->pwm.signal.start(PERIOD_NS_FOR_25KHZ, (this->pwm.period_ns * duty_cycle_x100) / 100) != pwm_controller_interface::ErrorCode::Ok) {
		this->error = fan_controller::ErrorCode::PwmStart;
		return this->error;
	}

	this->pwm.duty_cycle_x100 = duty_cycle_x100;

	this->error = fan_controller::ErrorCode::Ok;
	return this->error;
}

fan_controller::ErrorCode fan_controller::FourWireFan::error_get() const {
	return this->error;
}
