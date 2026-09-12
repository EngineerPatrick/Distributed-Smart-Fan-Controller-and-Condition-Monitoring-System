#include "bme280.hpp"
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor_data_types.h>

const struct sensor_decoder_api* const bme280::Bme280::init_operations(const struct device* const bme280_device_ptr) {
	const struct sensor_decoder_api* decoder = {};

	this->error.return_value = sensor_get_decoder(bme280_device_ptr, &decoder);

	if (this->error.return_value != 0) {
		this->error.code = bme280::ErrorCode::SensorDecoder;
		return nullptr;
	}

	this->error = {bme280::ErrorCode::Ok, 0};
	return decoder;
}

bme280::Bme280::Bme280(const struct device* const bme280_device_ptr, const struct rtio_iodev* iodev_ptr, struct rtio* ctx_ptr) :
sensor{{bme280_device_ptr}, {iodev_ptr}, {ctx_ptr}, {this->init_operations(bme280_device_ptr)}} {

	if (!device_is_ready(this->sensor.device_ptr)) {
		this->error = {bme280::ErrorCode::DeviceUnready, 0};
		return;
	}

	this->system.decoder_ready = true;
	this->error = {bme280::ErrorCode::Ok, 0};
}

bme280::ErrorCode bme280::Bme280::temp_read(std::int32_t* temp_c_x100) {
	std::uint8_t rx_buff[128];
	std::uint32_t fit = 0;

	if (!this->system.decoder_ready) {
		this->error = {bme280::ErrorCode::DecoderUnready, 0};
		return this->error.code;
	}

	this->error.return_value = sensor_read(this->sensor.iodev_ptr, this->sensor.ctx_ptr, rx_buff, 128);

	if (this->error.return_value != 0) {
		this->error.code = bme280::ErrorCode::SensorRead;
		return this->error.code;
	}

	this->error.return_value = this->sensor.decoder->decode(rx_buff, (struct sensor_chan_spec) {SENSOR_CHAN_AMBIENT_TEMP, 0}, &fit, 1, &(this->temp.internal_data));

	if (this->error.return_value < 1) {
		this->error.code = bme280::ErrorCode::SensorDecode;
		return this->error.code;
	}

	this->temp.value_c_x100 = static_cast<std::int32_t>((static_cast<std::int64_t>(this->temp.internal_data.readings[0].temperature) * 100 * 65536) / 2147483648);

	*temp_c_x100 = this->temp.value_c_x100;

	this->error = {bme280::ErrorCode::Ok, 0};
	return this->error.code;
}
