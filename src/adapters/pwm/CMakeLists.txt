target_sources(app

	PRIVATE
		pwm.cpp

	PUBLIC
		FILE_SET pwm_headers
		TYPE HEADERS
		FILES
		pwm.hpp
)
