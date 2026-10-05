/**
*
*	@file		dashboard.cpp
*
*	@brief		Implementation for the dashboard module
*
*	@details	Prints multiple strings and	digits on the display
*
*				Handles integer to string conversion without allocating
*				dynamic memory to enforce embedded systems constraints
*
*/

#include "dashboard.hpp"
#include "grid_printer_interface.hpp"
#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>

dashboard::Dashboard::Dashboard(grid_printer_interface::DisplayGridInterface& dashboard_grid) :
dashboard_grid{dashboard_grid} {

	if (this->dashboard_grid.error_state_get().code != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridUnready;
		return;
	}

	this->system.grid_ready = true;
	this->error = dashboard::ErrorCode::Ok;
}

dashboard::ErrorCode dashboard::Dashboard::structure_print() {

	if (!this->system.grid_ready) {
		this->error = dashboard::ErrorCode::GridUnready;
		return this->error;
	}

	if (this->dashboard_grid.cells_clear() != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridClear;
		return this->error;
	}

	if (this->dashboard_grid.cells_string_write("Readings", 0, 2) != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridStringLoad;
		return this->error;
	}

	if (this->dashboard_grid.cells_string_write("T     . degC", 1, 0) != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridStringLoad;
		return this->error;
	}

	if (this->dashboard_grid.cells_string_write("S        RPM", 2, 0) != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridStringLoad;
		return this->error;
	}

	if (this->dashboard_grid.cells_string_write("N         dB", 3, 0) != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridStringLoad;
		return this->error;
	}

	if (this->dashboard_grid.cells_print() != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridPrint;
		return this->error;
	}

	this->system.structure = true;
	this->error = dashboard::ErrorCode::Ok;
	return this->error;
}

static void digits_extract(int full_value, std::array<unsigned int, 3>& digits) {
	unsigned int divisor = 100;

	full_value = (full_value < 0) ? full_value * -1 : full_value;

	for (std::size_t digit_index = 0; digit_index < digits.size(); digit_index++) {
		digits.at(digit_index) = (!digit_index) ? static_cast<unsigned int>(full_value / divisor) : static_cast<unsigned int>((full_value / divisor) % 10);
		divisor /= 10;
	}
}

static void digits_extract(unsigned int full_value, std::array<unsigned int, 4>& digits) {
	unsigned int divisor = 1000;

	for (std::size_t digit_index = 0; digit_index < digits.size(); digit_index++) {
		digits.at(digit_index) = (!digit_index) ? static_cast<unsigned int>(full_value / divisor) : static_cast<unsigned int>((full_value / divisor) % 10);
		divisor /= 10;
	}
}

static std::string_view digit_to_str_view(unsigned int digit) {

	switch (digit) {

		case 0:

			return "0";

		case 1:

			return "1";

		case 2:

			return "2";

		case 3:

			return "3";

		case 4:

			return "4";

		case 5:

			return "5";

		case 6:

			return "6";

		case 7:

			return "7";

		case 8:

			return "8";

		case 9:

			return "9";
	}

	return "ErrorInternal";
}

dashboard::ErrorCode dashboard::Dashboard::empty_digit_handler(unsigned int& digit, const std::size_t row_idx, const std::size_t column_idx, bool& digit_flag) {

	if (digit) {
		std::string_view digit_str_view = digit_to_str_view(digit);

		if (this->dashboard_grid.cells_string_write(digit_str_view, row_idx, column_idx) != grid_printer_interface::ErrorCode::Ok) {
			this->error = dashboard::ErrorCode::GridStringLoad;
			return this->error;
		}

		digit_flag = (!digit_flag) ? true : digit_flag;
	}

	else if (!digit && digit_flag) {

		if (this->dashboard_grid.cells_string_write(" ", row_idx, column_idx) != grid_printer_interface::ErrorCode::Ok) {
			this->error = dashboard::ErrorCode::GridStringLoad;
			return this->error;
		}

		digit_flag = false;
	}

	this->error = dashboard::ErrorCode::Ok;
	return this->error;
}

dashboard::ErrorCode dashboard::Dashboard::temp_value_print(int temp_c_x10) {
	std::array<unsigned int, 3> digits = {0, 0, 0};

	if (!this->system.structure) {
		this->error = dashboard::ErrorCode::FixedUiUnready;
		return this->error;
	}

	if (temp_c_x10 > 999 || temp_c_x10 < -999) {
		this->error = dashboard::ErrorCode::ParamTemp;
		return this->error;
	}

	if (temp_c_x10 < 0 && !this->system.temp.negative_sign) {

			if (this->dashboard_grid.cells_string_write("-", 1, 3) != grid_printer_interface::ErrorCode::Ok) {
				this->error = dashboard::ErrorCode::GridStringLoad;
				return this->error;
			}

			this->system.temp.negative_sign = true;
	}

	else if (temp_c_x10 >= 0 && this->system.temp.negative_sign) {

			if (this->dashboard_grid.cells_string_write(" ", 1, 3) != grid_printer_interface::ErrorCode::Ok) {
				this->error = dashboard::ErrorCode::GridStringLoad;
				return this->error;
			}

			this->system.temp.negative_sign = false;
	}

	digits_extract(temp_c_x10, digits);

	if (this->empty_digit_handler(digits.at(0), 1, 4, this->system.temp.first_digit) != dashboard::ErrorCode::Ok) {
		return this->error;
	}

	std::string_view digit2_str_view = digit_to_str_view(digits.at(1));

	if (this->dashboard_grid.cells_string_write(digit2_str_view, 1, 5) != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridStringLoad;
		return this->error;
	}

	std::string_view digit3_str_view = digit_to_str_view(digits.at(2));

	if (this->dashboard_grid.cells_string_write(digit3_str_view, 1, 7) != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridStringLoad;
		return this->error;
	}

	if (this->dashboard_grid.cells_print() != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridPrint;
		return this->error;
	}

	this->error = dashboard::ErrorCode::Ok;
	return this->error;
}

dashboard::ErrorCode dashboard::Dashboard::speed_value_print(unsigned int speed_rpm) {
	std::array<unsigned int, 4> digits = {0, 0, 0, 0};

	if (!this->system.structure) {
		this->error = dashboard::ErrorCode::FixedUiUnready;
		return this->error;
	}

	if (speed_rpm > 9999) {
		this->error = dashboard::ErrorCode::ParamSpeed;
		return this->error;
	}

	digits_extract(speed_rpm, digits);

	if (this->empty_digit_handler(digits.at(0), 2, 5, this->system.speed.first_digit) != dashboard::ErrorCode::Ok) {
		return this->error;
	}

	if (!this->system.speed.first_digit) {

		if (this->empty_digit_handler(digits.at(1), 2, 6, this->system.speed.second_digit) != dashboard::ErrorCode::Ok) {
			return this->error;
 		}
	}

	else {
		std::string_view digit2_str_view = digit_to_str_view(digits.at(1));

		this->system.speed.second_digit = true;

		if (this->dashboard_grid.cells_string_write(digit2_str_view, 2, 6) != grid_printer_interface::ErrorCode::Ok) {
			this->error = dashboard::ErrorCode::GridStringLoad;
			return this->error;
		}
	}

	if (!this->system.speed.second_digit) {

		if (this->empty_digit_handler(digits.at(2), 2, 7, this->system.speed.third_digit) != dashboard::ErrorCode::Ok) {
			return this->error;
		}
	}

	else {
		std::string_view digit3_str_view = digit_to_str_view(digits.at(2));

		this->system.speed.third_digit = true;

		if (this->dashboard_grid.cells_string_write(digit3_str_view, 2, 7) != grid_printer_interface::ErrorCode::Ok) {
			this->error = dashboard::ErrorCode::GridStringLoad;
			return this->error;
		}
	}

	std::string_view digit4_str_view = digit_to_str_view(digits.at(3));

	if (this->dashboard_grid.cells_string_write(digit4_str_view, 2, 8) != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridStringLoad;
		return this->error;
	}

	if (this->dashboard_grid.cells_print() != grid_printer_interface::ErrorCode::Ok) {
		this->error = dashboard::ErrorCode::GridPrint;
		return this->error;
	}

	this->error = dashboard::ErrorCode::Ok;
	return this->error;
}

dashboard::ErrorCode dashboard::Dashboard::error_get() const {
	return this->error;
}

