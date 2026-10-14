//#include "vxWorks.h"
#include "EvolWrapperTemperatureInfo.h"

#include "Debug.h"
#include "UInteger.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"
#include "util/e_string/utilstring.h"
using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperTemperatureInfo::_dataType_ = 0;
std::string EvolWrapperTemperatureInfo::_dataTypeName_("EvolWrapperTemperatureInfo"); 
#endif

EvolWrapperTemperatureInfo::EvolWrapperTemperatureInfo() : nodeTemps(_maxNodes_)
														 
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	sealerBeltTemp = 0;
	sealerBeltSetting = 0;
	sealerBeltStatusBits = 0;
	sealerBeltHeaterEnabled = true;
	filmTemp = 0;
	filmSetting = 0;
	filmStatusBits = 0;
	filmTempEnabled = false;
	ambientTemp = 0;
	ambientTempStatusBits = 0;
	internalTempFailed = false;
}

EvolWrapperTemperatureInfo::EvolWrapperTemperatureInfo(const char *buffer) : nodeTemps(_maxNodes_)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	int j = 0;

	char *pChar = (char *) &sealerBeltTemp;
	for (unsigned int i = 0; i < sizeof(sealerBeltTemp); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sealerBeltSetting;
	for (unsigned int i = 0; i < sizeof(sealerBeltSetting); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sealerBeltStatusBits;
	for (unsigned int i = 0; i < sizeof(sealerBeltStatusBits); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sealerBeltHeaterEnabled;
	for (unsigned int i = 0; i < sizeof(sealerBeltHeaterEnabled); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmTemp;
	for (unsigned int i = 0; i < sizeof(filmTemp); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmSetting;
	for (unsigned int i = 0; i < sizeof(filmSetting); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmStatusBits;
	for (unsigned int i = 0; i < sizeof(filmStatusBits); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filmTempEnabled;
	for (unsigned int i = 0; i < sizeof(filmTempEnabled); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &ambientTemp;
	for (unsigned int i = 0; i < sizeof(ambientTemp); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &ambientTempStatusBits;
	for (unsigned int i = 0; i < sizeof(ambientTempStatusBits); i++)
	{
		*pChar++ = buffer[j++];
	}

	UINT16 numNodeBoards = 0;

	pChar = (char *) &numNodeBoards;
	for (unsigned int i = 0; i < sizeof(numNodeBoards); i++)
	{
		*pChar++ = buffer[j++];
	}

	for (UINT16 i = 0; i < numNodeBoards; i++)
	{
		nodeTemps[i] = NodeInternalTemperature(&buffer[j]);

		nodeTemps[i].serialize();
		j += nodeTemps[i].getSerializedLength();
	}	

	pChar = (char *) &internalTempFailed;
	for (unsigned int i = 0; i < sizeof(internalTempFailed); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperTemperatureInfo::EvolWrapperTemperatureInfo(const EvolWrapperTemperatureInfo &ewti) : nodeTemps(_maxNodes_)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("EvolWrapperTemperatureInfo.Memory", "Copy constructor:  this = " + HobString(Hex::toString((long long) this)));

	sealerBeltTemp = ewti.getSealerBeltTemp();
	sealerBeltSetting = ewti.getSealerBeltSetting();
	sealerBeltStatusBits = ewti.getSealerBeltStatusBits();
	sealerBeltHeaterEnabled = ewti.isSealerBeltHeaterEnabled();
	filmTemp = ewti.getFilmTemp();
	filmSetting = ewti.getFilmSetting();
	filmStatusBits = ewti.getFilmStatusBits();
	filmTempEnabled = ewti.isFilmTempEnabled();
	ambientTemp = ewti.getAmbientTemp();
	ambientTempStatusBits = ewti.getAmbientTempStatusBits();

	for (UINT16 i = 0; i < _maxNodes_; ++i)
	{
		nodeTemps[i] = ewti.getNodeBoardTemp(i);
	}
	internalTempFailed = ewti.getInternalTempFailed();
}

EvolWrapperTemperatureInfo::~EvolWrapperTemperatureInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperTemperatureInfo::serialize() const
{
	serializedData = "";
	unsigned int i = 0;

	char *pChar = (char *) &sealerBeltTemp;
	for (i = 0; i < sizeof(sealerBeltTemp); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sealerBeltSetting;
	for (i = 0; i < sizeof(sealerBeltSetting); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sealerBeltStatusBits;
	for (i = 0; i < sizeof(sealerBeltStatusBits); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sealerBeltHeaterEnabled;
	for (i = 0; i < sizeof(sealerBeltHeaterEnabled); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTemp;
	for (i = 0; i < sizeof(filmTemp); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmSetting;
	for (i = 0; i < sizeof(filmSetting); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmStatusBits;
	for (i = 0; i < sizeof(filmStatusBits); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filmTempEnabled;
	for (i = 0; i < sizeof(filmTempEnabled); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &ambientTemp;
	for (unsigned int i = 0; i < sizeof(ambientTemp); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &ambientTempStatusBits;
	for (i = 0; i < sizeof(ambientTempStatusBits); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	UINT16 numNodeBoards = _maxNodes_;
	Debug::trace("EvolWrapperTemperatureInfo", "serialize:  numNodeBoards = " + util_string::inttostring(numNodeBoards));

	pChar = (char *) &numNodeBoards;
	for (i = 0; i < sizeof(numNodeBoards); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	for (UINT16 i = 0; i < _maxNodes_; ++i )
	{
		Debug::trace("EvolWrapperTemperatureInfo", "serialize:  NodeInternalTemperature,  i = " + util_string::inttostring(i));
		const char *pCC = nodeTemps[i].serialize();
		int mLength = nodeTemps[i].getSerializedLength();

		serializedData.append(pCC, mLength);
	}	

	pChar = (char *) &internalTempFailed;
	for (i = 0; i < sizeof(internalTempFailed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 EvolWrapperTemperatureInfo::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void EvolWrapperTemperatureInfo::show()
{
	bool isTracing = Debug::isTracing("EvolWrapperTemperatureInfo");

	if (isTracing == false)
	{
	//	Debug::addTraceCategory("EvolWrapperTemperatureInfo");
	}

	Debug::trace("EvolWrapperTemperatureInfo", "EvolWrapperTemperatureInfo: ");

	Debug::trace("EvolWrapperTemperatureInfo", HobString("sealerBeltTemp: ") + HobString(UInteger::toString(sealerBeltTemp)));
	Debug::trace("EvolWrapperTemperatureInfo", HobString("sealerBeltSetting: ") + HobString(UInteger::toString(sealerBeltSetting)));
	Debug::trace("EvolWrapperTemperatureInfo", HobString("sealerBeltStatusBits: 0x") + HobString(Hex::toString(sealerBeltStatusBits)));
	Debug::trace("EvolWrapperTemperatureInfo", HobString("sealerBeltHeaterEnabled: ") + HobString(Bool::toString(sealerBeltHeaterEnabled)));

	Debug::trace("EvolWrapperTemperatureInfo", HobString("filmTemp: ") + HobString(UInteger::toString(filmTemp)));
	Debug::trace("EvolWrapperTemperatureInfo", HobString("filmSetting: ") + HobString(UInteger::toString(filmSetting)));
	Debug::trace("EvolWrapperTemperatureInfo", HobString("filmStatusBits: 0x") + HobString(Hex::toString(filmStatusBits)));
	Debug::trace("EvolWrapperTemperatureInfo", HobString("filmTempEnabled: ") + HobString(Bool::toString(filmTempEnabled)));

	Debug::trace("EvolWrapperTemperatureInfo", HobString("ambientTemp: ") + HobString(UInteger::toString(ambientTemp)));
	Debug::trace("EvolWrapperTemperatureInfo", HobString("ambientTempStatusBits: 0x") + HobString(Hex::toString(ambientTempStatusBits)));

	for ( int i = 0; i < _maxNodes_; ++i )
	{
		nodeTemps[i].show();
	}
	Debug::trace("EvolWrapperTemperatureInfo", HobString("internalTempFailed: 0x") + HobString(Bool::toString(internalTempFailed)));

	if (isTracing == false)
	{
	//	Debug::removeTraceCategory("EvolWrapperTemperatureInfo");
	}
}

bool EvolWrapperTemperatureInfo::equals(const EvolWrapperTemperatureInfo &evolTemp) const
{
	bool rv = false;

	if ((sealerBeltTemp == evolTemp.getSealerBeltTemp()) &&
		(sealerBeltSetting == evolTemp.getSealerBeltSetting()) &&
		(sealerBeltStatusBits == evolTemp.getSealerBeltStatusBits()) &&
		(sealerBeltHeaterEnabled == evolTemp.getSealerBeltHeaterEnabled()) &&
		(filmTemp == evolTemp.getFilmTemp()) &&
		(filmSetting == evolTemp.getFilmSetting()) &&
		(filmStatusBits == evolTemp.getFilmStatusBits()) &&
		(filmTempEnabled == evolTemp.getFilmTempEnabled()) &&
		(ambientTemp == evolTemp.getAmbientTemp()) &&
		(ambientTempStatusBits == evolTemp.getAmbientTempStatusBits()) &&
//		(nodeBoardTemp == evolTemp.getNodeBoardTemp()) &&
		(internalTempFailed == evolTemp.getInternalTempFailed()))
	{
		rv = true;
		for (UINT16 i = 0; i < _maxNodes_; ++i)
		{
			if (nodeTemps[i] != evolTemp.getNodeBoardTemp(i))
			{
				rv = false;
				break;
			}
		}
	}

	return rv;
}

UINT16 EvolWrapperTemperatureInfo::getSealerBeltTemp() const
{
	return sealerBeltTemp;
}

void EvolWrapperTemperatureInfo::setSealerBeltTemp(UINT16 sealerBeltTemp)
{
	this->sealerBeltTemp = sealerBeltTemp;
}

UINT16 EvolWrapperTemperatureInfo::getSealerBeltSetting() const
{
	return sealerBeltSetting;
}

void EvolWrapperTemperatureInfo::setSealerBeltSetting(UINT16 sealerBeltSetting)
{
	this->sealerBeltSetting = sealerBeltSetting;
}

UINT16 EvolWrapperTemperatureInfo::getSealerBeltStatusBits() const
{
	return sealerBeltStatusBits;
}

void EvolWrapperTemperatureInfo::setSealerBeltStatusBits(UINT16 sealerBeltStatusBits)
{
	this->sealerBeltStatusBits = sealerBeltStatusBits;
}

bool EvolWrapperTemperatureInfo::isSealerBeltHeaterEnabled() const
{
	return sealerBeltHeaterEnabled;
}

void EvolWrapperTemperatureInfo::setSealerBeltHeaterEnabled()
{
	sealerBeltHeaterEnabled = true;
}

void EvolWrapperTemperatureInfo::clearSealerBeltHeaterEnabled()
{
	sealerBeltHeaterEnabled = false;
}

bool EvolWrapperTemperatureInfo::getSealerBeltHeaterEnabled() const
{
	return sealerBeltHeaterEnabled;
}

void EvolWrapperTemperatureInfo::setSealerBeltHeaterEnabled(bool sealerBeltHeaterEnabled)
{
	this->sealerBeltHeaterEnabled = sealerBeltHeaterEnabled;
}

UINT16 EvolWrapperTemperatureInfo::getFilmTemp() const
{
	return filmTemp;
}

void EvolWrapperTemperatureInfo::setFilmTemp(UINT16 filmTemp)
{
	this->filmTemp = filmTemp;
}

UINT16 EvolWrapperTemperatureInfo::getFilmSetting() const
{
	return filmSetting;
}

void EvolWrapperTemperatureInfo::setFilmSetting(UINT16 filmSetting)
{
	this->filmSetting = filmSetting;
}

UINT16 EvolWrapperTemperatureInfo::getFilmStatusBits() const
{
	return filmStatusBits;
}

void EvolWrapperTemperatureInfo::setFilmStatusBits(UINT16 filmStatusBits)
{
	this->filmStatusBits = filmStatusBits;
}

bool EvolWrapperTemperatureInfo::isFilmTempEnabled() const
{
	return filmTempEnabled;
}

void EvolWrapperTemperatureInfo::setFilmTempEnabled()
{
	filmTempEnabled = true;
}

void EvolWrapperTemperatureInfo::clearFilmTempEnabled()
{
	filmTempEnabled = false;
}

bool EvolWrapperTemperatureInfo::getFilmTempEnabled() const
{
	return filmTempEnabled;
}

void EvolWrapperTemperatureInfo::setFilmTempEnabled(bool filmTempEnabled)
{
	this->filmTempEnabled = filmTempEnabled;
}

UINT16 EvolWrapperTemperatureInfo::getAmbientTemp() const
{
	return ambientTemp;
}

void EvolWrapperTemperatureInfo::setAmbientTemp(UINT16 ambientTemp)
{
	this->ambientTemp = ambientTemp;
}

UINT16 EvolWrapperTemperatureInfo::getAmbientTempStatusBits() const
{
	return ambientTempStatusBits;
}

void EvolWrapperTemperatureInfo::setAmbientTempStatusBits(UINT16 ambientTempStatusBits)
{
	this->ambientTempStatusBits = ambientTempStatusBits;
}

const NodeInternalTemperature &EvolWrapperTemperatureInfo::getNodeBoardTemp(UINT16 nodeId) const
{
	Debug::trace("EvolWrapperTemperatureInfo", "getNodeBoardTemp(" + util_string::inttostring(nodeId) + ")");

	return nodeTemps[nodeId];
}

void EvolWrapperTemperatureInfo::setNodeBoardTemp(UINT16 nodeId, UINT16 temp, UINT16 statusBits)
{
	bool isTracing = Debug::isTracing("EvolWrapperTemperatureInfo");
	HobString msgDebug;
	if (isTracing)
	{
		msgDebug = "setNodeBoardTemp(";
		msgDebug.append(util_string::inttostring(nodeId)).append(", ");
		msgDebug.append(util_string::inttostring(temp)).append(", ");
		msgDebug.append(util_string::inttostring(statusBits)).append(")");
		Debug::trace("EvolWrapperTemperatureInfo", msgDebug);
	}

	if (nodeId < _maxNodes_)
	{
		Debug::trace("EvolWrapperTemperatureInfo", msgDebug + "Setting the node.");
		nodeTemps[nodeId] = NodeInternalTemperature(nodeId, temp, statusBits);
	}
}

bool EvolWrapperTemperatureInfo::isInternalTempFailed() const
{
	return internalTempFailed;
}

void EvolWrapperTemperatureInfo::setInternalTempFailed()
{
	internalTempFailed = true;
}

void EvolWrapperTemperatureInfo::clearInternalTempFailed()
{
	internalTempFailed = false;
}

bool EvolWrapperTemperatureInfo::getInternalTempFailed() const
{
	return internalTempFailed;
}

void EvolWrapperTemperatureInfo::setInternalTempFailed(bool internalTempFailed)
{
	this->internalTempFailed = internalTempFailed;
}

void EvolWrapperTemperatureInfo::operator=(const EvolWrapperTemperatureInfo& ewti)
{
	bool isTracing = Debug::isTracing("EvolWrapperTemperatureInfo");
	HobString msgDebug;
	if (isTracing)
	{
		msgDebug = "operator=():  ";
		//Debug::trace("EvolWrapperTemperatureInfo", msgDebug + "this = " + HobString(Hex::toString((long) this)));
	}
	sealerBeltTemp = ewti.getSealerBeltTemp();
	sealerBeltSetting = ewti.getSealerBeltSetting();
	sealerBeltStatusBits = ewti.getSealerBeltStatusBits();
	sealerBeltHeaterEnabled = ewti.isSealerBeltHeaterEnabled();
	filmTemp = ewti.getFilmTemp();
	filmSetting = ewti.getFilmSetting();
	filmStatusBits = ewti.getFilmStatusBits();
	filmTempEnabled = ewti.isFilmTempEnabled();
	ambientTemp = ewti.getAmbientTemp();
	ambientTempStatusBits = ewti.getAmbientTempStatusBits();

	for (UINT16 i = 0; i < _maxNodes_; ++i)
	{
		nodeTemps[i] = ewti.getNodeBoardTemp(i);
	}
	internalTempFailed = ewti.getInternalTempFailed();
}

