#ifndef READINGS_REPORTER_HPP
#define READINGS_REPORTER_HPP

#include "grid_printer_interface.hpp"
#include <cstddef>

namespace readings_reporter {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		StructureUnready,
		ParamTemp,
		ParamSpeed,
		GridUnready,
		GridClear,
		GridStringLoad,
		GridPrint
	};

	class Dashboard {

		public:

			explicit Dashboard(grid_printer_interface::DisplayGridInterface& dashboard_grid);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			Dashboard(const Dashboard&) = delete;
			Dashboard(Dashboard&&) = delete;
			Dashboard& operator=(const Dashboard&) = delete;
			Dashboard& operator=(Dashboard&&) = delete;

			readings_reporter::ErrorCode structure_print();
			readings_reporter::ErrorCode temp_value_print(const int temp_c_x10);
			readings_reporter::ErrorCode speed_value_print(const unsigned int speed_rpm);

			[[nodiscard("Called error getter and discarded its return value")]]
			readings_reporter::ErrorCode error_get() const;

		private:

			struct TempValueState {
				int value_c_x10 = 0;
				bool negative_sign = false;
				bool first_digit = false;
			};

			struct SpeedValueState {
				unsigned int value_rpm = 0;
				bool first_digit = false;
				bool second_digit = false;
				bool third_digit = false;
			};

			struct SystemState {
				bool grid_acquired = false;
				bool structure_ready = false;
			};

			grid_printer_interface::DisplayGridInterface& dashboard_grid;

			readings_reporter::Dashboard::SystemState system;
			readings_reporter::Dashboard::TempValueState temp;
			readings_reporter::Dashboard::SpeedValueState speed;
			readings_reporter::ErrorCode error = readings_reporter::ErrorCode::Ok;

			readings_reporter::ErrorCode empty_digit_handler(unsigned int& digit, const std::size_t row_idx, const std::size_t column_idx, bool& digit_flag);
	};
}

#endif
