#include "input_capture.hpp"
#include "counter_capture.hpp"
#include "atomic_operations.hpp"
#include <cstdint>

void input_capture::InputCaptureSignal::capture_callback(void* context_ptr, const std::uint32_t current_timestamp_ticks) {
	input_capture::InputCaptureSignal* this_signal_ptr = static_cast<input_capture::InputCaptureSignal*>(context_ptr);

	if (this_signal_ptr->system.storage_ready) {
		return;
	}

	atomic_operations::atomic_var_set(
		&(this_signal_ptr->input_signal.previous_timestamp_ticks),
		atomic_operations::atomic_var_get(&(this_signal_ptr->input_signal.current_timestamp_ticks))
	);

	atomic_operations::atomic_var_set(&(this_signal_ptr->input_signal.current_timestamp_ticks), current_timestamp_ticks);

	atomic_operations::atomic_var_set(&(this_signal_ptr->system.new_reading), 1);
}

input_capture::InputCaptureSignal::InputCaptureSignal(const counter_capture_dt_spec counter_capture_counter, const counter_capture_flags_t additional_flags) :
counter{
	counter_capture::CounterCapture{counter_capture_counter, additional_flags, input_capture::InputCaptureSignal::capture_callback, this},
	this->counter.capture.timer_resolution_ticks_get()
} {

	if (this->counter.capture.error_state_get().code != counter_capture::ErrorCode::Ok) {
		this->error = input_capture::ErrorCode::CounterUnready;
		return;
	}

	if (this->counter.capture.start() != counter_capture::ErrorCode::Ok) {
		this->error = input_capture::ErrorCode::CounterStart;
		return;
	}

	this->system.capture_ready = true;
	this->error = input_capture::ErrorCode::Ok;
}

input_capture::ErrorCode input_capture::InputCaptureSignal::period_ns_get(std::uint64_t& signal_period_ns) {
	std::uint32_t current_timestamp_ticks = 0;
	std::uint32_t previous_timestamp_ticks = 0;

	if (!this->system.capture_ready) {
		this->error = input_capture::ErrorCode::CaptureUnready;
		return this->error;
	}

	if (!atomic_operations::atomic_var_get(&(this->system.new_reading))) {
		this->error = input_capture::ErrorCode::NewUnavailable;
		return this->error;
	}

	this->system.storage_ready = true;

	current_timestamp_ticks = atomic_operations::atomic_var_get(&(this->input_signal.current_timestamp_ticks));
	previous_timestamp_ticks = atomic_operations::atomic_var_get(&(this->input_signal.previous_timestamp_ticks));

	if (current_timestamp_ticks >= previous_timestamp_ticks) {
		signal_period_ns = this->counter.capture.timestamp_ticks_to_ns((current_timestamp_ticks - previous_timestamp_ticks));
	}

	else {
		signal_period_ns = this->counter.capture.timestamp_ticks_to_ns((current_timestamp_ticks + (this->counter.resolution_ticks - previous_timestamp_ticks)));
	}

	atomic_operations::atomic_var_set(&(this->system.new_reading), 0);
	this->system.storage_ready = false;
	this->error = input_capture::ErrorCode::Ok;
	return this->error;
}

input_capture::ErrorCode input_capture::InputCaptureSignal::error_get() const {
	return this->error;
}
