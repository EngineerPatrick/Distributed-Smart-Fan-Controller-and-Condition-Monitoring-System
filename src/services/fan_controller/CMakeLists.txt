target_sources(app

	PRIVATE
		fan_controller.cpp

	PUBLIC
		FILE_SET fan_controller_headers
		TYPE HEADERS
		FILES
		fan_controller.hpp
)
