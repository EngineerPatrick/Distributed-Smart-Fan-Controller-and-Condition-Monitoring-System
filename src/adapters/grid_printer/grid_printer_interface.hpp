#ifndef GRID_PRINTER_INTERFACE_HPP
#define GRID_PRINTER_INTERFACE_HPP

#include "fonts_adapter.hpp"
#include <cstddef>
#include <string_view>

namespace grid_printer_interface {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		CfbUnready,
		TextUnready,
		DisplayResolution,
		ParamGridCoordinates,
		ParamStringLength,
		ParamFontIndex,
		ZDeviceUnready,
		ZCfbInit,
		ZCfbFontSet,
		ZCfbFontSizeGet,
		ZCfbFontKerningSet,
		ZCfbRamClear,
		ZCfbStringWrite,
		ZCfbRamFlush
	};

	struct ErrorState {
		grid_printer_interface::ErrorCode code = grid_printer_interface::ErrorCode::Ok;
		int return_value = 0;
		std::size_t row_idx = 0;
		std::size_t column_idx = 0;
	};

	enum class FontName {FONT_NAME_INIT};

	class DisplayGridInterface {

		public:

			virtual grid_printer_interface::ErrorCode font_set(grid_printer_interface::FontName font_name) = 0;

			virtual grid_printer_interface::ErrorCode cells_clear() = 0;
			virtual grid_printer_interface::ErrorCode cells_string_write(const std::string_view input_string, const std::size_t row_idx, const std::size_t column_idx) = 0;
			virtual grid_printer_interface::ErrorCode cells_print() = 0;

			virtual void grid_sizes_get(std::size_t& grid_width_cells, std::size_t& grid_height_cells) const = 0;
			[[nodiscard("Called error getter and discarded its return value")]]
			virtual grid_printer_interface::ErrorState error_state_get() const = 0;

		protected:

			/*
			*
			*	The destructor is protected to prevent the destruction of any instance but allow derived class to override it
			*
			*/
			virtual ~DisplayGridInterface() = default;
	};
}

#endif
