/**
*
*	@file		pwm.hpp
*
*	@brief		Public API for the pwm module
*
*	@details	Adapter module for Zephyr's PWM API
*
*				Supports frequency and duty cycle modification, and Zephyr's PWM API return values
*
*/

#ifndef PWM_HPP
#define PWM_HPP

#include <cstddef>
#include <cstdint>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

#define PERIOD_NS_FOR_25KHZ 40000UL							//25 KHz Frequency
#define PULSE_NS_FOR_HALF_DC 20000UL						//50% Duty-cycle

namespace pwm {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		WaveformNotSet,
		PwmSet,
		ParamWaveform
	};

	struct ErrorState {
		pwm::ErrorCode code = pwm::ErrorCode::Ok;
		int return_value = 0;
	};

	class PwmSignal {

		public:

			PwmSignal(struct pwm_dt_spec pwm_dt_spec);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			PwmSignal(const PwmSignal&) = delete;
			PwmSignal(PwmSignal&&) = delete;
			PwmSignal& operator=(const PwmSignal&) = delete;
			PwmSignal& operator=(PwmSignal&&) = delete;

			pwm::ErrorCode start(std::size_t waveform_period_ns, std::size_t waveform_pulse_width_ns);
			pwm::ErrorCode stop();

			pwm::ErrorCode waveform_params_get(std::size_t& waveform_period_ns, std::size_t& waveform_pulse_width_ns, std::size_t& waveform_duty_cycle_x100);
			[[nodiscard("Called error getter and discarded its return value")]] pwm::ErrorState error_state_get() const;

		private:

			struct SystemState {
				bool pwm_ready = false;
				bool params_set = false;
			};

			struct WaveformState {
				std::size_t period_ns = 0;
				std::size_t pulse_width_ns = 0;
				std::uint8_t duty_cycle_x100 = 0;
			};

			struct pwm_dt_spec dt_spec = {};

			pwm::ErrorState error;
			pwm::PwmSignal::SystemState system;

			pwm::PwmSignal::WaveformState waveform;

	};
}

#endif

/**
*
*	@enum		pwm::ErrorCode
*
*	@brief		Error codes of the module
*
*	@invariant	If an error related to Zephyr's PWM API occurs then the code is prefixed with "Pwm"
*
*/

/**
*
*	@struct		pwm::ErrorState
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
*	@class		pwm::PwmSignal
*
*	@brief		Class for Zephyr's PWM API
*
*	@warning	Since each instance of this class represents a different device there should not exist copies
*
*/

/**
*
*	@fn			pwm::PwmSignal::PwmSignal(struct pwm_dt_spec pwm_dt_spec)
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
*	@fn			pwm::ErrorCode start(std::size_t waveform_period_ns, std::size_t waveform_pulse_width_ns)
*
*	@brief		Method to start the PWM with specific waveform parameters
*
*	@param[in]	waveform_period_ns				Waveform period in nanoseconds
*	@param[in]	waveform_pulse_width_ns			Waveform pulse width in nanoseconds
*
*	@retval		DeviceUnready					If the PWM is not ready to be used
*	@retval		ParamWaveform					If the passed period is 0 or less than the pulse width
*	@retval		PwmSet							If an error occurs when setting the waveform parameters
*	@retval		Ok								If no error occurs
*
*	@post		If the PWM is not ready to be used then error.code is set to DeviceUnready
*	@post		If the passed period is 0 or less than the pulse width then error.code is set to ParamWaveform
*	@post		If an error occurs when setting the waveform parameters with Zephyr's PWM API then its return value is saved in error.return_value and error.code is set to PwmSet
*	@post		On success the passed values are set to the waveform parameters and error.code is set to Ok
*
*/

/**
*
*	@fn			pwm::ErrorCode stop()
*
*	@brief		Method to stop the PWM
*
*	@retval		PwmSet							If an error occurs when setting the waveform parameters
*	@retval		Ok								If no error occurs
*
*	@post		If an error occurs when setting the waveform parameters with Zephyr's PWM API then its return value is saved in error.return_value and error.code is set to PwmSet
*	@post		On success the PWM is stopped and error.code is set to Ok
*
*/

/**
*
*	@fn			pwm::ErrorCode waveform_params_get(std::size_t& waveform_period_ns, std::size_t& waveform_pulse_width_ns, std::size_t& waveform_duty_cycle_x100)
*
*	@brief		Method to obtain the last set waveform parameters
*
*	@param[out]	waveform_period_ns				Reference to the variable where to store the waveform period in nanoseconds
*	@param[out]	waveform_pulse_width_ns			Reference to the variable where to store the waveform pulse width in nanoseconds
*
*	@retval		WaveformNotSet					If the PWM waveform parameters have never been set since the creation of the instance
*	@retval		Ok								If no error occurs
*
*	@post		If the PWM waveform parameters have never been set since the creation of the instance then error.code is set to WaveformNotSet
*	@post		On success the waveform parameter values are assigned to the passed references and error.code is set to Ok
*
*/



/**
*
*	@fn			pwm::ErrorState error_state_get() const
*
*	@brief		Method to obtain the last set waveform parameters
*
*	@return		The full error report
*
*/
