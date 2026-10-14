/**
 * @file Sam7xCANUpdater.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @brief Responsible for manages firmware updates to all SAM7X CAN based peripherals
 * Created on: may 13, 2015
 * @author Aaron Swift
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#if defined ( __linux__ )
#include <sys/socket.h>
#include <unistd.h>
#else
// These headers do not exist for Windows. There are two options
// 1. Cygwin
// 2. Winsock
// 3. Plus something new will have to be made for unistd
// See http://stackoverflow.com/questions/2952733/using-sys-socket-h-functions-on-windows to see talk on this discussion
#endif // __linux__
#include "Sam7xCANUpdater.h"
#include "Sam7xUpdaterMsg.h"
#include "deviceProductIDs.h"
#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Hex.h"

#if defined ( __linux__ )
Sam7xCANUpdater *Sam7xCANUpdater::pInstance 			= (Sam7xCANUpdater *) NULL;

const std::string Sam7xCANUpdater::gpioPath 			= "/usr/local/gpio/";

const std::string Sam7xCANUpdater::upgradePath			= "/lib/firmware/ngw/";
const std::string Sam7xCANUpdater::wrapperDirectory		= "wrapper/";
const std::string Sam7xCANUpdater::weigherDirectory 	= "weigher/";
const std::string Sam7xCANUpdater::printerDirectory		= "printer/";

const std::string Sam7xCANUpdater::ifname 				= "can0";
const std::string Sam7xCANUpdater::wrapperResetGpio 	= "wrapper_reset";
const std::string Sam7xCANUpdater::weigherResetGpio 	= "weigher_reset";
const std::string Sam7xCANUpdater::printer1ResetGpio 	= "printer1_reset";
const std::string Sam7xCANUpdater::printer2ResetGpio 	= "printer2_reset";
const std::string Sam7xCANUpdater::printer3ResetGpio 	= "printer3_reset";
#endif // __linux__

Sam7xCANUpdater *Sam7xCANUpdater::getInstance()
{
#if defined ( __linux__ )
    if (pInstance == 0)
    {
    	pInstance = new Sam7xCANUpdater();
    }

    return pInstance;
#else
	return 0;				// had to return something
#endif // __linux__
}

#if defined ( __linux__ )
Sam7xCANUpdater::Sam7xCANUpdater()
{
	Util::CLockCriticalSection lock( initLock );

	/*initialize our list of known devices*/
	//scanList.push_back( _SAM7XWrapper );
	scanList.push_back( _SAM7XWeigher );
	scanList.push_back( _SAM7XPrinter1 );
	scanList.push_back( _SAM7XPrinter2 );
	scanList.push_back( _SAM7XPrinter3 );
	scanList.push_back( _SAM7XNone );

	/*initialize reset lines to the devices and
	  open the CAN bus */
	initGpio();
	int result = openCANBus();
	if( result == 0 )
	{
		processDevices();
	}
	else
	{
		Debug::trace( "Sam7xCANUpdater.init", "Failed to open CAN bus!" );
	}
}
#endif // __linux__

Sam7xCANUpdater::~Sam7xCANUpdater()
{
#if defined ( __linux__ )	
	if( sockFd > 0 )
		close(sockFd);

	delete pInstance;
	pInstance = 0;
#endif // __linux__
}

#if defined ( __linux__ )
void Sam7xCANUpdater::initGpio()
{
	Debug::trace( "Sam7xCANUpdater.init", "Initializing GPIO");

	/*put all peripherals into reset*/
	wrapperResetControl( _ON );
	weigherResetControl( _ON );
	printer1ResetControl( _ON );
	printer2ResetControl( _ON );
	printer3ResetControl( _ON );

}

int Sam7xCANUpdater::openCANBus()
{


	if((sockFd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{
		Debug::trace( "Sam7xCANUpdater", "openCANBus failed to open socket. Error: " + sockFd);
		return -1;
	}

	struct ifreq ifr;
	strcpy(ifr.ifr_name, (const char *)ifname.data());

	int result = ioctl(sockFd, SIOCGIFINDEX, &ifr);
	if( result < 0 )
	{
		std::string str(ifr.ifr_name);
		Debug::trace("Sam7xCANUpdater",  "openCANBus(): Failed to index: " + str + " " + Integer::toString(ifr.ifr_ifindex));
	}

	socAddr.can_family  = AF_CAN;
	socAddr.can_ifindex = ifr.ifr_ifindex;

	fcntl(sockFd, F_SETFL, O_NONBLOCK);

	result = bind(sockFd, (struct sockaddr *)&socAddr, sizeof(socAddr));
	if( result < 0 )
	{

		Debug::trace( "Sam7xCANUpdater", "openCANBus failed to bind socket: " + Integer::toString(result) );
		return -2;
	}
	else
	{
		Debug::trace( "Sam7xCANUpdater", "CAN Bus Socket is ready." );
	}
	return 0;

}

void Sam7xCANUpdater::wrapperResetControl(GpioControl control)
{
	std::string io = gpioPath + wrapperResetGpio + "/value";
	int fd = open( (const char *)io.data(), O_WRONLY );
	if( fd > 0 )
	{
		if(control == _ON)
			write(fd, "0", 1); 	/*put wrapper into reset*/
		else
			write(fd, "1", 1); 	/*release wrapper from reset */
	}
	else
	{
		Debug::trace( "Sam7xCANUpdater", "Failed to control: " + io + " error: " + Integer::toString(fd) );
	}
	close(fd);


}

void Sam7xCANUpdater::weigherResetControl( GpioControl control )
{
	std::string io = gpioPath + weigherResetGpio + "/value";
	int fd = open( (const char *)io.data(), O_WRONLY );
	if( fd > 0 )
	{
		if(control == _ON)
			write(fd, "0", 1); 	/*put weigher into reset*/
		else
			write(fd, "1", 1); 	/*release weigher from reset*/
	}
	else
	{
		Debug::trace( "Sam7xCANUpdater.init", "Failed to control: " + io  + "error: " + Integer::toString(fd) );
	}
	close(fd);

}

void Sam7xCANUpdater::printer1ResetControl(GpioControl control)
{
	std::string io = gpioPath + printer1ResetGpio + "/value";
	int fd = open( (const char *)io.data(), O_WRONLY );
	if( fd > 0 )
	{
		if(control == _ON)
			write(fd, "0", 1); 	/*put printer1 into reset*/
		else
			write(fd, "1", 1); 	/*release printer1 from reset*/
	}
	else
	{
		Debug::trace( "Sam7xCANUpdater.init", "Failed to control: " + io  + "error: " + Integer::toString(fd) );
	}
	close(fd);

}

void Sam7xCANUpdater::printer2ResetControl(GpioControl control)
{
	std::string io = gpioPath + printer2ResetGpio + "/value";
	int fd = open( (const char *)io.data(), O_WRONLY );
	if( fd > 0 )
	{
		if( control == _ON )
			write(fd, "0", 1); 	/*put printer2 into reset*/
		else
			write(fd, "1", 1); 	/*release printer2 from reset*/
	}
	else
	{
		Debug::trace( "Sam7xCANUpdater.init", "Failed to control: " + io  + "error: " + Integer::toString(fd) );
	}
	close(fd);

}

void Sam7xCANUpdater::printer3ResetControl(GpioControl control)
{
	std::string io = gpioPath + printer3ResetGpio + "/value";
	int fd = open( (const char *)io.data(), O_WRONLY );
	if( fd > 0 )
	{
		if( control == _ON )
			write( fd, "0", 1 ); 	/*put printer3 into reset*/
		else
			write( fd, "1", 1 ); 	/*release printer3 from reset*/
	}
	else
	{
		Debug::trace( "Sam7xCANUpdater.init", "Failed to control: " + io  + "error: " + Integer::toString(fd) );
	}
	close( fd );

}

void Sam7xCANUpdater::processDevices()
{
	Util::CLockCriticalSection lock( processLock );

	std::list< Sam7xDevices >::iterator iter = scanList.begin();
	std::string str;
	while( iter != scanList.end() )
	{
		switch( *iter )
		{
			case _SAM7XWrapper:
			{
				Debug::trace( "Sam7xCANUpdater.process", "Process _SAM7XWrapper" );
				str = "Wrapper";
				/*enable the wrapper*/
				wrapperResetControl( _OFF );
				break;
			}
			case _SAM7XWeigher:
			{
				Debug::trace( "Sam7xCANUpdater.process", "Process _SAM7XWeigher" );
				str = "Weigher";
				/*enable the weigher*/
				weigherResetControl( _OFF );
				break;
			}
			case _SAM7XPrinter1:
			{
				Debug::trace( "Sam7xCANUpdater.process", "Process _SAM7XPrinter1" );
				str = "Printer1";
				/*enable the printer1*/
				printer1ResetControl( _OFF );
				break;
			}
			case _SAM7XPrinter2:
			{
				Debug::trace( "Sam7xCANUpdater.process", "Process _SAM7XPrinter2" );
				str = "Printer2";
				/*enable the printer2*/
				printer2ResetControl( _OFF );
				break;
			}
			case _SAM7XPrinter3:
			{
				Debug::trace( "Sam7xCANUpdater.process", "Process _SAM7XPrinter3" );
				str = "Printer3";
				/*enable the printer3*/
				printer3ResetControl( _OFF );
				break;
			}
			case _SAM7XNone:
			{
				/*stop our Run thread and close socket*/
				Debug::trace( "Sam7xCANUpdater.process", "Process _SAM7XNone" );

				close(sockFd);
				sockFd = -1;
				return;
			}

		}
		/* can I talk to the device? */
		if( openCommInterface() )
		{
			/* are we dealing with a Hobart device */
			if( isHobartDevice() )
			{
				//Debug::trace( "Sam7xCANUpdater.process", "readProductId" );

				int id = readProductId();

				Debug::trace( "Sam7xCANUpdater.process", "Product Id: " + Hex::toString( id ) );

				/* am I talking to the right device? */
				if( doesIDMatchDevice( id, *iter ) )
				{
					Debug::trace( "Sam7xCANUpdater.process", "Correct device" );

					if( isUpgradeAvailable( *iter ) )
					{
						Debug::trace( "Sam7xCANUpdater.process", "Upgrade available." );
						int result = updateDevice( *iter );
						if( result < 0 )
							Debug::trace( "Sam7xCANUpdater.process", "Upgrade of device: " + str + "failed!" );
						else
							/* assign node ID to the device */
							if( updateDeviceNodeID( *iter ) < 0 )
							{
								/* command the device to start running it's application */
								if( launchApplication( *iter ) < 0 )
									Debug::trace( "Sam7xCANUpdater.process", "Failed to launch application on device: " + str );
								else
									Debug::trace( "Sam7xCANUpdater.process", "Launched application on device: " + str );
							}
							else
							{
								Debug::trace( "Sam7xCANUpdater.process", "Failed to update node id on: " + str );
							}
					}
					else
					{
						Debug::trace( "Sam7xCANUpdater.process", "Upgrade not available." );
						/* command the device to start running it's application */
						if( launchApplication( *iter ) < 0 )
							Debug::trace( "Sam7xCANUpdater.process", "Failed to launch application on device: " + str );
						else
							Debug::trace( "Sam7xCANUpdater.process", "Launched application on device: " + str );

					}
				}
				else
				{
					Debug::trace( "Sam7xCANUpdater.process", "Product id does not match device!" );
				}
			}
		}
		else
		{
			Debug::trace( "Sam7xCANUpdater.process", "Failed to open device interface." );
		}
		iter++;
	}

}

bool Sam7xCANUpdater::openCommInterface()
{

	Sam7xUpdaterMsg msg( Sam7xUpdaterMsg::_OpenInterface, 0 );
	sendCanMsg( msg.getMessage() );

	/*get the device response to command*/
	Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
	receiveCanMsg( rxMsg.getMessage() );
	rxMsg.show();
	return ( rxMsg.isDeviceInterfaceOpen() );
}


bool Sam7xCANUpdater::isHobartDevice()
{
	Sam7xUpdaterMsg msg( Sam7xUpdaterMsg::_ReadVendorId, 0 );
	sendCanMsg( msg.getMessage() );

	Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
	receiveCanMsg( rxMsg.getMessage() );
	rxMsg.show();
	return( rxMsg.isHobartDevice() );
}

bool Sam7xCANUpdater::isUpgradeAvailable(Sam7xDevices device)
{
	FILE *fp;
	std::string filename = upgradePath;
	if( device == _SAM7XWrapper )
		filename += wrapperDirectory + "";
	else if ( device == _SAM7XWeigher )
		filename += weigherDirectory + "source.bin";
	else if (( device == _SAM7XPrinter1 ) || ( device == _SAM7XPrinter2 ) || ( device == _SAM7XPrinter3 ) )
		filename += printerDirectory + "source.bin";
	else
		filename = "";

	Debug::trace( "Sam7xCANUpdater.process", "isUpgradeAvailable: " +  filename );
	/*check firmware directory if upgrade binary is present*/
	fp = fopen(filename.data(),"r");

	if(fp == NULL)
	    return false;
	else
		fclose(fp);
		return true;

}

bool Sam7xCANUpdater::doesIDMatchDevice(int id, Sam7xDevices device)
{
	bool result = false;
	if( device == _SAM7XWrapper )
	{
		if( id == HOBART_NGW_WRAPPER_ID )
		{
			result = true;
		}
	}
	else if( device == _SAM7XWeigher )
	{
		if( id == HOBART_NGW_WEIGHER_ID )
		{
			result = true;
		}
	}
	else if(( device == _SAM7XPrinter1 ) || ( device == _SAM7XPrinter2 ) || ( device == _SAM7XPrinter3 ) )
	{
		if( id == HOBART_NGW_PRINTER_ID )
		{
			result = true;
		}
	}

	return result;
}

int Sam7xCANUpdater::readProductId()
{
	int id = 0;
	Sam7xUpdaterMsg msg( Sam7xUpdaterMsg::_ReadProductId, 0 );
	sendCanMsg( msg.getMessage() );

	Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
	receiveCanMsg( rxMsg.getMessage() );
	rxMsg.show();
	return ( rxMsg.getProductId() );
}


int Sam7xCANUpdater::sendCanMsg( can_frame *pFrame )
{
	int nbytes = 0, retry = 0;
	while( ( (nbytes == 0) || (nbytes == -1) ) && (retry != 3) )
	{
		nbytes = write( sockFd, pFrame, sizeof( struct can_frame ) );
		if(nbytes == -1)
			retry++;
	}

	if(retry == 3)
		Debug::trace( "Sam7xCANUpdater.write", "Failed to write to device. Error: " + Integer::toString(nbytes));
	return nbytes;
}

int Sam7xCANUpdater::receiveCanMsg( can_frame *pFrame )
{
    struct timeval timeout;
    int result = 0, nbytes = 0, retries = 0;
    //1 second timeout
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;

	while( ( result == 0 ) && ( retries < 3 ) )
	{
		PAL_absclass::PAL_sleep(1);

		retries++;

		fd_set rdSet;
		FD_ZERO(&rdSet);
		FD_SET(sockFd, &rdSet);

		result = select((sockFd + 1), &rdSet, NULL, NULL, &timeout);
	}

	if( result > 0 )
	{
		nbytes = read( sockFd, pFrame, sizeof( struct can_frame ) );
	}
	if(nbytes < 0)
		Debug::trace( "Sam7xCANUpdater.read", "Failed to read from device. Error: " + Integer::toString(nbytes));
	return nbytes;
}

void Sam7xCANUpdater::packDataLong(unsigned char *pBuffer, unsigned long data1, unsigned long data2)
{
	*pBuffer++ = (( data1 & 0xff000000 ) >> 24);
	*pBuffer++ = (( data1 & 0x00ff0000 ) >> 16);
	*pBuffer++ = (( data1 & 0x0000ff00) >> 8);
	*pBuffer++ = data1;

	*pBuffer++ = (( data2 & 0xff000000 ) >> 24);
	*pBuffer++ = (( data2 & 0x00ff0000 ) >> 16);
	*pBuffer++ = (( data2 & 0x0000ff00) >> 8);
	*pBuffer++ = data2;
}

int Sam7xCANUpdater::handleDeviceResponse(int response)
{
	if( ( response == COMMAND_OK ) || (response == COMMAND_WAIT) )
		return 0;
	else if( response == COMMAND_NOK )
		return -1;
	else
	{
		Debug::trace( "Sam7xCANUpdater", "Unknown command response! " + Integer::toString( response ) );
		return -1;
	}
}

int Sam7xCANUpdater::calNumTrasfers(int size, int remainder)
{
	int transactions = 0;

	if( size % frameDataSize )
	{
		transactions = size / frameDataSize;
		remainder = size % frameDataSize;
	}
	else
	{
		transactions = size / frameDataSize;
		remainder = 0;
	}
	return transactions;
}

int Sam7xCANUpdater::send(unsigned char *pImage, int imageSize)
{
	//calculate our end address
	unsigned long imageEndAddress = imageStartAddr + imageSize;
	unsigned char buffer[frameDataSize];
	packDataLong( &buffer[0], imageStartAddr, imageEndAddress );

	/*initiate writing image buffer to SAM7x*/
	Sam7xUpdaterMsg msg( Sam7xUpdaterMsg::_WriteBuffer, 0,  &buffer[0], 0 );
	sendCanMsg( msg.getMessage() );

	/*get response from device*/
	Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
	if ( receiveCanMsg( rxMsg.getMessage() ) > 0 )
	{
		Debug::trace( "Sam7xCANUpdater", "rxMsg.getCommandResponse(): " + Integer::toString( rxMsg.getCommandResponse() ) );
		//int result = handleDeviceResponse( rxMsg.getCommandResponse() );
		int result = rxMsg.getCommandResponse();
		if( result == COMMAND_OK )
		{
			/* Device is ready to write image */
			int remainder = 0, numBytes = 0;
			/* calculate the number of transfers the image will take. */
			int numTransfers = calNumTrasfers( imageSize, remainder );
			int transfers = 0;
			while( transfers <= numTransfers )
			{
				/* write 8 image bytes to the device */
				Sam7xUpdaterMsg msg( Sam7xUpdaterMsg::_WriteData, 0, pImage, 8 );
				numBytes += sendCanMsg( msg.getMessage() );

				/* Did the device write the 8 bytes? */
				Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
				receiveCanMsg( rxMsg.getMessage() );
				int cmdRep = rxMsg.getCommandResponse();
				if(( cmdRep  == COMMAND_OK ) || ( cmdRep  == COMMAND_WAIT ) )
				{
					pImage += frameDataSize;
					transfers++;
				}
				else
				{
					Debug::trace( "Sam7xCANUpdater.send", "Device failed write data command! " +  Integer::toString(cmdRep) );
					Debug::trace( "Sam7xCANUpdater.send", "Device failed after writing:" + Integer::toString(numBytes) + "bytes!");
					Debug::trace( "Sam7xCANUpdater.send", "Device failed number of transfers:" + Integer::toString(transfers) + "!");
					return -1;
				}
			}

			if( remainder )
			{
				Sam7xUpdaterMsg msg( Sam7xUpdaterMsg::_WriteData, 0, pImage,  remainder);
				numBytes += sendCanMsg( msg.getMessage() );

				/* Did the device write the 8 bytes? */
				Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
				receiveCanMsg( rxMsg.getMessage() );
				int cmdRep = rxMsg.getCommandResponse();
				if(( cmdRep  == COMMAND_OK ) || ( cmdRep  == COMMAND_WAIT ) )
				{
					return 0;
				}
				else
				{
					return -1;
				}

			}
			Debug::trace( "Sam7xCANUpdater.send", "Wrote: " + Integer::toString(numBytes) + "bytes in " + Integer::toString(transfers) + "transfers." );

			/*get the results of the update */
			Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
			receiveCanMsg( rxMsg.getMessage() );
			int cmdRep = rxMsg.getCommandResponse();
			if(( cmdRep  == COMMAND_OK ) || ( cmdRep  == COMMAND_WAIT ) )
			{
				Debug::trace( "Sam7xCANUpdater.send", "Device upgrade successful upgrade complete.");
				return 0;
			}
			else
				return -1;
		}
		else
		{
			Debug::trace( "Sam7xCANUpdater.send", "Device failed write buffer command!");
			return -1;
		}
	}
	else
	{
		Debug::trace( "Sam7xCANUpdater.send", "Failed to receive command response from device!");
		return -1;
	}

}

int Sam7xCANUpdater::updateDevice(Sam7xDevices device)
{
	FILE *fp;
	std::string filename = upgradePath;
	int result = 0, size = 0;

	if( device == _SAM7XWrapper )
		filename += wrapperDirectory + "";
	else if ( device == _SAM7XWeigher )
		filename += weigherDirectory + "source.bin";
	else if (( device == _SAM7XPrinter1 ) || ( device == _SAM7XPrinter2 ) || ( device == _SAM7XPrinter3 ) )
		filename += printerDirectory + "source.bin";
	else
		filename = "";

	/* check firmware directory if upgrade binary is present */
	fp = fopen(filename.data(),"r");

	if( fp != NULL )
	{
		/* get the size of the image */
		fseek( fp, 0, SEEK_END );
		size = ftell ( fp );
		rewind( fp );

		unsigned char *pBuffer = new unsigned char [size];
		fread( (void *)pBuffer, sizeof(char), size, fp );
		fclose(fp);

		if( send( pBuffer, size ) == 0 )
		{
			Debug::trace( "Sam7xCANUpdater.update", "Device upgraded: " + filename);
			result = 1;
		}
		else
		{
			Debug::trace( "Sam7xCANUpdater.update", "Failed to update device: " + filename);
			result = -1;
		}

	}
	else
	{
		Debug::trace( "Sam7xCANUpdater.update", "Failed to open source file: " + filename);
		result = -1;
	}
	return result;

}

int Sam7xCANUpdater::updateDeviceNodeID(Sam7xDevices device)
{
	unsigned char nodeId = 0;
	if( device == _SAM7XWeigher )
		nodeId = WeigherNodeID;
	if( device == _SAM7XWrapper )
		nodeId = WrapperNodeID;
	if( device == _SAM7XPrinter1 )
		nodeId = Printer1NodeID;
	if( device == _SAM7XPrinter2 )
		nodeId = Printer2NodeID;
	if( device == _SAM7XPrinter3 )
		nodeId = Printer3NodeID;

	/* write the new node id to the device */
	Sam7xUpdaterMsg msg( Sam7xUpdaterMsg::_WriteNodeId, nodeId );
	sendCanMsg( msg.getMessage() );

	/* get response from device */
	Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
	int cmdRep = handleDeviceResponse( receiveCanMsg( rxMsg.getMessage() ) );
	if( cmdRep < 0 )
	{
		Debug::trace( "Sam7xCANUpdater.update", "Failed to Write node id: " + nodeId);
		return -1;
	}
	return 0;
}

int Sam7xCANUpdater::launchApplication(Sam7xDevices device)
{
	/* write the new node id to the device */
	Sam7xUpdaterMsg msg( Sam7xUpdaterMsg::_JumpToApplication, 0 );
	sendCanMsg( msg.getMessage() );

	/* get response from device */
	Sam7xUpdaterMsg rxMsg( Sam7xUpdaterMsg::_Empty, 0 );
	int cmdRep = handleDeviceResponse( receiveCanMsg( rxMsg.getMessage() ) );
	if( cmdRep < 0 )
	{
		Debug::trace( "Sam7xCANUpdater", "Failed to launch device application: " + Integer::toString(device) );
		return -1;
	}
	return 0;
}
#endif // __linux__