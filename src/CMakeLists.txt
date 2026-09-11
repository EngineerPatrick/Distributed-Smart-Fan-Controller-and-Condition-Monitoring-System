target_sources(app

	PRIVATE
		main.cpp
)

add_subdirectory(monochrome_display)

add_subdirectory(character_framebuffer)

add_subdirectory(ui_display)
