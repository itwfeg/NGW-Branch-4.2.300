/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperControl.h"
#include "HfcEvolWrapperConfig.h"
#include "HobTokenizerCSV.h"
#include "Configuration.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

	int HfcEvolWrapperControl::_dataType_ = 0;
	std::string HfcEvolWrapperControl::_dataTypeName_("HfcEvolWrapperControl"); 
#endif

HfcEvolWrapperControl::HfcEvolWrapperControl(const char *data) : EvolWrapperControl(data)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

}

HfcEvolWrapperControl::HfcEvolWrapperControl(const char *data, int len, int formatType) : EvolWrapperControl()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    bool traceOn = Debug::isTracing("HfcEvolWrapperControl");
    HobString msgDebug = "HfcEvolWrapperControl()";

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

                //      New format for this message is as follows:  offset, value
                if (tok.next(&token) != -1)
                    fieldOffset = Integer::valueOf(token.c_str());

                while (tok.next(&token) != -1)
                {
                    setReceived(fieldOffset, (token.length() == 0) ? false : true);

                    switch (fieldOffset)
                    {
                        case fieldOffsetEvolWrSBHeaterSetting:
                            {
                                setSealerBeltTempSetting(Integer::valueOf(token.c_str()));
                                if (traceOn)
                                    gotWhat.append("HeaterSetting = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrSBHeaterEnable:
                            {
                                setSealerBeltHeaterEnabled(Bool::valueOf(token.c_str()));
                                if (traceOn)
                                    gotWhat.append("HeaterEnable = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrFilmSupplySetting:
                            {
                                setFilmSupplyTempSetting(Integer::valueOf(token.c_str()));
                                if (traceOn)
                                    gotWhat.append("FilmSupplySetting = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrFilmSupplyEnable:
                            {
                                setFilmSupplyHeaterEnabled(Bool::valueOf(token.c_str()));
                                if (traceOn)
                                    gotWhat.append("FilmSupplyEnable = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrFilmTension:
                            {
                                setFilmTension((WrFilmTension) (Integer::valueOf(token.c_str())));
                                if (traceOn)
                                    gotWhat.append("FilmTension = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrFilmTensionFrontToBack:
                            {
                                setFilmTensionFrontToBack(Integer::valueOf(token.c_str()));
                                if (traceOn)
                                    gotWhat.append("FilmTensionFrontToBack = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrFilmTensionRightToLeft:
                            {
                                setFilmTensionRightToLeft(Integer::valueOf(token.c_str()));
                                if (traceOn)
                                    gotWhat.append("FilmTensionRightToLeft = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrFilmWidth:
                            {
                                setFilmWidthSelection((WrFilmWidthSelection) (Integer::valueOf(token.c_str())));
                                if (traceOn)
                                    gotWhat.append("FilmWidth = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrIntakeMode:
                            {
                                setIntakeMode((WrapperIntakeMode::IntakeMode) (Integer::valueOf(token.c_str())));
                                if (traceOn)
                                    gotWhat.append("IntakeMode = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrUpperRollWidth:
                            {
                                setUpperRollWidth(Integer::valueOf(token.c_str()));
                                if (traceOn)
                                    gotWhat.append("UpperRollWidth = ").append(token);
                                break;
                            }
                        case fieldOffsetEvolWrLowerRollWidth:
                            {
                                setLowerRollWidth(Integer::valueOf(token.c_str()));
                                if (traceOn)
                                    gotWhat.append("LowerRollWidth = ").append(token);
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
                Debug::trace("HfcEvolWrapperControl", msgDebug);
            }
            break;

        default:
            // todo - throw exception("invalid format");
            break;
    }
}

HfcEvolWrapperControl::HfcEvolWrapperControl(HfcEvolWrapperConfig *wrConfig) : EvolWrapperControl()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    setSealerBeltHeaterEnabled(true);
    setFilmSupplyHeaterEnabled(false);
    setFilmWidthSelection(Automatic);

    setFilmTension((WrFilmTension) Configuration::getInstance()->getDefaultFilmTension());
    setIntakeMode((WrapperIntakeMode::IntakeMode) Configuration::getInstance()->getDefaultIntakeMode());
    setFilmTensionFrontToBack(Configuration::getInstance()->getDefaultFilmTensionFrontToBack());
    setFilmTensionRightToLeft(Configuration::getInstance()->getDefaultFilmTensionLeftToRight());

    EvolWrapperHeaterConfig heaterConfig = wrConfig->getHeaterConfig();

    HeaterConfiguration sealerHeater = heaterConfig.getSealerHeaterConfig();
    setSealerBeltTempSetting(sealerHeater.getDefaultTempSetting());

    HeaterConfiguration filmHeater = heaterConfig.getFilmHeaterConfig();
    setFilmSupplyTempSetting(filmHeater.getDefaultTempSetting());

    EvolWrapperGeneralConfig wrapperConfig = wrConfig->getWrapperConfig();
    setUpperRollWidth(wrapperConfig.getTopFilmRollDimensions().getRollWidth());
    setLowerRollWidth(wrapperConfig.getBottomFilmRollDimensions().getRollWidth());
}

HfcEvolWrapperControl::~HfcEvolWrapperControl()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperControl::serialize(int format)const
{
    switch (format)
    {
        case InternalFormat:
            {
                Debug::trace("HfcEvolWrapperControl", "serialize(InternalFormat)");
                EvolWrapperControl::serialize();
                EvolWrapperControl::getSerializedLength();
                HfcObject::serializedData = EvolWrapperControl::serializedData;
            }
            break;

        case BrowserFormat:
            {
                Debug::trace("HfcEvolWrapperControl", "serialize(BrowserFormat)");
                short loopCnt = 1;

                HfcObject::serializedData = Integer::toString((int)getDeviceType());
                HfcObject::serializedData.append(BrowserFormatSeparator);
                HfcObject::serializedData.append(Integer::toString((int)getEventType()));

                while(loopCnt < endHfcEvolWrapperControlFieldOffsets)
                {
                    HfcObject::serializedData.append(BrowserFormatSeparator);
                    switch (loopCnt)
                    {
                        case fieldOffsetEvolWrSBHeaterSetting:
                            HfcObject::serializedData.append(Integer::toString(getSealerBeltTempSetting()));
                            break;
                        case fieldOffsetEvolWrSBHeaterEnable:
                            HfcObject::serializedData.append(Bool::toString(isSealerBeltHeaterEnabled()));
                            break;
                        case fieldOffsetEvolWrFilmSupplySetting:
                            HfcObject::serializedData.append(Integer::toString(getFilmSupplyTempSetting()));
                            break;
                        case fieldOffsetEvolWrFilmSupplyEnable:
                            HfcObject::serializedData.append(Bool::toString(isFilmSupplyHeaterEnabled()));
                            break;
                        case fieldOffsetEvolWrFilmTension:
                            HfcObject::serializedData.append(Integer::toString((INT32) getFilmTension()));
                            break;
                        case fieldOffsetEvolWrFilmTensionFrontToBack:
                            HfcObject::serializedData.append(Integer::toString(getFilmTensionFrontToBack()));
                            break;
                        case fieldOffsetEvolWrFilmTensionRightToLeft:
                            HfcObject::serializedData.append(Integer::toString(getFilmTensionRightToLeft()));
                            break;
                        case fieldOffsetEvolWrFilmWidth:
                            HfcObject::serializedData.append(Integer::toString((INT32) getFilmWidthSelection()));
                            break;
                        case fieldOffsetEvolWrIntakeMode:
                            HfcObject::serializedData.append(Integer::toString((INT32) getIntakeMode()));
                            break;
                        case fieldOffsetEvolWrUpperRollWidth:
                            HfcObject::serializedData.append(Integer::toString((int) getUpperRollWidth()));
                            break;
                        case fieldOffsetEvolWrLowerRollWidth:
                            HfcObject::serializedData.append(Integer::toString((int) getLowerRollWidth()));
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

bool HfcEvolWrapperControl::update(HfcEvolWrapperControl *wrControl, bool &sbTempValid, bool &sbTempChanged)
{
    bool traceOn = Debug::isTracing("HfcEvolWrapperControl");
    HobString msgDebug("update(HfcEvolWrapperControl *wrControl): ");

    bool        controlChanged = false;
    short       loopCnt = 1;

    sbTempChanged = false;

    while (loopCnt < endHfcEvolWrapperControlFieldOffsets)
    {
        if (wrControl->isReceived(loopCnt))
        {
            switch (loopCnt)
            {
                case fieldOffsetEvolWrSBHeaterSetting:
                    {
                        if (getSealerBeltTempSetting() != wrControl->getSealerBeltTempSetting())
                        {
                            sbTempValid = isSealerBeltTempSettingValid(wrControl->getSealerBeltTempSetting());
                            if (sbTempValid)
                            {
                                if (traceOn)
                                {
                                    HobString msgDebug2 = msgDebug;
                                    msgDebug2.append("HeaterSetting changed: ").append(Integer::toString(wrControl->getSealerBeltTempSetting()));
                                    Debug::trace("HfcEvolWrapperControl", msgDebug2);
                                }
                                controlChanged = true;
                                sbTempChanged = true;
                                setSealerBeltTempSetting(wrControl->getSealerBeltTempSetting());
                            }
                        }
                    }
                    break;

                case fieldOffsetEvolWrSBHeaterEnable:
                    {
                        if (isSealerBeltHeaterEnabled() != wrControl->isSealerBeltHeaterEnabled())
                        {
                            if (traceOn)
                            {
                                HobString msgDebug2 = msgDebug;
                                msgDebug2.append("HeaterEnable changed: ").append(Bool::toString(wrControl->isSealerBeltHeaterEnabled()));
                                Debug::trace("HfcEvolWrapperControl", msgDebug2);
                            }
                            controlChanged = true;
                            setSealerBeltHeaterEnabled(wrControl->isSealerBeltHeaterEnabled());
                        }
                    }
                    break;

                case fieldOffsetEvolWrFilmSupplySetting:
                    {
                        if (getFilmSupplyTempSetting() != wrControl->getFilmSupplyTempSetting())
                        {
                            if (traceOn)
                            {
                                HobString msgDebug2 = msgDebug;
                                msgDebug2.append("FilmSupplySetting changed: ").append(Integer::toString(wrControl->getFilmSupplyTempSetting()));
                                Debug::trace("HfcEvolWrapperControl", msgDebug2);
                            }
                            controlChanged = true;
                            setFilmSupplyTempSetting(wrControl->getFilmSupplyTempSetting());
                        }
                    }
                    break;

                case fieldOffsetEvolWrFilmSupplyEnable:
                    {
                        if (isFilmSupplyHeaterEnabled() != wrControl->isFilmSupplyHeaterEnabled())
                        {
                            if (traceOn)
                            {
                                HobString msgDebug2 = msgDebug;
                                msgDebug2.append("FilmSupplyHeaterEnable changed: ").append(Bool::toString(wrControl->isFilmSupplyHeaterEnabled()));
                                Debug::trace("HfcEvolWrapperControl", msgDebug2);
                            }
                            controlChanged = true;
                            setFilmSupplyHeaterEnabled(wrControl->isFilmSupplyHeaterEnabled());
                        }
                    }
                    break;

                case fieldOffsetEvolWrFilmTension:
                    {
                        if (getFilmTension() != wrControl->getFilmTension())
                        {
                            if (traceOn)
                            {
                                HobString msgDebug2 = msgDebug;
                                msgDebug2.append("FilmTension changed: ").append(Integer::toString(wrControl->getFilmTension()));
                                Debug::trace("HfcEvolWrapperControl", msgDebug2);
                            }
                            controlChanged = true;
                            setFilmTension(wrControl->getFilmTension());
                        }
                    }
                    break;

                case fieldOffsetEvolWrFilmTensionFrontToBack:
                    {
                        if (getFilmTensionFrontToBack() != wrControl->getFilmTensionFrontToBack())
                        {
                            if (traceOn)
                            {
                                HobString msgDebug2 = msgDebug;
                                msgDebug2.append("FilmTensionFrontToBack changed: ").append(Integer::toString(wrControl->getFilmTensionFrontToBack()));
                                Debug::trace("HfcEvolWrapperControl", msgDebug2);
                            }
                            controlChanged = true;
                            setFilmTensionFrontToBack(wrControl->getFilmTensionFrontToBack());
                        }
                    }
                    break;

                case fieldOffsetEvolWrFilmTensionRightToLeft:
                    {
                        if (getFilmTensionRightToLeft() != wrControl->getFilmTensionRightToLeft())
                        {
                            if (traceOn)
                            {
                                HobString msgDebug2 = msgDebug;
                                msgDebug2.append("FilmTensionRightToLeft changed: ").append(Integer::toString(wrControl->getFilmTensionRightToLeft()));
                                Debug::trace("HfcEvolWrapperControl", msgDebug2);
                            }
                            controlChanged = true;
                            setFilmTensionRightToLeft(wrControl->getFilmTensionRightToLeft());
                        }
                    }
                    break;

                case fieldOffsetEvolWrFilmWidth:
                    {
                        if (getFilmWidthSelection() != wrControl->getFilmWidthSelection())
                        {
                            if (traceOn)
                            {
                                HobString msgDebug2 = msgDebug;
                                msgDebug2.append("FilmWidth changed: ").append(Integer::toString(wrControl->getFilmWidthSelection()));
                                Debug::trace("HfcEvolWrapperControl", msgDebug2);
                            }
                            controlChanged = true;
                            setFilmWidthSelection(wrControl->getFilmWidthSelection());
                        }
                    }
                    break;

                case fieldOffsetEvolWrIntakeMode:
                    {
                        if (getIntakeMode() != wrControl->getIntakeMode())
                        {
                            if (traceOn)
                            {
                                HobString msgDebug2 = msgDebug;
                                msgDebug2.append("IntakeMode changed: ").append(Integer::toString(wrControl->getIntakeMode()));
                                Debug::trace("HfcEvolWrapperControl", msgDebug2);
                            }
                            controlChanged = true;
                            setIntakeMode(wrControl->getIntakeMode());
                        }
                    }
                    break;

                default:
                    break;
            }
        }
        loopCnt++;
    }
    if (traceOn)
        Debug::trace("HfcEvolWrapperControl", "update():  controlChanged = " + HobString(Bool::toString(controlChanged)));
    return controlChanged;
}

bool HfcEvolWrapperControl::update(HfcEvolWrapperConfig *wrConfig)
{
    bool traceOn = Debug::isTracing("HfcEvolWrapperControl");
    HobString msgDebug("update(HfcEvolWrapperConfig *wrConfig): ");

    bool        controlChanged = false;

    if (!wrConfig)
        return controlChanged;

    if (getSealerBeltTempSetting() != wrConfig->getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting())
    {
        if (traceOn)
        {
            HobString msgDebug2 = msgDebug;
            msgDebug2.append("HeaterSetting changed: ").append(Integer::toString(wrConfig->getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting()));
            Debug::trace("HfcEvolWrapperControl", msgDebug2);
        }
        controlChanged = true;
        setSealerBeltTempSetting(wrConfig->getHeaterConfig().getSealerHeaterConfig().getDefaultTempSetting());
    }

    if (traceOn)
        Debug::trace("HfcEvolWrapperControl", "update():  controlChanged = " + HobString(Bool::toString(controlChanged)));
    return controlChanged;
}

unsigned int HfcEvolWrapperControl::getUpperRollWidth() const
{
    return upperRollWidth;
}

void HfcEvolWrapperControl::setUpperRollWidth(unsigned int width)
{
    upperRollWidth = width;
}

unsigned int HfcEvolWrapperControl::getLowerRollWidth() const
{
    return lowerRollWidth;
}

void HfcEvolWrapperControl::setLowerRollWidth(unsigned int width)
{
    lowerRollWidth = width;
}


bool HfcEvolWrapperControl::operator==(HfcEvolWrapperControl &hewc)
{
    bool rv = equals(hewc);

    HobString m(Bool::toString(rv));
    Debug::trace("HfcEvolWrapperControl", "operator== is " + m);
    return rv;
}
