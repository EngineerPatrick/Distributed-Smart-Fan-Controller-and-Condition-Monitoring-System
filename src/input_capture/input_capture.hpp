#ifndef INPUT_CAPTURE_HPP
#define INPUT_CAPTURE_HPP

#include <cstddef>
#include <cstdint>
#include <zephyr/sys/atomic.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#define CONCAT_UNDRSCR(str1, str2) str1 ## _ ## str2

#define DEFINE_INPUT_CAPTURE(alias) input_capture::TimerDevice CONCAT_UNDRSCR(input_capture_timer, alias){ \
	{DEVICE_DT_GET(DT_PWMS_CTLR_BY_NAME(DT_ALIAS(alias), tachometer))}, \
	{DT_PWMS_CHANNEL_BY_NAME(DT_ALIAS(alias), tachometer)}, \
};

#define INPUT_CAPTURE_TIMER(alias) CONCAT_UNDRSCR(input_capture_timer, alias)

namespace input_capture {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		TimerUnready,
		CaptureTimerFrequency,
		CaptureStatus,
		CaptureConfigure,
		CaptureEnable
	};

	struct ErrorState {
		input_capture::ErrorCode code = input_capture::ErrorCode::Ok;
		int return_value = 0;
	};

	struct TimerDevice {
		const struct device* const device_ptr = nullptr;
		const std::uint32_t channel = 0;
		const std::uint32_t period_ns = 0;
	};

	class InputCaptureSignal {

		public:

			InputCaptureSignal(const struct TimerDevice input_capture_timer);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			InputCaptureSignal(const InputCaptureSignal&) = delete;
			InputCaptureSignal(InputCaptureSignal&&) = delete;
			InputCaptureSignal& operator=(const InputCaptureSignal&) = delete;
			InputCaptureSignal& operator=(InputCaptureSignal&&) = delete;

			input_capture::ErrorCode capture_period_ns_get(std::uint32_t& capture_period_ns);
			[[nodiscard("Called error getter and discarded its return value")]] input_capture::ErrorState error_state_get() const;


		private:

			struct SystemState {
				bool timer_ready = false;
				bool capture_ready = false;
				atomic_val_t capture_valid = 0;
			};

			struct CaptureState {
				atomic_val_t period_ns = ATOMIC_INIT(0);
				atomic_val_t status = ATOMIC_INIT(0);
			};

			input_capture::ErrorState error;
			input_capture::InputCaptureSignal::SystemState system;
			input_capture::TimerDevice timer;

			input_capture::InputCaptureSignal::CaptureState capture;

			std::uint32_t init_operations();

			static void capture_callback(
				const struct device* const timer_device_ptr,
				const std::uint32_t timer_channel,
				const std::uint32_t period_cycles_hz,
				const std::uint32_t pulse_cycles_hz,
				const int capture_status,
				void* input_capture_signal
			);

	};
}

#endif
