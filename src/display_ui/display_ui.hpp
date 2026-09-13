/**
*
*	@file		display_ui.hpp
*
*	@brief		Public API for the display_ui module
*
*	@details	Prints the UI on the dsplay
*
*				Accesses an instance of MonochromeDisplay
*
*/

#ifndef DISPLAY_UI_HPP
#define DISPLAY_UI_HPP

#include "monochrome_display.hpp"
#include <cstdint>

namespace display_ui {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DisplayOperation,
		ParamTemp
	};

	display_ui::ErrorCode fixed_ui_print(monochrome_display::MonochromeDisplay& mc_obj);
	display_ui::ErrorCode temp_value_print(monochrome_display::MonochromeDisplay& mc_obj, std::int16_t temp_c_x10);
}

#endif

/**
*
*	@enum 		display_ui::ErrorCode
*
*	@brief		Error codes of the module
*
*/

/**
*
*	@fn 		display_ui::ErrorCode fixed_ui_print(monochrome_display::MonochromeDisplay& mc_obj)
*
*	@brief		Prints the fixed part of the UI on the display
*
*	@param[in]	mc_obj							Instance of the MonochromeDisplay class
*
*	@retval		DisplayOperation				If an error occurs whith the MonochromeDisplay class
*	@retval		Ok								If no error occurs
*
*	@pre		mc_obj must be a valid instance of the MonochromeDisplay class correctly initialized
*	@post		If an error occurs whith the MonochromeDisplay class then changes are left on RAM's content
*	@post		On success the title and the units of measurement are printed on the display
*
*/

/**
*
*	@fn 		display_ui::ErrorCode temp_value_print(monochrome_display::MonochromeDisplay& mc_obj, std::int16_t temp_c_x10)
*
*	@brief		Prints the value of the tempearature reading on the display
*
*	@param[in]	mc_obj							Instance of the MonochromeDisplay class
*	@param[in]	temp_c_x10						Value of the temperature reading in tenth of Celsius degrees
*
*	@retval		ParamTemp						If the value of the temperature is out of range
*	@retval		DisplayOperation				If an error occurs whith the MonochromeDisplay class
*	@retval		Ok								If no error occurs
*
*	@pre		mc_obj must be a valid instance of the MonochromeDisplay class correctly initialized
*	@post		If the value of the temperature is out of range then RAM's content are left unchanged
*	@post		If an error occurs whith the MonochromeDisplay class then changes are left on RAM's content
*	@post		On success the temperature value is printed on the display
*
*/
