#ifndef BME280_HPP
#define BME280_HPP

#include <cstdint>
#include <cstddef>
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor_data_types.h>

namespace bme280 {

	enum class ErrorCode {
		Ok,
		DeviceUnready,
		SensorDecoder,
		SensorRead,
		DecoderUnready,
		SensorDecode
	};

	struct ErrorState {
		bme280::ErrorCode code = bme280::ErrorCode::Ok;
		int return_value = 0;
	};

	class Bme280 {

		public:

			Bme280(const struct device* const bme280_device_ptr, const struct rtio_iodev* iodev_ptr, struct rtio* ctx_ptr);

			bme280::ErrorCode temp_read(std::int32_t* temp_c_x100);

			bme280::ErrorState error_state_get() const;

		private:

			struct TargetSensor {
				const struct device* const device_ptr;
				const struct rtio_iodev* iodev_ptr;
				struct rtio* ctx_ptr;
				const struct sensor_decoder_api* const decoder = {};
			};

			struct SystemState {
				bool decoder_ready = false;
			};

			struct TempReading {
				struct sensor_q31_data internal_data = {};
				std::int32_t value_c_x100 = 0;
			};


			bme280::ErrorState error;
			bme280::Bme280::TargetSensor sensor;
			bme280::Bme280::SystemState system;

			bme280::Bme280::TempReading temp;

			const struct sensor_decoder_api* const init_operations(const struct device* const bme280_device_ptr);
	};
}

#endif
