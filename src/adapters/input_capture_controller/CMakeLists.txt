target_sources(app

	PRIVATE
		input_capture_controller.cpp

	PUBLIC
		FILE_SET input_capture_controller_headers
		TYPE HEADERS
		FILES
		input_capture_controller.hpp
)
