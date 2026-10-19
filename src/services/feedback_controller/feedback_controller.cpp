#include "feedback_controller.hpp"

feedback_controller::FeedbackCode feedback_controller::ClosedLoopSystem::signal_stability_check(const unsigned int measured_signal) {

	if (measured_signal >= this->signal.stable.current) {

		if (measured_signal - this->signal.stable.current > 10) {
			this->signal.stable.current = measured_signal;
			this->signal.stable.count = 0;
			this->feedback = feedback_controller::FeedbackCode::SignalUnstable;
			return this->feedback;
		}

		this->signal.stable.count++;
	}

	else {

		if (this->signal.stable.current - measured_signal > 10) {
			this->signal.stable.current = measured_signal;
			this->signal.stable.count = 0;
			this->feedback = feedback_controller::FeedbackCode::SignalUnstable;
			return this->feedback;
		}

		this->signal.stable.count++;
	}

	if (this->signal.stable.count == 5) {
		this->feedback = feedback_controller::FeedbackCode::SignalStable;
		return this->feedback;
	}

	this->feedback = feedback_controller::FeedbackCode::SignalUnstable;
	return this->feedback;
}

feedback_controller::FeedbackCode feedback_controller::ClosedLoopSystem::feedback_generate(const unsigned int measured_signal) {

	if (this->system.last_step) {
		this->feedback = feedback_controller::FeedbackCode::ClosestTargetReached;
		return this->feedback;
	}

	if (this->signal_stability_check(measured_signal) == feedback_controller::FeedbackCode::SignalUnstable) {
		return this->feedback;
	}

	if (this->signal.stable.current == this->signal.target) {
		this->feedback = feedback_controller::FeedbackCode::TargetReached;
		return this->feedback;
	}

	this->signal.delta.previous = this->signal.delta.current;

	if (this->signal.stable.current > this->signal.stable.previous) {
		this->signal.delta.current = this->signal.stable.current - this->signal.stable.previous;
	}

	else {
		this->signal.delta.current = this->signal.stable.previous - this->signal.stable.current;
	}

	if (this->signal.stable.current > this->signal.target) {
		this->feedback = feedback_controller::FeedbackCode::StepDown;
	}

	else if (this->signal.stable.current < this->signal.target) {
		this->feedback = feedback_controller::FeedbackCode::StepUp;
	}

	if (this->system.first_step && this->signal.delta.current > this->signal.delta.previous) {
		this->system.last_step = true;
		this->signal.stable.current = this->signal.stable.previous;
	}

	else {
		this->signal.stable.previous = this->signal.stable.current;
	}

	if (!this->system.first_step) {
		this->system.first_step = true;
	}

	return this->feedback;
}

void feedback_controller::ClosedLoopSystem::target_signal_update(const unsigned int target_signal) {
	this->signal.target = target_signal;

	this->signal.stable.count = 0;

	this->signal.delta.current = 0;
	this->signal.delta.previous = 0;

	this->system.first_step = false;
	this->system.last_step = false;
}

void feedback_controller::ClosedLoopSystem::signal_metrics_get(unsigned int& target_signal, unsigned int& stable_signal) const {
	target_signal = this->signal.target;
	stable_signal = this->signal.stable.current;
}

feedback_controller::FeedbackCode feedback_controller::ClosedLoopSystem::feedback_get() const {
	return this->feedback;
}
