/*
 *  @file Information.h
 *
 *  DIGIMARC CORPORATION PROPRIETARY INFORMATION
 *  Copyright, Digimarc Corporation.  All Rights Reserved.
 *
 *  This software contains confidential information, including trade secrets,
 *  of Digimarc that is protected by the confidentiality agreement entered into
 *  between you and Digimarc.  It is important that this information be used only
 *  in accordance with your agreement with Digimarc.  Receipt of this software
 *  conveys no license under Digimarc's patent, trademark, trade secret, or
 *  copyright rights.  Use of this software - and the technology detailed therein -
 *  requires a license from Digimarc. The technology embodied in this software is
 *  protected by US and International Patents.
 *
 *  DLSDK Information
 */

#ifndef INFORMATION_H
#define INFORMATION_H

namespace DigimarcLabelEmbedder {

/**
 * @brief The Information class can be used to programmatically access details about this SDK.
 * Package version, build number, vendor, etc.
 * @ingroup SDK_Structs
 */
class Information {
public:

	static const char* Vendor;				///< Digimarc: Name of the company or group who authored this software.
	static const char* ProductName;			///< Digimarc Label SDK: Official name of software package.

	static const char* TargetPlatform;		///< Linux: Name of the targeted system.
	static const char* TargetProcessor;		///< ARM: Name of the targeted processor type.
	static const char* TargetSystem;		///< Linux: Name of the targeted OS.
	static const char* BasicCompilerName;	///< GNU: Name of the compiler used (version not included).
	static const char* BuildType;			///< release:Debug or Release build.

	static const int VersionMajor;			///< 1: Major software version (X.y.z.b)
	static const int VersionMinor;			///< 6: Minor software version (x.Y.z.b)
	static const int VersionPatch;			///< 1: patch software version (x.y.Z.b)
	static const char* VersionBuild;		///< 20190807143243: Build number of software (x.y.z.B)
	static const char* VersionConstructed;	///< 1.6.1 - 20190807143243: Fully constructed version string useful for general printing.
};

}

#endif
