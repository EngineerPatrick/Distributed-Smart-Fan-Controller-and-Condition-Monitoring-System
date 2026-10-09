/**
*
*	@file		pwm_generator.cpp
*
*	@brief		Implementation for the pwm module
*
*	@details	Acquires Zephyr's PWM resource, keeps track of its return
*				values and performs start/stop operations
*
*				Supports waveform parameters setting and getting
*
*/

#include "pwm_generator.hpp"
#include "pwm_generator_interface.hpp"
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

pwm_generator::PwmSignal::PwmSignal (struct pwm_dt_spec timer_device) :
timer{timer_device} {

	if (!pwm_is_ready_dt(&(this->timer))) {
		this->error = {pwm_generator_interface::ErrorCode::ZDeviceUnready, 0};
		return;
	}

	this->system.pwm_acquired = true;
	this->error = {pwm_generator_interface::ErrorCode::Ok, 0};
}

pwm_generator_interface::ErrorCode pwm_generator::PwmSignal::set(unsigned long long int waveform_period_ns, unsigned long long int waveform_pulse_width_ns) {

	if (!this->system.pwm_acquired) {
		this->error = {pwm_generator_interface::ErrorCode::PwmUnready, 0};
		return this->error.code;
	}

	if (!waveform_period_ns || waveform_period_ns < waveform_pulse_width_ns) {
		this->error = {pwm_generator_interface::ErrorCode::ParamWaveform, 0};
		return this->error.code;
	}

	this->error.return_value = pwm_set_dt(&(this->timer), waveform_period_ns, waveform_pulse_width_ns);

	if (this->error.return_value != 0) {
		this->error.code = pwm_generator_interface::ErrorCode::ZPwmSet;
		return this->error.code;
	}

	this->waveform = {waveform_period_ns, waveform_pulse_width_ns};
	this->system.pwm_ready = true;
	this->error = {pwm_generator_interface::ErrorCode::Ok, 0};
	return this->error.code;
}

pwm_generator_interface::ErrorCode pwm_generator::PwmSignal::stop() {

	if (!this->system.pwm_ready) {
		this->error = {pwm_generator_interface::ErrorCode::Ok, 0};
		return this->error.code;
	}

	this->error.return_value = pwm_set_pulse_dt(&(this->timer), 0);

	if (this->error.return_value != 0) {
		this->error.code = pwm_generator_interface::ErrorCode::ZPwmSet;
		return this->error.code;
	}

	this->waveform.pulse_width_ns = 0;
	this->system.pwm_ready = false;
	this->error = {pwm_generator_interface::ErrorCode::Ok, 0};
	return this->error.code;
}

void pwm_generator::PwmSignal::params_get(unsigned long long int& waveform_period_ns, unsigned long long int& waveform_pulse_width_ns) const {
	waveform_period_ns = this->waveform.period_ns;
	waveform_pulse_width_ns = this->waveform.pulse_width_ns;
}

pwm_generator_interface::ErrorState pwm_generator::PwmSignal::error_state_get() const {
	return this->error;
}
