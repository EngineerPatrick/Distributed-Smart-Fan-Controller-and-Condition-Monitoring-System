#ifndef UI_DISPLAY_HPP
#define UI_DISPLAY_HPP

#include "monochrome_display.hpp"

namespace ui_display {

	enum class ErrorCode {
		Ok,
		DisplayOperation,
		ParamTemp
	};

	ui_display::ErrorCode units_write(monochrome_display::MonochromeDisplay& mc_obj);
	ui_display::ErrorCode temp_write(monochrome_display::MonochromeDisplay& mc_obj, int32_t temp);
}

#endif
