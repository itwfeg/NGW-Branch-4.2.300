/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "GenericHfcObject.h"
#include "EvolMessage.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GenericHfcObject::dataType = 0;
std::string GenericHfcObject::dataTypeName("GenericHfcObject"); 
#endif

GenericHfcObject::GenericHfcObject()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("GenericHfcObject", "default constructor()");
	data = "";
}

GenericHfcObject::GenericHfcObject(const char *initialData)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("GenericHfcObject", HobString("constructor(char *initialData) - initialData:") + HobString((initialData) ? initialData : ""));
	data = "";
	if (initialData)
		data.append(initialData, strlen(initialData));	
}

GenericHfcObject::GenericHfcObject(const std::string &initialData)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("GenericHfcObject", HobString("constructor(string &initialData) - initialData:") + HobString(initialData));
	data = initialData;
}

GenericHfcObject::GenericHfcObject(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	HobString hf(Integer::toString(formatType));
	Debug::trace("GenericHfcObject", HobString("constructor(data,len,format) - data:") + HobString((data) ? data : "") +
								HobString("  len:") + HobString(Integer::toString(len)) +
								HobString("  format:") + hf);
	this->data = "";
	if (data && len > 0)
    {
		if (formatType == InternalFormat)
		{
			UINT16 evolLen = EvolMessage::getEvolMessageLengthInBuffer();
			if (len > evolLen)
				this->data.append(data + evolLen, len - evolLen);
        #if defined(LINUX_FIX)
            else {
                // Nothing
            }
        #else
			else
				;
        #endif
		}
		else
			this->data.append(data, len);
    }
}

GenericHfcObject::~GenericHfcObject()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *GenericHfcObject::serialize(int /* format */) const
{
	serializedData = data;
	return serializedData.data();
}

const HobString &GenericHfcObject::getData() const
{
	return data;
}
void GenericHfcObject::setData(const HobString &data)
{
	this->data = data;
}
