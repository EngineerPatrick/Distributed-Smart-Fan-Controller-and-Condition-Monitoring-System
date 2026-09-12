#ifndef TEMPERATURE_SENSOR_HPP
#define TEMPERATURE_SENSOR_HPP

#include <cstdint>
#include <cstddef>
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor_data_types.h>

namespace temperature_sensor {

	enum class ErrorCode {
		Ok,
		DeviceUnready,
		DecoderUnready,
		SensorDecoderGet,
		SensorRead,
		SensorDecode
	};

	struct ErrorState {
		temperature_sensor::ErrorCode code = temperature_sensor::ErrorCode::Ok;
		int return_value = 0;
	};

	class TemperatureSensor {

		public:

			TemperatureSensor(const struct device* const temperature_sensor_device_ptr, const struct rtio_iodev* iodev_ptr, struct rtio* ctx_ptr);

			temperature_sensor::ErrorCode temp_read(std::int32_t* temp_c_x100);

			temperature_sensor::ErrorState error_state_get() const;

		private:

			struct SensorDevice {
				const struct device* const device_ptr;
				const struct rtio_iodev* const iodev_ptr;
				struct rtio* const ctx_ptr;
				const struct sensor_decoder_api* const decoder = {};
			};

			struct SystemState {
				bool decoder_ready = false;
			};

			struct TemperatureReading {
				struct sensor_q31_data internal_data = {};
				std::int32_t value_c_x100 = 0;
			};


			temperature_sensor::ErrorState error;
			temperature_sensor::TemperatureSensor::SystemState system;
			temperature_sensor::TemperatureSensor::SensorDevice sensor;

			temperature_sensor::TemperatureSensor::TemperatureReading temp;

			const struct sensor_decoder_api* const init_operations(const struct device* const temperature_sensor_device_ptr);
	};
}

#endif
