target_sources(app

	PRIVATE
		input_capture.cpp

	PUBLIC
		FILE_SET input_capture_headers
		TYPE HEADERS
		FILES
		input_capture.hpp
)
