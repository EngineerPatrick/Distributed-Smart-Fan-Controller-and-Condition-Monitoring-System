#include <cstdint>
#include <cstddef>
#include <string>
#include <zephyr/devicetree.h>
#include <zephyr/display/cfb.h>
#include "display.hpp"

using namespace std;

monochrome_display::monochrome_display(const struct device* const monochrome_display_dt_spec, const string monochrome_display_alias) :

dt_spec{monochrome_display_dt_spec},

alias{monochrome_display_alias},

height_px{static_cast<size_t>(cfb_get_display_parameter(monochrome_display_dt_spec, CFB_DISPLAY_HEIGHT))},

width_px{static_cast<size_t>(cfb_get_display_parameter(monochrome_display_dt_spec, CFB_DISPLAY_WIDTH))},

font_height_px{0},

font_width_px{0},

error_code{DISPLAY_ERR_OK} {

	const struct device* const dt_spec_copy = this->dt_spec;
	size_t* font_width_px_copy = &(this->font_width_px);
	size_t* font_height_px_copy = &(this->font_height_px);

	if (cfb_get_font_size(dt_spec_copy, 0, reinterpret_cast<uint8_t*>(font_width_px_copy), reinterpret_cast<uint8_t*>(font_height_px_copy)) != 0) {
		this->error_code = DISPLAY_ERR_FONT_SIZE;
	}

	else {
		this->error_code = DISPLAY_ERR_OK;
	}
}

display_errors monochrome_display::screen_clear(void) {
	const struct device* const dt_spec_copy = this->dt_spec;

	if (this->error_code != DISPLAY_ERR_OK) {
		return this->error_code;
	}

	if (cfb_framebuffer_clear(dt_spec_copy, true) != 0) {
		this->error_code = DISPLAY_ERR_BUFF_CLEAR;
		return this->error_code;
	}

	if (cfb_framebuffer_finalize(dt_spec_copy) != 0) {
		this->error_code = DISPLAY_ERR_BUFF_WRITE;
		return this->error_code;
	}

	return this->error_code;
}

display_errors monochrome_display::string_print(string input_string, size_t row_idx, size_t column_idx) {
	const struct device* const dt_spec_copy = this->dt_spec;
	const size_t font_width_px_copy = this->font_width_px;
	const size_t font_height_px_copy = this->font_height_px;

	if (this->error_code != DISPLAY_ERR_OK) {
		return this->error_code;
	}

	if (row_idx + 1 > (this->height_px / this->font_height_px) || input_string.length() + column_idx > (this->width_px / this->font_width_px)) {
		this->error_code = DISPLAY_ERR_PARAM;
		return this->error_code;
	}

	if (cfb_print(dt_spec_copy, input_string.c_str(), static_cast<int16_t>(column_idx * font_width_px_copy), static_cast<int16_t>(row_idx * font_height_px_copy)) != 0) {
		this->error_code = DISPLAY_ERR_BUFF_STRING;
		return this->error_code;
	}

	if (cfb_framebuffer_finalize(dt_spec_copy) != 0) {
		this->error_code = DISPLAY_ERR_BUFF_WRITE;
		return this->error_code;
	}

	return this->error_code;
}

display_errors monochrome_display_controller::init_sequence(const struct device* const monochrome_display_dt_spec) {

	if (!device_is_ready(monochrome_display_dt_spec)) {
		return DISPLAY_ERR_DEVICE;
	}

	if (cfb_framebuffer_init(monochrome_display_dt_spec) != 0) {
		return DISPLAY_ERR_BUFF_INIT;
	}

	return DISPLAY_ERR_OK;
}

monochrome_display_controller::monochrome_display_controller(const struct device* const monochrome_display_dt_spec, const string monochrome_display_alias) :

init_status{init_sequence(monochrome_display_dt_spec)},

display{monochrome_display_dt_spec, monochrome_display_alias} {}
