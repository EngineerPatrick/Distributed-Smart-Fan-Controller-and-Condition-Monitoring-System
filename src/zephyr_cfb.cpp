#include <cstdint>
#include <cstddef>
#include <string>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>
#include "display.hpp"

using namespace std;

static uint8_t init_flag = 0;

class monochrome_display {
	public:
		monochrome_display(const struct device* const monochrome_display_dt_spec);
		display_errors screen_clear(void);
		display_errors string_print(string input_string, size_t row_idx, size_t column_idx);

	private:
		const struct device* const dt_spec;
		const size_t height_px;
		const size_t width_px;
		size_t font_width_px;
		size_t font_height_px;
		uint8_t constructor_flag;
};

monochrome_display::monochrome_display(const struct device* const monochrome_display_dt_spec) :

dt_spec{monochrome_display_dt_spec},

height_px{static_cast<size_t>(cfb_get_display_parameter(monochrome_display_dt_spec, CFB_DISPLAY_HEIGHT))},

width_px{static_cast<size_t>(cfb_get_display_parameter(monochrome_display_dt_spec, CFB_DISPLAY_WIDTH))} {

	const struct device* const dt_spec_copy = this->dt_spec;
	size_t* font_width_px_copy = &(this->font_width_px);
	size_t* font_height_px_copy = &(this->font_height_px);

	if (cfb_get_font_size(dt_spec_copy, 0, reinterpret_cast<uint8_t*>(font_width_px_copy), reinterpret_cast<uint8_t*>(font_height_px_copy))) {
		this->constructor_flag = 0;
	}

	else {
		this->constructor_flag = 1;
	}
}

display_errors monochrome_display::screen_clear(void) {
	const struct device* const dt_spec_copy = this->dt_spec;

	if (!this->constructor_flag) {
		return DISPLAY_ERR_CONSTRUCTOR;
	}

	if (cfb_framebuffer_clear(dt_spec_copy, true) != 0) {
		return 	DISPLAY_ERR_BUFF_CLEAR;
	}

	if (cfb_framebuffer_finalize(dt_spec_copy) != 0) {
		return DISPLAY_ERR_BUFF_WRITE;
	}

	return DISPLAY_ERR_OK;
}

display_errors monochrome_display::string_print(string input_string, size_t row_idx, size_t column_idx) {
	const struct device* const dt_spec_copy = this->dt_spec;
	const size_t font_width_px_copy = this->font_width_px;
	const size_t font_height_px_copy = this->font_height_px;

	if (!this->constructor_flag) {
		return DISPLAY_ERR_CONSTRUCTOR;
	}

	if (row_idx + 1 > (this->height_px / this->font_height_px) || input_string.length() + column_idx > (this->width_px / this->font_width_px)) {
		return DISPLAY_ERR_PARAM;
	}

	if (cfb_print(dt_spec_copy, input_string.c_str(), static_cast<int16_t>(column_idx * font_width_px_copy), static_cast<int16_t>(row_idx * font_height_px_copy)) != 0) {
		return DISPLAY_ERR_BUFF_STRING;
	}

	if (cfb_framebuffer_finalize(dt_spec_copy) != 0) {
		return DISPLAY_ERR_BUFF_WRITE;
	}

	return DISPLAY_ERR_OK;
}

static display_errors init_sequence(const struct device* const monochrome_display_dt_spec) {

	if (init_flag) {
		return DISPLAY_ERR_OK;
	}

	if (!device_is_ready(monochrome_display_dt_spec)) {
		return DISPLAY_ERR_DEVICE;
	}

	if (cfb_framebuffer_init(monochrome_display_dt_spec) != 0) {
		return DISPLAY_ERR_BUFF_INIT;
	}

	init_flag = 1;
	return DISPLAY_ERR_OK;
}

display_errors display_screen_clear(void) {
	display_errors error_code = DISPLAY_ERR_OK;
	const struct device* const monochrome_display_dt_spec = DEVICE_DT_GET(DT_ALIAS(DISPLAY_ALIAS));

	error_code = init_sequence(monochrome_display_dt_spec);

	if (error_code != DISPLAY_ERR_OK) {
		return error_code;
	}

	monochrome_display local_obj{monochrome_display_dt_spec};
	error_code = local_obj.screen_clear();
	return error_code;
}

display_errors display_string_print(string input_string, size_t row_idx, size_t column_idx) {
	display_errors error_code = DISPLAY_ERR_OK;
	const struct device* const monochrome_display_dt_spec = DEVICE_DT_GET(DT_ALIAS(DISPLAY_ALIAS));

	error_code = init_sequence(monochrome_display_dt_spec);

	if (error_code != DISPLAY_ERR_OK) {
		return error_code;
	}

	monochrome_display local_obj{monochrome_display_dt_spec};
	error_code = local_obj.string_print(input_string, row_idx, column_idx);
	return error_code;
}
