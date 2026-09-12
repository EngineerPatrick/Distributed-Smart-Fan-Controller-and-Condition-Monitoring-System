#include "ui_display.hpp"
#include "monochrome_display.hpp"
#include <cstdint>
#include <string_view>

ui_display::ErrorCode ui_display::units_write(monochrome_display::MonochromeDisplay& mc_obj) {

	if (mc_obj.grid_clear() != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write("Readings", 0, 2) != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write("T     . degC", 1, 0) != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write("S        RPM", 2, 0) != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write("N         dB", 3, 0) != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_print() != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	return ui_display::ErrorCode::Ok;
}

static void digit_extract(std::uint32_t full_value, std::uint8_t (&dig_arr)[3]) {

	if (full_value / 1000 == 0) {
		dig_arr[0] = 0;
		dig_arr[1] = full_value / 100;
		dig_arr[2] = (full_value / 10) % 10;
	}

	else {
		full_value /= 10;
		dig_arr[0] = full_value / 100;
		dig_arr[1] = (full_value / 10) % 10;
		dig_arr[2] = full_value % 10;
	}
}

static std::string_view digit_to_str_view(std::uint8_t dig) {

	if (dig > 9) {
		return "Error: digit out of range";
	}

	switch (dig) {

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

	return "Error: internal";
}

ui_display::ErrorCode ui_display::temp_write(monochrome_display::MonochromeDisplay& mc_obj, std::uint32_t temp) {
	std::uint8_t dig_arr[3] = {0, 0, 0};

	digit_extract(temp, dig_arr);
	std::string_view dig1_str_view = digit_to_str_view(dig_arr[0]);
	std::string_view dig2_str_view = digit_to_str_view(dig_arr[1]);
	std::string_view dig3_str_view = digit_to_str_view(dig_arr[2]);

	if (mc_obj.grid_string_write(dig1_str_view, 1, 4) != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write(dig2_str_view, 1, 5) != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_string_write(dig3_str_view, 1, 7) != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	if (mc_obj.grid_print() != monochrome_display::ErrorCode::Ok) {
		return ui_display::ErrorCode::DisplayOperation;
	}

	return ui_display::ErrorCode::Ok;
}
