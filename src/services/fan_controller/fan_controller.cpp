#include "fan_controller.hpp"
#include "pwm_generator_interface.hpp"
#include "pulse_reader_interface.hpp"

fan_controller::FourWireFan::FourWireFan(
	pwm_generator_interface::PwmSignalInterface& fan_pwm,
	pulse_reader_interface::PulseSignalInterface& fan_tachometer
) :
pwm{fan_pwm}, tachometer{fan_tachometer} {

	if (this->pwm.signal.error_state_get().code != pwm_generator_interface::ErrorCode::Ok) {
		this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::PwmUnready, fan_controller::ErrorCode::Ok};
		return;
	}

	this->system.pwm_ready = true;

	if (this->tachometer.signal.error_state_get().code != pulse_reader_interface::ErrorCode::Ok) {
		this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::TachometerUnready};
		return;
	}

	this->system.tachometer_ready = true;
	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
}

fan_controller::ErrorState fan_controller::FourWireFan::boot() {

	if (!this->system.pwm_ready || !this->system.tachometer_ready) {
		this->error = {fan_controller::ErrorCode::FanUnready, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (this->system.pwm_running && this->system.tachometer_running) {
		this->error = {fan_controller::ErrorCode::FanRunning, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (!this->system.pwm_running) {

		if (this->pwm.signal.start(PERIOD_NS_FOR_25KHZ, PERIOD_NS_FOR_25KHZ / 2) != pwm_generator_interface::ErrorCode::Ok) {
			this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::PwmStart, fan_controller::ErrorCode::Ok};
			return this->error;
		}

		this->pwm.period_ns = PERIOD_NS_FOR_25KHZ;
		this->pwm.duty_cycle_x100 = 50;
		this->system.pwm_running = true;
	}

	if (!this->system.tachometer_running) {

		if (this->tachometer.signal.capture_start() != pulse_reader_interface::ErrorCode::Ok) {
			this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::TachometerReadingStart};
			return this->error;
		}

		this->system.tachometer_running = true;
	}

	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
	return this->error;
}

fan_controller::ErrorState fan_controller::FourWireFan::stop() {

	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};

	if (this->system.pwm_running) {

		if (this->pwm.signal.stop() != pwm_generator_interface::ErrorCode::Ok) {
			this->error.general = fan_controller::ErrorCode::SpecificError;
			this->error.pwm = fan_controller::ErrorCode::PwmStop;
		}

		else {
			this->system.pwm_running = false;
		}
	}

	if (this->system.tachometer_running) {

		if (this->tachometer.signal.capture_stop() != pulse_reader_interface::ErrorCode::Ok) {
			this->error.general = fan_controller::ErrorCode::SpecificError;
			this->error.tachometer = fan_controller::ErrorCode::TachometerReadingStop;
		}

		else {
			this->system.tachometer_running = false;
		}
	}

	return this->error;
}

fan_controller::ErrorState fan_controller::FourWireFan::speed_measure(unsigned int& measured_speed_rpm) {
	unsigned long long int tachometer_period_ns = 0;

	if (!(this->system.pwm_running && this->system.tachometer_running)) {
		this->error = {fan_controller::ErrorCode::FanNotRunning, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (this->tachometer.signal.capture_period_ns_get(tachometer_period_ns) != pulse_reader_interface::ErrorCode::Ok) {
		this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::TachometerReadingCapture};
		return this->error;
	}

	this->tachometer.speed_rpm = static_cast<unsigned int>(60000000000ULL / (tachometer_period_ns * 2));
	measured_speed_rpm = this->tachometer.speed_rpm;

	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
 	return this->error;
}

fan_controller::ErrorState fan_controller::FourWireFan::duty_cycle_update(unsigned int duty_cycle_x100) {

	if (!(this->system.pwm_running && this->system.tachometer_running)) {
		this->error = {fan_controller::ErrorCode::FanNotRunning, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (duty_cycle_x100 > 100) {
		this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::ParamDutyCycle, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (this->pwm.signal.start(PERIOD_NS_FOR_25KHZ, (this->pwm.period_ns * duty_cycle_x100) / 100) != pwm_generator_interface::ErrorCode::Ok) {
		this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::PwmStart, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	this->pwm.duty_cycle_x100 = duty_cycle_x100;

	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
	return this->error;
}

fan_controller::ErrorState fan_controller::FourWireFan::error_state_get() const {
	return this->error;
}
