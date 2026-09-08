target_sources(app

	PRIVATE
		display_test.cpp
		character_framebuffer.cpp

	PUBLIC
		FILE_SET HEADERS
		FILES
		character_framebuffer.hpp
)
