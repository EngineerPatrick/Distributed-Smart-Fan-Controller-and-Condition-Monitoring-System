#ifndef INPUT_CAPTURE_HPP
#define INPUT_CAPTURE_HPP

#include <cstddef>
#include <cstdint>
#include <zephyr/sys/atomic.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>

#define INPUT_CAPTURE_TIMER(alias) { \
	DEVICE_DT_GET(DT_COUNTER_CAPTURES_CTLR_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
	DT_COUNTER_CAPTURES_FLAGS_BY_IDX(DT_ALIAS(alias), counter_captures, 0), \
	static_cast<uint8_t>(DT_COUNTER_CAPTURES_CHANNEL_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
}

namespace input_capture {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		CaptureUnready,
		NewUnavailable,
		CounterCaptureConfigure,
		CounterCaptureEnable,
		CounterStart
	};

	struct ErrorState {
		input_capture::ErrorCode code = input_capture::ErrorCode::Ok;
		int return_value = 0;
	};

	class InputCaptureSignal {

		public:

			InputCaptureSignal(const counter_capture_dt_spec input_capture_timer, const counter_capture_flags_t hardware-specific_flags);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			InputCaptureSignal(const InputCaptureSignal&) = delete;
			InputCaptureSignal(InputCaptureSignal&&) = delete;
			InputCaptureSignal& operator=(const InputCaptureSignal&) = delete;
			InputCaptureSignal& operator=(InputCaptureSignal&&) = delete;

			input_capture::ErrorCode capture_period_us_get(std::uint32_t& capture_period_us);
			[[nodiscard("Called error getter and discarded its return value")]] input_capture::ErrorState error_state_get() const;

		private:

			struct SystemState {
				bool capture_ready = false;
				atomic_val_t new_available = 0;
			};

			struct CaptureState {
				atomic_val_t current_timestamp_ticks = ATOMIC_INIT(0);
				atomic_val_t previous_timestamp_ticks = ATOMIC_INIT(0);
			};

			const counter_capture_dt_spec timer = {};

			input_capture::ErrorState error;
			input_capture::InputCaptureSignal::SystemState system;
			input_capture::InputCaptureSignal::CaptureState capture;

			static void capture_callback(
				const struct device* const timer_device_ptr,
				const std::uint8_t timer_channel,
				counter_capture_flags_t timer_flags,
				const std::uint32_t timer_timestamp_ticks,
				void* input_capture_signal
			);
	};
}

#endif
