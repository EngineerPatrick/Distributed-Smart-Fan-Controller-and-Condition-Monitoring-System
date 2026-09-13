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

static bool negative_sign = false;
static bool first_digit = false;

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

static void digit_extract(std::int16_t full_value, std::array<uint8_t, 3>& digits) {
	uint8_t divisor = 100;

	full_value = (full_value < 0) ? full_value * -1 : full_value;

	for (std::size_t i = 0; i < digits.size(); i++) {
		digits.at(i) = (!i) ? (full_value / divisor) : ((full_value / divisor) % 10);
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

display_ui::ErrorCode display_ui::temp_value_print(monochrome_display::MonochromeDisplay& mc_obj, std::int16_t temp_c_x10) {
	std::array<uint8_t, 3> digits = {0, 0, 0};

	if (temp_c_x10 > 999 || temp_c_x10 < -999) {
		return display_ui::ErrorCode::ParamTemp;
	}

	if (temp_c_x10 < 0 && !negative_sign) {

			if (mc_obj.grid_string_write("-", 1, 3) != monochrome_display::ErrorCode::Ok) {
				return display_ui::ErrorCode::DisplayOperation;
			}

			negative_sign = true;
	}

	else if (temp_c_x10 >= 0 && negative_sign) {

			if (mc_obj.grid_string_write(" ", 1, 3) != monochrome_display::ErrorCode::Ok) {
				return display_ui::ErrorCode::DisplayOperation;
			}

			negative_sign = false;
	}

	digit_extract(temp_c_x10, digits);

	if (digits.at(0)) {
		std::string_view digit1_str_view = digit_to_str_view(digits.at(0));

		if (mc_obj.grid_string_write(digit1_str_view, 1, 4) != monochrome_display::ErrorCode::Ok) {
			return display_ui::ErrorCode::DisplayOperation;
		}

		first_digit = (!first_digit) ? true : first_digit;
	}

	else if (!digits.at(0) && first_digit) {

		if (mc_obj.grid_string_write(" ", 1, 4) != monochrome_display::ErrorCode::Ok) {
			return display_ui::ErrorCode::DisplayOperation;
		}

		first_digit = false;
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
