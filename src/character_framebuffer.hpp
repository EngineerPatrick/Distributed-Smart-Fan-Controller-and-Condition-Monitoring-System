#ifndef CHARACTER_FRAMEBUFFER_HPP
#define CHARACTER_FRAMEBUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>

namespace character_framebuffer {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		DisplayResolution,
		ParamPositionIndex,
		ParamStringLength,
		ParamFontIndex,
		CfbUnready,
		CfbFontUnready,
		CfbInit,
		CfbFontSet,
		CfbFontSizeGet,
		CfbFontKerningSet,
		CfbRamClear,
		CfbStringLoad,
		CfbRamWrite
	};

	struct ErrorState {
		ErrorCode code = character_framebuffer::ErrorCode::Ok;
		int return_value = 0;
		size_t last_loaded_row_idx = 0;
		size_t last_loaded_column_idx = 0;
	};

	class CharacterFramebuffer {
		public:
			explicit CharacterFramebuffer(const struct device* const monochrome_display_device_ptr);
			~CharacterFramebuffer();

			CharacterFramebuffer(const CharacterFramebuffer&) = delete;
			CharacterFramebuffer(CharacterFramebuffer&&) = delete;
			CharacterFramebuffer& operator=(const CharacterFramebuffer&) = delete;
			CharacterFramebuffer& operator=(CharacterFramebuffer&&) = delete;

			character_framebuffer::ErrorCode font_set(uint8_t font_idx);
			character_framebuffer::ErrorCode ram_clear();
			character_framebuffer::ErrorCode string_load(const std::string_view input_string, const size_t row_idx, const size_t column_idx);
			character_framebuffer::ErrorCode ram_write();

			[[nodiscard("Called error getter and discarded its return value")]] character_framebuffer::ErrorState error_state_get() const;

		private:
			struct TargetDisplay {
				const struct device* const device_ptr = nullptr;
				const size_t width_px = 0;
				const size_t height_px = 0;
			};

			struct FontState {
				uint8_t idx = 0;
				uint8_t width_px = 0;
				uint8_t height_px = 0;
			};

			struct SystemState {
				bool cfb_init = false;
				bool cfb_ready = false;
				bool font_ready = false;
			};

			character_framebuffer::ErrorState error;
			SystemState system;
			TargetDisplay display;
			FontState font;

			TargetDisplay init_operations(const struct device* const monochrome_display_device_ptr);
	};
}

#endif
