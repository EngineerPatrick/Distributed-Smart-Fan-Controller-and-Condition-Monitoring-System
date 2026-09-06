#ifndef CHARACTER_FRAMEBUFF_HPP
#define CHARACTER_FRAMEBUFF_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>

namespace z_cbf {


	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		DisplayResolution,
		ParamPositionIndex,
		ParamStringLength,
		ParamFontIndex,
		CfbInit,
		CfbFontSet,
		CfbFontSizeGet,
		CfbFontKerningSet,
		CfbScreenClear,
		CfbStringLoad,
		CfbScreenPrint
	};

	struct ErrorState {
		ErrorCode code = z_cbf::ErrorCode::Ok;
		int return_value = 0;
	};

	class CharacterFramebuffer {
		public:
			explicit CharacterFramebuffer(const struct device* const monochrome_display_device_ptr);
			~CharacterFramebuffer();

			CharacterFramebuffer(const CharacterFramebuffer&) = delete;
			CharacterFramebuffer(const CharacterFramebuffer&&) = delete;
			CharacterFramebuffer& operator=(const CharacterFramebuffer&) = delete;
			CharacterFramebuffer& operator=(const CharacterFramebuffer&&) = delete;

			z_cbf::ErrorCode font_set(uint8_t font_idx);
			z_cbf::ErrorCode font_kerning_set(int8_t font_kerning_px);
			z_cbf::ErrorCode ram_clear();
			z_cbf::ErrorCode string_load(const std::string_view input_string, const size_t row_idx, const size_t column_idx);
			z_cbf::ErrorCode ram_write();

			[[nodiscard("Called error getter and discarded its return value")]] z_cbf::ErrorState error_state_get() const;

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
				int8_t kerning_px = 0;
			};

			TargetDisplay display;
			z_cbf::ErrorState error;
			FontState font;

			TargetDisplay init_operations(const struct device* const monochrome_display_device_ptr);
	};
}

#endif
