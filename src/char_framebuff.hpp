#ifndef CHAR_FRAMEBUFF_HPP
#define CHAR_FRAMEBUFF_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>

enum class DisplayErrors:uint8_t {
	Ok,
	Device,
	Param,
	CfbInit,
	CfbOperation
};

struct DisplayErrorReport {
	DisplayErrors display;
	int cfb;
};

class MonochromeDisplay {
	public:
		MonochromeDisplay(const struct device* const monochrome_display_dt_spec);
		DisplayErrors screen_clear(void);
		DisplayErrors string_load(const std::string_view input_string, const size_t row_idx, const size_t column_idx);
		DisplayErrors screen_print(void);

	private:
		const struct device* const dt_spec;
		DisplayErrorReport error_report;
};

#endif
