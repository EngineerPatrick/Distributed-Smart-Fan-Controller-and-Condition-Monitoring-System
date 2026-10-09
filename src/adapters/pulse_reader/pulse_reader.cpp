#include "pulse_reader.hpp"
#include "pulse_reader_interface.hpp"
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/drivers/counter.h>

void pulse_reader::PulseSignal::capture_callback(
	const struct device* const timer_device_ptr,
	const std::uint8_t timer_channel_id,
	const counter_capture_flags_t timer_flags,
	const std::uint32_t current_timestamp_ticks,
	void* const context_ptr
) {
	pulse_reader::PulseSignal* const this_context_ptr = static_cast<pulse_reader::PulseSignal*>(context_ptr);

	atomic_set(
		&(this_context_ptr->capture.previous_timestamp_ticks),
		atomic_set(
			&(this_context_ptr->capture.current_timestamp_ticks),
			current_timestamp_ticks
		)
	);

	if (atomic_get(&(this_context_ptr->system.save_copy))) {
		atomic_set(&(this_context_ptr->capture_copy.current_timestamp_ticks), this_context_ptr->capture.current_timestamp_ticks);
		atomic_set(&(this_context_ptr->capture_copy.previous_timestamp_ticks), this_context_ptr->capture.previous_timestamp_ticks);
		atomic_set(&(this_context_ptr->capture.last), atomic_get(&(this_context_ptr->capture.last)) + 1);

		if (atomic_get(&(this_context_ptr->capture.last)) == 2) {
			atomic_set(&(this_context_ptr->system.period_measure_ready), 1);
		}
	}
}

pulse_reader::PulseSignal::PulseSignal(const counter_capture_dt_spec timer_device, const counter_capture_flags_t additional_flags) :
counter{
	{{timer_device.dev}, {timer_device.flags | additional_flags}, {timer_device.chan_id}},
	static_cast<std::uint64_t>(counter_get_max_top_value(this->counter.timer.dev)) + 1
} {

	if (!device_is_ready(this->counter.timer.dev)) {
		this->error = {pulse_reader_interface::ErrorCode::ZDeviceUnready, 0};
		return;
	}

	this->error.return_value = counter_capture_configure_dt(&(this->counter.timer), pulse_reader::PulseSignal::capture_callback, this);

	if (this->error.return_value != 0) {
		this->error.code = pulse_reader_interface::ErrorCode::ZCounterCaptureConfigure;
		return;
	}

	this->error.return_value = counter_enable_capture_dt(&(this->counter.timer));

	if (this->error.return_value != 0) {
		this->error.code = pulse_reader_interface::ErrorCode::ZCounterCaptureEnable;
		return;
	}

	this->system.counter_capture_acquired = true;
	this->error = {pulse_reader_interface::ErrorCode::Ok, 0};
}

pulse_reader::PulseSignal::~PulseSignal() {

	if (this->system.counter_capture_acquired) {
		counter_disable_capture_dt(&(this->counter.timer));
	}
}

pulse_reader_interface::ErrorCode pulse_reader::PulseSignal::capture_start() {

	if (!this->system.counter_capture_acquired) {
		this->error = {pulse_reader_interface::ErrorCode::CounterCaptureUnready, 0};
		return this->error.code;
	}

	if (this->system.counter_capture_ready) {
		this->error = {pulse_reader_interface::ErrorCode::Ok, 0};
		return this->error.code;
	}

	this->error.return_value = counter_start(this->counter.timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = pulse_reader_interface::ErrorCode::ZCounterStart;
		return this->error.code;
	}

	this->system.counter_capture_ready = true;
	this->error = {pulse_reader_interface::ErrorCode::Ok, 0};
	return this->error.code;
}

pulse_reader_interface::ErrorCode pulse_reader::PulseSignal::capture_stop() {

	if (!this->system.counter_capture_ready) {
		this->error = {pulse_reader_interface::ErrorCode::Ok, 0};
		return this->error.code;
	}

	this->error.return_value = counter_stop(this->counter.timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = pulse_reader_interface::ErrorCode::ZCounterStop;
		return this->error.code;
	}

	this->error.return_value = counter_reset(this->counter.timer.dev);

	if (this->error.return_value != 0) {
		this->error.code = pulse_reader_interface::ErrorCode::ZCounterReset;
		return this->error.code;
	}

	this->capture.read = 0;
	atomic_set(&(this->capture.last), 0);
	atomic_set(&(this->system.save_copy), 1);
	atomic_set(&(this->system.period_measure_ready), 0);
	atomic_set(&(this->capture.current_timestamp_ticks), 0);
	atomic_set(&(this->capture.previous_timestamp_ticks), 0);
	atomic_set(&(this->capture_copy.current_timestamp_ticks), 0);
	atomic_set(&(this->capture_copy.previous_timestamp_ticks), 0);

	this->system.counter_capture_ready = false;
	this->error = {pulse_reader_interface::ErrorCode::Ok, 0};
	return this->error.code;
}

pulse_reader_interface::ErrorCode pulse_reader::PulseSignal::capture_period_measure(unsigned long long int& capture_period_ns) {
	std::uint32_t last_capture = 0;
	std::uint32_t current_timestamp_ticks = 0;
	std::uint32_t previous_timestamp_ticks = 0;

	if (!atomic_get(&(this->system.period_measure_ready))) {
		this->error = {pulse_reader_interface::ErrorCode::PeriodMeasureUnready, 0};
		return this->error.code;
	}

	atomic_set(&(this->system.save_copy), 0);
	last_capture = atomic_get(&(this->capture.last));

	if (last_capture == this->capture.read) {
		atomic_set(&(this->system.save_copy), 1);
		this->error = {pulse_reader_interface::ErrorCode::NewDataUnready, 0};
		return this->error.code;
	}

	current_timestamp_ticks = atomic_get(&(this->capture_copy.current_timestamp_ticks));
	previous_timestamp_ticks = atomic_get(&(this->capture_copy.previous_timestamp_ticks));

	if (current_timestamp_ticks >= previous_timestamp_ticks) {
		capture_period_ns = counter_ticks_to_ns(this->counter.timer.dev, current_timestamp_ticks - previous_timestamp_ticks);
	}

	else {
		capture_period_ns = counter_ticks_to_ns(this->counter.timer.dev, current_timestamp_ticks + this->counter.resolution_ticks - previous_timestamp_ticks);
	}

	this->capture.read = last_capture;
	atomic_set(&(this->system.save_copy), 1);
	this->error = {pulse_reader_interface::ErrorCode::Ok, 0};
	return this->error.code;
}

pulse_reader_interface::ErrorState pulse_reader::PulseSignal::error_state_get() const {
	return this->error;
}
