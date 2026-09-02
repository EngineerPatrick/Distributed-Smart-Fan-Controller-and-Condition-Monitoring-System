#include <cstdint>
#include <cstddef>
#include <string_view>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/display/cfb.h>
#include "char_framebuff.hpp"

MonochromeDisplay::MonochromeDisplay(const struct device* const monochrome_display_dt_spec) :

dt_spec{monochrome_display_dt_spec},

error_report{DisplayErrors::Ok, 0} {

	if (!device_is_ready(monochrome_display_dt_spec)) {
		this->error_report.display = DisplayErrors::Device;
	}

	this->error_report.cfb = cfb_framebuffer_init(monochrome_display_dt_spec);

	if (this->error_report.cfb != 0) {
		this->error_report.display = DisplayErrors::CfbInit;
	}

}

DisplayErrors MonochromeDisplay::screen_clear(void) {

	if (this->error_report.display == DisplayErrors::Device || this->error_report.display == DisplayErrors::CfbInit) {
		return this->error_report.display;
	}

	this->error_report.cfb = cfb_framebuffer_clear(this->dt_spec, true);

	if (this->error_report.cfb != 0) {
		this->error_report.display = DisplayErrors::CfbOperation;
	}

	return this->error_report.display;
}

DisplayErrors MonochromeDisplay::string_load(const std::string_view input_string, const size_t row_idx, const size_t column_idx) {
	uint8_t font_width_px = 0;
	uint8_t font_height_px = 0;
	size_t display_width_px = 0;
	size_t display_height_px = 0;

	if (this->error_report.display == DisplayErrors::Device || this->error_report.display == DisplayErrors::CfbInit) {
		return this->error_report.display;
	}

	this->error_report.cfb = cfb_framebuffer_set_font(this->dt_spec, 0);

	if (this->error_report.cfb != 0) {
		this->error_report.display = DisplayErrors::CfbOperation;
		return this->error_report.display;
	}

	this->error_report.cfb = cfb_set_kerning(this->dt_spec, 0);

	if (this->error_report.cfb != 0) {
		this->error_report.display = DisplayErrors::CfbOperation;
		return this->error_report.display;
	}

	this->error_report.cfb = cfb_get_font_size(this->dt_spec, 0, &font_width_px, &font_height_px);

	if (this->error_report.cfb != 0) {
		this->error_report.display = DisplayErrors::CfbOperation;
		return this->error_report.display;
	}

	display_width_px = cfb_get_display_parameter(this->dt_spec, CFB_DISPLAY_WIDTH);
	display_height_px = cfb_get_display_parameter(this->dt_spec, CFB_DISPLAY_HEIGHT);

	if (row_idx > (display_height_px / font_height_px) - 1 || input_string.size() > (display_width_px / font_width_px) - column_idx) {
		this->error_report.display = DisplayErrors::Param;
		return this->error_report.display;
	}

	char input_string_c_str[input_string.size() + 1];
	input_string.copy(input_string_c_str, input_string.size());
	input_string_c_str[input_string.size()] = '\0';

	this->error_report.cfb = cfb_print(this->dt_spec, input_string_c_str, static_cast<int16_t>(column_idx * font_width_px), static_cast<int16_t>(row_idx * font_height_px));

	if (this->error_report.cfb != 0) {
		this->error_report.display = DisplayErrors::CfbOperation;
	}

	return this->error_report.display;
}

DisplayErrors MonochromeDisplay::screen_print(void) {

	if (this->error_report.display == DisplayErrors::Device || this->error_report.display == DisplayErrors::CfbInit) {
		return this->error_report.display;
	}

	this->error_report.cfb = cfb_framebuffer_finalize(this->dt_spec);

	if (this->error_report.cfb != 0) {
		this->error_report.display = DisplayErrors::CfbOperation;
	}

	return this->error_report.display;
}
