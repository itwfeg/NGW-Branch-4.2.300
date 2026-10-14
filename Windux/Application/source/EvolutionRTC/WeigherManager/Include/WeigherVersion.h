/*
 * WeigherVersion.h
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */

#ifndef WEIGHERVERSION_H_
#define WEIGHERVERSION_H_
#include "WeigherGeneric.h"

class WeigherVersion : public WeigherGeneric
{

	public:
	WeigherVersion( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherVersion( const char *buffer );
	~WeigherVersion();

	UINT16 getSoftwareMajor() { return _softwareMajor; }
	void setSoftwareMajor( UINT16 major ) { _softwareMajor = major; }

	UINT16 getSoftwareMinor() { return _softwareMinor; }
	void setSoftwareMinor( UINT16 minor ) { _softwareMinor = minor; }

	UINT8 getHardwareMajor() { return _hardwareMajor; }
	void setHardwareMajor( UINT8 major ) { _hardwareMajor = major; }

	UINT8 getHardwareMinor() { return _hardwareMinor; }
	void setHardwareMinor( UINT8 minor ) { _hardwareMinor = minor; }

	const char *serialize() const;

	private:
	UINT16	_softwareMajor;
	UINT16	_softwareMinor;
	UINT8	_hardwareMajor;
	UINT8	_hardwareMinor;
};



#endif /* WEIGHERVERSION_H_ */
