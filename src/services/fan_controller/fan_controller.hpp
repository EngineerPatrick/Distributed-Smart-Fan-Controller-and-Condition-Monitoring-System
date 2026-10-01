#ifndef FAN_CONTROLLER_HPP
#define FAN_CONTROLLER_HPP

#include "pwm_controller_interface.hpp"
#include "input_capture_controller_interface.hpp"

#define PERIOD_NS_FOR_25KHZ 40000UL								//25 KHz Frequency

namespace fan_controller {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		ParamDutyCycle,
		FanUnready,
		FanNotRunning,
		PwmUnready,
		PwmStart,
		PwmStop,
		TachometerUnready,
		TachometerReadingStart,
		TachometerReadingStop,
		TachometerReadingCapture
	};

	class FourWireFan {

		public:

			explicit FourWireFan(pwm_controller_interface::PwmSignalInterface& fan_pwm, input_capture_controller_interface::InputCaptureSignalInterface& fan_tachometer);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			FourWireFan(const FourWireFan&) = delete;
			FourWireFan(FourWireFan&&) = delete;
			FourWireFan& operator=(const FourWireFan&) = delete;
			FourWireFan& operator=(FourWireFan&&) = delete;

			fan_controller::ErrorCode boot();
			fan_controller::ErrorCode stop();
			fan_controller::ErrorCode speed_measure(unsigned int& measured_speed_rpm);
			fan_controller::ErrorCode duty_cycle_update(unsigned int duty_cycle_x100);

			[[nodiscard("Called error getter and discarded its return value")]]
			fan_controller::ErrorCode error_get() const;

		private:

			struct PwmState {
				pwm_controller_interface::PwmSignalInterface& signal;
				unsigned long long int period_ns = 0;
				unsigned int duty_cycle_x100 = 0;
			};

			struct TachometerState {
				input_capture_controller_interface::InputCaptureSignalInterface& signal;
				unsigned int speed_rpm = 0;
			};

			struct SystemState {
				bool fan_ready = false;
				bool fan_running = false;
			};

			fan_controller::FourWireFan::PwmState pwm;
			fan_controller::FourWireFan::TachometerState tachometer;
			fan_controller::FourWireFan::SystemState system;
			fan_controller::ErrorCode error = fan_controller::ErrorCode::Ok;
	};
}

#endif
