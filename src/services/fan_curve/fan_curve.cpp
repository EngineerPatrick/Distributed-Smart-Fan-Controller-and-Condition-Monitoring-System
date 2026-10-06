#include "fan_curve.hpp"
#include <cstddef>
#include <array>

fan_curve::FanCurve::FanCurve(const std::array<fan_curve::Node, fan_curve::NODE_COUNT>& fan_curve_nodes) :
nodes{fan_curve_nodes} {

	for (std::size_t node_idx = 0; node_idx < this->nodes.size(); node_idx++) {

		if (this->nodes.at(node_idx).temp_c_x10 < fan_curve::MIN_TEMP_C_X10 || this->nodes.at(node_idx).temp_c_x10 > fan_curve::MAX_TEMP_C_X10) {
			this->error = fan_curve::ErrorCode::ParamTempRange;
			return;
		}

		if (this->nodes.at(node_idx).speed_rpm > fan_curve::MAX_SPEED_RPM) {
			this->error = fan_curve::ErrorCode::ParamSpeedRange;
			return;
		}

		if (node_idx < this->nodes.size() - 1) {

			if (this->nodes.at(node_idx + 1).temp_c_x10 <= this->nodes.at(node_idx).temp_c_x10) {
				this->error = fan_curve::ErrorCode::ParamTempSequence;
				return;
			}

			if (this->nodes.at(node_idx + 1).speed_rpm < this->nodes.at(node_idx).speed_rpm) {
				this->error = fan_curve::ErrorCode::ParamSpeedSequence;
				return;
			}

		}
	}

	this->system.curve_ready = true;
	this->error = fan_curve::ErrorCode::Ok;
}

fan_curve::ErrorCode fan_curve::FanCurve::target_speed_rpm_get(int measured_temp_c_x10, unsigned int& target_speed_rpm) {
	unsigned int dx = 0;
	unsigned int dy = 0;
	unsigned int dT = 0;

	if (!this->system.curve_ready) {
		this->error = fan_curve::ErrorCode::CurveUnready;
		return this->error;
	}

	if (measured_temp_c_x10 <= this->nodes.at(0).temp_c_x10) {
		target_speed_rpm = this->nodes.at(0).speed_rpm;
		this->error = fan_curve::ErrorCode::Ok;
		return this->error;
	}

	if (measured_temp_c_x10 >= this->nodes.at(this->nodes.size() - 1).temp_c_x10) {
		target_speed_rpm = this->nodes.at(this->nodes.size() - 1).speed_rpm;
		this->error = fan_curve::ErrorCode::Ok;
		return this->error;
	}

	for (std::size_t node_idx = 0; node_idx < this->nodes.size() - 1; node_idx++) {

		if (measured_temp_c_x10 < this->nodes.at(node_idx + 1).temp_c_x10) {
			dx = static_cast<unsigned int>(this->nodes.at(node_idx + 1).temp_c_x10 - this->nodes.at(node_idx).temp_c_x10);
			dy = this->nodes.at(node_idx + 1).speed_rpm - this->nodes.at(node_idx).speed_rpm;
			dT = static_cast<unsigned int>(measured_temp_c_x10 - this->nodes.at(node_idx).temp_c_x10);

			target_speed_rpm = this->nodes.at(node_idx).speed_rpm + (dT * dy) / dx;
			break;
		}
	}

	this->error = fan_curve::ErrorCode::Ok;
	return this->error;
}

fan_curve::ErrorCode fan_curve::FanCurve::error_get() const {
	return this->error;
}
