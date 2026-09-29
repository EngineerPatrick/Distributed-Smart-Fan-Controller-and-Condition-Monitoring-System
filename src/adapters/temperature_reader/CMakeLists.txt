target_sources(app

	PRIVATE
		temperature_reader.cpp

	PUBLIC
		FILE_SET temperature_reader_headers
		TYPE HEADERS
		FILES
		temperature_reader.hpp
		temperature_reader_interface.hpp
)
