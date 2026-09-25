/**
*
*	@file		temperature_sensor.cpp
*
*	@brief		Implementation for the temperature_sensor module
*
*	@details	Employs Zephyr's Sensor API to read the temperature
*
*				Integrates Zephyr's decoder and Q31 formula to obtain
*				the temperature value in hundredth of Celsius degrees
*
*/

#include "temperature_sensor.hpp"
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor_data_types.h>

const struct sensor_decoder_api* const temperature_sensor::TemperatureSensor::init_operations(const struct device* const temperature_sensor_device_ptr) {
	const struct sensor_decoder_api* decoder = {};

	if (!device_is_ready(temperature_sensor_device_ptr)) {
		this->error = {temperature_sensor::ErrorCode::DeviceUnready, 0};
		return nullptr;
	}

	this->error.return_value = sensor_get_decoder(temperature_sensor_device_ptr, &decoder);

	if (this->error.return_value != 0) {
		this->error.code = temperature_sensor::ErrorCode::ZSensorDecoderGet;
		return nullptr;
	}

	this->system.reading_ready = true;
	this->error = {temperature_sensor::ErrorCode::Ok, 0};
	return decoder;
}

temperature_sensor::TemperatureSensor::TemperatureSensor(temperature_sensor::SensorDevice temperature_sensor_device) :
sensor{{temperature_sensor_device.device_ptr},
{temperature_sensor_device.iodev_ptr},
{temperature_sensor_device.ctx_ptr},
{this->init_operations(temperature_sensor_device.device_ptr)}} {}

temperature_sensor::ErrorCode temperature_sensor::TemperatureSensor::temp_read(std::int16_t& temp_c_x100) {
	std::uint8_t rx_buff[128];
	std::uint32_t fit = 0;

	if (!this->system.reading_ready) {
		this->error = {temperature_sensor::ErrorCode::ReadingUnready, 0};
		return this->error.code;
	}

	this->error.return_value = sensor_read(this->sensor.iodev_ptr, this->sensor.ctx_ptr, rx_buff, 128);

	if (this->error.return_value != 0) {
		this->error.code = temperature_sensor::ErrorCode::ZSensorRead;
		return this->error.code;
	}

	this->error.return_value = this->sensor.decoder->decode(rx_buff, (struct sensor_chan_spec) {SENSOR_CHAN_AMBIENT_TEMP, 0}, &fit, 1, &(this->temp.internal_data));

	if (this->error.return_value < 1) {
		this->error.code = temperature_sensor::ErrorCode::ZSensorDecode;
		return this->error.code;
	}

	/*
	*
	*	This formula is the inverse of the one provided by Zephyr's API to calculate Q31 data: Q31 * 2^16 / 2^31
	*
	*/
	if (this->temp.internal_data.shift >= 0) {
		this->temp.value_c_x100 = static_cast<std::int16_t>((static_cast<std::int64_t>(this->temp.internal_data.readings[0].temperature) * 100
		<< this->temp.internal_data.shift) >> 31);
	}

	else {
		this->temp.value_c_x100 = static_cast<std::int16_t>((static_cast<std::int64_t>(this->temp.internal_data.readings[0].temperature) * 100)
		>> (31 + (-this->temp.internal_data.shift)));
	}

	temp_c_x100 = this->temp.value_c_x100;

	this->error = {temperature_sensor::ErrorCode::Ok, 0};
	return this->error.code;
}

temperature_sensor::ErrorState temperature_sensor::TemperatureSensor::error_state_get() const {
	return this->error;
}
