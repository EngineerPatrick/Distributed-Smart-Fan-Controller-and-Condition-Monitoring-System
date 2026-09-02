set(BOARD black_f407ve)
set(DTC_OVERLAY_FILE app.overlay)
set(CONF_FILE prj.conf)

cmake_minimum_required(VERSION 4.2.3)
find_package(Zephyr)
project(Distributed-Smart-Fan-Controller-and-Condition-Monitoring-System)

target_sources(app
	PRIVATE
	src/main.cpp
	src/display_test.cpp
	src/char_framebuff.cpp

	PUBLIC
	FILE_SET HEADERS
	FILES
	src/char_framebuff.hpp
)
