/**
*
*	@file		dashboard_controller.hpp
*
*	@brief		Public API for the dashboard_controller module
*
*	@details	Prints the UI on the dsplay
*
*				Accesses an instance of DisplayGridInterface
*
*/

#ifndef DASHBOARD_CONTROLLER_HPP
#define DASHBOARD_CONTROLLER_HPP

#include "grid_printer_interface.hpp"
#include <cstddef>

namespace dashboard_controller {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		GridUnready,
		FixedUiUnready,
		GridClear,
		GridStringLoad,
		GridPrint,
		ParamTemp,
		ParamSpeed
	};

	class DashboardUi {

		public:

			explicit DashboardUi(grid_printer_interface::DisplayGridInterface& dashboard_grid);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			DashboardUi(const DashboardUi&) = delete;
			DashboardUi(DashboardUi&&) = delete;
			DashboardUi& operator=(const DashboardUi&) = delete;
			DashboardUi& operator=(DashboardUi&&) = delete;

			dashboard_controller::ErrorCode structure_print();
			dashboard_controller::ErrorCode temp_value_print(int temp_c_x10);
			dashboard_controller::ErrorCode speed_value_print(unsigned int speed_rpm);

			[[nodiscard("Called error getter and discarded its return value")]]
			dashboard_controller::ErrorCode error_get() const;


		private:

			struct TempValueState {
				bool negative_sign = false;
				bool first_digit = false;
			};

			struct SpeedValueState {
				bool first_digit = false;
				bool second_digit = false;
				bool third_digit = false;
			};

			struct SystemState {
				bool grid_ready = false;
				bool structure = false;
				dashboard_controller::DashboardUi::TempValueState temp;
				dashboard_controller::DashboardUi::SpeedValueState speed;
			};

			grid_printer_interface::DisplayGridInterface& dashboard_grid;

			dashboard_controller::DashboardUi::SystemState system;
			dashboard_controller::ErrorCode error;

			dashboard_controller::ErrorCode empty_digit_handler(unsigned int& digit, const std::size_t row_idx, const std::size_t column_idx, bool& digit_flag);
	};
}

#endif

/**
*
*	@enum 		dashboard_controller::ErrorCode
*
*	@brief		Error codes of the module
*
*/

/**
*
*	@fn 		dashboard_controller::ErrorCode structure_print(grid_printer_interface::DisplayGridInterface& readings_grid)
*
*	@brief		Prints the fixed part of the UI on the display
*
*	@param[in]	readings_grid							Instance of the DisplayGridInterface class
*
*	@retval		DisplayOperation				If an error occurs whith the DisplayGridInterface class
*	@retval		Ok								If no error occurs
*
*	@pre		readings_grid must be a valid instance of the DisplayGridInterface class correctly initialized
*	@post		If an error occurs whith the DisplayGridInterface class then changes are left on RAM's content
*	@post		On success the title and the units of measurement are printed on the display
*
*/

/**
*
*	@fn 		dashboard_controller::ErrorCode temp_value_print(grid_printer_interface::DisplayGridInterface& readings_grid, std::int16_t temp_c_x10)
*
*	@brief		Prints the value of the tempearature reading on the display
*
*	@param[in]	readings_grid							Instance of the DisplayGridInterface class
*	@param[in]	temp_c_x10						Value of the temperature reading in tenth of Celsius degrees
*
*	@retval		ParamTemp						If the value of the temperature is out of range
*	@retval		DisplayOperation				If an error occurs whith the DisplayGridInterface class
*	@retval		Ok								If no error occurs
*
*	@pre		readings_grid must be a valid instance of the DisplayGridInterface class correctly initialized
*	@post		If the value of the temperature is out of range then RAM's content are left unchanged
*	@post		If an error occurs whith the DisplayGridInterface class then changes are left on RAM's content
*	@post		On success the temperature value is printed on the display
*
*/

/**
*
*	@fn 		dashboard_controller::ErrorCode speed_value_print(grid_printer_interface::DisplayGridInterface& readings_grid, std::uint16_t speed_rpm)
*
*	@brief		Prints the value of the tempearature reading on the display
*
*	@param[in]	readings_grid							Instance of the DisplayGridInterface class
*	@param[in]	speed_rpm						Value of the speed reading in RPM
*
*	@retval		ParamSpeed						If the value of the speed is out of range
*	@retval		DisplayOperation				If an error occurs whith the DisplayGridInterface class
*	@retval		Ok								If no error occurs
*
*	@pre		readings_grid must be a valid instance of the DisplayGridInterface class correctly initialized
*	@post		If the value of the speed is out of range then RAM's content are left unchanged
*	@post		If an error occurs whith the DisplayGridInterface class then changes are left on RAM's content
*	@post		On success the speed value is printed on the display
*
*/
