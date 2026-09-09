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
		DeviceUnready,
		CfbUnready,
		CfbTextUnready,
		CfbRam,
		ParamPositionIndexes,
		ParamStringLength
	};

	class MonochromeDisplay {

		public:

			MonochromeDisplay (const struct device* const monochrome_display_device_ptr);

			/*
			*
			*	Copy/move constructors/operators are deleted because there should be only 1 instance of this class
			*
			*/
			MonochromeDisplay(const MonochromeDisplay&) = delete;
			MonochromeDisplay(MonochromeDisplay&&) = delete;
			MonochromeDisplay& operator=(const MonochromeDisplay&) = delete;
			MonochromeDisplay& operator=(MonochromeDisplay&&) = delete;

			monochrome_display::ErrorCode grid_clear();
			monochrome_display::ErrorCode grid_string_write(std::string_view input_string, size_t row_idx, size_t column_idx);
//			monochrome_display::ErrorCode string_blink();
			monochrome_display::ErrorCode grid_print();

			[[nodiscard("Called error getter and discarded its return value")]] monochrome_display::ErrorCode error_get() const;

		private:

			struct SystemState {
				const struct device* const device_ptr = nullptr;
				bool main_ready = false;
				bool text_ready = false;
			};

			struct FontState {
				uint8_t idx = 0;
				size_t width_px = 0;
				size_t height_px = 0;
			};

			struct DisplayGrid {
				size_t width_cells = 0;
				size_t height_cells = 0;
			};

			SystemState system;
			character_framebuffer::CharacterFramebuffer cfb;

			FontState font;
			DisplayGrid grid;
			monochrome_display::ErrorCode error = monochrome_display::ErrorCode::Ok;
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
*	@post		If the target display is not ready to be used then error is set to DeviceUnready
*	@post		If the CFB is not ready to be used then error is set to CfbUnready
*	@post		If the text is not ready to be used then error is set to CfbTextUnready
*	@post		On success the CFB is ready to be used, the font sizes are saved in font, the grid sizes are saved in grid and error is set to Ok
*
*	@invariant	The cell-width of the grid is equal to the pixel-width of the display divided by the pixel-width of the font
*	@invariant	The cell-height of the grid is equal to the pixel-height of the display divided by the pixel-height of the font
*
*/

/**
*
*	@fn			monochrome_display::MonochromeDisplay::grid_clear()
*
*	@brief		Method to clear the grid
*
*	@retval		CfbUnready						If the CFB is not ready to be used
*	@retval		CfbRam						If an error occurs with the CFB
*	@retval		Ok								If no error occurs
*
*	@post		If the CFB is not ready to be used then error is set to CfbUnready
*	@post		If an error occurs with the CFB then error is set to CfbRam
*	@post		On success the grid is cleared and error is set to Ok
*
*/

/**
*
*	@fn			monochrome_display::MonochromeDisplay::grid_string_write(std::string_view input_string, size_t row_idx, size_t column_idx)
*
*	@brief		Method to write a string at a specific position of the grid
*
*	@param[in]	input_string					String to print
*	@param[in]	row_idx							Row index of the grid
*	@param[in]	column_idx						Column index of the grid
*
*	@retval		CfbTextUnready					If the text is not ready to be used
*	@retval		ParamPositionIndexes			If the position indexes are out of range
*	@retval		ParamStringLength				If the length of the string from column_idx exceeds the grid width
*	@retval		CfbRam						If an error occurs with the CFB
*	@retval		Ok								If no error occurs
*
*	@post		If the text is not ready to be used then error is set to CfbTextUnready
*	@post		If the position indexes are out of range then error is set to ParamPositionIndexes
*	@post		If the length of the string from column_idx exceeds the grid width then error is set to ParamStringLength
*	@post		If an error occurs with the CFB then error is set to CfbRam
*	@post		On success input_string is written in the grid at the position specified by the indexes and error is set to Ok
*
*/

/**
*
*	@fn			monochrome_display::ErrorCode monochrome_display::MonochromeDisplay::grid_print()
*
*	@brief		Method to print the grid on the display
*
*	@retval		CfbUnready						If the CFB is not ready to be used
*	@retval		CfbRam						If an error occurs with the CFB
*	@retval		Ok								If no error occurs
*
*	@post		If the CFB is not ready to be used then error is set to CfbUnready
*	@post		If an error occurs with the CFB then error is set to CfbRam
*	@post		On success the grid is printed on the display and error is set to Ok
*
*/

/**
*
*	@fn			monochrome_display::MonochromeDisplay::error_get() const
*
*	@return		The error variable
*
*/
