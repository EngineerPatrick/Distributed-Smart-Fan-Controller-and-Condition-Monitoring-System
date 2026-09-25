/**
*
*	@file		grid_printer.cpp
*
*	@brief		Implementation for the grid_printer.module
*
*	@details	Handles the calculation of the grid and the
*				conversion from grid index to pixel coordinate
*
*				BLINKING TIMER DESCRIPTIO TO DO
*
*/

#include "grid_printer.hpp"
#include "character_framebuffer.hpp"
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>

grid_printer::GridPrinter::DisplayState grid_printer::GridPrinter::init_operations() {
	std::size_t display_width_px = 0;
	std::size_t display_height_px = 0;

	if (this->cfb.error_state_get().code != character_framebuffer::ErrorCode::Ok) {
		this->error = grid_printer::ErrorCode::CfbUnready;
		return {};
	}

	if (this->cfb.display_sizes_get(display_width_px, display_height_px) != character_framebuffer::ErrorCode::Ok) {
		this->error = grid_printer::ErrorCode::CfbDisplaySizes;
		return {};
	}

	grid_printer::GridPrinter::DisplayState display{{display_width_px}, {display_height_px}};
	this->error = grid_printer::ErrorCode::Ok;
	return display;
}

grid_printer::GridPrinter::GridPrinter(const struct device* const monochrome_display_device_ptr) :
cfb{monochrome_display_device_ptr},
display{init_operations()} {

	if (this->error != grid_printer::ErrorCode::Ok) {
		return;
	}

	this->system.cfb_ready = true;

	if (this->cfb.font_sizes_get(this->font.width_px, this->font.height_px) != character_framebuffer::ErrorCode::Ok) {
		this->error = grid_printer::ErrorCode::CfbFontSizesGet;
		return;
	}

	this->grid = {(this->display.width_px / this->font.width_px), (this->display.height_px / this->font.height_px)};
	this->system.text_ready = true;
	this->error = grid_printer::ErrorCode::Ok;
}

grid_printer::ErrorCode grid_printer::GridPrinter::font_set(grid_printer::FontName font_name) {

	if (!this->system.cfb_ready) {
		this->error = grid_printer::ErrorCode::CfbUnready;
		return this->error;
	}

	this->system.text_ready = false;

	if (this->cfb.font_set(static_cast<character_framebuffer::FontName>(font_name)) != character_framebuffer::ErrorCode::Ok) {
		this->error = grid_printer::ErrorCode::CfbFontSet;
		return this->error;
	}

	if (this->cfb.font_sizes_get(this->font.width_px, this->font.height_px) != character_framebuffer::ErrorCode::Ok) {
		this->error = grid_printer::ErrorCode::CfbFontSizesGet;
		return this->error;
	}

	this->grid = {(this->display.width_px / this->font.width_px), (this->display.height_px / this->font.height_px)};
	this->system.text_ready = true;
	this->error = grid_printer::ErrorCode::Ok;
	return this->error;
}

grid_printer::ErrorCode grid_printer::GridPrinter::cells_clear() {

	if (!this->system.cfb_ready) {
		this->error = grid_printer::ErrorCode::CfbUnready;
		return this->error;
	}

	if (this->cfb.ram_clear() != character_framebuffer::ErrorCode::Ok) {
		this->error = grid_printer::ErrorCode::CfbRamClear;
		return this->error;
	}

	this->error = grid_printer::ErrorCode::Ok;
	return this->error;
}

grid_printer::ErrorCode grid_printer::GridPrinter::cells_string_write(std::string_view input_string, std::size_t row_idx, std::size_t column_idx) {

	if (!this->system.text_ready) {
		this->error = grid_printer::ErrorCode::TextUnready;
		return this->error;
	}

	if (row_idx > (this->grid.height_cells - 1) || column_idx > (this->grid.width_cells - 1)) {
		this->error = grid_printer::ErrorCode::ParamPositionIndexes;
		return this->error;
	}

	if (input_string.size() > (this->grid.width_cells - column_idx)) {
		this->error = grid_printer::ErrorCode::ParamStringLength;
		return this->error;
	}

	if (this->cfb.ram_string_write(input_string, (row_idx * this->font.height_px), (column_idx * this->font.width_px)) !=
	character_framebuffer::ErrorCode::Ok) {

		this->error = grid_printer::ErrorCode::CfbRamStringWrite;
		return this->error;
	}

	this->error = grid_printer::ErrorCode::Ok;
	return this->error;
}

/*
grid_printer::ErrorCode string_blink(std::string_view input_string, std::size_t row_idx, std::size_t column_idx, std::size_t blink_time_ms) {
	timer logic
	maybe thread logic
}
*/

grid_printer::ErrorCode grid_printer::GridPrinter::cells_print() {

	if (!system.cfb_ready) {
		this->error = grid_printer::ErrorCode::CfbUnready;
		return this->error;
	}

	if (this->cfb.ram_flush() != character_framebuffer::ErrorCode::Ok) {
		this->error = grid_printer::ErrorCode::CfbRamFlush;
		return this->error;
	}

	this->error = grid_printer::ErrorCode::Ok;
	return this->error;
}

grid_printer::ErrorCode grid_printer::GridPrinter::error_get() const {
	return this->error;
}
