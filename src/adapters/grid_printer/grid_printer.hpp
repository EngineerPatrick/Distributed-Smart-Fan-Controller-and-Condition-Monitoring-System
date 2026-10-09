#ifndef GRID_PRINTER_HPP
#define GRID_PRINTER_HPP

#include "grid_printer_interface.hpp"
#include "fonts_adapter.hpp"
#include <cstddef>
#include <cstdint>
#include <array>
#include <string_view>
#include <zephyr/device.h>

namespace grid_printer {

	class DisplayGrid : public grid_printer_interface::DisplayGridInterface {

		public:

			explicit DisplayGrid(const struct device* const display_device_ptr);
			~DisplayGrid() override;

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			DisplayGrid(const DisplayGrid&) = delete;
			DisplayGrid(DisplayGrid&&) = delete;
			DisplayGrid& operator=(const DisplayGrid&) = delete;
			DisplayGrid& operator=(DisplayGrid&&) = delete;

			grid_printer_interface::ErrorCode font_set(grid_printer_interface::FontName font_name) override;

			grid_printer_interface::ErrorCode cells_clear() override;
			grid_printer_interface::ErrorCode cells_string_write(const std::string_view input_string, const std::size_t row_idx, const std::size_t column_idx) override;
			grid_printer_interface::ErrorCode cells_print() override;

			void grid_sizes_get(std::size_t& grid_width_cells, std::size_t& grid_height_cells) const override;
			[[nodiscard("Called error getter and discarded its return value")]]
			grid_printer_interface::ErrorState error_state_get() const override;

		private:

			struct SystemState {
				bool character_framebuffer_acquired = false;
				bool character_framebuffer_ready = false;
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

			grid_printer_interface::ErrorState error;
			grid_printer::DisplayGrid::SystemState system;
			grid_printer::DisplayGrid::DisplayState display;
			grid_printer::DisplayGrid::GridState grid;

			std::array<grid_printer_interface::FontName, FONTS_NUMBER> font_list = {FONT_LIST_INIT};

			grid_printer::DisplayGrid::FontState font;

			[[nodiscard("Internal error: necessary struct discarded")]]
			grid_printer::DisplayGrid::DisplayState init_operations(const struct device* const display_device_ptr);
	};
}

#endif
