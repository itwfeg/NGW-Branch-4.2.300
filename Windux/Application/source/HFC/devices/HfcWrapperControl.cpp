/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperControl.h"
#include "HfcWrapperSettings.h"
#include "HfcWrapperStatus.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperControl::dataType = 0;
std::string HfcWrapperControl::dataTypeName("HfcWrapperControl"); 
#endif

HfcWrapperControl::HfcWrapperControl(const char *data) : WrapperControl(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcWrapperControl::HfcWrapperControl(const char *data, int len, int formatType) : WrapperControl()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("HfcWrapperControl");
	HobString msgDebug = "HfcWrapperControl()";

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
			HobString gotWhat = ": Received ";
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short fieldOffset = 0;

			//	New format for this message is as follows:  offset, value
			if (tok.next(&token) != -1)
				fieldOffset = Integer::valueOf(token.c_str());

			if (tok.next(&token) != -1)
			{
				setReceived(fieldOffset, (token.length() == 0) ? false : true);

				switch (fieldOffset)
				{
					case fieldOffsetWrControlFilmWidth:
					{
						setFilmWidth((WrPackWidth) Integer::valueOf(token.c_str()));
						if (traceOn)
							gotWhat.append("FilmWidth = ").append(token);
						break;
					}
					case fieldOffsetWrControlFilmTension:
					{
						setFilmTension((WrFilmTension) Integer::valueOf(token.c_str()));
						if (traceOn)
							gotWhat.append("FilmTension = ").append(token);
						break;
					}
					case fieldOffsetWrControlIntakeMode:
					{
						setIntakeMode((WrapperIntakeMode::IntakeMode) Integer::valueOf(token.c_str()));
						if (traceOn)
							gotWhat.append("IntakeMode = ").append(token);
						break;
					}
					case fieldOffsetWrControlHeaterSetting:
					{
						setHeaterSetting(Integer::valueOf(token.c_str()));
						if (traceOn)
							gotWhat.append("HeaterSetting = ").append(token);
						break;
					}
					case fieldOffsetWrControlHeaterEnable:
					{
						setHeaterEnable(Bool::valueOf(token.c_str()));
						if (traceOn)
							gotWhat.append("HeaterEnable = ").append(token);
						break;
					}
					default:
					{
						if (traceOn)
							gotWhat.append("Nothing valid: ").append(token);
						break;
					}
				}
			}
			msgDebug.append(gotWhat);
			Debug::trace("HfcWrapperControl", msgDebug);
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWrapperControl::HfcWrapperControl(HfcWrapperSettings *settings) : WrapperControl()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	setFilmWidth(settings->getFilmWidth());
	setFilmTension(settings->getFilmTension());
	WrapperHeaterInfo heaterInfo = settings->getHeaterInfo();
	setHeaterSetting(heaterInfo.getSetting());
	setHeaterEnable(heaterInfo.isState());
	setIntakeMode(settings->getIntakeMode().getIntakeMode());
}

HfcWrapperControl::~HfcWrapperControl()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperControl::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			WrapperControl::serialize();
			HfcObject::serializedData = WrapperControl::serializedData;
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWrapperControlFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetWrControlFilmWidth:
						HfcObject::serializedData.append(Integer::toString((int) getFilmWidth()));
						break;
					case fieldOffsetWrControlFilmTension:
						HfcObject::serializedData.append(Integer::toString((int) getFilmTension()));
						break;
					case fieldOffsetWrControlIntakeMode:
						HfcObject::serializedData.append(Integer::toString((int) (getIntakeMode())));
						break;
					case fieldOffsetWrControlHeaterSetting:
						HfcObject::serializedData.append(Integer::toString(getHeaterSetting()));
						break;
					case fieldOffsetWrControlHeaterEnable:
						HfcObject::serializedData.append(Bool::toString(isHeaterEnable()));
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

	return HfcObject::serializedData.data();
}


bool HfcWrapperControl::update(HfcWrapperControl *wrControl, bool &settingChanged)
{
	bool traceOn = Debug::isTracing("HfcWrapperControl");
	HobString msgDebug("update(HfcWrapperControl *wrControl): ");

	bool	controlChanged = false;

	settingChanged = false;

	if (!wrControl)
		return controlChanged;

	short	loopCnt = 1;
	while (loopCnt < endHfcWrapperControlFieldOffsets)
	{
		if (wrControl->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetWrControlFilmWidth:
				{
					if (getFilmWidth() != wrControl->getFilmWidth())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("FilmWidth changed: ").append(Integer::toString(wrControl->getFilmWidth()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						controlChanged = true;
						setFilmWidth(wrControl->getFilmWidth());
					}
				}
				break;

				case fieldOffsetWrControlFilmTension:
				{
					if (getFilmTension() != wrControl->getFilmTension())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("FilmTension changed: ").append(Integer::toString(wrControl->getFilmTension()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						controlChanged = true;
						setFilmTension(wrControl->getFilmTension());
					}
				}
				break;

				case fieldOffsetWrControlIntakeMode:
				{
					if (getIntakeMode() != wrControl->getIntakeMode())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("IntakeMode changed: ").append(Integer::toString(wrControl->getIntakeMode()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						controlChanged = true;
						setIntakeMode(wrControl->getIntakeMode());
					}
				}
				break;

				case fieldOffsetWrControlHeaterSetting:
				{
					if (getHeaterSetting() != wrControl->getHeaterSetting())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("HeaterSetting changed: ").append(Integer::toString(wrControl->getHeaterSetting()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						settingChanged = true;
						controlChanged = true;
						setHeaterSetting(wrControl->getHeaterSetting());
					}
				}
				break;

				case fieldOffsetWrControlHeaterEnable:
				{
					if (isHeaterEnable() != wrControl->isHeaterEnable())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("HeaterEnable changed: ").append(Bool::toString(wrControl->isHeaterEnable()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						Debug::trace("HfcWrapperControl", "HeaterEnable changed. ");
						controlChanged = true;
						setHeaterEnable(wrControl->isHeaterEnable());
					}
				}
				break;

				default:
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("Invalid offset = ").append(Integer::toString(loopCnt));
						Debug::trace("HfcWrapperControl", msgDebug2);
					}
				}
				break;
			}
		}
		loopCnt++;
	}
	if (traceOn)
		Debug::trace("HfcWrapperControl", "update():  controlChanged = " + HobString(Bool::toString(controlChanged)));
	return controlChanged;
}

void HfcWrapperControl::update(HfcWrapperStatus *wrStatus)
{
	bool traceOn = Debug::isTracing("HfcWrapperControl");
	HobString msgDebug("update(HfcWrapperStatus *wrStatus): ");

	short	loopCnt = 1;

	while (loopCnt < endHfcWrapperStatusFieldOffsets)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("loopCnt = ").append(Integer::toString(loopCnt));
			msgDebug2.append("  isReceived = ").append(Bool::toString(wrStatus->isReceived(loopCnt)));
			Debug::trace("HfcWrapperControl", msgDebug2);
		}
		if (wrStatus->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetWrStFilmWidth:
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("getFilmWidth() = ").append(Integer::toString(getFilmWidth()));
						msgDebug2.append("  wrStatus->getStatusInfo().getPackWidth() = ").append(Integer::toString(wrStatus->getStatusInfo().getPackWidth()));
						Debug::trace("HfcWrapperControl", msgDebug2);
					}
					if (getFilmWidth() != wrStatus->getStatusInfo().getPackWidth())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("FilmWidth changed: ").append(Integer::toString(wrStatus->getStatusInfo().getPackWidth()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						setFilmWidth(wrStatus->getStatusInfo().getPackWidth());
					}
				}
				break;

				case fieldOffsetWrStFilmTension:
				{
					if (getFilmTension() != wrStatus->getStatusInfo().getFilmTension())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("FilmTension changed: ").append(Integer::toString(wrStatus->getStatusInfo().getFilmTension()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						setFilmTension(wrStatus->getStatusInfo().getFilmTension());
					}
				}
				break;

				case fieldOffsetWrStBeltTempSetting:
				{
					if (getHeaterSetting() != wrStatus->getHeaterInfo().getSetting())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("HeaterSetting changed: ").append(Integer::toString(wrStatus->getHeaterInfo().getSetting()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						setHeaterSetting(wrStatus->getHeaterInfo().getSetting());
					}
				}
				break;

				case fieldOffsetWrStBeltOnOff:
				{
					if (isHeaterEnable() != wrStatus->getHeaterInfo().isState())
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("HeaterEnable changed: ").append(Bool::toString(wrStatus->getHeaterInfo().isState()));
							Debug::trace("HfcWrapperControl", msgDebug2);
						}
						setHeaterEnable(wrStatus->getHeaterInfo().isState());
					}
				}
				break;

				default:
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("Invalid offset = ").append(Integer::toString(loopCnt));
						Debug::trace("HfcWrapperControl", msgDebug2);
					}
				}
				break;
			}
		}
		loopCnt++;
	}
}
