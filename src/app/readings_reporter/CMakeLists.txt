target_sources(app

	PRIVATE
		readings_reporter.cpp

	PUBLIC
		FILE_SET readings_reporter_headers
		TYPE HEADERS
		FILES
		readings_reporter.hpp
)
