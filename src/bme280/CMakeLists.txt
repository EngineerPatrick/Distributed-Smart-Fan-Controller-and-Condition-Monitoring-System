target_sources(app

	PRIVATE
		bme280.cpp

	PUBLIC
		FILE_SET bme280_headers
		TYPE HEADERS
		FILES
		bme280.hpp
)
