#include "counter_capture.hpp"
#include <cstddef>
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>

static counter_capture::capture_callback_t capture_callback_static_ptr = nullptr;

void counter_capture::CounterCapture::capture_callback(
	const struct device* const timer_device,
	const std::uint8_t timer_channel_id,
	const counter_capture_flags_t timer_flags,
	const std::uint32_t current_timestamp_ticks,
	void* const context_ptr
) {
	capture_callback_static_ptr(context_ptr, current_timestamp_ticks);
}

counter_capture::CounterCapture::CounterCapture(
	const counter_capture_dt_spec counter_capture_timer,
	const counter_capture_flags_t additional_flags,
	counter_capture::capture_callback_t const capture_callback_ptr,
	void* const context_ptr
) :
timer{{counter_capture_timer.dev}, {counter_capture_timer.flags | additional_flags}, {counter_capture_timer.chan_id}},
callback{{capture_callback_ptr}, {context_ptr}} {

	if (!device_is_ready(this->timer.dev)) {
		this->error = {counter_capture::ErrorCode::DeviceUnready, 0};
		return;
	}

	capture_callback_static_ptr = this->callback.capture_callback_ptr;
	this->error.return_value = counter_capture_configure_dt(&(this->timer), counter_capture::CounterCapture::capture_callback, this->callback.context_ptr);

	if (this->error.return_value != 0) {
		this->error.code = counter_capture::ErrorCode::ZCounterCaptureConfigure;
		return;
	}

	this->error.return_value = counter_enable_capture_dt(&(this->timer));

	if (this->error.return_value != 0) {
		this->error.code = counter_capture::ErrorCode::ZCounterCaptureEnable;
		return;
	}

	this->system.counter_ready = true;
	this->error = {counter_capture::ErrorCode::Ok, 0};
}

counter_capture::ErrorCode counter_capture::CounterCapture::start() {

	if (!this->system.counter_ready) {
		this->error = {counter_capture::ErrorCode::CounterUnready, 0};
		return this->error.code;
	}

	this->error.return_value = counter_start(this->timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = counter_capture::ErrorCode::ZCounterStart;
		return this->error.code;
	}

	this->system.counter_running = true;
	this->error = {counter_capture::ErrorCode::Ok, 0};
	return this->error.code;
}

counter_capture::ErrorCode counter_capture::CounterCapture::stop() {

	if (!this->system.counter_running) {
		this->error = {counter_capture::ErrorCode::CounterNotRunning, 0};
		return this->error.code;
	}

	this->error.return_value = counter_stop(this->timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = counter_capture::ErrorCode::ZCounterStop;
		return this->error.code;
	}

	this->system.counter_running = false;
	this->error = {counter_capture::ErrorCode::Ok, 0};
	return this->error.code;
}

std::uint32_t counter_capture::CounterCapture::timer_resolution_ticks_get() const {
	return counter_get_max_top_value(this->timer.dev);
}

std::uint64_t counter_capture::CounterCapture::timestamp_ticks_to_ns(std::uint32_t timestamp_ticks) const {
	return counter_ticks_to_ns(this->timer.dev, timestamp_ticks);
}

counter_capture::ErrorState counter_capture::CounterCapture::error_state_get() const {
	return this->error;
}
