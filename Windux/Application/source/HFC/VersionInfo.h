/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include "evolution.h"
#include "HfcObject.h"

#include <string>
 

typedef enum VersionInfoFieldOffsets
{
	fieldOffsetBuildNumber = 1,
	endVersionInfoFieldOffsets
} VersionInfoFieldOffsets_t;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Backend version information.
class VersionInfo : public HfcObject
{
	typedef VersionInfo _Myt;
	typedef HfcObject _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	VersionInfo();

	/// Destructor.
	virtual ~VersionInfo();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Serialize object.
	/**
	 * @param[in] format  format to serialize into
	 * @return  serialized data
	 */
	virtual const char *serialize( int format ) const;

	// ========================================================================
	// Static Methods
	// ========================================================================

	/// Retrieve ip address and gateway address.
	/**
	 * @param[out] ip  ip address
	 * @param[out] gateway  gateway address
	 */
	static void getIpAndGatewayAddress( std::string &ip, std::string &gateway );

	/// Retrieve list of DNS addresses.
	/**
	 * @param[out] buf  buffer for data
	 * @param[in] bufLen  buffer length
	 * @return  @c true upon success, @c false otherwise
	 */
	static bool getDNSList( char* buf, unsigned int bufLen );

	/// Retrieve Application version.
	/**
	 * @return  app version string
	 */
	static std::string getApplicationVersion();

	/// Retrieve SM Backend version.
	/**
	 * @return  SM Backend version string
	 */
	static std::string getSMBackendVersion();

	/// Retrieve hardware revision.
	/**
	 * @return  Hardware revision string
	 */
	static std::string getHardwareRev();

	/// Retrieve list of installed features.
	/**
	 * @return  comma delimited list of features
	 */
	static std::string getInstalledFeatures();

private:

#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

	// not implemented
	_Myt &operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VERSIONINFO_H
