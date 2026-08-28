set(BOARD black_f407ve)
set(DTC_OVERLAY_FILE app.overlay)
set(CONF_FILE prj.conf)

find_package(Zephyr)

target_sources(app
	PRIVATE
	src/main.cpp
)
