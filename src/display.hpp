#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstddef>
#include <string>

#define DISPLAY_ALIAS readings_display

using namespace std;

typedef enum {
	DISPLAY_ERR_OK = 0,
	DISPLAY_ERR_DEVICE,
	DISPLAY_ERR_CONSTRUCTOR,
	DISPLAY_ERR_BUFF_INIT,
	DISPLAY_ERR_BUFF_STRING,
	DISPLAY_ERR_BUFF_CLEAR,
	DISPLAY_ERR_BUFF_WRITE,
	DISPLAY_ERR_PARAM
} display_errors;

display_errors display_screen_clear();

display_errors display_string_print(string input_string, size_t row, size_t column);

#endif
