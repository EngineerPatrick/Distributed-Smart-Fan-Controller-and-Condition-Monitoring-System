#ifndef GRID_PRINTER_HPP
#define GRID_PRINTER_HPP

#include "fonts_adapter.hpp"
#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>
#include <zephyr/device.h>

namespace grid_printer {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		CfbUnready,
		TextUnready,
		DisplayResolution,
		ParamGridCoordinates,
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
		ErrorCode code = grid_printer::ErrorCode::Ok;
		int return_value = 0;
		std::size_t row_idx = 0;
		std::size_t column_idx = 0;
	};

	enum class FontName {FONT_NAME_INIT};

	class GridPrinter {

		public:

			explicit GridPrinter(const struct device* const display_device_ptr);
			~GridPrinter();

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
			grid_printer::ErrorCode cells_string_write(const std::string_view input_string, const std::size_t row_idx, const std::size_t column_idx);
			grid_printer::ErrorCode cells_print();

			[[nodiscard("Called error getter and discarded its return value")]] grid_printer::ErrorState error_state_get() const;

		private:

			struct SystemState {
				bool cfb_init = false;
				bool cfb_ready = false;
				bool text_ready = false;
			};

			struct DisplayState {
				const struct device* const device_ptr = nullptr;
				const std::size_t width_px = 0;
				const std::size_t height_px = 0;
			};

			struct GridState {
				std::size_t width_cells = 0;
				std::size_t height_cells = 0;
			};

			struct FontState {
				std::uint8_t idx = 0;
				std::uint8_t width_px = 0;
				std::uint8_t height_px = 0;
			};

			grid_printer::ErrorState error;
			grid_printer::GridPrinter::SystemState system;
			grid_printer::GridPrinter::DisplayState display;
			grid_printer::GridPrinter::GridState grid;

			std::array<grid_printer::FontName, FONTS_NUMBER> font_list = {FONT_LIST_INIT};

			grid_printer::GridPrinter::FontState font;

			[[nodiscard("Internal error: necessary struct discarded")]]
			grid_printer::GridPrinter::DisplayState init_operations(const struct device* const display_device_ptr);
	};
}

#endif
