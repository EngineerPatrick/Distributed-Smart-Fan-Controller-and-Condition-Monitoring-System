#include "character_framebuffer.hpp"
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>

z_cbf::CharacterFramebuffer::TargetDisplay z_cbf::CharacterFramebuffer::init_operations(const struct device* const monochrome_display_device_ptr) {

	if (!device_is_ready(monochrome_display_device_ptr)) {
		this->error = {z_cbf::ErrorCode::DeviceUnready, 0};
		return {};
	}

	this->error.return_value = cfb_framebuffer_init(monochrome_display_device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = z_cbf::ErrorCode::CfbInit;
		return {};
	}

	TargetDisplay display{{monochrome_display_device_ptr},
	{static_cast<size_t>(cfb_get_display_parameter(monochrome_display_device_ptr, CFB_DISPLAY_WIDTH))},
	{static_cast<size_t>(cfb_get_display_parameter(monochrome_display_device_ptr, CFB_DISPLAY_HEIGHT))}};

	if (!display.width_px || !display.height_px) {
		this->error = {z_cbf::ErrorCode::DisplayResolution, 0};
		return {};
	}

	this->error = {z_cbf::ErrorCode::Ok, 0};
	return display;

}

z_cbf::ErrorCode z_cbf::CharacterFramebuffer::font_set(uint8_t font_idx) {

	if (this->error.code == z_cbf::ErrorCode::DeviceUnready || this->error.code == z_cbf::ErrorCode::CfbInit ||
	this->error.code == z_cbf::ErrorCode::DisplayResolution) {
		return this->error.code;
	}

	if (font_idx > cfb_get_numof_fonts(this->display.device_ptr) - 1) {
		this->error = {z_cbf::ErrorCode::ParamFontIndex, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_set_font(this->display.device_ptr, font_idx);

	if (this->error.return_value != 0) {
		this->error.code = z_cbf::ErrorCode::CfbFontSet;
		return this->error.code;
	}

	this->font.idx = font_idx;
	this->error.return_value = cfb_get_font_size(this->display.device_ptr, this->font.idx, &(this->font.width_px), &(this->font.height_px));

	if (this->error.return_value != 0) {
		this->error.code = z_cbf::ErrorCode::CfbFontSizeGet;
		return this->error.code;
	}

	if (this->display.width_px < this->font.width_px || this->display.height_px < this->font.height_px) {
		this->error = {z_cbf::ErrorCode::DisplayResolution, 0};
		return this->error.code;
	}

	this->error = {z_cbf::ErrorCode::Ok, 0};
	return this->error.code;
}

z_cbf::ErrorCode z_cbf::CharacterFramebuffer::font_kerning_set(int8_t font_kerning_px) {

	if (this->error.code == z_cbf::ErrorCode::DeviceUnready || this->error.code == z_cbf::ErrorCode::CfbInit ||
	this->error.code == z_cbf::ErrorCode::DisplayResolution) {
		return this->error.code;
	}

	this->error.return_value = cfb_set_kerning(this->display.device_ptr, font_kerning_px);

	if (this->error.return_value != 0) {
		this->error.code = z_cbf::ErrorCode::CfbFontKerningSet;
		return this->error.code;
	}

	this->font.kerning_px = font_kerning_px;
	this->error = {z_cbf::ErrorCode::Ok, 0};
	return this->error.code;
}

z_cbf::CharacterFramebuffer::CharacterFramebuffer(const struct device* const monochrome_display_device_ptr) :
display{init_operations(monochrome_display_device_ptr)} {

	if (this->error.code == z_cbf::ErrorCode::DeviceUnready || this->error.code == z_cbf::ErrorCode::CfbInit ||
	this->error.code == z_cbf::ErrorCode::DisplayResolution) {
		return;
	}

	if (this->font_set(0) != z_cbf::ErrorCode::Ok) {
		return;
	}

	if (this->font_kerning_set(0) != z_cbf::ErrorCode::Ok) {
		return;
	}

	this->error = {z_cbf::ErrorCode::Ok, 0};
}

z_cbf::CharacterFramebuffer::~CharacterFramebuffer() {

	if (this->error.code != z_cbf::ErrorCode::DeviceUnready && this->error.code != z_cbf::ErrorCode::CfbInit) {
		cfb_framebuffer_deinit(this->display.device_ptr);
	}
}

z_cbf::ErrorCode z_cbf::CharacterFramebuffer::ram_clear() {

	if (this->error.code == z_cbf::ErrorCode::DeviceUnready || this->error.code == z_cbf::ErrorCode::CfbInit ||
	this->error.code == z_cbf::ErrorCode::DisplayResolution) {
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_clear(this->display.device_ptr, false);

	if (this->error.return_value != 0) {
		this->error.code = z_cbf::ErrorCode::CfbScreenClear;
		return this->error.code;
	}

	this->error = {z_cbf::ErrorCode::Ok, 0};
	return this->error.code;
}

z_cbf::ErrorCode z_cbf::CharacterFramebuffer::string_load(const std::string_view input_string, const size_t row_idx, const size_t column_idx) {

	if (this->error.code == z_cbf::ErrorCode::DeviceUnready || this->error.code == z_cbf::ErrorCode::CfbInit ||
	this->error.code == z_cbf::ErrorCode::DisplayResolution ||
	this->error.code == z_cbf::ErrorCode::CfbFontSet || this->error.code == z_cbf::ErrorCode::CfbFontSizeGet ||
	this->error.code == z_cbf::ErrorCode::CfbFontKerningSet) {
		return this->error.code;
	}

	if (row_idx > (this->display.height_px / this->font.height_px) - 1 || column_idx > (this->display.width_px / this->font.width_px) - 1) {
		this->error = {z_cbf::ErrorCode::ParamPositionIndex, 0};
		return this->error.code;
	}

	if (input_string.size() > (this->display.width_px / this->font.width_px) - column_idx) {
		this->error = {z_cbf::ErrorCode::ParamStringLength, 0};
		return this->error.code;
	}

	this->error.return_value = cfb_draw_text(this->display.device_ptr, input_string.data(),
	static_cast<int16_t>(column_idx * (this->font.width_px + this->font.kerning_px)),
	static_cast<int16_t>(row_idx * this->font.height_px));

	if (this->error.return_value != 0) {
		this->error.code = z_cbf::ErrorCode::CfbStringLoad;
		return this->error.code;
	}

	this->error = {z_cbf::ErrorCode::Ok, 0};
	return this->error.code;
}

z_cbf::ErrorCode z_cbf::CharacterFramebuffer::ram_write() {

	if (this->error.code == z_cbf::ErrorCode::DeviceUnready || this->error.code == z_cbf::ErrorCode::CfbInit ||
	this->error.code == z_cbf::ErrorCode::DisplayResolution) {
		return this->error.code;
	}

	this->error.return_value = cfb_framebuffer_finalize(this->display.device_ptr);

	if (this->error.return_value != 0) {
		this->error.code = z_cbf::ErrorCode::CfbScreenPrint;
		return this->error.code;
	}

	this->error = {z_cbf::ErrorCode::Ok, 0};
	return this->error.code;
}

z_cbf::ErrorState z_cbf::CharacterFramebuffer::error_state_get() const {
		return this->error;
}
