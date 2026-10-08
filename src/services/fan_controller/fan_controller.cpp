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

	this->system.pwm_acquired = true;

	if (this->tachometer.signal.error_state_get().code != pulse_reader_interface::ErrorCode::Ok) {
		this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::TachometerUnready};
		return;
	}

	this->system.tachometer_acquired = true;
	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
}

fan_controller::ErrorState fan_controller::FourWireFan::boot() {

	if (!this->system.pwm_acquired || !this->system.tachometer_acquired) {
		this->error = {fan_controller::ErrorCode::FanUnready, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (this->system.pwm_ready && this->system.tachometer_ready) {
		this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (!this->system.pwm_ready) {

		if (this->pwm.signal.start(PERIOD_NS_FOR_25KHZ, PERIOD_NS_FOR_25KHZ / 2) != pwm_generator_interface::ErrorCode::Ok) {
			this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::PwmStart, fan_controller::ErrorCode::Ok};
			return this->error;
		}

		this->pwm.period_ns = PERIOD_NS_FOR_25KHZ;
		this->pwm.duty_cycle_x100 = 50;
		this->system.pwm_ready = true;
	}

	if (!this->system.tachometer_ready) {

		if (this->tachometer.signal.capture_start() != pulse_reader_interface::ErrorCode::Ok) {
			this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::TachometerStart};
			return this->error;
		}
	}

	this->system.tachometer_ready = true;
	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
	return this->error;
}

fan_controller::ErrorState fan_controller::FourWireFan::stop() {

	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};

	if (this->system.pwm_ready) {

		if (this->pwm.signal.stop() != pwm_generator_interface::ErrorCode::Ok) {
			this->error.general = fan_controller::ErrorCode::SpecificError;
			this->error.pwm = fan_controller::ErrorCode::PwmStop;
		}

		else {
			this->pwm.duty_cycle_x100 = 0;
			this->pwm.period_ns = 0;
			this->system.pwm_ready = false;
		}
	}

	if (this->system.tachometer_ready) {

		if (this->tachometer.signal.capture_stop() != pulse_reader_interface::ErrorCode::Ok) {
			this->error.general = fan_controller::ErrorCode::SpecificError;
			this->error.tachometer = fan_controller::ErrorCode::TachometerStop;
		}

		else {
			this->tachometer.speed_rpm = 0;
			this->system.tachometer_ready = false;
		}
	}

	return this->error;
}

fan_controller::ErrorState fan_controller::FourWireFan::speed_measure(unsigned int& measured_speed_rpm) {
	unsigned long long int tachometer_period_ns = 0;

	if (!(this->system.pwm_ready && this->system.tachometer_ready)) {
		this->error = {fan_controller::ErrorCode::SpeedMeasureUnready, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (this->tachometer.signal.capture_period_ns_get(tachometer_period_ns) != pulse_reader_interface::ErrorCode::Ok || !tachometer_period_ns) {
		this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::TachometerCapture};
		return this->error;
	}

	this->tachometer.speed_rpm = static_cast<unsigned int>(60000000000ULL / (tachometer_period_ns * 2));
	measured_speed_rpm = this->tachometer.speed_rpm;

	this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
 	return this->error;
}

fan_controller::ErrorState fan_controller::FourWireFan::duty_cycle_update(unsigned int duty_cycle_x100) {

	if (!(this->system.pwm_ready && this->system.tachometer_ready)) {
		this->error = {fan_controller::ErrorCode::DutyCycleUpdateUnready, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (duty_cycle_x100 > 100) {
		this->error = {fan_controller::ErrorCode::SpecificError, fan_controller::ErrorCode::ParamDutyCycle, fan_controller::ErrorCode::Ok};
		return this->error;
	}

	if (this->pwm.duty_cycle_x100 == duty_cycle_x100) {
		this->error = {fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok, fan_controller::ErrorCode::Ok};
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

void fan_controller::FourWireFan::params_get(unsigned long long int& period_ns, unsigned int duty_cycle_x100, unsigned int& speed_rpm) const {
	period_ns = this->pwm.period_ns;
	duty_cycle_x100 = this->pwm.duty_cycle_x100;
	speed_rpm = this->tachometer.speed_rpm;
}

fan_controller::ErrorState fan_controller::FourWireFan::error_state_get() const {
	return this->error;
}
