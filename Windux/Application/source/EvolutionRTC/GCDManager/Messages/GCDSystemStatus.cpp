
#include "GCDSystemStatus.h"
#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GCDSystemStatus::dataType = 0;
std::string GCDSystemStatus::dataTypeName("GCDSystemStatus"); 
#endif

GCDSystemStatus::GCDSystemStatus(Hobart::CEvolMsgReceiver *pHobThread,
										 SystemStatusInfo *systemStatus,
										 bool   bDDRTestPassed,
										 UINT16 gcdBoardSoftwareMajor,
										 UINT16 gcdBoardSoftwareMinor,
										 UINT16 gcdWinCESoftwareMajor,
										 UINT16 gcdWinCESoftwareMinor,
										 UINT16 gcdMGRSoftwareMajor,
										 UINT16 gcdMGRSoftwareMinor,
										 UINT16 gcdDLLSoftwareMajor,
										 UINT16 gcdDLLSoftwareMinor) : SystemStatus(GCDSessionDev, pHobThread, systemStatus)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	gcdDDRTestPassed = bDDRTestPassed;
	gcdBoardVersion.setMajorRevision(gcdBoardSoftwareMajor);
	gcdBoardVersion.setMinorRevision(gcdBoardSoftwareMinor);
	gcdWinCEVersion.setMajorRevision(gcdWinCESoftwareMajor);
	gcdWinCEVersion.setMinorRevision(gcdWinCESoftwareMinor);
	gcdMGRSoftwareVersion.setMajorRevision(gcdMGRSoftwareMajor);
	gcdMGRSoftwareVersion.setMinorRevision(gcdMGRSoftwareMinor);
	gcdDLLSoftwareVersion.setMajorRevision(gcdDLLSoftwareMajor);
	gcdDLLSoftwareVersion.setMinorRevision(gcdDLLSoftwareMinor);
}

GCDSystemStatus::GCDSystemStatus(const char *buffer) : SystemStatus(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	SystemStatus::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &gcdDDRTestPassed;
	for (unsigned int i = 0; i < sizeof(gcdDDRTestPassed); i++)
	{
		*pChar++ = buffer[j++];
	}

	gcdBoardVersion = HobVersion(&buffer[j]);
	gcdBoardVersion.serialize();
	j += gcdBoardVersion.getSerializedLength();

	gcdWinCEVersion = HobVersion(&buffer[j]);
	gcdWinCEVersion.serialize();
	j += gcdWinCEVersion.getSerializedLength();

	gcdMGRSoftwareVersion = HobVersion(&buffer[j]);
	gcdMGRSoftwareVersion.serialize();
	j += gcdMGRSoftwareVersion.getSerializedLength();

	gcdDLLSoftwareVersion = HobVersion(&buffer[j]);
	gcdDLLSoftwareVersion.serialize();
	j += gcdDLLSoftwareVersion.getSerializedLength();
}

GCDSystemStatus::~GCDSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *GCDSystemStatus::serialize() const
{
	SystemStatus::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &gcdDDRTestPassed;
	for (unsigned int i = 0; i < sizeof(gcdDDRTestPassed); i++)
	{
		serializedData.append(1, pChar[i]);
	}
		
	const char *pCC = gcdBoardVersion.serialize();
	int mLength = gcdBoardVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = gcdWinCEVersion.serialize();
	mLength = gcdWinCEVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = gcdMGRSoftwareVersion.serialize();
	mLength = gcdMGRSoftwareVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = gcdDLLSoftwareVersion.serialize();
	mLength = gcdDLLSoftwareVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

void GCDSystemStatus::show() const
{
	bool isTracing = Debug::isTracing("GCDSystemStatus");

	if (isTracing == false)
	{
		Debug::addTraceCategory("GCDSystemStatus");
	}

	Debug::trace("GCDSystemStatus", "GCDSystemStatus: ");

	SystemStatus::show();


	Debug::trace("GCDSystemStatus", HobString("gcdDDRTestPassed: ") + HobString(Bool::toString(gcdDDRTestPassed)));
	Debug::trace("GCDSystemStatus", HobString("gcdBoardVersion: ") + const_cast<HobVersion&>(gcdBoardVersion).toString()->c_str());
	Debug::trace("GCDSystemStatus", HobString("gcdWinCEVersion: ") + const_cast<HobVersion&>(gcdWinCEVersion).toString()->c_str());
	Debug::trace("GCDSystemStatus", HobString("gcdMGRSoftwareVersion: ") + const_cast<HobVersion&>(gcdMGRSoftwareVersion).toString()->c_str());
	Debug::trace("GCDSystemStatus", HobString("gcdDLLSoftwareVersion: ") + const_cast<HobVersion&>(gcdDLLSoftwareVersion).toString()->c_str());

	if (isTracing == false)
	{
		Debug::removeTraceCategory("GCDSystemStatus");
	}
}

bool GCDSystemStatus::getDDRTestPassed() const
{
	return gcdDDRTestPassed;
}

void GCDSystemStatus::setDDRTestPassed(bool bMemTestPassed)
{
	gcdDDRTestPassed = bMemTestPassed;
}

UINT16 GCDSystemStatus::getBoardVersionMajor() const
{
	return gcdBoardVersion.getMajorRevision();
}

UINT16 GCDSystemStatus::getBoardVersionMinor() const
{
	return gcdBoardVersion.getMinorRevision();
}

const HobVersion &GCDSystemStatus::getBoardVersion() const
{
	return gcdBoardVersion;
}

void GCDSystemStatus::setBoardVersionMajor(UINT16 major)
{
	gcdBoardVersion.setMajorRevision(major);
}

void GCDSystemStatus::setBoardVersionMinor(UINT16 minor)
{
	gcdBoardVersion.setMinorRevision(minor);
}

void GCDSystemStatus::setBoardVersion(UINT16 major, UINT16 minor)
{
	gcdBoardVersion.setMajorRevision(major);
	gcdBoardVersion.setMinorRevision(minor);
}

void GCDSystemStatus::setBoardVersion(const HobVersion &softwareVersion)
{
	this->gcdBoardVersion.setMajorRevision(softwareVersion.getMajorRevision());
	this->gcdBoardVersion.setMinorRevision(softwareVersion.getMinorRevision());
}

UINT16 GCDSystemStatus::getWinCEVersionMajor() const
{
	return gcdWinCEVersion.getMajorRevision();
}

UINT16 GCDSystemStatus::getWinCEVersionMinor() const
{
	return gcdWinCEVersion.getMinorRevision();
}

const HobVersion &GCDSystemStatus::getWinCEVersion() const
{
	return gcdWinCEVersion;
}

void GCDSystemStatus::setWinCEVersionMajor(UINT16 major)
{
	gcdWinCEVersion.setMajorRevision(major);
}

void GCDSystemStatus::setWinCEVersionMinor(UINT16 minor)
{
	gcdWinCEVersion.setMinorRevision(minor);
}

void GCDSystemStatus::setWinCEVersion(UINT16 major, UINT16 minor)
{
	gcdWinCEVersion.setMajorRevision(major);
	gcdWinCEVersion.setMinorRevision(minor);
}

void GCDSystemStatus::setWinCEVersion(const HobVersion &softwareVersion)
{
	this->gcdWinCEVersion.setMajorRevision(softwareVersion.getMajorRevision());
	this->gcdWinCEVersion.setMinorRevision(softwareVersion.getMinorRevision());
}

UINT16 GCDSystemStatus::getGCDMGRSoftwareVersionMajor() const
{
	return gcdMGRSoftwareVersion.getMajorRevision();
}

UINT16 GCDSystemStatus::getGCDMGRSoftwareVersionMinor() const
{
	return gcdMGRSoftwareVersion.getMinorRevision();
}

const HobVersion &GCDSystemStatus::getGCDMGRSoftwareVersion() const
{
	return gcdMGRSoftwareVersion;
}

void GCDSystemStatus::setGCDMGRSoftwareVersionMajor(UINT16 major)
{
	gcdMGRSoftwareVersion.setMajorRevision(major);
}

void GCDSystemStatus::setGCDMGRSoftwareVersionMinor(UINT16 minor)
{
	gcdMGRSoftwareVersion.setMinorRevision(minor);
}

void GCDSystemStatus::setGCDMGRSoftwareVersion(UINT16 major, UINT16 minor)
{
	gcdMGRSoftwareVersion.setMajorRevision(major);
	gcdMGRSoftwareVersion.setMinorRevision(minor);
}

void GCDSystemStatus::setGCDMGRSoftwareVersion(const HobVersion &softwareVersion)
{
	this->gcdMGRSoftwareVersion.setMajorRevision(softwareVersion.getMajorRevision());
	this->gcdMGRSoftwareVersion.setMinorRevision(softwareVersion.getMinorRevision());
}

UINT16 GCDSystemStatus::getGCDDLLSoftwareVersionMajor() const
{
	return gcdDLLSoftwareVersion.getMajorRevision();
}

UINT16 GCDSystemStatus::getGCDDLLSoftwareVersionMinor() const
{
	return gcdDLLSoftwareVersion.getMinorRevision();
}

const HobVersion &GCDSystemStatus::getGCDDLLSoftwareVersion() const
{
	return gcdDLLSoftwareVersion;
}

void GCDSystemStatus::setGCDDLLSoftwareVersionMajor(UINT16 major)
{
	gcdDLLSoftwareVersion.setMajorRevision(major);
}

void GCDSystemStatus::setGCDDLLSoftwareVersionMinor(UINT16 minor)
{
	gcdDLLSoftwareVersion.setMinorRevision(minor);
}

void GCDSystemStatus::setGCDDLLSoftwareVersion(UINT16 major, UINT16 minor)
{
	gcdDLLSoftwareVersion.setMajorRevision(major);
	gcdDLLSoftwareVersion.setMinorRevision(minor);
}

void GCDSystemStatus::setGCDDLLSoftwareVersion(const HobVersion &softwareVersion)
{
	this->gcdDLLSoftwareVersion.setMajorRevision(softwareVersion.getMajorRevision());
	this->gcdDLLSoftwareVersion.setMinorRevision(softwareVersion.getMinorRevision());
}
