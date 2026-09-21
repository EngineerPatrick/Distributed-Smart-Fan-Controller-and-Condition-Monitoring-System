/**
*
*	@file		display_ui.cpp
*
*	@brief		Implementation for the display_ui module
*
*	@details	Prints multiple strings and	digits on the display
*
*				Handles integer to string conversion without allocating
*				dynamic memory to enforce embedded systems constraints
*
*/

#include "display_ui.hpp"
#include "monochrome_display.hpp"
#include <cstdint>
#include <cstddef>
#include <array>
#include <string_view>

struct TempValue {
	bool negative_sign = false;
	bool first_digit = false;
};

struct SpeedValue {
	bool first_digit = false;
	bool second_digit = false;
	bool third_digit = false;

};

static TempValue temp;

static SpeedValue speed;

display_ui::ErrorCode display_ui::fixed_ui_print(monochrome_display::MonochromeDisplay& mc_obj) {

	if (mc_obj.grid_clear() != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write("Readings", 0, 2) != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write("T     . degC", 1, 0) != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write("S        RPM", 2, 0) != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write("N         dB", 3, 0) != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_print() != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	return display_ui::ErrorCode::Ok;
}

static void digits_extract(std::int16_t full_value, std::array<std::uint8_t, 3>& digits) {
	std::uint8_t divisor = 100;

	full_value = (full_value < 0) ? full_value * -1 : full_value;

	for (std::size_t i = 0; i < digits.size(); i++) {
		digits.at(i) = (!i) ? static_cast<std::uint8_t>(full_value / divisor) : static_cast<std::uint8_t>((full_value / divisor) % 10);
		divisor /= 10;
	}
}

static void digits_extract(std::uint16_t full_value, std::array<std::uint8_t, 4>& digits) {
	std::uint16_t divisor = 1000;

	for (std::size_t i = 0; i < digits.size(); i++) {
		digits.at(i) = (!i) ? static_cast<std::uint8_t>(full_value / divisor) : static_cast<std::uint8_t>((full_value / divisor) % 10);
		divisor /= 10;
	}
}

static std::string_view digit_to_str_view(std::uint8_t digit) {

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

static display_ui::ErrorCode empty_digit_handler(std::uint8_t& digit, const std::size_t row_idx, const std::size_t column_idx, bool& digit_flag, monochrome_display::MonochromeDisplay& mc_obj) {

	if (digit) {
		std::string_view digit_str_view = digit_to_str_view(digit);

		if (mc_obj.grid_string_write(digit_str_view, row_idx, column_idx) != monochrome_display::ErrorCode::Ok) {
			return display_ui::ErrorCode::DisplayOperation;
		}

		digit_flag = (!digit_flag) ? true : digit_flag;
	}

	else if (!digit && digit_flag) {

		if (mc_obj.grid_string_write(" ", row_idx, column_idx) != monochrome_display::ErrorCode::Ok) {
			return display_ui::ErrorCode::DisplayOperation;
		}

		digit_flag = false;
	}

	return display_ui::ErrorCode::Ok;
}

display_ui::ErrorCode display_ui::temp_value_print(monochrome_display::MonochromeDisplay& mc_obj, std::int16_t temp_c_x10) {
	display_ui::ErrorCode error_code = display_ui::ErrorCode::Ok;
	std::array<std::uint8_t, 3> digits = {0, 0, 0};

	if (temp_c_x10 > 999 || temp_c_x10 < -999) {
		return display_ui::ErrorCode::ParamTemp;
	}

	if (temp_c_x10 < 0 && !temp.negative_sign) {

			if (mc_obj.grid_string_write("-", 1, 3) != monochrome_display::ErrorCode::Ok) {
				return display_ui::ErrorCode::DisplayOperation;
			}

			temp.negative_sign = true;
	}

	else if (temp_c_x10 >= 0 && temp.negative_sign) {

			if (mc_obj.grid_string_write(" ", 1, 3) != monochrome_display::ErrorCode::Ok) {
				return display_ui::ErrorCode::DisplayOperation;
			}

			temp.negative_sign = false;
	}

	digits_extract(temp_c_x10, digits);

	error_code = empty_digit_handler(digits.at(0), 1, 4, temp.first_digit, mc_obj);

	if (error_code != display_ui::ErrorCode::Ok) {
		return error_code;
	}

	std::string_view digit2_str_view = digit_to_str_view(digits.at(1));

	if (mc_obj.grid_string_write(digit2_str_view, 1, 5) != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	std::string_view digit3_str_view = digit_to_str_view(digits.at(2));

	if (mc_obj.grid_string_write(digit3_str_view, 1, 7) != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_print() != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	return display_ui::ErrorCode::Ok;
}

display_ui::ErrorCode display_ui::speed_value_print(monochrome_display::MonochromeDisplay& mc_obj, std::uint16_t speed_rpm) {
	display_ui::ErrorCode error_code = display_ui::ErrorCode::Ok;
	std::array<std::uint8_t, 4> digits = {0, 0, 0, 0};

	if (speed_rpm > 9999) {
		return display_ui::ErrorCode::ParamSpeed;
	}

	digits_extract(speed_rpm, digits);

	error_code = empty_digit_handler(digits.at(0), 2, 5, speed.first_digit, mc_obj);

	if (error_code != display_ui::ErrorCode::Ok) {
		return error_code;
	}

	if (!speed.first_digit) {
		error_code = empty_digit_handler(digits.at(1), 2, 6, speed.second_digit, mc_obj);

		if (error_code != display_ui::ErrorCode::Ok) {
			return error_code;
		}
	}

	else {
		std::string_view digit2_str_view = digit_to_str_view(digits.at(1));

		if (mc_obj.grid_string_write(digit2_str_view, 2, 6) != monochrome_display::ErrorCode::Ok) {
			return display_ui::ErrorCode::DisplayOperation;
		}
	}

	if (!speed.second_digit) {
		error_code = empty_digit_handler(digits.at(2), 2, 7, speed.third_digit, mc_obj);

		if (error_code != display_ui::ErrorCode::Ok) {
			return error_code;
		}
	}

	else {
		std::string_view digit3_str_view = digit_to_str_view(digits.at(2));

		if (mc_obj.grid_string_write(digit3_str_view, 2, 7) != monochrome_display::ErrorCode::Ok) {
			return display_ui::ErrorCode::DisplayOperation;
		}
	}

	std::string_view digit4_str_view = digit_to_str_view(digits.at(3));

	if (mc_obj.grid_string_write(digit4_str_view, 2, 8) != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_print() != monochrome_display::ErrorCode::Ok) {
		return display_ui::ErrorCode::DisplayOperation;
	}

	return display_ui::ErrorCode::Ok;
}
