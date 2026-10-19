#include "control_thread.hpp"
#include "fan_controller.hpp"
#include "fan_curve.hpp"
#include "feedback_controller.hpp"
#include "temperature_reader_interface.hpp"

control_thread::ControlThread::ControlThread(
	fan_controller::FourWireFan& control_fan,
	temperature_reader_interface::TemperatureSignalInterface& temperature_signal,
	fan_curve::FanCurve& fan_curve,
	feedback_controller::ClosedLoopSystem& smart_controller
) :
control_fan {control_fan},
temperature_signal{temperature_signal},
fan_curve{fan_curve},
smart_controller{smart_controller},
fan_metrics{this->control_fan.metrics_get()} {

	if (this->control_fan.error_state_get().general != fan_controller::ErrorCode::Ok)  {
		this->error = control_thread::ErrorCode::ControlFanUnready;
		return;
	}

	this->system.control_fan_acquired = true;

	if (this->temperature_signal.error_state_get().code != temperature_reader_interface::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::TemperatureReaderUnready;
		return;
	}

	this->system.temperature_signal_acquired = true;

	if (this->fan_curve.error_get() != fan_curve::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::FanCurveUnready;
		return;
	}

	this->system.fan_curve_acquired = true;
	this->error = control_thread::ErrorCode::Ok;
}

control_thread::ErrorCode control_thread::ControlThread::boot() {

	if (!this->system.control_fan_acquired || !this->system.temperature_signal_acquired || !this->system.fan_curve_acquired) {
		this->error = control_thread::ErrorCode::BootUnready;
		return this->error;
	}

	if (this->control_fan.boot().general != fan_controller::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::ControlFanBoot;
		return this->error;
	}

	this->system.cycle_ready = true;
	this->error = control_thread::ErrorCode::Ok;
	return this->error;
}

control_thread::ErrorCode control_thread::ControlThread::cycle(int& temp_c_x100, unsigned int& measured_speed_rpm, unsigned int& target_speed_rpm) {
	fan_controller::ErrorCode fan_controller_error = fan_controller::ErrorCode::Ok;

	if (!this->system.cycle_ready) {
		this->error = control_thread::ErrorCode::CycleUnready;
		return this->error;
	}

	if (this->temperature_signal.single_read(this->control.temp_c_x100) != temperature_reader_interface::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::TemperatureReaderValue;
		return this->error;
	}

	temp_c_x100 = this->control.temp_c_x100;

	fan_controller_error = this->control_fan.speed_measure(this->control.measured_speed_rpm).general;

	if (fan_controller_error == fan_controller::ErrorCode::NewSpeedUnready) {
		this->error = control_thread::ErrorCode::ControlFanSpeedUnready;
		return this->error;
	}

	if (fan_controller_error != fan_controller::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::ControlFanSpeedMeasure;
		return this->error;
	}

	measured_speed_rpm = this->control.measured_speed_rpm;

	this->control.target_speed.previous_rpm = this->control.target_speed.current_rpm;

	if (this->fan_curve.target_speed_compute(this->control.temp_c_x100 / 10, this->control.target_speed.current_rpm) != fan_curve::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::FanCurveSpeedGet;
		return this->error;
 	}

	target_speed_rpm = this->control.target_speed.current_rpm;

 	if (this->control.target_speed.previous_rpm >= this->control.target_speed.current_rpm) {
		this->control.target_speed.delta_rpm = this->control.target_speed.previous_rpm - this->control.target_speed.current_rpm;
 	}

 	else {
		this->control.target_speed.delta_rpm = this->control.target_speed.current_rpm - this->control.target_speed.previous_rpm;
 	}

	if (!this->system.first_cycle) {
		this->smart_controller.target_signal_update(this->control.target_speed.current_rpm);
		this->control.duty_cycle_x100 = (this->control.target_speed.current_rpm * 100) / (this->fan_metrics.max_speed_rpm - this->fan_metrics.min_speed_rpm);

		if (this->control_fan.duty_cycle_update(this->control.duty_cycle_x100).general != fan_controller::ErrorCode::Ok) {
			this->error = control_thread::ErrorCode::ControlFanDutyUpdate;
			return this->error;
		}

		this->system.first_cycle = true;
	}

	else {

	 	if (this->control.target_speed.delta_rpm > 300) {
			this->smart_controller.target_signal_update(this->control.target_speed.current_rpm);

			this->control.duty_cycle_x100 = ((this->control.target_speed.current_rpm - this->fan_metrics.min_speed_rpm) * 100) /
				(this->fan_metrics.max_speed_rpm - this->fan_metrics.min_speed_rpm);

			if (this->control_fan.duty_cycle_update(this->control.duty_cycle_x100).general != fan_controller::ErrorCode::Ok) {
				this->error = control_thread::ErrorCode::ControlFanDutyUpdate;
				return this->error;
			}
	 	}

	 	else {
			this->control.target_speed.current_rpm = this->control.target_speed.previous_rpm;
			this->control.feedback = this->smart_controller.feedback_generate(this->control.measured_speed_rpm);

			switch (this->control.feedback) {

				case feedback_controller::FeedbackCode::StepUp:

					if (this->control.duty_cycle_x100 + 1 <= 100) {
						this->control.duty_cycle_x100++;

						if (this->control_fan.duty_cycle_update(this->control.duty_cycle_x100).general != fan_controller::ErrorCode::Ok) {
							this->error = control_thread::ErrorCode::ControlFanDutyUpdate;
							return this->error;
						}
					}

					break;

				case feedback_controller::FeedbackCode::StepDown:

					if (this->control.duty_cycle_x100 >= 1) {
						this->control.duty_cycle_x100--;

						if (this->control_fan.duty_cycle_update(this->control.duty_cycle_x100).general != fan_controller::ErrorCode::Ok) {
							this->error = control_thread::ErrorCode::ControlFanDutyUpdate;
							return this->error;
						}
					}

					break;

				default:

					;
			}
	 	}
	}

	this->error = control_thread::ErrorCode::Ok;
	return this->error;
}

control_thread::ErrorCode control_thread::ControlThread::stop() {

	if (this->control_fan.stop().general != fan_controller::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::ControlFanStop;
		return this->error;
	}

	this->error = control_thread::ErrorCode::Ok;
	return this->error;
}

control_thread::ErrorCode control_thread::ControlThread::error_get() const {
	return this->error;
}
