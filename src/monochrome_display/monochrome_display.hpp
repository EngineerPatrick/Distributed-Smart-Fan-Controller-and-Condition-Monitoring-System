/**
*
*	@file		monochrome_display.hpp
*
*	@brief		Public API for the monochrome_display module
*
*	@details	Monochrome display service to manage a single device
*				by using the character framebuffer
*
*				Writes text on a grid, prints it on the display and manages blinking
*
*/

#ifndef MONOCHROME_DISPLAY_HPP
#define MONOCHROME_DISPLAY_HPP

#include "character_framebuffer.hpp"
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>

namespace monochrome_display {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		MainUnready,
		TextUnready,
		CfbRam,
		ParamPositionIndexes,
		ParamStringLength
	};

	enum class FontSize {
		Small,
		Medium,
		Large
	};

	class MonochromeDisplay {

		public:

			explicit MonochromeDisplay(const struct device* const monochrome_display_device_ptr);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			MonochromeDisplay(const MonochromeDisplay&) = delete;
			MonochromeDisplay(MonochromeDisplay&&) = delete;
			MonochromeDisplay& operator=(const MonochromeDisplay&) = delete;
			MonochromeDisplay& operator=(MonochromeDisplay&&) = delete;

			monochrome_display::ErrorCode font_set(monochrome_display::FontSize font_size);

			monochrome_display::ErrorCode grid_clear();
			monochrome_display::ErrorCode grid_string_write(std::string_view input_string, std::size_t row_idx, std::size_t column_idx);
//			monochrome_display::ErrorCode string_blink();
			monochrome_display::ErrorCode grid_print();

			[[nodiscard("Called error getter and discarded its return value")]] monochrome_display::ErrorCode error_get() const;

		private:

			struct SystemState {
				bool main_ready = false;
				bool text_ready = false;
			};

			struct FontState {
				monochrome_display::FontSize size = monochrome_display::FontSize::Small;
				std::size_t width_px = 0;
				std::size_t height_px = 0;
			};

			struct DisplayGrid {
				std::size_t width_cells = 0;
				std::size_t height_cells = 0;
			};

			struct DisplayState {
				const std::size_t width_px = 0;
				const std::size_t height_px = 0;
				monochrome_display::MonochromeDisplay::DisplayGrid grid;
			};

			SystemState system;
			character_framebuffer::CharacterFramebuffer cfb;

			monochrome_display::MonochromeDisplay::FontState font;
			monochrome_display::MonochromeDisplay::DisplayState display;
			monochrome_display::ErrorCode error = monochrome_display::ErrorCode::Ok;

			[[nodiscard("Internal error: necessary struct discarded")]] monochrome_display::MonochromeDisplay::DisplayState init_operations();
	};
}

#endif

/**
*
*	@enum		monochrome_display::ErrorCode
*
*	@brief		Error codes of the module
*
*/

/**
*
*	@class		monochrome_display::MonochromeDisplay
*
*	@brief		Class for a single monochrome display device
*
*	@warning	Since Zephyr currently supports only 1 character framebuffer there should be only 1 instance of this class
*
*/

/**
*
*	@var		monochrome_display::MonochromeDisplay::error
*
*	@brief		Class error reporting variable
*
*	@invariant	Every fallible method sets this variable at the end of its call
*
*/

/**
*
*	@fn			monochrome_display::MonochromeDisplay::MonochromeDisplay(const struct device* const monochrome_display_device_ptr)
*
*	@brief		Constructor to initialize the character framebuffer and the grid
*
*	@param[in]	monochrome_display_device_ptr	Pointer to the device struct of the target display
*
*	@pre		monochrome_display_device_ptr points to a valid device struct of a monochrome display
*	@post		If an error occurs in the initialization of the CFB then error is set to MainUnready
*	@post		If an error occurs when obtaining the sizes of the display and of the font then error is set to TextUnready
*	@post		On success the display is ready to be used, the font sizes are saved in font, the grid sizes are saved in grid and error is set to Ok
*
*	@invariant	The cell-width of the grid is equal to the pixel-width of the display divided by the pixel-width of the font
*	@invariant	The cell-height of the grid is equal to the pixel-height of the display divided by the pixel-height of the font
*
*/

/**
*
*	@fn			monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::font_set(monochrome_display::FontSize font_size)
*
*	@brief		Method to set the size of the font to one of the available options
*
*	@param[in]	font_size						Option for the size of the font
*
*	@retval		MainUnready						If an error occurs in the initialization of the CFB
*	@retval		TextUnready						If an error occurs when setting the font
*	@retval		Ok								If no error occurs
*
*	@post		If an error occurs in the initialization of the CFB then error is set to MainUnready
*	@post		If an error occurs when setting the font then error is set to TextUnready and a new call is required to use the text
*	@post		On success the font is set to the specified option, the new grid is computed and error is set to Ok
*
*/

/**
*
*	@fn			monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::grid_clear()
*
*	@brief		Method to clear the grid
*
*	@retval		MainUnready						If an error occurs in the initialization of the CFB
*	@retval		CfbRam							If an error occurs with CFB's RAM
*	@retval		Ok								If no error occurs
*
*	@post		If an error occurs in the initialization of the CFB then error is set to MainUnready
*	@post		If an error occurs with CFB's RAM then error is set to CfbRam
*	@post		On success the grid is cleared and error is set to Ok
*
*/

/**
*
*	@fn			monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::grid_string_write(std::string_view input_string, std::size_t row_idx, std::size_t column_idx)
*
*	@brief		Method to write a string at a specific position of the grid
*
*	@param[in]	input_string					String to write
*	@param[in]	row_idx							Row index of the grid
*	@param[in]	column_idx						Column index of the grid
*
*	@retval		TextUnready						If the text is not ready to be used
*	@retval		ParamPositionIndexes			If the position indexes are out of range
*	@retval		ParamStringLength				If the length of the string from column_idx exceeds the grid width
*	@retval		CfbRam							If an error occurs with CFB's RAM
*	@retval		Ok								If no error occurs
*
*	@post		If the text is not ready to be used then error is set to TextUnready
*	@post		If the position indexes are out of range then error is set to ParamPositionIndexes
*	@post		If the length of the string from column_idx exceeds the grid width then error is set to ParamStringLength
*	@post		If an error occurs with CFB's RAM then error is set to CfbRam
*	@post		On success input_string is written in the grid at the position specified by the indexes and error is set to Ok
*
*/

/**
*
*	@fn			monochrome_display::ErrorCode monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::grid_print()
*
*	@brief		Method to print the grid on the display
*
*	@retval		MainUnready						If an error occurs in the initialization of the CFB
*	@retval		CfbRam							If an error occurs with CFB's RAM
*	@retval		Ok								If no error occurs
*
*	@post		If an error occurs in the initialization of the CFB then error is set to MainUnready
*	@post		If an error occurs with CFB's RAM then error is set to CfbRam
*	@post		On success the grid is printed on the display and error is set to Ok
*
*/

/**
*
*	@fn			monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::error_get() const
*
*	@return		The error variable
*
*/
