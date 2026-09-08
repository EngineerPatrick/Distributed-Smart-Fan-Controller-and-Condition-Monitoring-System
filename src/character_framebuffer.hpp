/*
*
*	@file		character_framebuffer.hpp
*
*	@brief		Public API for the character_framebuffer module
*
*	@details	Adapter module for Zephyr's Monochrome Character Framebuffer API.
*
*			Supports strings loading, font selection, RAM clearing and writing
*
*/

#ifndef CHARACTER_FRAMEBUFFER_HPP
#define CHARACTER_FRAMEBUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>

namespace character_framebuffer {

/*
*
*	@brief	Error codes of the module
*
*	@par	Invariants
*
*		-If an error related to Zephyr's CFB API occurs then the code is prefixed with "Cfb"
*
*/
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

/*
*
*	@brief	Data structure for all types of errors
*
*	@par	Invariants
*
*		-All members of this struct are updated at the end of every method of CharacterFramebuffer
*
*		-If a module-based error occurs then it is represented by code and the other members are equal to 0
*
*		-If an error related to Zephyr's CFB API occurs then it is represented by both code and return_value
*
*		-If an error related to Zephyr's CFB API occurs during the loading phase in string_load then the position
*		on the display where the error has occurred is saved in the last 2 members, which are equal to 0 in any other error case
*
*/
	struct ErrorState {
		ErrorCode code = character_framebuffer::ErrorCode::Ok;
		int return_value = 0;
		size_t row_idx = 0;
		size_t column_idx = 0;
	};

/*
*
*	@brief	Class for Zephyr's Monochrome Character Framebuffer API
*
*/
	class CharacterFramebuffer {

		public:

/*
*
*	@brief	Constructor to initialize the framebuffer to target a specific display
*
*	@param	monochrome_display_device_ptr	Pointer to the device struct of the target display
*
*	@pre	The target display is ready to be used
*	@pre	monochrome_display_device_ptr points to the device struct of the target device
*	@post	If the target display is not ready to be used then error.code is set to DeviceUnready
*	@post	If an error occurs when initializing the framebuffer with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to CfbInit
*	@post	If the display width and height parameters obtained from Zephyr's CFB API are 0 then error.code is set to DisplayResolution
*	@post	If an error occurs when setting the kerning with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to CfbFontKerningSet
*	@post	If an error occurs when setting the font to that at index 0 with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to CfbFontSet
*	@post	If an error occurs when obtaining the size of the font with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to CfbFontSizeGet
*	@post	If the size of the font obtained with Zephyr's CFB API is 0 then its return value is saved in error.return_value and error.code is set to CfbFontSizeGet
*	@post	If the obtained size of the font is bigger then the size of the display obtained with Zephyr's CFB API then error.code is set to DisplayResolution
*	@post	On success the target device is ready, the CFB is initialized, the kerning is set to 0 px, the font is set to index 0, and error.code is set to Ok
*
*/
			explicit CharacterFramebuffer(const struct device* const monochrome_display_device_ptr);

/*
*
*	@brief	Destructor to deinitialize the framebuffer
*
*	@pre	The CFB has been successfully initialized by calling the constructor
*	@post	If the CFB has been successfully initialized by the constructor then it is deinitialized
*
*/
			~CharacterFramebuffer();

/*
*
*	@brief	Deleted copy/move constructors/operators to enforce a single instance of this class
*
*/
			CharacterFramebuffer(const CharacterFramebuffer&) = delete;
			CharacterFramebuffer(CharacterFramebuffer&&) = delete;
			CharacterFramebuffer& operator=(const CharacterFramebuffer&) = delete;
			CharacterFramebuffer& operator=(CharacterFramebuffer&&) = delete;

/*
*
*	@brief	Method to select the font from Zephyr's catalog
*
*	@param	font_idx			Index of the font in Zephyr's catalog
*
*	@retval	CfbUnready			If the CFB has not been successfully initialized
*	@retval	ParamFontIndex			If font_idx is bigger then the number of fonts available in Zephyr's catalog
*	@retval	CfbFontSet			If an error occurs when setting the font to that at index font_idx
*	@retval	CfbFontSizeGet			If an error occurs when obtaining the size of the font at index font_idx or if the obtained size is 0
*	@retval	DisplayResolution		If the obtained size of the font is bigger then the obtained size of the display
*	@retval	Ok				If no error occurs
*
*	@pre	The CFB has been successfully initialized
*	@pre	The required font has been set in prj.conf
*	@pre	font_idx is smaller then the available number of fonts
*	@post	If the CFB has not been successfully initialized then error.code is set to CfbUnready
*	@post	If font_idx is out of range then the font is not changed and it is still possible to use it
*	@post	If an error with Zephyr's CFB API occurs then another call is necessary to use the font
*	@post	If an error occurs when setting the font with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to CfbFontSet
*	@post	If an error occurs when obtaining the size of the font with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to CfbFontSizeGet
*	@post	If the size of the font is bigger than the size of the display obtained with Zephyr's CFB API then error.code is set to DisplayResolution
*	@post	On success the font is set to that at index font_idx and error.code is set to Ok
*
*/
			character_framebuffer::ErrorCode font_set(uint8_t font_idx);

/*
*
*	@brief	Method to clear the framebuffer RAM
*
*	@retval	CfbUnready			If the CFB has not been successfully initialized
*	@retval	CfbRamClear			If an error occurs when clearing the RAM
*	@retval Ok				If no error occurs
*
*	@pre	The CFB has been successfully initialized
*	@post	If the CFB has not been successfully initialized then error.code is set to CfbUnready
*	@post	If an error occurs when clearing the RAM with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to CfbRamClear
*	@post	On success the framebuffer RAM is cleared and error.code is set to Ok
*
*/
			character_framebuffer::ErrorCode ram_clear();

/*
*
*	@brief	Method to load a string in a grid of the framebuffer RAM.
*
*	@param	input_string			String to load
*	@param	row_idx				Index of the row where to load the string
*	@param	column_idx			Index of the column where to load the string
*
*	@retval	CfbUnready			If the CFB has not been successfully initialized
*	@retval	ParamPositionIndex		If the position index is out of range
*	@retval	ParamStringLength		If the length of the string from column_idx exceeds the grid
*	@retval	CfbStringLoad			If an error occurs when loading a character in the framebuffer RAM
*	@retval Ok				If no error occurs
*
*	@pre	The CFB has been successfully initialized
*	@pre	The position index is within the valid range
*	@pre	The length of the string from column_idx lets it fit within the grid
*	@post	If the CFB has not been successfully initialized then error.code is set to CfbUnready
*	@post	If the position index is out of range then error.code is set to ParamPositionIndex
*	@post	If the length of the strings from column_idx exceeds the grid then error.code is set to ParamStringLength
*	@post	If an error occurs when loading a character with Zephyr's CFB API then its return value is saved in error.return_value, error.code is set to CfbStringLoad and its position index is saved in error.row_idx and error.column_idx
*	@post	On success input_string is loaded in the framebuffer RAM and error.code is set to Ok
*
*	@par	Invariants
*
*		-The size of the grid is the same as the size of the display obtained with Zephyr's CFB API
*
*		-The number of columns of the grid is equal to the width of the display divided by the width of the font obtained with Zephyr's CFB API
*
*		-The number of rows of the grid is equal to the height of the display divided by the height of the font obtained with Zephyr's CFB API
*
*/
			character_framebuffer::ErrorCode string_load(const std::string_view input_string, const size_t row_idx, const size_t column_idx);

/*
*
*	@brief	Method to write the loaded data in the framebuffer RAM.
*
*	@retval	CfbUnready			If the CFB has not been successfully initialized
*	@retval	CfbRamWrite			If an error occurs when writing the framebuffer RAM
*	@retval Ok				If no error occurs
*
*	@pre	The CFB has been successfully initialized
*	@post	If the CFB has not been successfully initialized then error.code is set to CfbUnready
*	@post	If an error occurs when writing the framebuffer RAM with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to CfbRamWrite
*	@post	On success the loaded data is written in the framebuffer RAM and error.code is set to Ok
*
*/
			character_framebuffer::ErrorCode ram_write();


/*
*
*	@brief	Method to obtain the full error report
*
*	@retval error				The full error struct
*
*/
			[[nodiscard("Called error getter and discarded its return value")]] character_framebuffer::ErrorState error_state_get() const;

		private:

			struct SystemState {
				bool cfb_init = false;
				bool cfb_ready = false;
				bool font_ready = false;
			};

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

			character_framebuffer::ErrorState error;
			SystemState system;
			TargetDisplay display;
			FontState font;

			[[nodiscard("Internal error: necessary struct discarded")]] TargetDisplay init_operations(const struct device* const monochrome_display_device_ptr);
	};
}

#endif
