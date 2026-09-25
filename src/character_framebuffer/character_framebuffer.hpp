/**
*
*	@file		character_framebuffer.hpp
*
*	@brief		Public API for the character_framebuffer module
*
*	@details	Adapter module for Zephyr's Monochrome Character Framebuffer API
*
*				Supports text rendering, font selection and Zephyr's CFB API return values
*
*/

#ifndef CHARACTER_FRAMEBUFFER_HPP
#define CHARACTER_FRAMEBUFFER_HPP

#include "character_framebuffer_fonts.hpp"
#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>
#include <zephyr/device.h>

namespace character_framebuffer {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		CfbUnready,
		TextUnready,
		DisplayResolution,
		ParamPixelCoordinates,
		ParamStringLength,
		ParamFontIndex,
		ZCfbInit,
		ZCfbFontSet,
		ZCfbFontSizeGet,
		ZCfbFontKerningSet,
		ZCfbRamClear,
		ZCfbStringWrite,
		ZCfbRamFlush
	};

	struct ErrorState {
		ErrorCode code = character_framebuffer::ErrorCode::Ok;
		int return_value = 0;
		std::size_t row_px = 0;
		std::size_t column_px = 0;
	};

	enum class FontName {FONT_NAME_INIT};

	class CharacterFramebuffer {

		public:

			explicit CharacterFramebuffer(const struct device* const monochrome_display_device_ptr);
			~CharacterFramebuffer();

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			CharacterFramebuffer(const CharacterFramebuffer&) = delete;
			CharacterFramebuffer(CharacterFramebuffer&&) = delete;
			CharacterFramebuffer& operator=(const CharacterFramebuffer&) = delete;
			CharacterFramebuffer& operator=(CharacterFramebuffer&&) = delete;

			character_framebuffer::ErrorCode font_set(character_framebuffer::FontName font_name);

			character_framebuffer::ErrorCode ram_clear();
			character_framebuffer::ErrorCode ram_string_write(const std::string_view input_string, const std::size_t row_px, const std::size_t column_px);
			character_framebuffer::ErrorCode ram_flush();

			character_framebuffer::ErrorCode display_sizes_get(std::size_t& display_width_px, std::size_t& display_height_px);
			character_framebuffer::ErrorCode font_sizes_get(std::size_t& font_width_px, std::size_t& font_height_px);
			[[nodiscard("Called error getter and discarded its return value")]] character_framebuffer::ErrorState error_state_get() const;

		private:

			struct SystemState {
				bool cfb_init = false;
				bool cfb_ready = false;
				bool kerning_ready = false;
				bool text_ready = false;
			};

			struct DisplayDevice {
				const struct device* const device_ptr = nullptr;
				const std::size_t width_px = 0;
				const std::size_t height_px = 0;
			};

			struct FontState {
				std::uint8_t idx = 0;
				std::uint8_t width_px = 0;
				std::uint8_t height_px = 0;
			};

			character_framebuffer::ErrorState error;
			character_framebuffer::CharacterFramebuffer::SystemState system;
			character_framebuffer::CharacterFramebuffer::DisplayDevice display;

			std::array<character_framebuffer::FontName, FONTS_NUMBER> font_list = {FONT_LIST_INIT};

			character_framebuffer::CharacterFramebuffer::FontState font;

			[[nodiscard("Internal error: necessary struct discarded")]]
			character_framebuffer::CharacterFramebuffer::DisplayDevice init_operations(const struct device* const monochrome_display_device_ptr);
	};
}

#endif

/**
*
*	@enum		character_framebuffer::ErrorCode
*
*	@brief		Error codes of the module
*
*	@invariant	If an error related to Zephyr's CFB API occurs then the code is prefixed with "ZCfb"
*
*/

/**
*
*	@struct		character_framebuffer::ErrorState
*
*	@brief		Data structure for all types of errors
*
*	@invariant	All members of this struct are set at the end of every fallible method of CharacterFramebuffer
*	@invariant	If a module-based error occurs then it is represented by code and the other members are equal to 0
*	@invariant	If an error related to Zephyr's CFB API occurs then it is represented by both code and return_value
*	@invariant	If an error related to Zephyr's CFB API occurs during the writing phase in ram_string_write then the position on the display where the error has occurred is saved in the last 2 members, which are equal to 0 in any other error case
*
*/

/**
*
*	@enum		character_framebuffer::FontName
*
*	@brief		Name of all the available fonts
*
*	@warning	Due to Zephyr's own limitation the user is responsible for configuring this in production time as explained in character_framebuffer_fonts.hpp
*
*/

/**
*
*	@class		character_framebuffer::CharacterFramebuffer
*
*	@brief		Class for Zephyr's Monochrome Character Framebuffer API
*
*	@warning	Since Zephyr currently supports only 1 character framebuffer there should be only 1 instance of this class
*
*/

/**
*
*	@var		character_framebuffer::CharacterFramebuffer::font_list
*
*	@brief		List of the available fonts
*
*	@invariant	The index of each font in Zephyr's internal system is equal to the index of the font in this array
*
*	@warning	Due to Zephyr's own limitation the user is responsible for configuring this in production time as explained in character_framebuffer_fonts.hpp
*
*/

/**
*
*	@fn			character_framebuffer::CharacterFramebuffer::CharacterFramebuffer(const struct device* const monochrome_display_device_ptr)
*
*	@brief		Constructor to initialize the framebuffer to target a specific display
*
*	@param[in]	monochrome_display_device_ptr	Pointer to the device struct of the target display
*
*	@pre		monochrome_display_device_ptr points to a valid device struct of a monochrome display
*	@post		If the target display is not ready to be used then error.code is set to DeviceUnready
*	@post		If an error occurs when initializing the framebuffer with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbInit
*	@post		If the display width and height parameters obtained from Zephyr's CFB API are 0 then error.code is set to DisplayResolution
*	@post		If an error occurs when setting the kerning to 0 with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbFontKerningSet
*	@post		If an error occurs when setting the font to that at index 0 with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbFontSet
*	@post		If an error occurs when obtaining the size of the font with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbFontSizeGet
*	@post		If the size of the font obtained with Zephyr's CFB API is 0 then its return value is saved in error.return_value and error.code is set to ZCfbFontSizeGet
*	@post		If the obtained size of the font is greater than the size of the display obtained with Zephyr's CFB API then error.code is set to DisplayResolution
*	@post		On success the target device is ready, the CFB is initialized, the kerning is set to 0 px, the font is set to index 0, and error.code is set to Ok
*
*/

/**
*
*	@fn			character_framebuffer::CharacterFramebuffer::~CharacterFramebuffer()
*
*	@brief		Destructor to deinitialize the framebuffer
*
*	@post		If the CFB has been successfully initialized by the constructor then it is deinitialized
*
*/

/**
*
*	@fn			character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::font_set(character_framebuffer::FontName font_name)
*
*	@brief		Method to select the font from the configured list
*
*	@param[in]	font_name						Name of the font in the configured list
*
*	@retval		CfbUnready					If the display is not ready to be used
*	@retval		ZCfbFontKerningSet				If an error occurs when setting the kerning to 0
*	@retval		ParamFontIndex					If the assigned font index is greater than or equal to the number of fonts available in Zephyr's catalog
*	@retval		ZCfbFontSet						If an error occurs when setting the font to that at the specified index
*	@retval		ZCfbFontSizeGet					If an error occurs when obtaining the size of the font at the specified index or if the obtained size is 0
*	@retval		DisplayResolution				If the obtained size of the font is greater than the obtained size of the display
*	@retval		Ok								If no error occurs
*
*	@pre		The required font is available from Zephyr's system
*	@pre		The required font has been correctly configured in the font header
*	@post		If the display is not ready to be used then error.code is set to CfbUnready
*	@post		If any error besides the CFB unready occurs then another call is required to use the font
*	@post		If an error occurs when setting the kerning to 0 with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbFontKerningSet
*	@post		If an error occurs when setting the font to that at the specified index with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbFontSet
*	@post		If an error occurs when obtaining the size of the font with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbFontSizeGet
*	@post		If the size of the font is greater than the size of the display obtained with Zephyr's CFB API then error.code is set to DisplayResolution
*	@post		On success the font is set to that at the specified index and error.code is set to Ok
*
*/

/**
*
*	@fn			character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::ram_clear()
*
*	@brief		Method to clear framebuffer's RAM
*
*	@retval		CfbUnready					If the display is not ready to be used
*	@retval		ZCfbRamClear						If an error occurs when clearing the RAM
*	@retval 	Ok								If no error occurs
*
*	@post		If the display is not ready to be used then error.code is set to CfbUnready
*	@post		If an error occurs when clearing the RAM with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbRamClear
*	@post		On success framebuffer's RAM is cleared and error.code is set to Ok
*
*/

/**
*
*	@fn			character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::ram_string_write(const std::string_view input_string, const std::size_t row_px, const std::size_t column_px)
*
*	@brief		Method to write a string at specific pixel coordinates of framebuffer's RAM
*
*	@param[in]	input_string					String to write
*	@param[in]	row_px							Pixel row coordinate
*	@param[in]	column_px						Pixel column coordinate
*
*	@retval		TextUnready						If the font is not ready to be used
*	@retval		ParamPixelCoordinates			If the pixel coordinates are out of range
*	@retval		ParamStringLength				If the length of the string from column_px exceeds the display width
*	@retval		ZCfbStringWrite					If an error occurs when writing a character in framebuffer's RAM
*	@retval 	Ok								If no error occurs
*
*	@pre		input_string contains a string compatible with Zephyr's font
*	@post		If the font is not ready to be used then error.code is set to TextUnready
*	@post		If the pixel coordinates are out of range then error.code is set to ParamPixelCoordinates
*	@post		If the length of the strings from column_px exceeds the display width then error.code is set to ParamStringLength
*	@post		If an error occurs when writing a character with Zephyr's CFB API then its return value is saved in error.return_value, error.code is set to ZCfbStringWrite and its pixel coordinates are saved in error.row_px and error.column_px
*	@post		On success input_string is written in framebuffer's RAM and error.code is set to Ok
*
*	@invariant	All characters of the string are print at the same pixel row coordinate
*	@invariant	All characters of the string are print at progressively increasing pixel column coordinates with increment equal to the width of the font
*
*/

/**
*
*	@fn			character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::ram_flush()
*
*	@brief		Method to flush framebuffer's RAM to the target display's RAM
*
*	@retval		CfbUnready					If the display is not ready to be used
*	@retval		ZCfbRamFlush						If an error occurs when flushing framebuffer's RAM
*	@retval 	Ok								If no error occurs
*
*	@post		If the display is not ready to be used then error.code is set to CfbUnready
*	@post		If an error occurs when flushing framebuffer's RAM with Zephyr's CFB API then its return value is saved in error.return_value and error.code is set to ZCfbRamFlush
*	@post		On success the data in framebuffer's RAM is flushed in display's RAM and error.code is set to Ok
*
*/

/**
*
*	@fn			character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::display_sizes_get(std::size_t& display_width_px, std::size_t& display_height_px)
*
*	@brief		Method to obtain the sizes of the targeted display
*
*	@param[out]	display_width_px				Reference to the variable where to store the display width
*	@param[out]	display_height_px				Reference to the variable where to store the display height
*
*	@retval		CfbUnready					If the display is not ready to be used
*	@retval		Ok								If no error occurs
*
*	@post		If the display is not ready to be used then error.code is set to CfbUnready
*	@post		On success the sizes of the display are assigned to the references and error.code is set to Ok
*
*/

/**
*
*	@fn			character_framebuffer::ErrorCode character_framebuffer::CharacterFramebuffer::font_sizes_get(std::size_t& font_width_px, std::size_t& font_height_px)
*
*	@brief		Method to obtain the sizes of the font
*
*	@param[out]	font_width_px					Reference to the variable where to store the font width
*	@param[out]	font_height_px					Reference to the variable where to store the font height
*
*	@retval		TextUnready						If the font is not ready to be used
*	@retval		Ok								If no error occurs
*
*	@post		If the font is not ready to be used then error.code is set to TextUnready
*	@post		On success the sizes of the font are assigned to the references and error.code is set to Ok
*
*/

/**
*
*	@fn			character_framebuffer::ErrorState character_framebuffer::CharacterFramebuffer::error_state_get() const
*
*	@brief		Method to obtain the full error report
*
*	@return										The full error struct
*
*/
