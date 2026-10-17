target_sources(app

	PRIVATE
		threads_scheduler.cpp

	PUBLIC
		FILE_SET threads_scheduler_headers
		TYPE HEADERS
		FILES
		threads_scheduler.hpp
)
