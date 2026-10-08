/**
*
*	@file		readings_reporter.hpp
*
*	@brief		Public API for the readings_reporter module
*
*	@details	Prints the UI on the dsplay
*
*				Accesses an instance of DisplayGridInterface
*
*/

#ifndef READINGS_REPORTER_HPP
#define READINGS_REPORTER_HPP

#include "grid_printer_interface.hpp"
#include <cstddef>

namespace readings_reporter {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		StructureUnready,
		StructureReady,
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
			readings_reporter::ErrorCode temp_value_print(int temp_c_x10);
			readings_reporter::ErrorCode speed_value_print(unsigned int speed_rpm);

			[[nodiscard("Called error getter and discarded its return value")]]
			readings_reporter::ErrorCode error_get() const;

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
				bool structure_ready = false;
				readings_reporter::Dashboard::TempValueState temp;
				readings_reporter::Dashboard::SpeedValueState speed;
			};

			grid_printer_interface::DisplayGridInterface& dashboard_grid;

			readings_reporter::Dashboard::SystemState system;
			readings_reporter::ErrorCode error;

			readings_reporter::ErrorCode empty_digit_handler(unsigned int& digit, const std::size_t row_idx, const std::size_t column_idx, bool& digit_flag);
	};
}

#endif

/**
*
*	@enum 		readings_reporter::ErrorCode
*
*	@brief		Error codes of the module
*
*/

/**
*
*	@fn 		readings_reporter::ErrorCode structure_print(grid_printer_interface::DisplayGridInterface& readings_grid)
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
*	@fn 		readings_reporter::ErrorCode temp_value_print(grid_printer_interface::DisplayGridInterface& readings_grid, std::int16_t temp_c_x10)
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
*	@fn 		readings_reporter::ErrorCode speed_value_print(grid_printer_interface::DisplayGridInterface& readings_grid, std::uint16_t speed_rpm)
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
