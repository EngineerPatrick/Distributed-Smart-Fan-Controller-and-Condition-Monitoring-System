#include "report_thread.hpp"
#include "readings_reporter.hpp"

report_thread::ReportThread::ReportThread(readings_reporter::Dashboard& controller_state) : controller_state{controller_state} {

	if (this->controller_state.error_get() != readings_reporter::ErrorCode::Ok) {
		this->error = report_thread::ErrorCode::DashboardUnready;
		return;
	}

	this->system.dashboard_acquired = true;
	this->error = report_thread::ErrorCode::Ok;
}

report_thread::ErrorCode report_thread::ReportThread::boot() {

	if (!this->system.dashboard_acquired) {
		this->error = report_thread::ErrorCode::DashboardUnready;
		return this->error;
	}

	if (this->controller_state.structure_print() != readings_reporter::ErrorCode::Ok) {
		this->error = report_thread::ErrorCode::StructurePrint;
		return this->error;
	}

	this->system.update_ready = true;
	this->error = report_thread::ErrorCode::Ok;
	return this->error;
}

report_thread::ErrorCode report_thread::ReportThread::update(int temp_c_x10, unsigned int speed_rpm) {

	if (!this->system.update_ready) {
		this->error = report_thread::ErrorCode::UpdateUnready;
		return this->error;
	}

	if (this->controller_state.temp_value_print(temp_c_x10) != readings_reporter::ErrorCode::Ok) {
		this->error = report_thread::ErrorCode::TemperaturePrint;
		return this->error;
	}

	if (this->controller_state.speed_value_print(speed_rpm) != readings_reporter::ErrorCode::Ok) {
		this->error = report_thread::ErrorCode::SpeedPrint;
		return this->error;
	}

	this->error = report_thread::ErrorCode::Ok;
	return this->error;
}

report_thread::ErrorCode report_thread::ReportThread::error_get() const {
	return this->error;
}
