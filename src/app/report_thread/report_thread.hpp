#ifndef DASHBOARD_THREAD_HPP
#define DASHBOARD_THREAD_HPP

#include "readings_reporter.hpp"

namespace report_thread {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DashboardUnready,
		UpdateUnready,
		StructurePrint,
		SpeedPrint,
		TemperaturePrint
	};

	class ReportThread {

		public:

			ReportThread(readings_reporter::Dashboard& controller_state);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			ReportThread(const ReportThread&) = delete;
			ReportThread(ReportThread&&) = delete;
			ReportThread& operator=(const ReportThread&) = delete;
			ReportThread& operator=(ReportThread&&) = delete;

			report_thread::ErrorCode boot();
			report_thread::ErrorCode update(const int measured_temp_c_x10, const unsigned int speed_rpm);

			[[nodiscard("Called error getter and discarded its return value")]]
			report_thread::ErrorCode error_get() const;

		private:

			struct SystemState {
				bool dashboard_acquired = false;
				bool update_ready = false;
			};

			readings_reporter::Dashboard& controller_state;

			report_thread::ReportThread::SystemState system;
			report_thread::ErrorCode error = report_thread::ErrorCode::Ok;

	};
}

#endif
