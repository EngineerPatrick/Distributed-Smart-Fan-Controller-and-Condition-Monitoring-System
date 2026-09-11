#include "ui_display.hpp"
#include "monochrome_display.hpp"

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
