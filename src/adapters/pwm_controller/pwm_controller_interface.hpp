#ifndef PWM_CONTROLLER_INTERFACE_HPP
#define PWM_CONTROLLER_INTERFACE_HPP

namespace pwm_controller_interface {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		PwmUnready,
		ParamWaveform,
		PwmNotRunning,
		ZPwmSet
	};

	struct ErrorState {
		pwm_controller_interface::ErrorCode code = pwm_controller_interface::ErrorCode::Ok;
		int return_value = 0;
	};

	class PwmSignalInterface {

		public:

			virtual pwm_controller_interface::ErrorCode start(unsigned long long int waveform_period_ns, unsigned long long int waveform_pulse_width_ns) = 0;
			virtual pwm_controller_interface::ErrorCode stop() = 0;

			[[nodiscard("Called error getter and discarded its return value")]]
			virtual pwm_controller_interface::ErrorState error_state_get() const = 0;

		protected:

			/*
			*
			*	The destructor is protected to prevent the destruction of any instance but allow derived class to override it
			*
			*/
			virtual ~PwmSignalInterface() = default;
	};
}

#endif
