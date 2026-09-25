target_sources(app

	PRIVATE
		temperature_sensor.cpp

	PUBLIC
		FILE_SET temperature_sensor_headers
		TYPE HEADERS
		FILES
		temperature_sensor.hpp
)
