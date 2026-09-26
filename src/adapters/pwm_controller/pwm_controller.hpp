/**
*
*	@file		pwm_controller.hpp
*
*	@brief		Public API for the pwm module
*
*	@details	Adapter module for Zephyr's PWM API
*
*				Supports frequency and duty cycle modification, and Zephyr's PWM API return values
*
*/

#ifndef PWM_CONTROLLER_HPP
#define PWM_CONTROLLER_HPP

#include <cstddef>
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#define PERIOD_NS_FOR_25KHZ 40000UL							//25 KHz Frequency
#define PULSE_NS_FOR_HALF_DC 20000UL						//50% Duty-cycle

namespace pwm_controller {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		PwmUnready,
		ParamWaveform,
		PwmNotRunning,
		ZPwmSet
	};

	struct ErrorState {
		pwm_controller::ErrorCode code = pwm_controller::ErrorCode::Ok;
		int return_value = 0;
	};

	class PwmSignal {

		public:

			PwmSignal(struct pwm_dt_spec timer_device);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			PwmSignal(const PwmSignal&) = delete;
			PwmSignal(PwmSignal&&) = delete;
			PwmSignal& operator=(const PwmSignal&) = delete;
			PwmSignal& operator=(PwmSignal&&) = delete;

			pwm_controller::ErrorCode start(std::size_t waveform_period_ns, std::size_t waveform_pulse_width_ns);
			pwm_controller::ErrorCode stop();

			[[nodiscard("Called error getter and discarded its return value")]] pwm_controller::ErrorState error_state_get() const;

		private:

			struct SystemState {
				bool pwm_ready = false;
				bool pwm_running = false;
			};

			struct WaveformState {
				std::size_t period_ns = 0;
				std::size_t pulse_width_ns = 0;
				std::uint8_t duty_cycle_x100 = 0;
			};

			struct pwm_dt_spec timer = {};

			pwm_controller::ErrorState error;
			pwm_controller::PwmSignal::SystemState system;
			pwm_controller::PwmSignal::WaveformState waveform;
	};
}

#endif

/**
*
*	@enum		pwm_controller::ErrorCode
*
*	@brief		Error codes of the module
*
*	@invariant	If an error related to Zephyr's PWM API occurs then the code is prefixed with "ZPwm"
*
*/

/**
*
*	@struct		pwm_controller::ErrorState
*
*	@brief		Data structure for all types of errors
*
*	@invariant	All members of this struct are set at the end of every fallible method of PwmSignal
*	@invariant	If a module-based error occurs then it is represented by code and the other member is equal to 0
*	@invariant	If an error related to Zephyr's PWM API occurs then it is represented by both code and return_value
*
*/

/**
*
*	@class		pwm_controller::PwmSignal
*
*	@brief		Class for Zephyr's PWM API
*
*	@warning	Since each instance of this class represents a different device there should not exist copies
*
*/

/**
*
*	@fn			pwm_controller::PwmSignal::PwmSignal(struct pwm_dt_spec pwm_dt_spec)
*
*	@brief		Constructor to initialize an instance for a specific PWM
*
*	@param[in]	pwm_dt_spec						PWM-specific struct of the target PWM
*
*	@pre		pwm_dt_spec is a valid PWM-specific struct of an existing PWM
*	@post		If the target PWM is not ready to be used then error.code is set to DeviceUnready
*	@post		On success the target PWM is ready, the instance is initialized for it, and error.code is set to Ok
*
*/

/**
*
*	@fn			pwm_controller::ErrorCode start(std::size_t waveform_period_ns, std::size_t waveform_pulse_width_ns)
*
*	@brief		Method to start the PWM with specific waveform parameters
*
*	@param[in]	waveform_period_ns				Waveform period in nanoseconds
*	@param[in]	waveform_pulse_width_ns			Waveform pulse width in nanoseconds
*
*	@retval		DeviceUnready					If the PWM is not ready to be used
*	@retval		ParamWaveform					If the passed period is 0 or less than the pulse width
*	@retval		ZPwmSet							If an error occurs when setting the waveform parameters
*	@retval		Ok								If no error occurs
*
*	@post		If the PWM is not ready to be used then error.code is set to DeviceUnready
*	@post		If the passed period is 0 or less than the pulse width then error.code is set to ParamWaveform
*	@post		If an error occurs when setting the waveform parameters with Zephyr's PWM API then its return value is saved in error.return_value and error.code is set to ZPwmSet
*	@post		On success the passed values are set to the waveform parameters and error.code is set to Ok
*
*/

/**
*
*	@fn			pwm_controller::ErrorCode stop()
*
*	@brief		Method to stop the PWM
*
*	@retval		ZPwmSet							If an error occurs when setting the waveform parameters
*	@retval		Ok								If no error occurs
*
*	@post		If an error occurs when setting the waveform parameters with Zephyr's PWM API then its return value is saved in error.return_value and error.code is set to ZPwmSet
*	@post		On success the PWM is stopped and error.code is set to Ok
*
*/

/**
*
*	@fn			pwm_controller::ErrorState error_state_get() const
*
*	@brief		Method to obtain the last set waveform parameters
*
*	@return		The full error report
*
*/
