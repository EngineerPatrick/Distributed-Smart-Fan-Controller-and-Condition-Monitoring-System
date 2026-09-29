#ifndef INPUT_CAPTURE_CONTROLLER_INTERFACE_HPP
#define INPUT_CAPTURE_CONTROLLER_INTERFACE_HPP

#define INPUT_CAPTURE_TIMER_DEVICE(alias) { \
	DEVICE_DT_GET(DT_COUNTER_CAPTURES_CTLR_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
	DT_COUNTER_CAPTURES_FLAGS_BY_IDX(DT_ALIAS(alias), counter_captures, 0), \
	static_cast<uint8_t>(DT_COUNTER_CAPTURES_CHANNEL_BY_IDX(DT_ALIAS(alias), counter_captures, 0)), \
}

namespace input_capture_controller_interface {

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

	class InputCaptureSignalInterface {

		public:

			virtual input_capture_controller_interface::ErrorCode capture_start() = 0;
			virtual input_capture_controller_interface::ErrorCode capture_stop() = 0;

			virtual input_capture_controller_interface::ErrorCode capture_period_ns_get(unsigned long long int& capture_period_ns) = 0;
			[[nodiscard("Called error getter and discarded its return value")]]
			virtual input_capture_controller_interface::ErrorState error_state_get() const = 0;

		protected:

			/*
			*
			*	The destructor is protected to prevent the destruction of any instance but allow derived class to override it
			*
			*/
			virtual ~InputCaptureSignalInterface() = default;
	};
}

#endif
