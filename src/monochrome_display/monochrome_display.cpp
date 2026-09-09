/**
*
*	@file		monochrome_display.cpp
*
*	@brief		Implementation for the monochrome_display module
*
*	@details	Handles the calculation of the screen grid and the
*				conversion from grid index to pixel coordinate
*
*				BLINKING TIMER DESCRIPTIO TO DO
*
*/

#include "monochrome_display.hpp"
#include "character_framebuffer.hpp"
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>

monochrome_display::MonochromeDisplay::MonochromeDisplay(const struct device* const monochrome_display_device_ptr) :
system{{monochrome_display_device_ptr}},
cfb{monochrome_display_device_ptr} {

	size_t display_width_px = 0;
	size_t display_height_px = 0;
	character_framebuffer::ErrorState cfb_error_state{this->cfb.error_state_get()};

	if (cfb_error_state.code == character_framebuffer::ErrorCode::DeviceUnready) {
		this->error = monochrome_display::ErrorCode::DeviceUnready;
		return;
	}

	if (cfb_error_state.code == character_framebuffer::ErrorCode::DisplayResolution ||
	cfb_error_state.code == character_framebuffer::ErrorCode::CfbInit) {
		this->error = monochrome_display::ErrorCode::CfbUnready;
		return;
	}

	this->system.main_ready = true;

	if (this->cfb.display_sizes_get(display_width_px, display_height_px) != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbInternal;
		return;
	}

	if (this->cfb.font_sizes_get(this->font.width_px, this->font.height_px) != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbInternal;
		return;
	}

	this->grid = {(display_width_px / this->font.width_px), (display_height_px / this->font.height_px)};
	this->system.text_ready = true;
	this->error = monochrome_display::ErrorCode::Ok;
}

monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::screen_clear() {

	if (!(this->system.main_ready)) {
		this->error = monochrome_display::ErrorCode::CfbUnready;
		return this->error;
	}

	if (this->cfb.ram_clear() != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbInternal;
		return this->error;
	}

	if (this->cfb.ram_flush() != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbInternal;
		return this->error;
	}

	this->error = monochrome_display::ErrorCode::Ok;
	return this->error;
}

monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::screen_string_print(std::string_view input_string, size_t row_idx, size_t column_idx) {

	if (!(this->system.text_ready)) {
		this->error = monochrome_display::ErrorCode::CfbTextUnready;
		return this->error;
	}

	if (row_idx > (this->grid.height_cells - 1) || column_idx > (this->grid.width_cells - 1)) {
		this->error = monochrome_display::ErrorCode::ParamPositionIndexes;
		return this->error;
	}

	if (input_string.size() > (this->grid.width_cells - column_idx)) {
		this->error = monochrome_display::ErrorCode::ParamStringLength;
		return this->error;
	}

	if (this->cfb.ram_string_write(input_string, (row_idx * this->font.height_px), (column_idx * this->font.width_px)) !=
	character_framebuffer::ErrorCode::Ok) {

		this->error = monochrome_display::ErrorCode::CfbInternal;
		return this->error;
	}

	if (this->cfb.ram_flush() != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbInternal;
		return this->error;
	}

	this->error = monochrome_display::ErrorCode::Ok;
	return this->error;
}

/*
monochrome_display::ErrorCode screen_string_blink(std::string_view input_string, size_t row_idx, size_t column_idx, size_t blink_time_ms) {
	timer logic
	maybe thread logic
}
*/

monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::error_get() const {
	return this->error;
}
