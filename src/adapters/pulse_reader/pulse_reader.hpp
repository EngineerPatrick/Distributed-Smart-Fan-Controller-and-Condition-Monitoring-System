#ifndef PULSE_READER_HPP
#define PULSE_READER_HPP

#include "pulse_reader_interface.hpp"
#include <cstdint>
#include <zephyr/sys/atomic.h>
#include <zephyr/drivers/counter.h>

#define INPUT_CAPTURE_TIMER_DEVICE(alias) { \
	DEVICE_DT_GET(DT_COUNTER_CAPTURES_CTLR_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
	DT_COUNTER_CAPTURES_FLAGS_BY_IDX(DT_ALIAS(alias), counter_captures, 0), \
	static_cast<uint8_t>(DT_COUNTER_CAPTURES_CHANNEL_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
}

namespace pulse_reader {

	class PulseSignal : public pulse_reader_interface::PulseSignalInterface {

		public:

			PulseSignal(const counter_capture_dt_spec timer_device, const counter_capture_flags_t additional_flags);
			~PulseSignal() override;

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			PulseSignal(const PulseSignal&) = delete;
			PulseSignal(PulseSignal&&) = delete;
			PulseSignal& operator=(const PulseSignal&) = delete;
			PulseSignal& operator=(PulseSignal&&) = delete;

			pulse_reader_interface::ErrorCode capture_start() override;
			pulse_reader_interface::ErrorCode capture_stop() override;
			pulse_reader_interface::ErrorCode capture_period_measure(unsigned long long int& capture_period_ns) override;

			[[nodiscard("Called error getter and discarded its return value")]]
			pulse_reader_interface::ErrorState error_state_get() const override;

		private:

			struct CounterState {
				const counter_capture_dt_spec timer = {};
				std::uint64_t resolution_ticks = 0;
			};

			struct CaptureState {
				atomic_t current_timestamp_ticks = ATOMIC_INIT(0);
				atomic_t previous_timestamp_ticks = ATOMIC_INIT(0);
				atomic_t last = ATOMIC_INIT(0);
				std::uint32_t read = 0;
			};

			struct SystemState {
				bool counter_capture_acquired = false;
				bool counter_capture_ready = false;
				atomic_t period_measure_ready = ATOMIC_INIT(0);
				atomic_t save_copy = ATOMIC_INIT(0);
			};

			pulse_reader::PulseSignal::CounterState counter;
			pulse_reader::PulseSignal::CaptureState capture;
			pulse_reader::PulseSignal::CaptureState capture_copy;
			pulse_reader::PulseSignal::SystemState system;
			pulse_reader_interface::ErrorState error;

			static void capture_callback(
				const struct device* const timer_device_ptr,
				const std::uint8_t timer_channel_id,
				const counter_capture_flags_t timer_flags,
				const std::uint32_t current_timestamp_ticks,
				void* const context_ptr
			);
	};
}

#endif
