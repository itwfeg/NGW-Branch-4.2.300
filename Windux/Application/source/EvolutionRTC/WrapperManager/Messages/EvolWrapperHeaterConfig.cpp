
#include "EvolWrapperHeaterConfig.h"

#include "Debug.h"
#include "Long.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperHeaterConfig::_dataType_ = 0;
std::string EvolWrapperHeaterConfig::_dataTypeName_("EvolWrapperHeaterConfig"); 
#endif

EvolWrapperHeaterConfig::EvolWrapperHeaterConfig()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperHeaterConfig::EvolWrapperHeaterConfig(const char *buffer)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
    int j = 0;

    char *pChar = (char *) &checksum;
    for (unsigned int i = 0; i < sizeof(checksum); i++)
    {
        *pChar++ = buffer[j++];
    }

    filmHeater = HeaterConfiguration(&buffer[j]);
    filmHeater.serialize();
    j += filmHeater.getSerializedLength();

    sealerHeater = HeaterConfiguration(&buffer[j]);
}

EvolWrapperHeaterConfig::EvolWrapperHeaterConfig(const EvolWrapperHeaterConfig &ewhc)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
    checksum = ewhc.getChecksum();
    filmHeater = ewhc.getFilmHeaterConfig();
    sealerHeater = ewhc.getSealerHeaterConfig();
}

EvolWrapperHeaterConfig::~EvolWrapperHeaterConfig()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperHeaterConfig::serialize() const
{
    serializedData = "";

    char *pChar = (char *) &checksum;
    for (unsigned int i = 0; i < sizeof(checksum); i++)
    {
        serializedData.append(1, pChar[i]);
    }

    const char *pCC = filmHeater.serialize();
    int mLength = filmHeater.getSerializedLength();
    serializedData.append(pCC, mLength);

    pCC = sealerHeater.serialize();
    mLength = sealerHeater.getSerializedLength();
    serializedData.append(pCC, mLength);

    return serializedData.data();
}

UINT32 EvolWrapperHeaterConfig::getSerializedLength() const
{
    return (UINT32)serializedData.length();
}

void EvolWrapperHeaterConfig::show()
{
    if (Debug::isTracing("EvolWrapperHeaterConfig"))
    {
        //              Debug::addTraceCategory("EvolWrapperHeaterConfig");

        HobString msgDebug("checksum = ");
        msgDebug.append(Long::toString(checksum));
        Debug::trace("EvolWrapperHeaterConfig", msgDebug);

        filmHeater.show();
        sealerHeater.show();

        //              Debug::removeTraceCategory("EvolWrapperHeaterConfig");
    }
}

ULONG EvolWrapperHeaterConfig::getChecksum() const
{
    return checksum;
}

void EvolWrapperHeaterConfig::setChecksum(ULONG checksum)
{
    this->checksum = checksum;
}

bool EvolWrapperHeaterConfig::getFilmHeaterIsUsed() const
{
	return filmHeater.isItUsed();
}

HeaterConfiguration &EvolWrapperHeaterConfig::getFilmHeaterConfig()
{
	return filmHeater;
}
const HeaterConfiguration &EvolWrapperHeaterConfig::getFilmHeaterConfig()const
{
    return filmHeater;
}

HeaterConfiguration &EvolWrapperHeaterConfig::getSealerHeaterConfig()
{
    return sealerHeater;
}
const HeaterConfiguration &EvolWrapperHeaterConfig::getSealerHeaterConfig()const
{
    return sealerHeater;
}

void EvolWrapperHeaterConfig::operator=(const EvolWrapperHeaterConfig &ewhc)
{
    checksum = ewhc.getChecksum();
    filmHeater = ewhc.getFilmHeaterConfig();
    sealerHeater = ewhc.getSealerHeaterConfig();
}
