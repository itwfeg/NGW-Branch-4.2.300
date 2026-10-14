/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperSettings.h"
#include "HfcWrapperConfig.h"
#include "HobTokenizerCSV.h"
#include "Bool.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperSettings::dataType = 0;
std::string HfcWrapperSettings::dataTypeName("HfcWrapperSettings"); 
#endif

HfcWrapperSettings::HfcWrapperSettings(const char *data) : WrapperSettings(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperSettings::HfcWrapperSettings(const char *data, int len, int formatType) : WrapperSettings()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				switch (loopCnt)
				{
					case fieldOffsetWrFilmWidth:
						setFilmWidth((WrPackWidth) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetWrFilmTension:
						setFilmTension((WrFilmTension) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetWrIntakeMode:
						setIntakeMode((WrapperIntakeMode::IntakeMode) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetWrHeaterSetting:
						getHeaterInfo().setSetting(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetWrHeaterEnable:
						getHeaterInfo().setState(Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetWrIsCelsius:
						getHeaterInfo().setCelsius(Bool::valueOf(token.c_str()));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWrapperSettings::HfcWrapperSettings(HfcWrapperConfig *wrConfig) : WrapperSettings()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (wrConfig)
	{
		setFilmWidth(wrConfig->getPackWidth());
		setFilmTension(wrConfig->getFilmTension());
	//	setIntakeMode(wrConfig->getFilmWidthSelection());
		getHeaterInfo().setSetting(wrConfig->getHeaterSetting());
		getHeaterInfo().setState(wrConfig->getHeaterEnable());
		getHeaterInfo().setCelsius(wrConfig->isCelsiusEnable());
	}
}

HfcWrapperSettings::~HfcWrapperSettings()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperSettings::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			WrapperSettings::serialize();
			HfcObject::serializedData = WrapperSettings::serializedData;
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWrapperSettingsFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetWrFilmWidth:
						HfcObject::serializedData.append(Integer::toString((int) getFilmWidth()));
						break;
					case fieldOffsetWrFilmTension:
						HfcObject::serializedData.append(Integer::toString((int) getFilmTension()));
						break;
					case fieldOffsetWrIntakeMode:
						HfcObject::serializedData.append(Integer::toString((int) (getIntakeMode().getIntakeMode())));
						break;
					case fieldOffsetWrHeaterSetting:
						HfcObject::serializedData.append(Integer::toString(getHeaterInfo().getSetting()));
						break;
					case fieldOffsetWrHeaterEnable:
						HfcObject::serializedData.append(Bool::toString(getHeaterInfo().isState()));
						break;
					case fieldOffsetWrIsCelsius:
						HfcObject::serializedData.append(Bool::toString(getHeaterInfo().isCelsius()));
						break;
					default:
						break;
				}
				loopCnt++;
			}

			const char *msgdata = HfcObject::serializedData.data();
			int msglen = HfcObject::getSerializedLength();

			Debug::trace("Session.WrapperStatus", "Operate Mode Msg: " + HobString(msgdata, msglen));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}

	return HfcObject::serializedData.data();
}
