target_sources(app

	PRIVATE
		pulse_reader.cpp

	PUBLIC
		FILE_SET pulse_reader_headers
		TYPE HEADERS
		FILES
		pulse_reader.hpp
		pulse_reader_interface.hpp
)
