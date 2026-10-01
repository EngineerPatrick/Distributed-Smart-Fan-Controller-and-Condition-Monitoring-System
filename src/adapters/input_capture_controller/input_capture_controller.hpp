#ifndef INPUT_CAPTURE_CONTROLLER_HPP
#define INPUT_CAPTURE_CONTROLLER_HPP

#include "input_capture_controller_interface.hpp"
#include <cstdint>
#include <zephyr/sys/atomic.h>
#include <zephyr/drivers/counter.h>

#define INPUT_CAPTURE_TIMER_DEVICE(alias) { \
	DEVICE_DT_GET(DT_COUNTER_CAPTURES_CTLR_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
	DT_COUNTER_CAPTURES_FLAGS_BY_IDX(DT_ALIAS(alias), counter_captures, 0), \
	static_cast<uint8_t>(DT_COUNTER_CAPTURES_CHANNEL_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
}

namespace input_capture_controller {

	class InputCaptureSignal : public input_capture_controller_interface::InputCaptureSignalInterface {

		public:

			InputCaptureSignal(const counter_capture_dt_spec timer_device, const counter_capture_flags_t additional_flags);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			InputCaptureSignal(const InputCaptureSignal&) = delete;
			InputCaptureSignal(InputCaptureSignal&&) = delete;
			InputCaptureSignal& operator=(const InputCaptureSignal&) = delete;
			InputCaptureSignal& operator=(InputCaptureSignal&&) = delete;

			input_capture_controller_interface::ErrorCode capture_start() override;
			input_capture_controller_interface::ErrorCode capture_stop() override;

			input_capture_controller_interface::ErrorCode capture_period_ns_get(unsigned long long int& capture_period_ns) override;
			[[nodiscard("Called error getter and discarded its return value")]]
			input_capture_controller_interface::ErrorState error_state_get() const override;

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
				bool capture_ready = false;
				bool capture_running = false;
				atomic_t second_capture = ATOMIC_INIT(0);
				atomic_t capture_reading = ATOMIC_INIT(0);
			};

			input_capture_controller::InputCaptureSignal::CounterState counter;
			input_capture_controller::InputCaptureSignal::CaptureState capture;
			input_capture_controller::InputCaptureSignal::CaptureState capture_copy;
			input_capture_controller::InputCaptureSignal::SystemState system;
			input_capture_controller_interface::ErrorState error;

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
