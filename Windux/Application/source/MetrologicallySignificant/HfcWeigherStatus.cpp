/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WeigherEvent.h"
#include "HfcWeigherStatus.h"
#include "Configuration.h"
#include "Debug.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherStatus::_dataType_ = 0;
std::string HfcWeigherStatus::_dataTypeName_("HfcWeigherStatus");
#endif

HfcWeigherStatus::HfcWeigherStatus(const HfcWeigherStatus& obj): WeigherStatus(obj), HfcObject(obj),
    weightDisplay(obj.weightDisplay)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif
}

HfcWeigherStatus::HfcWeigherStatus() : WeigherStatus()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();
}

HfcWeigherStatus::HfcWeigherStatus(const char *data) : WeigherStatus(data)
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();
}

HfcWeigherStatus::HfcWeigherStatus(const char *UNUSED(data), int UNUSED(len), int UNUSED(formatType) ) : WeigherStatus()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();

    //  switch (formatType)
    //  {
    /* This case should never happen, we use the above constructor (with the data buffer)
       is used for internal-formatted data.
       case InternalFormat:
       setAllReceived(true);
    // Nothing else to do, the WeigherStatus was filled in during initialization of this object.
    break;
    */
    //          default:
    // todo - throw exception("invalid format");
    //                  break;
    //  }
}

HfcWeigherStatus::HfcWeigherStatus(const HfcWeigherStatus *hws) : WeigherStatus()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    init();

    setRawCounts(hws->getRawCounts());
    setCounts(hws->getBaseCounts());
    setStatusBits(hws->getStatusBits());
    setScaleFactor(hws->getScaleFactor());
    setDualRangeEnabled(hws->isDualRangeEnabled());
    setDualRangeDirection(hws->getDualRangeDirection());
    setTiltX(hws->getTiltX());
    setTiltY(hws->getTiltY());

    hws->isPeripheralComm() ? setPeripheralComm() : clearPeripheralComm();
    hws->isManagerDeleted() ? setManagerDeleted() : clearManagerDeleted();
}

HfcWeigherStatus::~HfcWeigherStatus()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}

void HfcWeigherStatus::init()
{
    weightDisplay = "";
}


bool HfcWeigherStatus::isWeightValidForDisplaying() const
{
	return ((isStableWeight() || isSmallMotion()) && !isWeigherInError()) ? true : false;
}

bool HfcWeigherStatus::isStableWeight() const
{
	//Debug::trace("Weigher", HobString("isStableWeight->isPositiveNonZeroStableWeight(): ") + HobString(Bool::toString(isPositiveNonZeroStableWeight())));
	//Debug::trace("Weigher", HobString("isStableWeight->isZeroStableWeight(): ") + HobString(Bool::toString(isZeroStableWeight())));
	//Debug::trace("Weigher", HobString("isStableWeight->isPeripheralComm(): ") + HobString(Bool::toString(isPeripheralComm())));
	//Debug::trace("Weigher", HobString("isStableWeight->!isManagerDeleted(): ") + HobString(Bool::toString(!isManagerDeleted())));

    return ((isPositiveNonZeroStableWeight() || isZeroStableWeight()) && isPeripheralComm() && !isManagerDeleted()) ? true : false;
}

bool HfcWeigherStatus::isWeigherInError() const
{
    /* The basis for determining if weigher is in error was based on the ERROR_STATES macro
     * defined in wgt_mgr.h (Ultima), and the code in "update_state (LONG weight, FLAG status)"
     * from wgt_proc.c (Ultima).
     */
    return (isOverGrossWeightLimit() ||
            isNegativeGrossWeight() ||
            isZeroOutOfMaintZone() ||
            isCalibFailed() ||
            isEepReadFailed() ||
		    isValMaxTiltReached() ||
		    isValMaxEepAcclCommFailure() ||
		    isValMaxEepCommFailure() ||
            isWeigherConfigOutOfRange()) ? true : false;
}

const HobString &HfcWeigherStatus::getWeightDisplay() const
{
    return weightDisplay;
}

HobWeight HfcWeigherStatus::getWeight() const
{
	HobWeight w( WeigherStatus::getWeight() );
    return w;
}



/***************************************************************************************************************************
****************************************************************************************************************************
*****
*****   BEGIN
*****
*****   The following code is considered to be sealable by Weights & Measures; it determines first and final weight/price.
*****   If you change it, you MUST change the version number defined in evolution.h
*****
****************************************************************************************************************************
***************************************************************************************************************************/

void HfcWeigherStatus::determineDisplayValues() const
{
    weightDisplay = "";
    if (isWeigherInError())
    {
        weightDisplay = "----";
    }
    /* commented out because blanking the display is the default value initially set in weightDisplay.
       else if (isLargeMotion() || isScaleWarmup())
       {
    // blank the weight being displayed.
    weightDisplay = " ";
    }
    */
	else if (isPeripheralComm() && !isManagerDeleted())
	{
		if (isZeroStableWeight())
		{
			if (isDualRangeEnabled() && getScaleFactor() == 1)
			{
				Debug::trace("Weigher", HobString("HWS:  determineDisplayValues(1):   DualRangeDown"));
				Configuration::getInstance()->formatWeight((HobWeight)0, weightDisplay, true, DualRangeModeDown);
			}
			else
				Configuration::getInstance()->formatWeight((HobWeight)0, weightDisplay);
		}
		else if (isStableWeight() || isSmallMotion())
		{
			if (isDualRangeEnabled() && (getScaleFactor() == 1 || (MetSig::isScaleAvoir() && getDualRangeDirection() == DualRangeDown)))
			{
				Debug::trace("Weigher", HobString("HWS:  determineDisplayValues(2):   DualRangeDown"));
				Configuration::getInstance()->formatWeight(getWeight(), weightDisplay, true, DualRangeModeDown);
			}
			else
				Configuration::getInstance()->formatWeight(getWeight(), weightDisplay);
		}
	}
}

const char *HfcWeigherStatus::serialize(int format) const
{
    HfcObject::serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    switch (format)
    {
        case InternalFormat:
            {
                const char *data = WeigherStatus::serialize();
                int datalen = WeigherStatus::getSerializedLength();
                HfcObject::serializedData.assign(data, datalen);
                break;
            }

        case BrowserFormat:
            {
                determineDisplayValues();

                /* The basis for what to write to the display was based on the code in
                 * "update_state (LONG weight, FLAG status)" from wgt_proc.c (Ultima).
                 */
                HfcObject::serializedData = Integer::toString((int)getDeviceType());
                HfcObject::serializedData.append(BrowserFormatSeparator);
                HfcObject::serializedData.append(Integer::toString((int)getEventType()));
                HfcObject::serializedData.append(BrowserFormatSeparator);
                HobString stringForQuoting(weightDisplay);
                stringForQuoting.quoteForCSV();
                HfcObject::serializedData.append(stringForQuoting);
                HfcObject::serializedData.append(BrowserFormatSeparator);
                HfcObject::serializedData.append(Bool::toString(isZeroStableWeight() && !isWeigherInError() && isPeripheralComm() && !isManagerDeleted()));
                HfcObject::serializedData.append(BrowserFormatSeparator);
                std::string dualRangeInd = "";
                if (MetSig::isScaleMetric())
                {
                    dualRangeInd = (getScaleFactor() == 5) ? "5g" : "2g";
                    if (isDualRangeEnabled() && getDualRangeDirection() == DualRangeDown)
                        dualRangeInd = "5g";
                }
                HfcObject::serializedData.append(dualRangeInd);
                HfcObject::serializedData.append(BrowserFormatSeparator);
                HfcObject::serializedData.append(Integer::toString(getTiltX()));
                HfcObject::serializedData.append(BrowserFormatSeparator);
                HfcObject::serializedData.append(Integer::toString(getTiltY()));
                break;
            }


        case WeightCountsFormat:
            {
                long zeroedCounts = getRelativeCounts(); // / 10;
                long nonZeroedCounts = getRawCounts(); // / 10;

                HfcObject::serializedData = Integer::toString((int)getDeviceType());
                HfcObject::serializedData.append(BrowserFormatSeparator);

                HfcObject::serializedData.append(Integer::toString(WgStatusCounts));
                HfcObject::serializedData.append(BrowserFormatSeparator);

                HfcObject::serializedData.append(Long::toString(nonZeroedCounts));
                HfcObject::serializedData.append(BrowserFormatSeparator);

                HfcObject::serializedData.append(Long::toString(zeroedCounts));
                HfcObject::serializedData.append(BrowserFormatSeparator);

                HobString weight(" ");
                //Configuration::getInstance()->formatWeight(zeroedCounts, weight, PrecisionPlusOne);
                double newWeight = (double) zeroedCounts;
                weight = Double::toString(newWeight / 10000, 4, 4);
                weight.quoteForCSV();
                HfcObject::serializedData.append(weight);
                HfcObject::serializedData.append(BrowserFormatSeparator);

                HfcObject::serializedData.append(Integer::toString(getTiltX()));
                HfcObject::serializedData.append(BrowserFormatSeparator);

                HfcObject::serializedData.append(Integer::toString(getTiltY()));
                break;
            }

        default:
            HfcObject::serializedData = HobString("HfcWeigherStatus::serialize - unknown format:") + HobString(Integer::toString(format));
            // todo - throw exception("invalid format");
            break;
    }
    return HfcObject::serializedData.c_str();
}

/***************************************************************************************************************************
****************************************************************************************************************************
*****
*****   END
*****
*****   The above code is considered to be sealable by Weights & Measures; it determines first and final weight/price.
*****   If you change it, you MUST change the version number defined in evolution.h
*****
****************************************************************************************************************************
***************************************************************************************************************************/



bool HfcWeigherStatus::operator==(HfcWeigherStatus &hws) const
{
    return (getRawCounts() == hws.getRawCounts() &&
            getCounts() == hws.getCounts() &&
            getStatusBits() == hws.getStatusBits() &&
            isWeigherConfigOutOfRange() == hws.isWeigherConfigOutOfRange()) ? true : false;
}
