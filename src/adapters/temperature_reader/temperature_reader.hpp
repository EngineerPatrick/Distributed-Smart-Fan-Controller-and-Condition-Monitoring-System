#ifndef TEMPERATURE_READER_HPP
#define TEMPERATURE_READER_HPP

#include "temperature_reader_interface.hpp"
#include <cstddef>
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor_data_types.h>

#define CONCAT_UNDRSCR(str1, str2) str1 ## _ ## str2
#define DEFINE_TEMPERATURE_SENSOR(alias) SENSOR_DT_READ_IODEV(CONCAT_UNDRSCR(iodev, alias), DT_ALIAS(alias), {SENSOR_CHAN_AMBIENT_TEMP, 0}); \
		\
		RTIO_DEFINE(CONCAT_UNDRSCR(ctx, alias), 1, 1); \
		\
		temperature_reader::SensorDevice CONCAT_UNDRSCR(alias, device){ \
			{DEVICE_DT_GET(DT_ALIAS(alias))}, \
			{&CONCAT_UNDRSCR(iodev, alias)}, \
			{&CONCAT_UNDRSCR(ctx, alias)} \
		};

#define TEMPERATURE_SENSOR_DEVICE(alias) CONCAT_UNDRSCR(alias, device)

namespace temperature_reader {

	struct SensorDevice {
		const struct device* const device_ptr = nullptr;
		const struct rtio_iodev* const iodev_ptr = nullptr;
		struct rtio* const ctx_ptr = nullptr;
	};

	class TemperatureSignal : public temperature_reader_interface::TemperatureSignalInterface {

		public:

			explicit TemperatureSignal(temperature_reader::SensorDevice temperature_sensor_device);

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of a copy of this class through these operations
			*
			*/
			TemperatureSignal(const TemperatureSignal&) = delete;
			TemperatureSignal(TemperatureSignal&&) = delete;
			TemperatureSignal& operator=(const TemperatureSignal&) = delete;
			TemperatureSignal& operator=(TemperatureSignal&&) = delete;

			temperature_reader_interface::ErrorCode single_read(int& temp_c_x100) override;

			void value_get(int& temp_c_x100) const override;
			[[nodiscard("Called error getter and discarded its return value")]]
			temperature_reader_interface::ErrorState error_state_get() const override;

		private:

			struct SensorState {
				const struct device* const device_ptr;
				const struct rtio_iodev* const iodev_ptr;
				struct rtio* const ctx_ptr;
				const struct sensor_decoder_api* const decoder = {};
			};

			struct SystemState {
				bool sensor_acquired = false;
			};

			struct ReadingState {
				struct sensor_q31_data internal_data = {};
				int temp_c_x100 = 0;
			};

			temperature_reader_interface::ErrorState error;
			temperature_reader::TemperatureSignal::SystemState system;
			temperature_reader::TemperatureSignal::SensorState sensor;
			temperature_reader::TemperatureSignal::ReadingState reading;

			[[nodiscard("Internal error: necessary struct discarded")]]
			const struct sensor_decoder_api* const init_operations(const struct device* const temperature_sensor_device_ptr);
	};
}

#endif
