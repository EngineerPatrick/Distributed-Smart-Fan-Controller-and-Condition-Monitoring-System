#ifndef UI_DISPLAY_HPP
#define UI_DISPLAY_HPP

#include "monochrome_display.hpp"

namespace ui_display {

	enum class ErrorCode {
		Ok,
		DisplayOperation
	};

	ui_display::ErrorCode units_write(monochrome_display::MonochromeDisplay& mc_obj);

}

#endif
