#ifndef FAN_CONTROLLER_HPP
#define FAN_CONTROLLER_HPP

#include "pwm_generator_interface.hpp"
#include "pulse_reader_interface.hpp"

#define PERIOD_NS_FOR_25KHZ 40000UL								//25 KHz Frequency

namespace fan_controller {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		SpecificError,
		FanUnready,
		FanRunning,
		FanNotRunning,
		ParamDutyCycle,
		PwmUnready,
		PwmStart,
		PwmStop,
		TachometerUnready,
		TachometerReadingStart,
		TachometerReadingStop,
		TachometerReadingCapture
	};

	struct ErrorState {
		fan_controller::ErrorCode general = fan_controller::ErrorCode::Ok;
		fan_controller::ErrorCode pwm = fan_controller::ErrorCode::Ok;
		fan_controller::ErrorCode tachometer = fan_controller::ErrorCode::Ok;
	};

	class FourWireFan {

		public:

			explicit FourWireFan(pwm_generator_interface::PwmSignalInterface& fan_pwm, pulse_reader_interface::PulseSignalInterface& fan_tachometer);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			FourWireFan(const FourWireFan&) = delete;
			FourWireFan(FourWireFan&&) = delete;
			FourWireFan& operator=(const FourWireFan&) = delete;
			FourWireFan& operator=(FourWireFan&&) = delete;

			fan_controller::ErrorState boot();
			fan_controller::ErrorState stop();
			fan_controller::ErrorState speed_measure(unsigned int& measured_speed_rpm);
			fan_controller::ErrorState duty_cycle_update(unsigned int duty_cycle_x100);

			[[nodiscard("Called error getter and discarded its return value")]]
			fan_controller::ErrorState error_state_get() const;

		private:

			struct PwmState {
				pwm_generator_interface::PwmSignalInterface& signal;
				unsigned long long int period_ns = 0;
				unsigned int duty_cycle_x100 = 0;
			};

			struct TachometerState {
				pulse_reader_interface::PulseSignalInterface& signal;
				unsigned int speed_rpm = 0;
			};

			struct SystemState {
				bool pwm_ready = false;
				bool pwm_running = false;
				bool tachometer_ready = false;
				bool tachometer_running = false;
			};

			fan_controller::FourWireFan::PwmState pwm;
			fan_controller::FourWireFan::TachometerState tachometer;
			fan_controller::FourWireFan::SystemState system;

			fan_controller::ErrorState error;
	};
}

#endif
