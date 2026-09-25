target_sources(app

	PRIVATE
		grid_printer.cpp

	PUBLIC
		FILE_SET grid_printer_headers
		TYPE HEADERS
		FILES
		grid_printer.hpp
)
