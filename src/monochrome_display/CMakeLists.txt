target_sources(app

	PRIVATE
		monochrome_display.cpp

	PUBLIC
		FILE_SET monochrome_display_headers
		TYPE HEADERS
		FILES
		monochrome_display.hpp
)
