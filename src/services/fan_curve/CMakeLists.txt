target_sources(app

	PRIVATE
		fan_curve.cpp

	PUBLIC
		FILE_SET fan_curve_headers
		TYPE HEADERS
		FILES
		fan_curve.hpp
)
