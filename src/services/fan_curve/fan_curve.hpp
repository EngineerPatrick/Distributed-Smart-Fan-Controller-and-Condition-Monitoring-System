#ifndef FAN_CURVE_HPP
#define FAN_CURVE_HPP

#include <cstddef>
#include <array>

namespace fan_curve {

	const std::size_t NODE_COUNT = 10U;
	const int MAX_TEMP_C_X10 = 999;
	const int MIN_TEMP_C_X10 = -999;
	const int MAX_SPEED_RPM = 9999U;

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		ParamTempRange,
		ParamSpeedRange,
		ParamTempSequence,
		ParamSpeedSequence,
		CurveUnready
	};

	struct Node {
		int temp_c_x10 = 0;
		unsigned int speed_rpm = 0;
	};

	class FanCurve {

		public:

			explicit FanCurve(const std::array<fan_curve::Node, fan_curve::NODE_COUNT>& fan_curve_nodes);

			fan_curve::ErrorCode target_speed_rpm_get(int measured_temp_c_x10, unsigned int& target_speed_rpm);

			[[nodiscard("Called error getter and discarded its return value")]]
			fan_curve::ErrorCode error_get() const;

		private:

			struct SystemState {
				bool curve_ready = false;
			};

			const std::array<fan_curve::Node, fan_curve::NODE_COUNT> nodes;

			fan_curve::FanCurve::SystemState system;
			fan_curve::ErrorCode error = fan_curve::ErrorCode::Ok;
	};
}

#endif
