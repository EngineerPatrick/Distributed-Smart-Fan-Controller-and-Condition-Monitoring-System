#ifndef CONTROL_THREAD_HPP
#define CONTROL_THREAD_HPP

#include "fan_controller.hpp"
#include "fan_curve.hpp"
#include "temperature_reader_interface.hpp"

namespace control_thread {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		BootUnready,
		RuntimeLoopUnready,
		ControlFanUnready,
		ControlFanBoot,
		ControlFanStop,
		ControlFanSpeedMeasure,
		TemperatureReaderUnready,
		TemperatureReaderValue,
		FanCurveUnready,
		FanCurveSpeedGet
	};

	class ControlThread {

		public:

			ControlThread(
				fan_controller::FourWireFan& control_fan,
				temperature_reader_interface::TemperatureSignalInterface& temperature_signal,
				fan_curve::FanCurve& fan_curve
			);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			ControlThread(const ControlThread&) = delete;
			ControlThread(ControlThread&&) = delete;
			ControlThread& operator=(const ControlThread&) = delete;
			ControlThread& operator=(ControlThread&&) = delete;

			control_thread::ErrorCode boot();
			control_thread::ErrorCode cycle();
			control_thread::ErrorCode stop();

			[[nodiscard("Called error getter and discarded its return value")]]
			control_thread::ErrorCode error_get() const;

		private:

			struct SystemState {
				bool boot_ready = false;
				bool loop_ready = false;
			};

			fan_controller::FourWireFan& control_fan;
			temperature_reader_interface::TemperatureSignalInterface& temperature_signal;
			fan_curve::FanCurve& fan_curve;

			control_thread::ControlThread::SystemState system;

			control_thread::ErrorCode error;

	};
}

#endif
