target_sources(app

	PRIVATE
		ui_display.cpp

	PUBLIC
		FILE_SET ui_display_headers
		TYPE HEADERS
		FILES
		ui_display.hpp
)
