/**
*
*	@file		temperature_sensor.hpp
*
*	@brief		Public API for the temperature_sensor module
*
*	@details	Adapter module for Zephyr's Sensor API
*
*				Supports temperature reading and Zephyr's Sensor API return values
*
*/

#ifndef TEMPERATURE_SENSOR_HPP
#define TEMPERATURE_SENSOR_HPP

#include <cstdint>
#include <cstddef>
#include <zephyr/device.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor_data_types.h>

namespace temperature_sensor {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		ReadingUnready,
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

			/*
			*
			*	Copy/move constructors/operators are deleted to prevent the creation of another instance of this class with the same SensorDevice values
			*
			*/
			TemperatureSensor(const TemperatureSensor&) = delete;
			TemperatureSensor(TemperatureSensor&&) = delete;
			TemperatureSensor& operator=(const TemperatureSensor&) = delete;
			TemperatureSensor& operator=(TemperatureSensor&&) = delete;

			temperature_sensor::ErrorCode temp_read(std::int32_t& temp_c_x100);

			[[nodiscard("Called error getter and discarded its return value")]] temperature_sensor::ErrorState error_state_get() const;

		private:

			struct SensorDevice {
				const struct device* const device_ptr;
				const struct rtio_iodev* const iodev_ptr;
				struct rtio* const ctx_ptr;
				const struct sensor_decoder_api* const decoder = {};
			};

			struct SystemState {
				bool reading_ready = false;
			};

			struct TemperatureReading {
				struct sensor_q31_data internal_data = {};
				std::int32_t value_c_x100 = 0;
			};


			temperature_sensor::ErrorState error;
			temperature_sensor::TemperatureSensor::SystemState system;
			temperature_sensor::TemperatureSensor::SensorDevice sensor;

			temperature_sensor::TemperatureSensor::TemperatureReading temp;

			[[nodiscard("Internal error: necessary struct discarded")]] const struct sensor_decoder_api* const init_operations(const struct device* const temperature_sensor_device_ptr);
	};
}

#endif

/**
*
*	@enum		temperature_sensor::ErrorCode
*
*	@brief		Error codes of the module
*
*	@invariant	If an error related to Zephyr's Sensor API occurs then the code is prefixed with "Sensor"
*
*/

/**
*
*	@struct		temperature_sensor::ErrorState
*
*	@brief		Data structure for all types of errors
*
*	@invariant	All members of this struct are set at the end of every fallible method of TemperatureSensor
*	@invariant	If a module-based error occurs then it is represented by code and the other member is equal to 0
*	@invariant	If an error related to Zephyr's Sensor API occurs then it is represented by both code and return_value
*
*/

/**
*
*	@class		temperature_sensor::TemperatureSensor
*
*	@brief		Class for Zephyr's Sensor API
*
*	@warning	Each instance of this class must have different SensorDevice values because it must refer to a different device
*
*/

/**
*
*	@fn			temperature_sensor::TemperatureSensor::TemperatureSensor(const struct device* const temperature_sensor_device_ptr, const struct rtio_iodev* iodev_ptr, struct rtio* ctx_ptr)
*
*	@brief		Constructor to initialize the sensor
*
*	@param[in]	temperature_sensor_device_ptr	Pointer to the device struct of the target sensor
*	@param[in]	iodev_ptr						Pointer to the IO device struct of the temperature sensor
*	@param[in]	ctx_ptr							Pointer to the RTIO context struct for the temperature sensor
*
*	@pre		temperature_sensor_device_ptr points to a valid device struct of a temperature sensor
*	@pre		iodev_ptr points to a valid IO device struct of a temperature sensor
*	@pre		ctx_ptr points to a valid RTIO context struct
*	@post		If the target sensor is not ready to be used then error.code is set to DeviceUnready
*	@post		If an error occurs when obtaining the decoder with Zephyr's Sensor API then its return value is saved in error.return_value and error.code is set to SensorDecoderGet
*	@post		On success the target device is ready, the sensor is initialized and error.code is set to Ok
*
*/

/**
*
*	@fn			temperature_sensor::ErrorCode temperature_sensor::TemperatureSensor::temp_read(std::int32_t& temp_c_x100);
*
*	@brief		Method to read the temperature from the sensor and expose it in hundredth of Celsius degrees
*
*	@param[out]	temp_c_x100						Reference to the variable where to store the temperature
*
*	@retval		ReadingUnready					If the reading function is not ready to be used
*	@retval		SensorRead						If an error occurs when reading the temperature from the sensor
*	@retval		SensorDecode					If an error occurs when decoding the reading value
*
*	@post		If the reading function is not ready to be used then error.code is set to ReadingUnready
*	@post		If an error occurs when reading the temperature from the sensor with Zephyr's Sensor API then its return value is saved in error.return_value and error.code is set to SensorRead
*	@post		If an error occurs when decoding the reading value with Zephyr's Sensor API then its return value is saved in error.return_value and error.code is set to SensorDecode
*	@post		On success the temperature is read from the sensor, its value is decoded and assigned to the reference and error.code is set to Ok
*
*/

/**
*
*	@fn			temperature_sensor::ErrorState temperature_sensor::TemperatureSensor::error_state_get() const
*
*	@brief		Method to obtain the full error report
*
*	@return										The full error struct
*
*/
