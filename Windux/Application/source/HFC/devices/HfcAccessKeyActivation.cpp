/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcAccessKeyActivation.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcAccessKeyActivation::dataType = 0;
std::string HfcAccessKeyActivation::dataTypeName("HfcAccessKeyActivation"); 
#endif


HfcAccessKeyActivation::HfcAccessKeyActivation()
	: AccessKeyActivation()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}


HfcAccessKeyActivation::HfcAccessKeyActivation(const char *data)
	: AccessKeyActivation(data)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
}


HfcAccessKeyActivation::HfcAccessKeyActivation(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case EvolutionTaggedFormat:
		{
			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					temp = token->getTag();
					if (temp == "ky")
						setKeys(Integer::valueOf(token->getData()));
				}	
				delete token;
			}
			break;	
		}		 

		default:
			// todo - throw exception("invalid format");
			break;
	}
}


HfcAccessKeyActivation::~HfcAccessKeyActivation()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

}


const char *HfcAccessKeyActivation::serialize(int format) const
{
    HfcObject::serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    switch (format)
    {
        case InternalFormat:
            {
                const char *data = AccessKeyActivation::serialize();
                int datalen = AccessKeyActivation::getSerializedLength();
                HfcObject::serializedData.assign(data, datalen);
            }
            break;

        default:
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.data();
}
