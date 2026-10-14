/*
 * NGWVersion.h
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */

#ifndef NGWVERSION_H_
#define NGWVERSION_H_
#include "WrapperGeneric.h"

class NGWVersion : public WrapperGeneric
{
	public:
	NGWVersion( Hobart::CEvolMsgReceiver	*pHobThread	= NULL );
	NGWVersion( const char *buffer );
	~NGWVersion();

	UINT16 getNodeId() { return _nodeId; }
	void setNodeId( UINT16 id ) { _nodeId = id; }

	UINT16 getWrapBoxChecksum() { return _wrapBoxChecksum; }
	void setWrapBoxChecksum( UINT16 checksum ) { _wrapBoxChecksum = checksum; }

	std::string getConfigVersion() {return _configVersion; }
	void setConfigVersion(UINT16 major, UINT16 minor);

	std::string getBootVersion() { return _bootVersion; }
	void setBootVersion( UINT16 major, UINT16 minor );

	std::string getDataVersion() { return _dataVersion; }
	void setDataVersion( UINT16 major, UINT16 minor );

	std::string getAppVersion() { return _appVersion; }
	void setAppVersion( UINT16 major, UINT16 minor );

	const char *serialize() const;

	static const int MAX_VER_LENGTH;

	private:
	UINT16 _nodeId;
	UINT16 _wrapBoxChecksum;
	std::string _configVersion;
	std::string _bootVersion;
	std::string _dataVersion;
	std::string _appVersion;

};






#endif /* NGWVERSION_H_ */
