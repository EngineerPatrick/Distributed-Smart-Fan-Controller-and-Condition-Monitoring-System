target_sources(app

	PRIVATE
		input_capture_service.cpp

	PUBLIC
		FILE_SET input_capture_service_headers
		TYPE HEADERS
		FILES
		input_capture_service.hpp
)
