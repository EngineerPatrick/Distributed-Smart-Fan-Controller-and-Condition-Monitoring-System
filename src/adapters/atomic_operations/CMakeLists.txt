target_sources(app

	PRIVATE
		atomic_operations.cpp

	PUBLIC
		FILE_SET atomic_operations_headers
		TYPE HEADERS
		FILES
		atomic_operations.hpp
)
