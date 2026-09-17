target_sources(app

	PRIVATE
		main.cpp
)

add_subdirectory(monochrome_display)

add_subdirectory(character_framebuffer)

add_subdirectory(display_ui)

add_subdirectory(temperature_sensor)

add_subdirectory(pwm)
