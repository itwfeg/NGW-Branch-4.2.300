/*
 * GpioDeviceReset.h
 *
 *  Created on: Jun 24, 2015
 *      Author: aaron
 */

#ifndef GPIODEVICERESET_H_
#define GPIODEVICERESET_H_
#include <string>



class GpioDeviceReset
{
	public:


	GpioDeviceReset();
	~GpioDeviceReset();


	void setDeviceTypeWeigher();
	void setDeviceTypePrinter1();
	void setDeviceTypePrinter2();
	void setDeviceTypePrinter3();
	void setDeviceTypeWrapper();
	void setDeviceTypeAWS3Wrapper();
	void setDeciceTypeKpc();

	void resetDevice();
	void releaseResetDevice();

	private:
		typedef enum
		{
			_GPIOWEIGHER,
			_GPIOPRINTER1,
			_GPIOPRINTER2,
			_GPIOPRINTER3,
			_GPIOWRAPPER,
			_GPIOAWS3WRAPPER,
			_GPIOKPC
		}GpioType;

		typedef enum _GpioControl
		{
			_OFF,
			_ON

		}GpioControl;

		static const std::string gpioPath;
		static const std::string wrapperResetGpio;
		static const std::string weigherResetGpio;
		static const std::string printer1ResetGpio;
		static const std::string printer2ResetGpio;
		static const std::string printer3ResetGpio;
		static const std::string aws3WrapperResetGpio;
		static const std::string kpcResetGpio;	

		int fd;
		void init(GpioType type);
		void controlReset(GpioControl control);

};

#endif /* GPIODEVICERESET_H_ */
