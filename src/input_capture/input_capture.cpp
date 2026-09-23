#include "input_capture.hpp"
#include <cstddef>
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/drivers/counter.h>

void input_capture::InputCaptureSignal::capture_callback(
	const struct device* const timer_device_ptr,
	const std::uint8_t timer_channel,
	counter_capture_flags_t timer_flags,
	const std::uint32_t timer_timestamp_ticks,
	void* input_capture_signal
) {
	atomic_set(
		&(static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->capture.previous_timestamp_ticks),
		atomic_get(&(static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->capture.current_timestamp_ticks))
	);

	atomic_set(&(static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->capture.current_timestamp_ticks), timer_timestamp_ticks);

	atomic_set(&(static_cast<input_capture::InputCaptureSignal*>(input_capture_signal)->system.new_available), 1);
}

input_capture::InputCaptureSignal::InputCaptureSignal(const counter_capture_dt_spec input_capture_timer, const counter_capture_flags_t hardware-specific_flags) :
timer{{input_capture_timer.dev}, {input_capture_timer.flags | hardware-specific_flags}, {input_capture_timer.chan_id}} {

	if (!device_is_ready(this->timer.dev)) {
		this->error = {input_capture::ErrorCode::DeviceUnready, 0};
		return;
	}

	this->error.return_value = counter_capture_configure_dt(&(this->timer), input_capture::InputCaptureSignal::capture_callback, this);

	if (this->error.return_value != 0) {
		this->error.code = input_capture::ErrorCode::CounterCaptureConfigure;
		return;
	}

	this->error.return_value = counter_enable_capture_dt(&(this->timer));

	if (this->error.return_value != 0) {
		this->error.code = input_capture::ErrorCode::CounterCaptureEnable;
		return;
	}

	this->error.return_value = counter_start(this->timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = input_capture::ErrorCode::CounterStart;
		return;
	}

	this->system.capture_ready = true;
	this->error = {input_capture::ErrorCode::Ok, 0};
}

input_capture::ErrorCode input_capture::InputCaptureSignal::capture_period_us_get(std::uint32_t& capture_period_us) {

	if (!this->system.capture_ready) {
		this->error = {input_capture::ErrorCode::CaptureUnready, 0};
		return this->error.code;
	}

	if (!atomic_get(&(this->system.new_available))) {
		this->error = {input_capture::ErrorCode::NewUnavailable, 0};
		return this->error.code;
	}

	atomic_set(&(this->system.new_available), 0);

	std::uint32_t timer_max_ticks = counter_get_max_top_value(this->timer.dev);
	std::uint32_t current_ticks = atomic_get(&(this->capture.current_timestamp_ticks));
	std::uint32_t previous_ticks = atomic_get(&(this->capture.previous_timestamp_ticks));

	if (current_ticks >= previous_ticks) {
		capture_period_us = static_cast<std::uint32_t>(counter_ticks_to_us(this->timer.dev, (current_ticks - previous_ticks)));
	}

	else {
		capture_period_us = static_cast<std::uint32_t>(counter_ticks_to_us(this->timer.dev, (current_ticks + (timer_max_ticks - previous_ticks))));
	}

	this->error = {input_capture::ErrorCode::Ok, 0};
	return this->error.code;
}

input_capture::ErrorState input_capture::InputCaptureSignal::error_state_get() const {
	return this->error;
}
