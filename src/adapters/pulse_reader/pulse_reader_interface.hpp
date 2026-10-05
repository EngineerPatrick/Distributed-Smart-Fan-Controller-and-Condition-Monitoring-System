#ifndef PULSE_READER_INTERFACE_HPP
#define PULSE_READER_INTERFACE_HPP

namespace pulse_reader_interface {

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
		pulse_reader_interface::ErrorCode code = pulse_reader_interface::ErrorCode::Ok;
		int return_value = 0;
	};

	class PulseSignalInterface {

		public:

			virtual pulse_reader_interface::ErrorCode capture_start() = 0;
			virtual pulse_reader_interface::ErrorCode capture_stop() = 0;

			virtual pulse_reader_interface::ErrorCode capture_period_ns_get(unsigned long long int& capture_period_ns) = 0;
			[[nodiscard("Called error getter and discarded its return value")]]
			virtual pulse_reader_interface::ErrorState error_state_get() const = 0;

		protected:

			/*
			*
			*	The destructor is protected to prevent the destruction of any instance but allow derived class to override it
			*
			*/
			virtual ~PulseSignalInterface() = default;
	};
}

#endif
