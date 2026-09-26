#include "grid_printer.hpp"
#include "fonts_adapter.hpp"
#include <cstdint>
#include <cstddef>
#include <array>
#include <string_view>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>

grid_printer::GridPrinter::DisplayState grid_printer::GridPrinter::init_operations(const struct device* const display_device_ptr) {
	std::size_t display_width_px = 0;
	std::size_t display_height_px = 0;

	if (!device_is_ready(display_device_ptr)) {
		this->error = {grid_printer::ErrorCode::DeviceUnready, 0, 0, 0};
		return {};
	}

	this->error.return_value = cfb_framebuffer_init(display_device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = grid_printer::ErrorCode::ZCfbInit;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return {};
	}

	this->system.cfb_init = true;

	display_width_px = static_cast<std::size_t>(cfb_get_display_parameter(display_device_ptr, CFB_DISPLAY_WIDTH));
	display_height_px = static_cast<std::size_t>(cfb_get_display_parameter(display_device_ptr, CFB_DISPLAY_HEIGHT));

	if (!display_width_px || !display_height_px) {
		this->error = {grid_printer::ErrorCode::DisplayResolution, 0, 0, 0};
		return {{display_device_ptr}};
	}

	grid_printer::GridPrinter::DisplayState display{{display_device_ptr}, {display_width_px}, {display_height_px}};

	this->system.cfb_ready = true;
	this->error = {grid_printer::ErrorCode::Ok, 0, 0, 0};
	return display;

}

grid_printer::ErrorCode grid_printer::GridPrinter::font_set(grid_printer::FontName font_name) {

	if (!this->system.cfb_ready) {
		this->error = {grid_printer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	this->system.text_ready = false;

	for (std::size_t font_idx = 0; font_idx < this->font_list.size(); font_idx++) {

		if (font_name == this->font_list.at(font_idx)) {
			this->font.idx = font_idx;
		}
	}

	if (this->font.idx >= cfb_get_numof_fonts(this->display.device_ptr)) {
		this->error = {grid_printer::ErrorCode::ParamFontIndex, 0, 0, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_set_font(this->display.device_ptr, this->font.idx);

	if (this->error.return_value != 0) {
		this->error.code = grid_printer::ErrorCode::ZCfbFontSet;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	this->error.return_value = cfb_get_font_size(this->display.device_ptr, this->font.idx, &(this->font.width_px), &(this->font.height_px));

	if (this->error.return_value != 0) {
		this->error.code = grid_printer::ErrorCode::ZCfbFontSizeGet;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	if (!this->font.width_px || !this->font.height_px) {
		this->error.code = grid_printer::ErrorCode::ZCfbFontSizeGet;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	if (this->display.width_px < this->font.width_px || this->display.height_px < this->font.height_px) {
		this->error = {grid_printer::ErrorCode::DisplayResolution, 0, 0, 0};
		return this->error.code;
	}

	this->grid = {(this->display.width_px / this->font.width_px), (this->display.height_px / this->font.height_px)};

	this->system.text_ready = true;
	this->error = {grid_printer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

grid_printer::GridPrinter::GridPrinter(const struct device* const display_device_ptr) :
display{init_operations(display_device_ptr)} {

	if (!this->system.cfb_ready) {
		return;
	}

	this->error.return_value = cfb_set_kerning(this->display.device_ptr, 0);

	if (this->error.return_value != 0) {
		this->error.code = grid_printer::ErrorCode::ZCfbFontKerningSet;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return;
	}

	if (this->font_set(DEFAULT_FONT) != grid_printer::ErrorCode::Ok) {
		return;
	}

	this->error = {grid_printer::ErrorCode::Ok, 0, 0, 0};
}

grid_printer::GridPrinter::~GridPrinter() {

	if (this->system.cfb_init) {
		cfb_framebuffer_deinit(this->display.device_ptr);
	}
}

grid_printer::ErrorCode grid_printer::GridPrinter::cells_clear() {

	if (!this->system.cfb_ready) {
		this->error = {grid_printer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_clear(this->display.device_ptr, false);

	if (this->error.return_value != 0) {
		this->error.code = grid_printer::ErrorCode::ZCfbRamClear;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	this->error = {grid_printer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

grid_printer::ErrorCode grid_printer::GridPrinter::cells_string_write(const std::string_view input_string, const std::size_t row_idx, const std::size_t column_idx) {
	char input_char[2] = {' ', '\0'};

	if (!this->system.text_ready) {
		this->error = {grid_printer::ErrorCode::TextUnready, 0, 0, 0};
		return this->error.code;
	}

	if (row_idx >= this->grid.height_cells || column_idx >= this->grid.width_cells) {
		this->error = {grid_printer::ErrorCode::ParamGridCoordinates, 0, 0, 0};
		return this->error.code;
	}

	if (input_string.size() > (this->grid.width_cells - column_idx)) {
		this->error = {grid_printer::ErrorCode::ParamStringLength, 0, 0, 0};
		return this->error.code;
	}

	for (std::size_t char_idx = 0; char_idx < input_string.size(); char_idx++) {
		input_char[0] = input_string.at(char_idx);

		this->error.return_value = cfb_print(
			this->display.device_ptr,
			input_char,
			static_cast<int16_t>((column_idx + char_idx) * this->font.width_px),
			static_cast<int16_t>(row_idx * this->font.height_px)
		);

		if (this->error.return_value != 0) {
			this->error.code = grid_printer::ErrorCode::ZCfbStringWrite;
			this->error.row_idx = row_idx;
			this->error.column_idx = column_idx + char_idx;
			return this->error.code;
		}
	}

	this->error = {grid_printer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

grid_printer::ErrorCode grid_printer::GridPrinter::cells_print() {

	if (!this->system.cfb_ready) {
		this->error = {grid_printer::ErrorCode::CfbUnready, 0, 0, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_finalize(this->display.device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = grid_printer::ErrorCode::ZCfbRamFlush;
		this->error.row_idx = 0;
		this->error.column_idx = 0;
		return this->error.code;
	}

	this->error = {grid_printer::ErrorCode::Ok, 0, 0, 0};
	return this->error.code;
}

grid_printer::ErrorState grid_printer::GridPrinter::error_state_get() const {
		return this->error;
}
