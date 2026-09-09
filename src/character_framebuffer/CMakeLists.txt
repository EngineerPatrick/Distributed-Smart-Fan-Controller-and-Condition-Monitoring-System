target_sources(app

	PRIVATE
		character_framebuffer.cpp

	PUBLIC
		FILE_SET character_framebuffer_headers
		TYPE HEADERS
		FILES
		character_framebuffer.hpp
)
