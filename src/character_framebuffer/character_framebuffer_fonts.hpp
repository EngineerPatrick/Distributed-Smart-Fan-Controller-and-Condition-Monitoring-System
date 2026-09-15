/**
*
*	@file		character_framebuffer_fonts.hpp
*
*	@brief		Configuration for Zephyr's CFB fonts
*
*	@details	Zephyr does not provide a clean method to link the font's index to the font's name
*
*				This module is meant to overcome that limitation, but the user is responsible to set
*				this configuration correctly during production time
*
*/

#ifndef CHARACTER_FRAMEBUFFER_FONTS_HPP
#define CHARACTER_FRAMEBUFFER_FONTS_HPP

#define FONTS_NUMBER 3
#define FONT_NAME_IDX_0 Small
#define FONT_NAME_IDX_1 Medium
#define FONT_NAME_IDX_2 Large

#define DEFAULT_FONT character_framebuffer::FontName::FONT_NAME_IDX_0

#define FONT_NAME_INIT FONT_NAME_IDX_0, FONT_NAME_IDX_1, FONT_NAME_IDX_2

#define FONT_LIST_INIT character_framebuffer::FontName::FONT_NAME_IDX_0, \
					   character_framebuffer::FontName::FONT_NAME_IDX_1, \
					   character_framebuffer::FontName::FONT_NAME_IDX_2

#endif

/**
*
*	@def		FONTS_NUMBER
*
*	@brief		Number of fonts registered in Zephyr's CFB list
*
*	@details	The default is currently 3 which corresponds to font1016, font1524 and font2032 configured in /subsys/fb/cfb_fonts.c
*
*/

/**
*
*	@def		FONT_NAME_IDX_0
*
*	@brief		Name to give to the font registered at index 0
*
*	@details	The default is currently font1016
*
*/

/**
*
*	@def		FONT_NAME_IDX_1
*
*	@brief		Name to give to the font registered at index 1
*
*	@details	The default is currently font1524
*
*/

/**
*
*	@def		FONT_NAME_IDX_2
*
*	@brief		Name to give to the font registered at index 2
*
*	@details	The default is currently font2032
*
*/

/**
*
*	@def		DEFAULT_FONT
*
*	@brief		Name of the default font to set in the CFB abstraction module
*
*	@details	The user is responsible to insert one of the names previously created with the prefix character_framebuffer::FontName::
*
*/

/**
*
*	@def		FONT_NAME_INIT
*
*	@brief		Initializer list for the FontName enums in the CFB and MC display abstraction modules
*
*	@details	The user is responsible to insert all the created names by separating each one with a comma and a space
*
*/

/**
*
*	@def		FONT_LIST_INIT
*
*	@brief		Initializer list for the fonts array in the CFB abstraction module
*
*	@details	The user is responsible to insert all the created names by separating each one with a comma and a space
*				and with the prefix character_framebuffer::FontName::
*
*	@warning	The user is responsible to order the names in this initializer list so that the index of the fonts in
*				Zephyr's own system corresponds to the index of the fonts in this list
*
*	@warning	Zephyr's currently does not provide a clean method to obtain the font index direclty: the user is responsible
*				to either inspect Zephyr's internal implementation as /subsys/fb/cfb_fonts.c or to evaluate the index of each
*				font by making attempts
*
*/
