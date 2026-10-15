#include "control_thread.hpp"
#include "fan_controller.hpp"
#include "fan_curve.hpp"
#include "temperature_reader_interface.hpp"

control_thread::ControlThread::ControlThread(
	fan_controller::FourWireFan& control_fan,
	temperature_reader_interface::TemperatureSignalInterface& temperature_signal,
	fan_curve::FanCurve& fan_curve
) :
control_fan {control_fan},
temperature_signal{temperature_signal},
fan_curve{fan_curve} {

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

	if (!this->system.cycle_ready) {
		this->error = control_thread::ErrorCode::CycleUnready;
		return this->error;
	}

	if (this->temperature_signal.single_read(temp_c_x100) != temperature_reader_interface::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::TemperatureReaderValue;
		return this->error;
	}

	if (this->fan_curve.target_speed_compute(temp_c_x100 / 10, target_speed_rpm) != fan_curve::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::FanCurveSpeedGet;
		return this->error;
 	}

	if (this->control_fan.speed_measure(measured_speed_rpm).general != fan_controller::ErrorCode::Ok) {
		this->error = control_thread::ErrorCode::ControlFanSpeedMeasure;
		return this->error;
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
