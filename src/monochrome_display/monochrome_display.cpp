/**
*
*	@file		monochrome_display.cpp
*
*	@brief		Implementation for the monochrome_display module
*
*	@details	Handles the calculation of the grid and the
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

monochrome_display::MonochromeDisplay::DisplayState monochrome_display::MonochromeDisplay::init_operations() {
	std::size_t display_width_px = 0;
	std::size_t display_height_px = 0;

	if (this->cfb.error_state_get().code != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbUnready;
		return {};
	}

	if (this->cfb.display_sizes_get(display_width_px, display_height_px) != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbDisplaySizes;
		return {};
	}

	monochrome_display::MonochromeDisplay::DisplayState display{{display_width_px}, {display_height_px}};
	this->error = monochrome_display::ErrorCode::Ok;
	return display;
}

monochrome_display::MonochromeDisplay::MonochromeDisplay(const struct device* const monochrome_display_device_ptr) :
cfb{monochrome_display_device_ptr},
display{init_operations()} {

	if (this->error != monochrome_display::ErrorCode::Ok) {
		return;
	}

	this->system.cfb_ready = true;

	if (this->cfb.font_sizes_get(this->font.width_px, this->font.height_px) != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbFontSizesGet;
		return;
	}

	this->display.grid = {(this->display.width_px / this->font.width_px), (this->display.height_px / this->font.height_px)};
	this->system.text_ready = true;
	this->error = monochrome_display::ErrorCode::Ok;
}

monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::font_set(monochrome_display::FontName font_name) {

	if (!this->system.cfb_ready) {
		this->error = monochrome_display::ErrorCode::CfbUnready;
		return this->error;
	}

	this->system.text_ready = false;

	if (this->cfb.font_set(static_cast<character_framebuffer::FontName>(font_name)) != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbFontSet;
		return this->error;
	}

	if (this->cfb.font_sizes_get(this->font.width_px, this->font.height_px) != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbFontSizesGet;
		return this->error;
	}

	this->display.grid = {(this->display.width_px / this->font.width_px), (this->display.height_px / this->font.height_px)};
	this->system.text_ready = true;
	this->error = monochrome_display::ErrorCode::Ok;
	return this->error;
}

monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::grid_clear() {

	if (!this->system.cfb_ready) {
		this->error = monochrome_display::ErrorCode::CfbUnready;
		return this->error;
	}

	if (this->cfb.ram_clear() != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbRamClear;
		return this->error;
	}

	this->error = monochrome_display::ErrorCode::Ok;
	return this->error;
}

monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::grid_string_write(std::string_view input_string, std::size_t row_idx, std::size_t column_idx) {

	if (!this->system.text_ready) {
		this->error = monochrome_display::ErrorCode::TextUnready;
		return this->error;
	}

	if (row_idx > (this->display.grid.height_cells - 1) || column_idx > (this->display.grid.width_cells - 1)) {
		this->error = monochrome_display::ErrorCode::ParamPositionIndexes;
		return this->error;
	}

	if (input_string.size() > (this->display.grid.width_cells - column_idx)) {
		this->error = monochrome_display::ErrorCode::ParamStringLength;
		return this->error;
	}

	if (this->cfb.ram_string_write(input_string, (row_idx * this->font.height_px), (column_idx * this->font.width_px)) !=
	character_framebuffer::ErrorCode::Ok) {

		this->error = monochrome_display::ErrorCode::CfbRamStringWrite;
		return this->error;
	}

	this->error = monochrome_display::ErrorCode::Ok;
	return this->error;
}

/*
monochrome_display::ErrorCode string_blink(std::string_view input_string, std::size_t row_idx, std::size_t column_idx, std::size_t blink_time_ms) {
	timer logic
	maybe thread logic
}
*/

monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::grid_print() {

	if (!system.cfb_ready) {
		this->error = monochrome_display::ErrorCode::CfbUnready;
		return this->error;
	}

	if (this->cfb.ram_flush() != character_framebuffer::ErrorCode::Ok) {
		this->error = monochrome_display::ErrorCode::CfbRamFlush;
		return this->error;
	}

	this->error = monochrome_display::ErrorCode::Ok;
	return this->error;
}

monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::error_get() const {
	return this->error;
}
