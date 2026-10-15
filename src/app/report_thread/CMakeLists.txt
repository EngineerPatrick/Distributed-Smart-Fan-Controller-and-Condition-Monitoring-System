target_sources(app

	PRIVATE
		report_thread.cpp

	PUBLIC
		FILE_SET report_thread_headers
		TYPE HEADERS
		FILES
		report_thread.hpp
)
