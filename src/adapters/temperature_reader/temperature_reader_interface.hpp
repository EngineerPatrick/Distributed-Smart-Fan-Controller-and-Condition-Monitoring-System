#ifndef TEMPERATURE_READER_INTERFACE_HPP
#define TEMPERATURE_READER_INTERFACE_HPP

namespace temperature_reader_interface {

	enum class [[nodiscard("Discarding an error of this type may result in a bug")]] ErrorCode {
		Ok,
		DeviceUnready,
		ReadingUnready,
		ZSensorDecoderGet,
		ZSensorRead,
		ZSensorDecode
	};

	struct ErrorState {
		temperature_reader_interface::ErrorCode code = temperature_reader_interface::ErrorCode::Ok;
		int return_value = 0;
	};

	class TemperatureSignalInterface {

		public:

			virtual temperature_reader_interface::ErrorCode value_read(int& temp_c_x100) = 0;

			[[nodiscard("Called error getter and discarded its return value")]]
			virtual temperature_reader_interface::ErrorState error_state_get() const = 0;

		protected:

			/*
			*
			*	The destructor is protected to prevent the destruction of any instance but allow derived class to override it
			*
			*/
			virtual ~TemperatureSignalInterface() = default;
	};
}

#endif
