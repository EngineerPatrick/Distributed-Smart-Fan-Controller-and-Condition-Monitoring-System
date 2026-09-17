/**
*
*	@file		pwm.cpp
*
*	@brief		Implementation for the pwm module
*
*	@details	Acquires Zephyr's PWM resource, keeps track of its return
*				values and performs start/stop operations
*
*				Supports waveform parameters setting and getting
*
*/

#include "pwm.hpp"
#include <cstddef>
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

pwm::PwmDevice::PwmDevice (struct pwm_dt_spec pwm_dt_spec) :
dt_spec{pwm_dt_spec} {

	if (!pwm_is_ready_dt(&(this->dt_spec))) {
		this->error = {pwm::ErrorCode::DeviceUnready, 0};
		return;
	}

	this->system.device_ready = true;
	this->error = {pwm::ErrorCode::Ok, 0};
}

pwm::ErrorCode pwm::PwmDevice::start(std::size_t waveform_period_ns, std::size_t waveform_pulse_width_ns) {

	if (!this->system.device_ready) {
		this->error = {pwm::ErrorCode::DeviceUnready, 0};
		return this->error.code;
	}

	if (!waveform_period_ns || waveform_period_ns < waveform_pulse_width_ns) {
		this->error = {pwm::ErrorCode::ParamWaveform, 0};
		return this->error.code;
	}

	this->error.return_value = pwm_set_dt(&(this->dt_spec), waveform_period_ns, waveform_pulse_width_ns);

	if (this->error.return_value != 0) {
		this->error.code = pwm::ErrorCode::PwmDeviceSet;
		return this->error.code;
	}

	this->waveform = {waveform_period_ns, waveform_pulse_width_ns, static_cast<uint8_t>((waveform_pulse_width_ns * 100) / waveform_period_ns)};
	this->system.pwm_running = true;
	this->system.params_set = true;
	this->error = {pwm::ErrorCode::Ok, 0};
	return this->error.code;
}

pwm::ErrorCode pwm::PwmDevice::stop() {

	if (!this->system.pwm_running) {
		this->error = {pwm::ErrorCode::NotRunning, 0};
		return this->error.code;
	}

	this->error.return_value = pwm_set_pulse_dt(&(this->dt_spec), 0);

	if (this->error.return_value != 0) {
		this->error.code = pwm::ErrorCode::PwmDeviceSet;
		return this->error.code;
	}

	this->waveform.pulse_width_ns = 0;
	this->waveform.duty_cycle_x100 = 0;
	this->system.pwm_running = false;
	this->error = {pwm::ErrorCode::Ok, 0};
	return this->error.code;
}

pwm::ErrorCode pwm::PwmDevice::waveform_params_get(std::size_t& waveform_period_ns, std::size_t& waveform_pulse_width_ns, std::size_t& waveform_duty_cycle_x100) {

	if (!this->system.params_set) {
		this->error = {pwm::ErrorCode::NotSet, 0};
		return this->error.code;
	}

	waveform_period_ns = this->waveform.period_ns;
	waveform_pulse_width_ns = this->waveform.pulse_width_ns;
	waveform_duty_cycle_x100 = this->waveform.duty_cycle_x100;

	this->error = {pwm::ErrorCode::Ok, 0};
	return this->error.code;
}

pwm::ErrorState pwm::PwmDevice::error_state_get() const {
	return this->error;
}
