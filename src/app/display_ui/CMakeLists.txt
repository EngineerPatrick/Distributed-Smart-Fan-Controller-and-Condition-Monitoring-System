target_sources(app

	PRIVATE
		display_ui.cpp

	PUBLIC
		FILE_SET display_ui_headers
		TYPE HEADERS
		FILES
		display_ui.hpp
)
