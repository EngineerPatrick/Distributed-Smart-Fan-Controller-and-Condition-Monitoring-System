#ifndef THREADS_SCHEDULER_HPP
#define THREADS_SCHEDULER_HPP

#include "control_thread.hpp"
#include "report_thread.hpp"
#include <zephyr/kernel.h>

namespace threads_scheduler {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		FeedbackControllerUnready,
		FeedbackControllerCycle,
		FeedbackControllerBoot,
		ReportUpdaterUnready,
		ReportUpdaterUpdate,
		ReportUpdaterBoot,
		RuntimeLoopUnready
	};

	class ThreadsScheduler {

		public:

			ThreadsScheduler(
				control_thread::ControlThread& feedback_controller,
				report_thread::ReportThread& report_updater
			);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class through these operations
			*
			*/
			ThreadsScheduler(const ThreadsScheduler&) = delete;
			ThreadsScheduler(ThreadsScheduler&&) = delete;
			ThreadsScheduler& operator=(const ThreadsScheduler&) = delete;
			ThreadsScheduler& operator=(ThreadsScheduler&&) = delete;

			threads_scheduler::ErrorCode runtime_loop();

			[[nodiscard("Called error getter and discarded its return value")]]
			threads_scheduler::ErrorCode error_get() const;

		private:

			struct SystemState {
				bool feedback_controller_acquired = false;
				bool report_updater_acquired = false;
			};

			struct FeedbackState {
				control_thread::ControlThread& controller;
				k_thread_stack_t* stack_ptr;
				struct k_thread thread;
			};

			struct ReportState {
				report_thread::ReportThread& updater;
				k_thread_stack_t* stack_ptr;
				struct k_thread thread;
			};

			struct ReadingsState {
				int temp_c_x10 = 0;
				unsigned int measured_speed_rpm = 0;
				unsigned int target_speed_rpm = 0;
			};

			threads_scheduler::ThreadsScheduler::FeedbackState feedback;
			threads_scheduler::ThreadsScheduler::ReportState report;
			threads_scheduler::ThreadsScheduler::ReadingsState readings;
			threads_scheduler::ThreadsScheduler::SystemState system;
			threads_scheduler::ErrorCode error = threads_scheduler::ErrorCode::Ok;

			static void feedback_controller_entry(void* context_ptr, void* unused, void* unused2);
			static void report_updater_entry(void* context_ptr, void* unused, void* unused2);
	};
}

#endif
