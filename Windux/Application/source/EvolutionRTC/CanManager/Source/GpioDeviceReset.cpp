
/**
 * @file GpioDeviceReset.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @brief Responsible for IO control of the peripheral reset lines
 * Created on: Jun 24, 2015
 * @author Aaron Swift
 */

#include <stdio.h>
#include <stdlib.h>
#include "GpioDeviceReset.h"
#include "Debug.h"

const std::string GpioDeviceReset::gpioPath 			= "/usr/local/gpio/";

const std::string GpioDeviceReset::wrapperResetGpio 		= "wrapper_reset";
const std::string GpioDeviceReset::weigherResetGpio 		= "weigher_reset";
const std::string GpioDeviceReset::printer1ResetGpio 		= "printer1_reset";
const std::string GpioDeviceReset::printer2ResetGpio 		= "printer2_reset";
const std::string GpioDeviceReset::printer3ResetGpio 		= "printer3_reset";
const std::string GpioDeviceReset::aws3WrapperResetGpio		= "hub1_enable";
const std::string GpioDeviceReset::kpcResetGpio				= "kpc_reset";


void GpioDeviceReset::setDeviceTypeWeigher() {
	init( _GPIOWEIGHER );
}

void GpioDeviceReset::setDeviceTypePrinter1() {
	init( _GPIOPRINTER1 );
}

void GpioDeviceReset::setDeviceTypePrinter2() {
	init( _GPIOPRINTER2 );
}

void GpioDeviceReset::setDeviceTypePrinter3() {
	init( _GPIOPRINTER3 );
}

void GpioDeviceReset::setDeviceTypeWrapper() {
	init( _GPIOWRAPPER );
}
void GpioDeviceReset::setDeviceTypeAWS3Wrapper() {
	init( _GPIOAWS3WRAPPER );
}
void GpioDeviceReset::setDeciceTypeKpc() {
	init( _GPIOKPC );
}

void GpioDeviceReset::init(GpioType type)
{
#if defined ( __linux__ )
	std::string io;

	if( type == _GPIOWEIGHER )
		 io = gpioPath + weigherResetGpio + "/value";
	else if( type == _GPIOPRINTER1 )
		io = gpioPath + printer1ResetGpio + "/value";
	else if( type == _GPIOPRINTER2 )
		io = gpioPath + printer2ResetGpio + "/value";
	else if( type == _GPIOPRINTER3 )
		io = gpioPath + printer3ResetGpio + "/value";
	else if( type == _GPIOAWS3WRAPPER )
		io = gpioPath + aws3WrapperResetGpio + "/value";
	else if (type == _GPIOWRAPPER)
		io = gpioPath + wrapperResetGpio + "/value";
	else
		io = gpioPath + kpcResetGpio + "/value";
	

	fd = open( (const char *)io.data(), O_WRONLY );
#else
	// TODO: dont have a windows solution at this time
#endif // __linux__
}

GpioDeviceReset::GpioDeviceReset()
{
	fd = 0;
}
GpioDeviceReset::~GpioDeviceReset()
{
#if defined ( __linux__ )
	close( fd );
#endif // __linux__
}

void GpioDeviceReset::resetDevice()
{
	controlReset( _ON );
}

void GpioDeviceReset::releaseResetDevice()
{
	controlReset( _OFF );
}

void GpioDeviceReset::controlReset(GpioControl control)
{
#if defined ( __linux__ )	
	if( fd > 0 )
	{
		if( control == _ON )
			write( fd, "0", 1 ); 	/*assert reset for device*/
		else
			write( fd, "1", 1 ); 	/*release device from reset */
	}
	else
	{
		Debug::trace( "GpioDeviceReset", "Failed to control!" );
	}
#else
	// TODO: dont have a windows solution at this time
#endif // __linux__
}


