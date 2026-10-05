target_sources(app

	PRIVATE
		pwm_generator.cpp

	PUBLIC
		FILE_SET pwm_generator_headers
		TYPE HEADERS
		FILES
		pwm_generator.hpp
		pwm_generator_interface.hpp
)
