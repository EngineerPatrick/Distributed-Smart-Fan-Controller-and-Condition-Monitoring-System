target_sources(app

	PRIVATE
		counter_capture.cpp

	PUBLIC
		FILE_SET counter_capture_headers
		TYPE HEADERS
		FILES
		counter_capture.hpp
)
