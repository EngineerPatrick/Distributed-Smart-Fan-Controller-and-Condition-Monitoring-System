target_sources(app

	PRIVATE
		control_thread.cpp

	PUBLIC
		FILE_SET control_thread_headers
		TYPE HEADERS
		FILES
		control_thread.hpp
)
