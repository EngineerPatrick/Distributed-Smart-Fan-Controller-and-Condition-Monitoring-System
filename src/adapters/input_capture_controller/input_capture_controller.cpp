#include "input_capture_controller.hpp"
#include "atomic_operations.hpp"
#include <cstddef>
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>

void input_capture_controller::InputCaptureSignal::capture_callback(
	const struct device* const timer_device_ptr,
	const std::uint8_t timer_channel_id,
	const counter_capture_flags_t timer_flags,
	const std::uint32_t current_timestamp_ticks,
	void* const context_ptr
) {
	input_capture_controller::InputCaptureSignal* const this_context_ptr = static_cast<input_capture_controller::InputCaptureSignal*>(context_ptr);

	if (this_context_ptr->system.capture_reading) {
		return;
	}

	atomic_operations::atomic_var_set(
		&(this_context_ptr->capture.previous_timestamp_ticks),
		atomic_operations::atomic_var_set(
			&(this_context_ptr->capture.current_timestamp_ticks),
			current_timestamp_ticks
		)
	);

	atomic_operations::atomic_var_set(&(this_context_ptr->system.new_capture), 1);
}

input_capture_controller::InputCaptureSignal::InputCaptureSignal(const counter_capture_dt_spec timer_device, const counter_capture_flags_t additional_flags) :
counter{
	{{timer_device.dev}, {timer_device.flags | additional_flags}, {timer_device.chan_id}},
	counter_get_max_top_value(this->counter.timer.dev)
} {

	if (!device_is_ready(this->counter.timer.dev)) {
		this->error = {input_capture_controller::ErrorCode::DeviceUnready, 0};
		return;
	}

	this->error.return_value = counter_capture_configure_dt(&(this->counter.timer), input_capture_controller::InputCaptureSignal::capture_callback, this);

	if (this->error.return_value != 0) {
		this->error.code = input_capture_controller::ErrorCode::ZCounterCaptureConfigure;
		return;
	}

	this->error.return_value = counter_enable_capture_dt(&(this->counter.timer));

	if (this->error.return_value != 0) {
		this->error.code = input_capture_controller::ErrorCode::ZCounterCaptureEnable;
		return;
	}

	this->system.capture_ready = true;
	this->error = {input_capture_controller::ErrorCode::Ok, 0};
}

input_capture_controller::ErrorCode input_capture_controller::InputCaptureSignal::capture_start() {

	if (!this->system.capture_ready) {
		this->error = {input_capture_controller::ErrorCode::CaptureUnready, 0};
		return this->error.code;
	}

	if (this->system.capture_running) {

		if (this->capture_stop() != input_capture_controller::ErrorCode::Ok) {
			return this->error.code;
		}
	}

	atomic_operations::atomic_var_set(&(this->system.new_capture), 0);
	atomic_operations::atomic_var_set(&(this->system.capture_reading), 0);
	this->error.return_value = counter_start(this->counter.timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = input_capture_controller::ErrorCode::ZCounterStart;
		return this->error.code;
	}

	this->system.capture_running = true;
	this->error = {input_capture_controller::ErrorCode::Ok, 0};
	return this->error.code;
}

input_capture_controller::ErrorCode input_capture_controller::InputCaptureSignal::capture_stop() {

	if (!this->system.capture_running) {
		this->error = {input_capture_controller::ErrorCode::CaptureNotRunning, 0};
		return this->error.code;
	}

	this->error.return_value = counter_stop(this->counter.timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = input_capture_controller::ErrorCode::ZCounterStop;
		return this->error.code;
	}

	this->error.return_value = counter_reset(this->counter.timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = input_capture_controller::ErrorCode::ZCounterReset;
		return this->error.code;
	}

	this->system.capture_running = false;
	this->error = {input_capture_controller::ErrorCode::Ok, 0};
	return this->error.code;
}

input_capture_controller::ErrorCode input_capture_controller::InputCaptureSignal::capture_period_ns_get(std::uint64_t& capture_period_ns) {
	std::uint32_t current_timestamp_ticks = 0;
	std::uint32_t previous_timestamp_ticks = 0;

	if (!this->system.capture_running) {
		this->error = {input_capture_controller::ErrorCode::CaptureNotRunning, 0};
		return this->error.code;
	}

	if (!atomic_operations::atomic_var_get(&(this->system.new_capture))) {
		this->error = {input_capture_controller::ErrorCode::NewCaptureUnavailable, 0};
		return this->error.code;
	}

	atomic_operations::atomic_var_set(&(this->system.capture_reading), 1);
	atomic_operations::atomic_var_set(&(this->system.new_capture), 0);

	current_timestamp_ticks = atomic_operations::atomic_var_get(&(this->capture.current_timestamp_ticks));
	previous_timestamp_ticks = atomic_operations::atomic_var_get(&(this->capture.previous_timestamp_ticks));

	if (current_timestamp_ticks > previous_timestamp_ticks) {
		capture_period_ns = counter_ticks_to_ns(this->counter.timer.dev, current_timestamp_ticks - previous_timestamp_ticks);
	}

	else {
		capture_period_ns = counter_ticks_to_ns(this->counter.timer.dev, current_timestamp_ticks + (this->counter.resolution_ticks - previous_timestamp_ticks));
	}

	atomic_operations::atomic_var_set(&(this->system.capture_reading), 0);
	this->error = {input_capture_controller::ErrorCode::Ok, 0};
	return this->error.code;
}

input_capture_controller::ErrorState input_capture_controller::InputCaptureSignal::error_state_get() const {
	return this->error;
}
