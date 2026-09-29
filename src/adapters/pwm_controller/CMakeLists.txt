target_sources(app

	PRIVATE
		pwm_controller.cpp

	PUBLIC
		FILE_SET pwm_controller_headers
		TYPE HEADERS
		FILES
		pwm_controller.hpp
		pwm_controller_interface.hpp
)
