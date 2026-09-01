#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstddef>
#include <string>

using namespace std;

typedef enum {
	DISPLAY_ERR_OK = 0,
	DISPLAY_ERR_DEVICE,
	DISPLAY_ERR_PARAM,
	DISPLAY_ERR_BUFF_INIT,
	DISPLAY_ERR_FONT_SIZE,
	DISPLAY_ERR_BUFF_CLEAR,
	DISPLAY_ERR_BUFF_STRING,
	DISPLAY_ERR_BUFF_WRITE
} display_errors;

class monochrome_display {
	public:
		monochrome_display(const struct device* const monochrome_display_dt_spec, const string monochrome_display_alias);
		display_errors screen_clear(void);
		display_errors string_print(string input_string, size_t row_idx, size_t column_idx);

	private:
		const struct device* const dt_spec;
		const string alias;
		const size_t height_px;
		const size_t width_px;
		size_t font_height_px;
		size_t font_width_px;
		display_errors error_code;
};

class monochrome_display_controller {
	private:
		display_errors init_sequence(const struct device* const monochrome_display_dt_spec);
		const display_errors init_status;
	public:
		monochrome_display_controller(const struct device* const monochrome_display_dt_spec, const string monochrome_display_alias);
		monochrome_display display;
};

#endif
