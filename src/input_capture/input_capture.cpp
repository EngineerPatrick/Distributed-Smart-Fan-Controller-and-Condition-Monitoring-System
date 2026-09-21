#include "input_capture.hpp"
#include <cstddef>
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/drivers/pwm.h>

std::uint32_t input_capture::InputCaptureSignal::init_operations() {
	std::uint64_t timer_frequency_hz = 0;

	if (!device_is_ready(this->timer.device_ptr)) {
		this->error = {input_capture::ErrorCode::DeviceUnready, 0};
		return 0;
	}

	this->error.return_value = pwm_get_cycles_per_sec(this->timer.device_ptr, this->timer.channel, &timer_frequency_hz);

	if (this->error.return_value != 0) {
		this->error.code = input_capture::ErrorCode::CaptureTimerFrequency;
		return 0;
	}

	this->system.timer_ready = true;
	this->error = {input_capture::ErrorCode::Ok, 0};
	return timer_frequency_hz;
}

void input_capture::InputCaptureSignal::capture_callback(
	const struct device* const timer_device_ptr,
	const std::uint32_t timer_channel,
	const std::uint32_t period_cycles_hz,
	const std::uint32_t pulse_cycles_hz,
	const int capture_status,
	void* input_capture_signal
) {

	atomic_set(&(static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->system.capture_valid), 0);

	if (capture_status != 0) {
		atomic_set(&(static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->capture.status), capture_status);
		return;
	}

	atomic_set(&(static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->capture.period_ns), (period_cycles_hz * static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->timer.period_ns));
	atomic_set(&(static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->system.capture_valid), 1);
}

input_capture::InputCaptureSignal::InputCaptureSignal(const input_capture::TimerDevice input_capture_timer) :
timer{{input_capture_timer.device_ptr}, {input_capture_timer.channel}, {this->init_operations()}} {

	if (!this->system.timer_ready) {
		this->error = {input_capture::ErrorCode::TimerUnready, 0};
		return;
	}

	this->error.return_value = pwm_configure_capture(this->timer.device_ptr, this->timer.channel,
													(PWM_CAPTURE_TYPE_PERIOD | PWM_CAPTURE_MODE_CONTINUOUS),
													input_capture::InputCaptureSignal::capture_callback, this);

	if (this->error.return_value != 0) {
		this->error.code = input_capture::ErrorCode::CaptureConfigure;
		return;
	}

	this->error.return_value = pwm_enable_capture(this->timer.device_ptr, this->timer.channel);

	if (this->error.return_value != 0) {
		this->error.code = input_capture::ErrorCode::CaptureEnable;
		return;
	}

	this->system.capture_ready = true;
	this->error = {input_capture::ErrorCode::Ok, 0};
}

input_capture::ErrorCode input_capture::InputCaptureSignal::capture_period_ns_get(std::uint32_t& capture_period_ns) {

	if (!atomic_get(&(this->system.capture_valid))) {
		this->error = {input_capture::ErrorCode::CaptureStatus, 0};
		return this->error.code;
	}

	capture_period_ns = atomic_get(&(this->capture.period_ns));
	this->error = {input_capture::ErrorCode::Ok, 0};
	return this->error.code;
}

input_capture::ErrorState input_capture::InputCaptureSignal::error_state_get() const {
	return this->error;
}
