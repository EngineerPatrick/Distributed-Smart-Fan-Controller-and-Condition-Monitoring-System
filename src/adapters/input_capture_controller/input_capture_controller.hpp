#ifndef INPUT_CAPTURE_CONTROLLER_HPP
#define INPUT_CAPTURE_CONTROLLER_HPP

#include "input_capture_controller_interface.hpp"
#include <cstdint>
#include <zephyr/sys/atomic.h>
#include <zephyr/drivers/counter.h>

namespace input_capture_controller {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		CaptureUnready,
		CaptureNotRunning,
		NewCaptureUnavailable,
		ZCounterCaptureConfigure,
		ZCounterCaptureEnable,
		ZCounterStart,
		ZCounterStop,
		ZCounterReset
	};

	struct ErrorState {
		input_capture_controller_interface::ErrorCode code = input_capture_controller_interface::ErrorCode::Ok;
		int return_value = 0;
	};

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

			input_capture_controller_interface::ErrorCode capture_period_ns_get(std::uint64_t& capture_period_ns) override;
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
			};

			struct SystemState {
				bool capture_ready = false;
				bool capture_running = false;
				atomic_t first_capture = ATOMIC_INIT(0);
				atomic_t new_capture = ATOMIC_INIT(0);
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
