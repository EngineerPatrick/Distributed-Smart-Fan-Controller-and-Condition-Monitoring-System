#ifndef INPUT_CAPTURE_SERVICE_HPP
#define INPUT_CAPTURE_SERVICE_HPP

#include "counter_capture.hpp"
#include "atomic_operations.hpp"
#include <cstdint>

namespace input_capture_service {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		TimerUnready,
		TimerStart,
		CaptureUnready,
		NewUnavailable
	};

	class InputCaptureSignal {

		public:

			InputCaptureSignal(const counter_capture_dt_spec counter_capture_timer, const counter_capture_flags_t additional_flags);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			InputCaptureSignal(const InputCaptureSignal&) = delete;
			InputCaptureSignal(InputCaptureSignal&&) = delete;
			InputCaptureSignal& operator=(const InputCaptureSignal&) = delete;
			InputCaptureSignal& operator=(InputCaptureSignal&&) = delete;

			input_capture_service::ErrorCode period_ns_get(std::uint64_t& signal_period_ns);
			[[nodiscard("Called error getter and discarded its return value")]] input_capture_service::ErrorCode error_get() const;

		private:

			struct TimerDevice {
				counter_capture::CounterCapture counter;
				const std::uint32_t resolution_ticks = 0;
			};

			struct InputSignalState {
				atomic_operations::atomic32_t current_timestamp_ticks;
				atomic_operations::atomic32_t previous_timestamp_ticks;
			};

			struct SystemState {
				bool capture_ready = false;
				bool storage_ready = false;
				atomic_operations::atomic32_t new_reading;
			};

			input_capture_service::InputCaptureSignal::TimerDevice timer;
			input_capture_service::InputCaptureSignal::InputSignalState input_signal;
			input_capture_service::InputCaptureSignal::SystemState system;
			input_capture_service::ErrorCode error;

			static void capture_callback(void* context_ptr, const std::uint32_t current_timestamp_ticks);
	};
}

#endif
