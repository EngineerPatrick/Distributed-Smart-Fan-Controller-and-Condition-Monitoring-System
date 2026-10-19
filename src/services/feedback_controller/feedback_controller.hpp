#ifndef FEEDBACK_CONTROLLER_HPP
#define FEEDBACK_CONTROLLER_HPP

#include <cstddef>

namespace feedback_controller {

	enum class [[nodiscard("Discarding an feedback of this type may result in a bug")]] FeedbackCode {
		SignalStable,
		SignalUnstable,
		TargetReached,
		ClosestTargetReached,
		StepDown,
		StepUp
	};

	class ClosedLoopSystem {

		public:

			void target_signal_update(const unsigned int target_signal);
			feedback_controller::FeedbackCode feedback_generate(const unsigned int measured_signal);

			void signal_metrics_get(unsigned int& target_signal, unsigned int& current_signal) const;
			[[nodiscard("Called feedback getter and discarded its return value")]]
			feedback_controller::FeedbackCode feedback_get() const;

		private:

			struct StableSignalState {
				unsigned int current = 0;
				unsigned int previous = 0;
				std::size_t count = 0;
			};

			struct SignalDeltaState {
				unsigned int current = 0;
				unsigned int previous = 0;
			};

			struct SignalState {
				unsigned int target = 0;
				feedback_controller::ClosedLoopSystem::StableSignalState stable;
				feedback_controller::ClosedLoopSystem::SignalDeltaState delta;
			};

			struct SystemState {
				bool last_step = false;
				bool first_step = false;
			};

			feedback_controller::ClosedLoopSystem::SignalState signal;
			feedback_controller::ClosedLoopSystem::SystemState system;
			feedback_controller::FeedbackCode feedback;

			feedback_controller::FeedbackCode signal_stability_check(const unsigned int measured_signal);
	};
}

#endif
