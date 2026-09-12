/**
*
*	@file		character_framebuffer.cpp
*
*	@brief		Implementation for the character_framebuffer module
*
*	@details	Acquires Zephyr's CFB resource, keeps track of its return
*				values and performs text operations
*
*				Implements RAII to acquire and release the employed resource
*
*/

#include "character_framebuffer.hpp"
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>

character_framebuffer::CharacterFramebuffer::DisplayDevice character_framebuffer::CharacterFramebuffer::init_operations(const struct device* const monochrome_display_device_ptr) {
	std::size_t display_width_px = 0;
	std::size_t display_height_px = 0;

	if (!device_is_ready(monochrome_display_device_ptr)) {
		this->error = {character_framebuffer::ErrorCode::DeviceUnready, 0, 0, 0};
		return {};
	}

	this->error.return_value = cfb_framebuffer_init(monochrome_display_device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbInit;
		this->error.row_px = 0;
		this->error.column_px = 0;
		return {};
	}

	this->system.cfb_init = true;

	display_width_px = static_cast<std::size_t>(cfb_get_display_parameter(monochrome_display_device_ptr, CFB_DISPLAY_WIDTH));
	display_height_px = static_cast<std::size_t>(cfb_get_display_parameter(monochrome_display_device_ptr, CFB_DISPLAY_HEIGHT));

	if (!display_width_px || !display_height_px) {
		this->error = {character_framebuffer::ErrorCode::DisplayResolution, 0, 0, 0};
		return {{monochrome_display_device_ptr}};
	}

	character_framebuffer::CharacterFramebuffer::DisplayDevice display{{monochrome_display_device_ptr}, {display_width_px}, {display_height_px}};

	this->system.cfb_ready = true;
	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return display;

}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::font_set(std::uint8_t font_idx) {

	if (!this->system.cfb_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	if (font_idx >= cfb_get_numof_fonts(this->display.device_ptr)) {
		this->error = {character_framebuffer::ErrorCode::ParamFontIndex, 0, 0, 0};
		return this->error.code;
	}

	this->system.font_ready = false;

	if(!this->system.kerning_ready) {
		this->error.return_value = cfb_set_kerning(this->display.device_ptr, 0);

		if (this->error.return_value != 0) {
			this->error.code = character_framebuffer::ErrorCode::CfbFontKerningSet;
			this->error.row_px = 0;
			this->error.column_px = 0;
			return this->error.code;
		}

		this->system.kerning_ready = true;
	}

	this->error.return_value = cfb_framebuffer_set_font(this->display.device_ptr, font_idx);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbFontSet;
		this->error.row_px = 0;
		this->error.column_px = 0;
		return this->error.code;
	}

	this->font.idx = font_idx;
	this->error.return_value = cfb_get_font_size(this->display.device_ptr, this->font.idx, &(this->font.width_px), &(this->font.height_px));

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbFontSizeGet;
		this->error.row_px = 0;
		this->error.column_px = 0;
		return this->error.code;
	}

	if (!this->font.width_px || !this->font.height_px) {
		this->error.code = character_framebuffer::ErrorCode::CfbFontSizeGet;
		this->error.row_px = 0;
		this->error.column_px = 0;
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
		this->error.row_px = 0;
		this->error.column_px = 0;
		return;
	}

	this->system.kerning_ready = true;

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

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::display_sizes_get(std::size_t& display_width_px, std::size_t& display_height_px) {

	if (!this->system.cfb_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	display_width_px = this->display.width_px;
	display_height_px = this->display.height_px;

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::font_sizes_get(std::size_t& font_width_px, std::size_t& font_height_px) {

	if (!this->system.font_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbFontUnready, 0, 0, 0};
		return this->error.code;
	}

	font_width_px = this->font.width_px;
	font_height_px = this->font.height_px;

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::ram_clear() {

	if (!this->system.cfb_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_clear(this->display.device_ptr, false);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbRamClear;
		this->error.row_px = 0;
		this->error.column_px = 0;
		return this->error.code;
	}

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::ram_string_write(std::string_view input_string, const std::size_t row_px, const std::size_t column_px) {
	char input_char[2] = {' ', '\0'};

	if (!this->system.font_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbFontUnready, 0, 0, 0};
		return this->error.code;
	}

	if (row_px > (this->display.height_px - this->font.height_px) || column_px > (this->display.width_px - this->font.width_px)) {
		this->error = {character_framebuffer::ErrorCode::ParamPixelCoordinates, 0, 0, 0};
		return this->error.code;
	}

	if (input_string.size() > ((this->display.width_px - column_px) / this->font.width_px)) {
		this->error = {character_framebuffer::ErrorCode::ParamStringLength, 0, 0, 0};
		return this->error.code;
	}

	for (std::size_t i = 0; i < input_string.size(); i++) {
		input_char[0] = input_string.at(i);

		this->error.return_value = cfb_print(this->display.device_ptr, input_char,
		static_cast<int16_t>(column_px + (this->font.width_px * i)),
		static_cast<int16_t>(row_px));

		if (this->error.return_value != 0) {
			this->error.code = character_framebuffer::ErrorCode::CfbStringWrite;
			this->error.row_px = row_px;
			this->error.column_px = column_px + (this->font.width_px * i);
			return this->error.code;
		}
	}

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::ram_flush() {

	if (!this->system.cfb_ready) {
		this->error = {character_framebuffer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_finalize(this->display.device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = character_framebuffer::ErrorCode::CfbRamFlush;
		this->error.row_px = 0;
		this->error.column_px = 0;
		return this->error.code;
	}

	this->error = {character_framebuffer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

character_framebuffer::ErrorState character_framebuffer::CharacterFramebuffer::error_state_get() const {
		return this->error;
}
