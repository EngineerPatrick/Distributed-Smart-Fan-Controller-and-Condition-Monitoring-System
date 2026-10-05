target_sources(app

	PRIVATE
		dashboard.cpp

	PUBLIC
		FILE_SET dashboard_headers
		TYPE HEADERS
		FILES
		dashboard.hpp
)
