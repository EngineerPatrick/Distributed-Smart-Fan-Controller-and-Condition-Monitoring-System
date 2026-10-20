#ifndef PWM_GENERATOR_HPP
#define PWM_GENERATOR_HPP

#include "pwm_generator_interface.hpp"
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

namespace pwm_generator {

	class PwmSignal : public pwm_generator_interface::PwmSignalInterface {

		public:

			explicit PwmSignal(struct pwm_dt_spec timer_device);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			PwmSignal(const PwmSignal&) = delete;
			PwmSignal(PwmSignal&&) = delete;
			PwmSignal& operator=(const PwmSignal&) = delete;
			PwmSignal& operator=(PwmSignal&&) = delete;

			pwm_generator_interface::ErrorCode set(unsigned long long int waveform_period_ns, unsigned long long int waveform_pulse_width_ns) override;
			pwm_generator_interface::ErrorCode stop() override;

			void params_get(unsigned long long int& waveform_period_ns, unsigned long long int& waveform_pulse_width_ns) const override;
			[[nodiscard("Called error getter and discarded its return value")]]
			pwm_generator_interface::ErrorState error_state_get() const override;

		private:

			struct SystemState {
				bool pwm_acquired = false;
				bool pwm_ready = false;
			};

			struct WaveformState {
				unsigned long long int period_ns = 0;
				unsigned long long int pulse_width_ns = 0;
			};

			struct pwm_dt_spec timer = {};

			pwm_generator_interface::ErrorState error;
			pwm_generator::PwmSignal::SystemState system;
			pwm_generator::PwmSignal::WaveformState waveform;
	};
}

#endif
