#include "character_framebuffer.hpp"
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>

character_framebuffer::CharacterFramebuffer::TargetDisplay character_framebuffer::CharacterFramebuffer::init_operations(const struct device* const monochrome_display_device_ptr) {

	if (!device_is_ready(monochrome_display_device_ptr)) {
		this->error = {character_framebuffer::ErrorCode::DeviceUnready, 0, 0, 0};
		return {};
	}

	this->error.return_value = cfb_framebuffer_init(monochrome_display_device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbInit;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return {};
	}

	this->system.cfb_init = true;
	TargetDisplay display{{monochrome_display_device_ptr},
	{static_cast<size_t>(cfb_get_display_parameter(monochrome_display_device_ptr, CFB_DISPLAY_WIDTH))},
	{static_cast<size_t>(cfb_get_display_parameter(monochrome_display_device_ptr, CFB_DISPLAY_HEIGHT))}};

	if (!display.width_px || !display.height_px) {
		this->error = {character_framebuffer::ErrorCode::DisplayResolution, 0, 0, 0};
		return display;
	}

	this->system.cfb_ready = true;
	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return display;

}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::font_set(uint8_t font_idx) {

	if (!this->system.cfb_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	if (font_idx >= cfb_get_numof_fonts(this->display.device_ptr)) {
		this->error = {character_framebuffer::ErrorCode::ParamFontIndex, 0, 0, 0};
		return this->error.code;
	}

	this->system.font_ready = false;
	this->error.return_value = cfb_framebuffer_set_font(this->display.device_ptr, font_idx);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbFontSet;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	this->font.idx = font_idx;
	this->error.return_value = cfb_get_font_size(this->display.device_ptr, this->font.idx, &(this->font.width_px), &(this->font.height_px));

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbFontSizeGet;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	if (!this->font.width_px ||!this->font.height_px) {
		this->error.code = character_framebuffer::ErrorCode::CfbFontSizeGet;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	if (this->display.width_px < this->font.width_px || this->display.height_px < this->font.height_px) {
		this->error = {character_framebuffer::ErrorCode::DisplayResolution, 0, 0, 0};
		return this->error.code;
	}

	this->system.font_ready = true;
	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::CharacterFramebuffer::CharacterFramebuffer(const struct device* const monochrome_display_device_ptr) :
display{init_operations(monochrome_display_device_ptr)} {

	if (!this->system.cfb_ready) {
		return;
	}

	this->error.return_value = cfb_set_kerning(this->display.device_ptr, 0);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbFontKerningSet;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return;
	}

	if (this->font_set(0) != character_framebuffer::ErrorCode::Ok) {
		return;
	}

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
}

character_framebuffer::CharacterFramebuffer::~CharacterFramebuffer() {

	if (this->system.cfb_init) {
		cfb_framebuffer_deinit(this->display.device_ptr);
	}
}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::ram_clear() {

	if (!this->system.cfb_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_clear(this->display.device_ptr, false);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbRamClear;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::string_load(std::string_view input_string, const size_t row_idx, const size_t column_idx) {
	char input_char[2] = {' ', '\0'};

	if (!this->system.font_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbFontUnready, 0, 0, 0};
		return this->error.code;
	}

	if (row_idx > (this->display.height_px / this->font.height_px) - 1 || column_idx > (this->display.width_px / this->font.width_px) - 1) {
		this->error = {character_framebuffer::ErrorCode::ParamPositionIndex, 0, 0, 0};
		return this->error.code;
	}

	if (input_string.size() > (this->display.width_px / this->font.width_px) - column_idx) {
		this->error = {character_framebuffer::ErrorCode::ParamStringLength, 0, 0, 0};
		return this->error.code;
	}

	for (size_t i = 0; i < input_string.size(); i++) {
		input_char[0] = input_string.at(i);

		this->error.return_value = cfb_draw_text(this->display.device_ptr, input_char,
		static_cast<int16_t>((column_idx + i) * this->font.width_px),
		static_cast<int16_t>(row_idx * this->font.height_px));

		if (this->error.return_value != 0) {
			this->error.code = character_framebuffer::ErrorCode::CfbStringLoad;
			this->error.row_idx = row_idx;
			this->error.column_idx = (column_idx + i);
			return this->error.code;
		}
	}

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::ram_write() {

	if (!this->system.cfb_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_finalize(this->display.device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbRamWrite;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::ErrorState character_framebuffer::CharacterFramebuffer::error_state_get() const {
		return this->error;
}
