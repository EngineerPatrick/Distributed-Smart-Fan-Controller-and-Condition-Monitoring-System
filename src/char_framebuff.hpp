#ifndef CHAR_FRAMEBUFF_HPP
#define CHAR_FRAMEBUFF_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>

namespace char_framebuff {
	struct TargetDisplay {
		const struct device* device_ptr = nullptr;
		size_t width_px = 0;
		size_t height_px = 0;
	};

	enum class ErrorCode {
		Ok,
		DeviceUnready,
		DisplayResolution,
		Param,
		CfbInit,
		CfbFontSet,
		CfbFontSizeGet,
		CfbFontKerningSet,
		CfbScreenClear,
		CfbStringLoad,
		CfbScreenPrint
	};

	struct ErrorState {
		ErrorCode code = char_framebuff::ErrorCode::Ok;
		int return_value = 0;
	};

	struct FontState {
		uint8_t idx = 0;
		uint8_t width_px = 0;
		uint8_t height_px = 0;
		int8_t kerning_px = 0;
	};

	class CharFramebuff {
		public:
			CharFramebuff(const struct device* const monochrome_display_device_ptr);
			~CharFramebuff();

			char_framebuff::ErrorCode device_set(const struct device* const monochrome_display_device_ptr);
			char_framebuff::ErrorCode font_set(uint8_t font_idx);
			char_framebuff::ErrorCode font_kerning_set(int8_t font_kerning_px);
			char_framebuff::ErrorCode screen_clear();
			char_framebuff::ErrorCode string_load(const std::string_view input_string, const size_t row_idx, const size_t column_idx);
			char_framebuff::ErrorCode screen_print();
			char_framebuff::ErrorState error_state_get();

		private:
			char_framebuff::TargetDisplay display;
			char_framebuff::ErrorState error;
			char_framebuff::FontState font;
	};
}

#endif
