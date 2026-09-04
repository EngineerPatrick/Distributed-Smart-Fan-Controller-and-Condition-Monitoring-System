#include "char_framebuff.hpp"
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>

char_framebuff::ErrorCode char_framebuff::CharFramebuff::device_set(const struct device* const monochrome_display_device_ptr) {

	if (!device_is_ready(monochrome_display_device_ptr)) {
		this->error.code = char_framebuff::ErrorCode::DeviceUnready;
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_init(monochrome_display_device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = char_framebuff::ErrorCode::CfbInit;
		return this->error.code;
	}

	this->display.device_ptr = monochrome_display_device_ptr;

	this->display.width_px = cfb_get_display_parameter(this->display.device_ptr, CFB_DISPLAY_WIDTH);
	this->display.height_px = cfb_get_display_parameter(this->display.device_ptr, CFB_DISPLAY_HEIGHT);
	this->error.code = char_framebuff::ErrorCode::Ok;
	return this->error.code;
}

char_framebuff::ErrorCode char_framebuff::CharFramebuff::font_set(uint8_t font_idx) {

	if (this->error.code == char_framebuff::ErrorCode::DeviceUnready || this->error.code == char_framebuff::ErrorCode::CfbInit) {
		return this->error.code;
	}


	if (font_idx > cfb_get_numof_fonts(this->display.device_ptr)) {
		this->error.code = char_framebuff::ErrorCode::Param;
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_set_font(this->display.device_ptr, font_idx);

	if (this->error.return_value != 0) {
		this->error.code = char_framebuff::ErrorCode::CfbFontSet;
		return this->error.code;
	}

	this->font.idx = font_idx;
	this->error.return_value = cfb_get_font_size(this->display.device_ptr, this->font.idx, &(this->font.width_px), &(this->font.height_px));

	if (this->error.return_value != 0) {
		this->error.code = char_framebuff::ErrorCode::CfbFontSizeGet;
		return this->error.code;
	}

	this->error.code = char_framebuff::ErrorCode::Ok;
	return this->error.code;
}

char_framebuff::ErrorCode char_framebuff::CharFramebuff::font_kerning_set(int8_t font_kerning_px) {

	if (this->error.code == char_framebuff::ErrorCode::DeviceUnready || this->error.code == char_framebuff::ErrorCode::CfbInit) {
		return this->error.code;
	}

	this->error.return_value = cfb_set_kerning(this->display.device_ptr, font_kerning_px);

	if (this->error.return_value != 0) {
		this->error.code = char_framebuff::ErrorCode::CfbFontKerningSet;
		return this->error.code;
	}

	this->font.kerning_px = font_kerning_px;
	this->error.code = char_framebuff::ErrorCode::Ok;
	return this->error.code;
}

char_framebuff::CharFramebuff::CharFramebuff(const struct device* const monochrome_display_device_ptr) {
	this->device_set(monochrome_display_device_ptr);

	if (this->error.code != char_framebuff::ErrorCode::Ok) {
		return;
	}

	this->font_set(0);

	if (this->error.code != char_framebuff::ErrorCode::Ok) {
		return;
	}

	this->font_kerning_set(0);

	if (this->error.code != char_framebuff::ErrorCode::Ok) {
		return;
	}

	this->error.code = char_framebuff::ErrorCode::Ok;
}

char_framebuff::CharFramebuff::~CharFramebuff() {
	cfb_framebuffer_deinit(this->display.device_ptr);
}

char_framebuff::ErrorCode char_framebuff::CharFramebuff::screen_clear() {

	if (this->error.code == char_framebuff::ErrorCode::DeviceUnready || this->error.code == char_framebuff::ErrorCode::CfbInit) {
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_clear(this->display.device_ptr, false);

	if (this->error.return_value != 0) {
		this->error.code = char_framebuff::ErrorCode::CfbScreenClear;
	}

	this->error.code = char_framebuff::ErrorCode::Ok;
	return this->error.code;
}

char_framebuff::ErrorCode char_framebuff::CharFramebuff::string_load(const std::string_view input_string, const size_t row_idx, const size_t column_idx) {

	if (this->error.code == char_framebuff::ErrorCode::DeviceUnready || this->error.code == char_framebuff::ErrorCode::CfbInit ||
	this->error.code == char_framebuff::ErrorCode::CfbFontSet || this->error.code == char_framebuff::ErrorCode::CfbFontSizeGet ||
	this->error.code == char_framebuff::ErrorCode::CfbFontKerningSet) {
		return this->error.code;
	}

	if (this->display.width_px < this->font.width_px || this->display.height_px < this->font.height_px) {
		this->error.code = char_framebuff::ErrorCode::DisplayResolution;
		return this->error.code;
	}

	if (row_idx > (this->display.height_px / this->font.height_px) - 1 || column_idx > (this->display.width_px / this->font.width_px) - 1) {
		this->error.code = char_framebuff::ErrorCode::Param;
		return this->error.code;
	}

	if (input_string.size() > (this->display.width_px / this->font.width_px) - column_idx) {
		this->error.code = char_framebuff::ErrorCode::Param;
		return this->error.code;
	}

	char input_string_c_str[(this->display.width_px / this->font.width_px) + 1];
	input_string.copy(input_string_c_str, input_string.size());
	input_string_c_str[input_string.size()] = '\0';

	this->error.return_value = cfb_draw_text(this->display.device_ptr, input_string_c_str, static_cast<int16_t>(column_idx * this->font.width_px),
	static_cast<int16_t>(row_idx * this->font.height_px));

	if (this->error.return_value != 0) {
		this->error.code = char_framebuff::ErrorCode::CfbStringLoad;
	}

	this->error.code = char_framebuff::ErrorCode::Ok;
	return this->error.code;
}

char_framebuff::ErrorCode char_framebuff::CharFramebuff::screen_print() {

	if (this->error.code == char_framebuff::ErrorCode::DeviceUnready || this->error.code == char_framebuff::ErrorCode::CfbInit) {
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_finalize(this->display.device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = char_framebuff::ErrorCode::CfbScreenPrint;
	}

	this->error.code = char_framebuff::ErrorCode::Ok;
	return this->error.code;
}

char_framebuff::ErrorState char_framebuff::CharFramebuff::error_state_get() {
	return this->error;
}
