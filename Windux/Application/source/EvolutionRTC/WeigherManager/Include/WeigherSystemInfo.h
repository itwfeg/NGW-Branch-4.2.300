/**
 * @file WeigherSystemInfo.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max current x,y and z gain factor
 * Created on: Oct 2, 2017
 *
 **/
#ifndef WEIGHERSYSTEMINFO_H
#define WEIGHERSYSTEMINFO_H
#include "Weigher.h"
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
#include "HobVersion.h"

class WeigherSystemInfo : public EvolMessage
{
	public:


	WeigherSystemInfo( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherSystemInfo( const char *buffer );
	~WeigherSystemInfo();

	const char *serialize()const;

	WeigherType getWeigherPosition() { return position; }
	void setWeigherPosition( WeigherType pos ) { position = pos; }

	bool isSerialFlashValid() { return serialFlashValid; }
	void setSerialFlashValid( bool valid ) { serialFlashValid = valid; }

	bool isEepromValid() { return eepromValid; }
	void setEepromValid( bool valid ) { eepromValid = valid; }

	const HobVersion &getSoftwareVersion() const { return softwareVersion; }

	void setSoftwareVersionType() { softwareVersion.setVersionType( HobVersion::KpcWeigherSW ); }
	void setSoftwareVersionMajor( unsigned short value ) { softwareVersion.setMajorRevision(value);}
	void setSoftwareVersionMinor( unsigned short value ) { softwareVersion.setMinorRevision(value);}
	void setSoftwareVersionBuild( unsigned long value )  { softwareVersion.setBuildNumber( value );}

	const HobVersion &getHardwareVersion() const { return hardwareVersion; }

	void setHardwareVersionType() { hardwareVersion.setVersionType( HobVersion::KpcWeigherHW ); }
	void setHardwareVersionMajor( unsigned short value ) { hardwareVersion.setMajorRevision(value);}
	void setHardwareVersionMinor( unsigned short value ) { hardwareVersion.setMinorRevision(value);}
	void setHardwareVersionBuild() { hardwareVersion.setBuildNumber(0); }
	void show();

	private:
	WeigherType position;
	bool serialFlashValid;
	bool eepromValid;
	HobVersion softwareVersion;
	HobVersion hardwareVersion;
};
#endif
