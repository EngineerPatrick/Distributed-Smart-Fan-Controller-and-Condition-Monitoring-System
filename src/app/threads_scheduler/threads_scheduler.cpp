#include "threads_scheduler.hpp"
#include "control_thread.hpp"
#include "report_thread.hpp"
#include <zephyr/kernel.h>
#include <zephyr/kernel/thread_stack.h>

K_THREAD_STACK_DEFINE(feedback_stack, 5000)
K_THREAD_STACK_DEFINE(report_stack, 5000)
K_MUTEX_DEFINE(snapshot);
K_MUTEX_DEFINE(error_report);
K_SEM_DEFINE(runtime_threads, 2, 2)

void threads_scheduler::ThreadsScheduler::feedback_controller_entry(void* context_ptr, void* unused, void* unused2) {
	threads_scheduler::ThreadsScheduler* this_ptr = static_cast<threads_scheduler::ThreadsScheduler*>(context_ptr);
	int temp_c_x100 = 0;
	unsigned int measured_speed_rpm = 0;
	unsigned int target_speed_rpm = 0;
	control_thread::ErrorCode control_thread_error = control_thread::ErrorCode::Ok;

	k_sem_take(&runtime_threads, K_NO_WAIT);

	while (1) {

		control_thread_error = this_ptr->feedback.controller.cycle(temp_c_x100, measured_speed_rpm, target_speed_rpm);

		if (control_thread_error != control_thread::ErrorCode::ControlFanSpeedUnready && control_thread_error != control_thread::ErrorCode::Ok) {

			if (k_mutex_lock(&error_report, K_NO_WAIT) != 0) {
				break;
			}

			this_ptr->error = threads_scheduler::ErrorCode::FeedbackControllerCycle;

			k_mutex_unlock(&error_report);

			break;
		}

		while (k_mutex_lock(&snapshot, K_NO_WAIT) != 0) {
			k_msleep(10);
		}

		this_ptr->readings.temp_c_x100 = temp_c_x100;
		this_ptr->readings.measured_speed_rpm = measured_speed_rpm;
		this_ptr->readings.target_speed_rpm = target_speed_rpm;

		k_mutex_unlock(&snapshot);

		k_msleep(200);
	}

	k_sem_give(&runtime_threads);
}

void threads_scheduler::ThreadsScheduler::report_updater_entry(void* context_ptr, void* unused, void* unused2) {
	threads_scheduler::ThreadsScheduler* this_ptr = static_cast<threads_scheduler::ThreadsScheduler*>(context_ptr);
	int temp_c_x100 = 0;
	unsigned int measured_speed_rpm = 0;

	k_sem_take(&runtime_threads, K_NO_WAIT);

	while (1) {

		while (k_mutex_lock(&snapshot, K_NO_WAIT) != 0) {
			k_msleep(10);
		}

		temp_c_x100 = this_ptr->readings.temp_c_x100;
		measured_speed_rpm = this_ptr->readings.measured_speed_rpm;

		k_mutex_unlock(&snapshot);

		if (this_ptr->report.updater.update(temp_c_x100 / 10, measured_speed_rpm) !=
			report_thread::ErrorCode::Ok) {

			if (k_mutex_lock(&error_report, K_NO_WAIT) != 0) {
				break;
			}

			this_ptr->error = threads_scheduler::ErrorCode::ReportUpdaterUpdate;

			k_mutex_unlock(&error_report);

			break;
		}

		k_msleep(300);
	}

	k_sem_give(&runtime_threads);
}

threads_scheduler::ThreadsScheduler::ThreadsScheduler(
	control_thread::ControlThread& feedback_controller,
	report_thread::ReportThread& report_updater
) :
feedback{{feedback_controller}, {feedback_stack}},
report{{report_updater}, {report_stack}} {

	if (this->feedback.controller.error_get() != control_thread::ErrorCode::Ok) {
		this->error = threads_scheduler::ErrorCode::FeedbackControllerUnready;
		return;
	}

	this->system.feedback_controller_acquired = true;

	if (this->report.updater.error_get() != report_thread::ErrorCode::Ok) {
		this->error = threads_scheduler::ErrorCode::ReportUpdaterUnready;
		return;
	}

	this->system.report_updater_acquired = true;
	this->error = threads_scheduler::ErrorCode::Ok;
}

threads_scheduler::ErrorCode threads_scheduler::ThreadsScheduler::runtime_loop() {

	if (!this->system.feedback_controller_acquired || !this->system.report_updater_acquired) {
		this->error = threads_scheduler::ErrorCode::RuntimeLoopUnready;
		return this->error;
	}

	if (this->feedback.controller.boot() != control_thread::ErrorCode::Ok) {
		this->error = threads_scheduler::ErrorCode::FeedbackControllerBoot;
		return this->error;
	}

	if (this->report.updater.boot() != report_thread::ErrorCode::Ok) {
		this->error = threads_scheduler::ErrorCode::ReportUpdaterBoot;
		return this->error;
	}

	k_msleep(800);

	k_thread_create(
		&(this->feedback.thread),
		this->feedback.stack_ptr,
		K_THREAD_STACK_SIZEOF(feedback_stack),
		this->feedback_controller_entry,
		this, NULL, NULL,
		2,
		0,
		K_NO_WAIT
	);

	k_thread_create(
		&(this->report.thread),
		this->report.stack_ptr,
		K_THREAD_STACK_SIZEOF(report_stack),
		this->report_updater_entry,
		this, NULL, NULL,
		7,
		0,
		K_NO_WAIT
	);

	while (1) {
		k_msleep(500);

		if (k_sem_count_get(&runtime_threads) > 0) {
			break;
		}
	}

	if (this->feedback.controller.stop() != control_thread::ErrorCode::Ok) {}

	return this->error;
}

threads_scheduler::ErrorCode threads_scheduler::ThreadsScheduler::error_get() const {
	return this->error;
}
