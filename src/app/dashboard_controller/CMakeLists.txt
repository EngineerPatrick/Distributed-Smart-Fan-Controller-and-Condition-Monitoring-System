target_sources(app

	PRIVATE
		dashboard_controller.cpp

	PUBLIC
		FILE_SET dashboard_controller_headers
		TYPE HEADERS
		FILES
		dashboard_controller.hpp
)
