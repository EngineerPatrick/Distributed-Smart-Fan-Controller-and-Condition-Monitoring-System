/**
*
*	@file		pwm_controller.cpp
*
*	@brief		Implementation for the pwm module
*
*	@details	Acquires Zephyr's PWM resource, keeps track of its return
*				values and performs start/stop operations
*
*				Supports waveform parameters setting and getting
*
*/

#include "pwm_controller.hpp"
#include "pwm_controller_interface.hpp"
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

pwm_controller::PwmSignal::PwmSignal (struct pwm_dt_spec timer_device) :
timer{timer_device} {

	if (!pwm_is_ready_dt(&(this->timer))) {
		this->error = {pwm_controller_interface::ErrorCode::DeviceUnready, 0};
		return;
	}

	this->system.pwm_ready = true;
	this->error = {pwm_controller_interface::ErrorCode::Ok, 0};
}

pwm_controller_interface::ErrorCode pwm_controller::PwmSignal::start(unsigned long long int waveform_period_ns, unsigned long long int waveform_pulse_width_ns) {

	if (!this->system.pwm_ready) {
		this->error = {pwm_controller_interface::ErrorCode::PwmUnready, 0};
		return this->error.code;
	}

	if (!waveform_period_ns || waveform_period_ns < waveform_pulse_width_ns) {
		this->error = {pwm_controller_interface::ErrorCode::ParamWaveform, 0};
		return this->error.code;
	}

	this->error.return_value = pwm_set_dt(&(this->timer), waveform_period_ns, waveform_pulse_width_ns);

	if (this->error.return_value != 0) {
		this->error.code = pwm_controller_interface::ErrorCode::ZPwmSet;
		return this->error.code;
	}

	this->waveform = {waveform_period_ns, waveform_pulse_width_ns};
	this->system.pwm_running = true;
	this->error = {pwm_controller_interface::ErrorCode::Ok, 0};
	return this->error.code;
}

pwm_controller_interface::ErrorCode pwm_controller::PwmSignal::stop() {

	if (!this->system.pwm_running) {
		this->error = {pwm_controller_interface::ErrorCode::PwmNotRunning, 0};
		return this->error.code;
	}

	this->error.return_value = pwm_set_pulse_dt(&(this->timer), 0);

	if (this->error.return_value != 0) {
		this->error.code = pwm_controller_interface::ErrorCode::ZPwmSet;
		return this->error.code;
	}

	this->waveform.pulse_width_ns = 0;
	this->system.pwm_running = false;
	this->error = {pwm_controller_interface::ErrorCode::Ok, 0};
	return this->error.code;
}

pwm_controller_interface::ErrorState pwm_controller::PwmSignal::error_state_get() const {
	return this->error;
}
