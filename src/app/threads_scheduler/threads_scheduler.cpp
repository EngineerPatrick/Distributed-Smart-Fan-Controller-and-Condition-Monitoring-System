#include "threads_scheduler.hpp"
#include "control_thread.hpp"
#include "report_thread.hpp"
#include <zephyr/kernel.h>
#include <zephyr/kernel/thread_stack.h>

K_THREAD_STACK_DEFINE(feedback_stack, 50000)
K_THREAD_STACK_DEFINE(report_stack, 50000)

void threads_scheduler::ThreadsScheduler::feedback_controller_entry(void* context_ptr, void* unused, void* unused2) {
	threads_scheduler::ThreadsScheduler* this_ptr = static_cast<threads_scheduler::ThreadsScheduler*>(context_ptr);

	if (this_ptr->feedback.controller.cycle(this_ptr->readings.temp_c_x10, this_ptr->readings.measured_speed_rpm, this_ptr->readings.target_speed_rpm) !=
		control_thread::ErrorCode::Ok) {
		this_ptr->error = threads_scheduler::ErrorCode::FeedbackControllerCycle;
	}
}

void threads_scheduler::ThreadsScheduler::report_updater_entry(void* context_ptr, void* unused, void* unused2) {
	threads_scheduler::ThreadsScheduler* this_ptr = static_cast<threads_scheduler::ThreadsScheduler*>(context_ptr);

	if (this_ptr->report.updater.update(this_ptr->readings.temp_c_x10, this_ptr->readings.measured_speed_rpm) !=
		report_thread::ErrorCode::Ok) {
		this_ptr->error = threads_scheduler::ErrorCode::ReportUpdaterUpdate;
	}
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

	k_thread_create(
		&(this->feedback.thread),
		this->feedback.stack_ptr,
		K_THREAD_STACK_SIZEOF(feedback_stack),
		this->feedback_controller_entry,
		this, NULL, NULL,
		K_HIGHEST_APPLICATION_THREAD_PRIO,
		0,
		K_NO_WAIT
	);

	this->system.feedback_controller_acquired = true;

	if (this->report.updater.error_get() != report_thread::ErrorCode::Ok) {
		this->error = threads_scheduler::ErrorCode::ReportUpdaterUnready;
		return;
	}

	k_thread_create(
		&(this->report.thread),
		this->report.stack_ptr,
		K_THREAD_STACK_SIZEOF(report_stack),
		this->report_updater_entry,
		this, NULL, NULL,
		K_HIGHEST_APPLICATION_THREAD_PRIO - 1,
		0,
		K_NO_WAIT
	);

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

	while (1) {

		k_msleep(100);

		k_thread_start(&this->feedback.thread);

		if (this->error != threads_scheduler::ErrorCode::Ok) {

			if (this->feedback.controller.stop() != control_thread::ErrorCode::Ok) {}

			break;
		}

		k_msleep(100);

		k_thread_start(&this->report.thread);

		if (this->error != threads_scheduler::ErrorCode::Ok) {

			if (this->feedback.controller.stop() != control_thread::ErrorCode::Ok) {}

			break;
		}
	}

	return this->error;
}

threads_scheduler::ErrorCode threads_scheduler::ThreadsScheduler::error_get() const {
	return this->error;
}
