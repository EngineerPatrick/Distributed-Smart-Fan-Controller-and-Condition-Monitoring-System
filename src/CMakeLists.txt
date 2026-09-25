target_sources(app

	PRIVATE
		main.cpp
)

add_subdirectory(app)

add_subdirectory(services)

add_subdirectory(adapters)
