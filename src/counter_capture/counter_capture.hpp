#ifndef COUNTER_CAPTURE_HPP
#define COUNTER_CAPTURE_HPP

#include <cstddef>
#include <cstdint>
#include <zephyr/drivers/counter.h>

#define COUNTER_CAPTURE_TIMER(alias) { \
	DEVICE_DT_GET(DT_COUNTER_CAPTURES_CTLR_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
	DT_COUNTER_CAPTURES_FLAGS_BY_IDX(DT_ALIAS(alias), counter_captures, 0), \
	static_cast<uint8_t>(DT_COUNTER_CAPTURES_CHANNEL_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
}

namespace counter_capture {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		CounterUnready,
		CounterNotRunning,
		ZCounterCaptureConfigure,
		ZCounterCaptureEnable,
		ZCounterStart,
		ZCounterStop
	};

	struct ErrorState {
		counter_capture::ErrorCode code = counter_capture::ErrorCode::Ok;
		int return_value = 0;
	};

	typedef void(* capture_callback_t)(void* context_ptr, const std::uint32_t timestamp_ticks);

	class CounterCapture {

		public:

			CounterCapture(
				const counter_capture_dt_spec counter_capture_timer,
				const counter_capture_flags_t additional_flags,
				capture_callback_t const capture_callback_ptr,
				void* const context_ptr
			);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			CounterCapture(const CounterCapture&) = delete;
			CounterCapture(CounterCapture&&) = delete;
			CounterCapture& operator=(const CounterCapture&) = delete;
			CounterCapture& operator=(CounterCapture&&) = delete;

			counter_capture::ErrorCode start();
			counter_capture::ErrorCode stop();

			std::uint32_t timer_resolution_ticks_get() const;
			std::uint64_t timestamp_ticks_to_ns(std::uint32_t timestamp_ticks) const;
			[[nodiscard("Called error getter and discarded its return value")]] counter_capture::ErrorState error_state_get() const;

		private:

			struct CallbackState {
				counter_capture::capture_callback_t const capture_callback_ptr = nullptr;
				void* const context_ptr = nullptr;
			};

			struct SystemState {
				bool counter_ready = false;
				bool counter_running = false;
			};

			const counter_capture_dt_spec timer = {};

			counter_capture::CounterCapture::CallbackState callback;
			counter_capture::CounterCapture::SystemState system;
			counter_capture::ErrorState error;

			static void capture_callback(
				const struct device* const timer_device,
				const std::uint8_t timer_channel_id,
				const counter_capture_flags_t timer_flags,
				const std::uint32_t current_timestamp_ticks,
				void* const context_ptr
			);
	};
}

#endif
