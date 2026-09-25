/**
*
*	@file		grid_printer.hpp
*
*	@brief		Public API for the grid_printer.module
*
*	@details	Monochrome display service to manage a single device
*				by using the character framebuffer
*
*				Writes text on a grid, prints it on the display and manages blinking
*
*/

#ifndef GRID_PRINTER_HPP
#define GRID_PRINTER_HPP

#include "character_framebuffer.hpp"
#include "character_framebuffer_fonts.hpp"
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>

namespace grid_printer {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		TextUnready,
		CfbUnready,
		CfbDisplaySizes,
		CfbFontSet,
		CfbFontSizesGet,
		CfbRamClear,
		CfbRamStringWrite,
		CfbRamFlush,
		ParamPositionIndexes,
		ParamStringLength
	};

	enum class FontName {FONT_NAME_INIT};

	class GridPrinter {

		public:

			explicit GridPrinter(const struct device* const monochrome_display_device_ptr);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			GridPrinter(const GridPrinter&) = delete;
			GridPrinter(GridPrinter&&) = delete;
			GridPrinter& operator=(const GridPrinter&) = delete;
			GridPrinter& operator=(GridPrinter&&) = delete;

			grid_printer::ErrorCode font_set(grid_printer::FontName font_name);

			grid_printer::ErrorCode cells_clear();
			grid_printer::ErrorCode cells_string_write(std::string_view input_string, std::size_t row_idx, std::size_t column_idx);
//			grid_printer::ErrorCode string_blink();
			grid_printer::ErrorCode cells_print();

			[[nodiscard("Called error getter and discarded its return value")]] grid_printer::ErrorCode error_get() const;

		private:

			struct SystemState {
				bool cfb_ready = false;
				bool text_ready = false;
			};

			struct FontState {
				std::size_t width_px = 0;
				std::size_t height_px = 0;
			};

			struct GridState {
				std::size_t width_cells = 0;
				std::size_t height_cells = 0;
			};

			struct DisplayState {
				const std::size_t width_px = 0;
				const std::size_t height_px = 0;
			};

			grid_printer::GridPrinter::SystemState system;
			character_framebuffer::CharacterFramebuffer cfb;

			grid_printer::ErrorCode error = grid_printer::ErrorCode::Ok;
			grid_printer::GridPrinter::GridState grid;
			grid_printer::GridPrinter::DisplayState display;
			grid_printer::GridPrinter::FontState font;

			[[nodiscard("Internal error: necessary struct discarded")]] grid_printer::GridPrinter::DisplayState init_operations();
	};
}

#endif

/**
*
*	@enum		grid_printer::ErrorCode
*
*	@brief		Error codes of the module
*
*/

/**
*
*	@enum		grid_printer::FontName
*
*	@brief		Names of the available fonts
*
*	@details	This enum is a copy of the FontName enum in the character_framebuffer namespace to decouple the caller from the character framebuffer module
*
*	@warning	All original enum's warnings and invariants apply to this copy as well
*
*/

/**
*
*	@class		grid_printer::GridPrinter
*
*	@brief		Class for a single monochrome display device
*
*	@warning	Since Zephyr currently supports only 1 character framebuffer there should be only 1 instance of this class
*
*/

/**
*
*	@var		grid_printer::GridPrinter::error
*
*	@brief		Class error reporting variable
*
*	@invariant	Every fallible method sets this variable at the end of its call
*
*/

/**
*
*	@fn			grid_printer::GridPrinter::GridPrinter(const struct device* const monochrome_display_device_ptr)
*
*	@brief		Constructor to initialize the character framebuffer and the grid
*
*	@param[in]	monochrome_display_device_ptr	Pointer to the device struct of the target display
*
*	@pre		monochrome_display_device_ptr points to a valid device struct of a monochrome display
*	@post		If the display is not ready to be used then error is set to DisplayUnready
*	@post		If an error occurs when obtaining the sizes of the display and of the font then error is set to TextUnready
*	@post		On success the display is ready to be used, the font sizes are saved in font, the grid sizes are saved in grid and error is set to Ok
*
*	@invariant	The cell-width of the grid is equal to the pixel-width of the display divided by the pixel-width of the font
*	@invariant	The cell-height of the grid is equal to the pixel-height of the display divided by the pixel-height of the font
*
*/

/**
*
*	@fn			grid_printer::ErrorCode grid_printer::GridPrinter::font_set(grid_printer::FontName font_name)
*
*	@brief		Method to set the font to one of the available options
*
*	@param[in]	font_name						Name of the font to be set
*
*	@retval		DisplayUnready					If the display is not ready to be used
*	@retval		TextUnready						If the text functions are not ready to be used
*	@retval		Ok								If no error occurs
*
*	@post		If the display is not ready to be used then error is set to DisplayUnready
*	@post		If the text functions are not ready to be used then error is set to TextUnready and a new call is required to use the text
*	@post		On success the font is set to the specified option, the new grid is computed and error is set to Ok
*
*/

/**
*
*	@fn			grid_printer::ErrorCode grid_printer::GridPrinter::cells_clear()
*
*	@brief		Method to clear the grid
*
*	@retval		DisplayUnready					If the display is not ready to be used
*	@retval		CfbRam							If an error occurs with CFB's RAM
*	@retval		Ok								If no error occurs
*
*	@post		If the display is not ready to be used then error is set to DisplayUnready
*	@post		If an error occurs with CFB's RAM then error is set to CfbRam
*	@post		On success the grid is cleared and error is set to Ok
*
*/

/**
*
*	@fn			grid_printer::ErrorCode grid_printer::GridPrinter::cells_string_write(std::string_view input_string, std::size_t row_idx, std::size_t column_idx)
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
*	@post		If no font has been set then the Small sized font will be used by default
*	@post		On success input_string is written in the grid at the position specified by the indexes and error is set to Ok
*
*/

/**
*
*	@fn			grid_printer::ErrorCode grid_printer::ErrorCode grid_printer::GridPrinter::cells_print()
*
*	@brief		Method to print the grid on the display
*
*	@retval		DisplayUnready					If the display is not ready to be used
*	@retval		CfbRam							If an error occurs with CFB's RAM
*	@retval		Ok								If no error occurs
*
*	@post		If the display is not ready to be used then error is set to DisplayUnready
*	@post		If an error occurs with CFB's RAM then error is set to CfbRam
*	@post		On success the grid is printed on the display and error is set to Ok
*
*/

/**
*
*	@fn			grid_printer::ErrorCode grid_printer::GridPrinter::error_get() const
*
*	@return		The error variable
*
*/
