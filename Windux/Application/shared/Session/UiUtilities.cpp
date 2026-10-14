//! UiUtilities.cpp
/*! UiUtilities is a replacement class for multiple script files the once use to exist in the web based models of scales.
 *  Currently the list of files converted from Javascript to this file consists of operateScaleScripts. Other files still to
 *  be translated include operateProgrammableFields, operateRunScreenDaysDatesScripts, and operateSaveProductInfo. This class
 *  may eventually include methods from MADP, domOperate, domSuper, and domService.
 *  Autor: Joe E. Crew
 */

#include "UiUtilities.h"

#include "Session.h"
#include "ScaleConfiguration.h"
#include <filesystem.h>

#include "Messages.h"

#include <QApplication>
#include "logmanager.h"
#if (0x050000 <= QT_VERSION)
#include <QSound>
#endif

//! UiUtilities()
/*!
    UiUtilities() Constructor
    \  params:  session - pointer to session
    \  returns: nothing
    \  Bugs\Issues: None
 */
UiUtilities::UiUtilities( Session *session ) :
    isPlaying(false),
    session_( session ),
    scaleConfig_( session->scaleConfig ),
    madpScaleConfig( scaleConfig_->scaleConfig ),
    internalConfig( scaleConfig_->internalConfig ),
    currencySettings( scaleConfig_->currencySettings ),
    peripheralList( scaleConfig_->peripheralList ),
    currentPLU( scaleConfig_->currentPLU ),
    defaultCurrentPLU( scaleConfig_->defaultCurrentPLU ),
    wrTimingsTestResults(scaleConfig_->wrTimingsTestResults),
    specialCharacter(""),
    noChecks(0)
{
    // map of features to key (string)
    features[SCALE_FPP] = "SCALE_FPP";
    features[SCALE_FULLSCREEN_FLASHKEYS] = "SCALE_FULLSCREEN_FLASHKEYS";
    features[SCALE_MEAL_BUILDER] = "SCALE_MEAL_BUILDER";
    features[SCALE_NOW_SERVING] = "SCALE_NOW_SERVING";
    features[SCALE_OFFICE_VIEWER] = "SCALE_OFFICE_VIEWER";
    features[SCALE_PDF_VIEWER] = "SCALE_PDF_VIEWER";
    features[SCALE_WEB_BROWSER] = "SCALE_WEB_BROWSER";
    features[SCALE_TRANS_GCD] = "SCALE_TRANS_GCD";
    features[SCALE_CAT3_AUDITTRAIL] = "SCALE_CAT3_AUDITTRAIL";
    features[SCALE_REMOTE_PRINTING] = "SCALE_REMOTE_PRINTING";
	features[SCALE_PICK5] = "SCALE_PICK5";
    features[SCALE_CODE_CHECKER] = "SCALE_CODE_CHECKER";
    features[SCALE_DIGIMARC] = "SCALE_DIGIMARC";
    features[SCALE_HANDSFREEDEVICE] = "SCALE_HANDSFREEDEVICE";
    features[SCALE_INTERNATIONAL_ROUNDING] = "SCALE_INTERNATIONAL_ROUNDING";
    features[SCALE_ENHANCED_CENTERSCREEN] = "SCALE_ENHANCED_CENTERSCREEN";
    features[SCALE_ONLINE_ORDERING] = "SCALE_ONLINE_ORDERING";

    // from ScaleScripts.asp
    format3EN << "error" << "January" << "February" << "March" << "April" << "May" << "June" << "July" << "August" << "September" << "October" << "November" << "December";
    format3ES << "error" << "Enero" << "Febrero" << "Marzo" << "Abril" << "Mayo" << "Junio" << "Julio" << "Agosto" << "Septiembre" << "Octubre" << "Noviembre" << "Diciembre";
    format3FR << "error" << "Janvier" << "Février" << "Mars" << "Avril" << "Mai" << "Juin" << "Juillet" << "Août" << "Septembre" << "Octobre" << "Novembre" << "Décembre";

    format3ENAbbr << "error" << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" << "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";
    format3ESAbbr << "error" << "Ene" << "Feb" << "Mar" << "Abr" << "May" << "Jun" << "Jul" << "Ago" << "Sep" << "Oct" << "Nov" << "Dic";
    format3FRAbbr << "error" << "Jan" << "Fév" << "Mar" << "Avr" << "Mai" << "Jun" << "Jul" << "Aoû" << "Sep" << "Oct" << "Nov" << "Déc";

    theScaleModel = madpScaleConfig.scaleModel().toInt();

}

//! updateDateTime()
/*!
    updateDateTime() updates the date and time on the currently displayed form
    \  params:  none
    \  returns: nothing
    \  Bugs\Issues: None
 */
QString UiUtilities::updateDateTime() const
{
    DomDateAndTime& dateAndTime = scaleConfig_->dateAndTime;

    QString displayHour = dateAndTime.hobHourRaw();
    QString displayMin = dateAndTime.hobMinuteRaw();
    QString displayAmPm = "";
    int displayMonth = dateAndTime.hobMonth();
    QString displayDay   = dateAndTime.hobDayRaw();
    QString displayYear  = dateAndTime.hobYearRaw();

    // check for empty message
    if (( displayYear.isEmpty() ) &&( displayHour.isEmpty() ) && ( displayMin.isEmpty() ))
        return QString();

    if ( 1 == displayHour.length() )
        displayHour = "0" + displayHour;

    if ( 1 == displayMin.length() )
        displayMin = "0" + displayMin;

    if ( !madpScaleConfig.timeFormat() )
        displayAmPm = dateAndTime.hobAMPM();

    return getFullFormattedDate( displayMonth, displayDay, displayYear, displayHour, displayMin, displayAmPm );
}

//! getFullFormattedDate()
/*!
    getFullFormattedDate() formats the date string based upon country and configuration settings
    \  param:  displayMonth - the current month
    \  param:  displayDay - the current date
    \  param:  displayYear - the current year
    \  param:  displayHour - the current hour
    \  param:  displayMin - the current minutes
    \  param:  displayAmPm - AM or PM
    \  returns: The properly formatted date string
    \  Bugs\Issues: None
 */
QString UiUtilities::getFullFormattedDate(
    int displayMonth,
    const QString& displayDay,
    const QString& displayYear,
    const QString& displayHour,
    const QString& displayMin,
    const QString& displayAmPm ) const
{
    QString retVal = "";
    QString ext;

    QVector<QString> localArray;

    if ( 1 == scaleConfig_->currentLanguage )
        ext = madpScaleConfig.languageCode();
    else
        ext = madpScaleConfig.secLanguageCode();

    if ( "en" == ext )
        localArray = format3ENAbbr;
    else if ( "es" == ext )
        localArray = format3ESAbbr;
    else if ( "fr" == ext )
        localArray = format3FRAbbr;
    else
    {
        return retVal;
    }

    switch ( madpScaleConfig.dateStyle() )
    {
    case 0:
    case 1:
    case 3:
        retVal = localArray[displayMonth] + " " + displayDay + ", " +  displayYear + " " + displayHour + ":" + displayMin + " " + displayAmPm;
        break;
    case 2:
    case 7:
        retVal = displayYear + " " + localArray[displayMonth] + ", " + displayDay + " " + displayHour + ":" + displayMin + " " + displayAmPm;
        break;
    case 5:
    case 6:
        retVal = displayDay + " " + localArray[displayMonth] + ", " + displayYear + " " + displayHour + ":" + displayMin + " " + displayAmPm;
        break;
    default:
        retVal = localArray[displayMonth] + " " + displayDay + ", " +  displayYear + " " + displayHour + ":" + displayMin + " " + displayAmPm;
        break;
    }

    return retVal;
}

//! youGottaSetTheSeparator()
/*!
    youGottaSetTheSeparator() formats a float value (weight, total price, unit price, etc) with a decimal
    or comma based upon currency settings
    \  param:  weight, but it could be any float value
    \  returns: The properly formatted float value with a comma or decimal character
    \  Bugs\Issues: None
 */
QString UiUtilities::youGottaSetTheSeparator( QString& weight ) const
{
    if ( "," == currencySettings.unitSeparator() )
       weight.replace( ".", "," );

    return weight;
}

//! youGottaSetTheSeparator()
/*!
    youGottaSetTheSeparator() formats a float value (weight, total price, unit price, etc) with a decimal
    or comma based upon currency settings
    \  param:  weight, but it could be any float value
    \  returns: The properly formatted float value with a comma or decimal character
    \  Bugs\Issues: None
 */
QString UiUtilities::youGottaResetTheSeparator( QString& field ) const
{
    if ( "," == currencySettings.unitSeparator() && field.contains(",") )
       field.replace( ",", "." );

    return field;
}

//! stripFontChanges()
/*!
    stripFontChanges() takes a font change enabled text string (description, etc) and strips
    the font change sequences so they are not shown when displaying the text
    \  param:  strToStrip - The text string containing font change sequences
    \  returns: The text string sans font change sequences
    \  Bugs\Issues: None
 */
QString UiUtilities::stripFontChanges( const QString& strToStrip ) const
{
    int pos = -1;
    QString tmpStr = strToStrip;
    QString stripStr = "";

    QString escSeq  = "\x001B";
    QString escSeq2 = "\x001B\x001B";
    QString DQ = "\x0022\x0022";

    tmpStr = tmpStr.replace(DQ, "\"");                               // replace all whitespace, \t\v\n\f\r  with a space
    tmpStr = tmpStr.simplified();                                   // replace double quote with single quote

    pos = tmpStr.indexOf(escSeq2);                                  // find first double escape

    if (pos >= 0)
    {
        while(pos >= 0)
        {
            if(pos > 0)
                stripStr += tmpStr.mid(0,pos);                      // grabs the str up to the but before the <ESC><ESC>

            tmpStr = tmpStr.mid(pos + 2,tmpStr.length());           // strip a dblESC, so +8
            pos = tmpStr.indexOf(escSeq);
            tmpStr = tmpStr.mid(pos + 1,tmpStr.length());           // find next double escape
            pos = tmpStr.indexOf(escSeq2);
        }

        stripStr += tmpStr;
        tmpStr = stripStr;
    }

    return tmpStr;
}

//! setupCapacityString()
/*!
    setupCapacityString() builds the current capacity string based on scale model and configuration settings
    \  param:  none
    \  returns: The built capacity string
    \  Bugs\Issues: None
 */
QString UiUtilities::setupCapacityString( const QString& capacity, QString& range, QString& dualRange ) const
{
    QString weigherCapacity = capacity;

    // no weigher attached
    if ( !madpScaleConfig.weigherAttached().toInt() )
        return weigherCapacity;

    // the weigher capacity string is kept locally in the operate dom
    DomWeigherConfigRead& weigherConfigRead = scaleConfig_->weigherConfigRead;

    float myMaxWeight = weigherConfigRead.weigherMaximumWt().toFloat();
    int myDualRangeLimit = weigherConfigRead.dualRangeLimit();
    //int myWeigherModel = weigherConfigRead.weigherModel();
    QString unitEndStr = (madpScaleConfig.isScaleMetric() ? "kg" : "lb");

    // we will not set up the weigherCapacity string unless we have a weigherMaximumWt value
    if ( myMaxWeight <= 0.0f )
        return weigherCapacity;

    int myScaleFactor = weigherConfigRead.weigherScaleFactor();

    QString myEMin;
    QString myEMinDR;

    dualRange = "";

    if ( madpScaleConfig.isScaleMetric() )
    {
        // Metric
        if ( 2 == myScaleFactor )
        {
            myEMinDR = "0.002";
            myEMin = "0.005";
        }
        else if ( 5 == myScaleFactor )
        {
            myEMinDR = "";
            myEMin = "0.005";
        }
    }
    else
    {
        // Avoir
        if ( 5 == myScaleFactor )
        {
            myEMinDR = "0.005";
            myEMin = "0.01";
        }
        else if ( 1 == myScaleFactor )
        {
            myEMinDR = "";
            myEMin = "0.01";
        }
    }

    if ( myEMinDR.compare("") != 0 )
        dualRange += QString::number( myDualRangeLimit ) + " x " + youGottaSetTheSeparator( myEMinDR ) + " " + unitEndStr;

    range = QString::number( myMaxWeight ) + " x " + youGottaSetTheSeparator( myEMin )  + " " + unitEndStr;

    weigherCapacity += " ";
    weigherCapacity += range;

    if ( dualRange.compare("") != 0 )
    {
        weigherCapacity += ", ";
        weigherCapacity += dualRange;
    }

    return weigherCapacity;
}

//! isDualRangeCapable()
/*!
    isDualRangeCapable() test if scale is dual range capable
    \  param:  none
    \  returns: true or false
 */
bool UiUtilities::isDualRangeCapable() const
{
    // the weigher capacity string is kept locally in the operate dom
    const DomWeigherConfigRead *weigherConfigRead( &scaleConfig_->weigherConfigRead );

    return ( weigherConfigRead->isDualRangeCapable() );
}

//! rezeroWeigher()
/*!
    rezeroWeigher() sends Rezero Weigher request.
    \  param:  none
    \  returns: nothing
    \  Bugs\Issues: QT TODO, needs to be further implemented.
 */
void UiUtilities::rezeroWeigher()
{
    session_->send( Weigher, WgRezero );             // 1,8
}

//! clearOperator()
/*!
    clearOperator() clears the operator data off the screen.
    \  param:  none
    \  returns: nothing
    \  Bugs\Issues:  We need the actual access to the operatorID field, which isnt created at this point. And then an update to it.
 */
void UiUtilities::clearOperator()
{
    //parseInt(scaleConfig.isRFIDHWPresent.value, 10) == 0 && parent.common_frame.document.getElementById("operatorID") != null)
    // && Operator Field Name != null


    if ( madpScaleConfig.isRFIDHWPresent() == 0 )
    {
        //Object labelElement = parent.common_frame.document.getElementById("operatorID");
        //element.text = "";
    }
}

//! clearPLUData()
/*!
    clearPLUData clear the entire plu data in the DOM, resets all fields to default values
    \  param: none
    \  returns: nothing
*/
void UiUtilities::clearPLUData()
{
    internalConfig.selfservInvalidPlu = false;

    currentPLU.itemNumber()  			= "";
    currentPLU.productTypeRaw()  		= "0";
    currentPLU.description()  			= " ";
    currentPLU.barcodeTypeRaw()  		= "0";
    currentPLU.barcodeFieldsRaw()  		= "0";
    currentPLU.numberSystem()  			= "2";
    currentPLU.productNumber()  		= "";
    currentPLU.vendorNumber()  			= "";
    currentPLU.eanCode() 	 			= "";
    currentPLU.byCountRaw()  			= "0";
    currentPLU.exceptionPrice()  		= "0.00";
    currentPLU.unitPrice()  			= "0.00";
    currentPLU.discountType()  		    = "0";
    currentPLU.discountValue1()  		= "0.00";
    currentPLU.discountValue2()  		= "0.00";
    currentPLU.discountValue3()  		= "0.00";
    currentPLU.discountLabel1()  		= "";
    currentPLU.discountLabel2()  		= "";
    currentPLU.discountLabel3()  		= "";
    currentPLU.tare()  					= "";               // Just blank it since we dont know 0.00 or 0.000
    currentPLU.proportionalTare()  		= "0.0";
    currentPLU.fixedWeight()  			= "0";
    currentPLU.shelfLifeDaysRaw()  		= "0";
    currentPLU.shelfLifeHoursRaw()  	= "0";
    currentPLU.productLifeDaysRaw() 	= "0";
    currentPLU.productLifeHoursRaw()	= "0";
    currentPLU.priceModifier()  		= "1";
    currentPLU.reportingClass()  		= "999999";
    currentPLU.primaryLabelTypeRaw()	= "0";
    currentPLU.primaryLabelRotationRaw()= "0";
    currentPLU.secondLabelTypeRaw()  	= "0";
    currentPLU.secondLabelRotationRaw() = "0";
    currentPLU.thirdLabelTypeRaw()  	= "0";
    currentPLU.thirdLabelRotationRaw()  = "0";
    currentPLU.backDateDaysRaw()  		= "";
    currentPLU.points()  				= "0";
    currentPLU.graphic1Name()  			= "None";
    currentPLU.graphic2Name()  			= "None";
    currentPLU.graphic3Name()  			= "None";
    currentPLU.graphic4Name()  			= "None";
    currentPLU.primaryLabelTypeWidth()  = "0";
    currentPLU.primaryLabelTypeLength() = "0";
    currentPLU.secondLabelTypeWidth()  	= "0";
    currentPLU.secondLabelTypeLength()  = "0";
    currentPLU.thirdLabelTypeWidth()  	= "0";
    currentPLU.thirdLabelTypeLength()  	= "0";
    currentPLU.forcedByCountRaw()  		= "0";
    currentPLU.forcedTareRaw()  		= "0";
    currentPLU.fractionalPriceUnitRaw() = "0";
    currentPLU.poundsForAmountRaw()  	= "0";
    currentPLU.forcedUnitPriceRaw()  	= "0";
    currentPLU.tareType()  				= "0";
    currentPLU.applySecurityRaw()  		= "0";
    currentPLU.dontPrintPackedOnRaw()  	= "0";
    currentPLU.dontPrintSellByRaw()  	= "0";
    currentPLU.dontPrintBestBeforeRaw() = "0";
    currentPLU.primaryLabelPlacementRaw()  = "2";
    currentPLU.secondLabelPlacementRaw()  	= "2";
    currentPLU.thirdLabelPlacementRaw()  	= "2";
    currentPLU.fractionalPrice()  		= "0";
    currentPLU.forcedFractionalPriceRaw() = "0";
    currentPLU.poundsForPrice()  		= "0";
    currentPLU.forcedPoundsForRaw()  	= "0";
    currentPLU.sellByDateRaw()  		= "";
    currentPLU.sellByTime()  			= "";
    currentPLU.bestBeforeDateRaw()  	= "";
    currentPLU.bestBeforeTime()  		= "";
    currentPLU.hasNutrifacts()  		= "0";
    currentPLU.intakeMode()  			= "0";
    currentPLU.hasFreqShopperRaw()  	= "0";
    currentPLU.productGraphic()  		= "None";
    currentPLU.productName1()  			= "";
    currentPLU.productName2()  			= "";
    currentPLU.categoryNumber()  		= "";
    currentPLU.specialType()  			= "";
    currentPLU.coolCategoryNumRaw()  	= "";
    currentPLU.coolPretextNumRaw()  	= "";
    currentPLU.coolDefaultTextNumRaw()  = "";
    currentPLU.coolTextNumRaw()  		= "";
    currentPLU.coolShortListNumRaw()  	= "";
    currentPLU.forcedCoolRaw()  		= "0";
    currentPLU.coolTrackingTextRaw()  	= "";
    currentPLU.coolRequiredRaw()  		= "0";
    currentPLU.keyboardNetWeight()  	= "";
    currentPLU.acceptKbdNetWeightRaw()  = "0";
    currentPLU.numOfLabelsForcedRaw()  	= "0";
    currentPLU.numOfLabelsRaw()  		= "0";
    currentPLU.productionPlanningTypeRaw() = "0";
    currentPLU.forcedBackDateDaysRaw()  = "0";
    currentPLU.expirationDaysForcedRaw()= "0";
    currentPLU.expirationDaysRaw()  	= "";
    currentPLU.eanFormatString()  		= "";
    currentPLU.ean128CFormatString()  	= "";
	currentPLU.gs1DatabarGTIN()			= "";
	currentPLU.gs1DatabarAI()			= "";
    currentPLU.backdateDateRaw()  		= "0";
    currentPLU.portions()  				= "0";
    currentPLU.showPortions()  			= "0";
    currentPLU.sideItemsGroupNumRaw()   = "0";
    currentPLU.mealSideItemsFreeRaw()   = "0";
    currentPLU.sideItemsIncludedInMealRaw() = "0";
    currentPLU.meatSpecificationSheetRaw() = "0";
    currentPLU.tareB()  				= "";
	currentPLU.qrCodeFormatString()		= "";
	currentPLU.dataMatrixFormatString()	= "";
	currentPLU.minWeight()				= "";
	currentPLU.maxWeight()				= "";
    currentPLU.minPrice()				= "";
	currentPLU.maxPrice()				= "";
	currentPLU.primaryDefaultLabelTypeRaw() = ""; 	
	currentPLU.secondDefaultLabelTypeRaw() = ""; 	
	currentPLU.thirdDefaultLabelTypeRaw() = ""; 		
	currentPLU.primaryManualLabelTypeRaw() = ""; 	
	currentPLU.secondManualLabelTypeRaw() = ""; 		
	currentPLU.thirdManualLabelTypeRaw() = ""; 		
	currentPLU.primaryPrepackLabelTypeRaw() = ""; 	
	currentPLU.secondPrepackLabelTypeRaw() = ""; 	
	currentPLU.thirdPrepackLabelTypeRaw() = ""; 		
	currentPLU.primaryProdEntryLabelTypeRaw() = "";
	currentPLU.secondProdEntryLabelTypeRaw() = ""; 
	currentPLU.thirdProdEntryLabelTypeRaw() = ""; 	
	currentPLU.primarySelfServLabelTypeRaw() = ""; 
	currentPLU.secondSelfServLabelTypeRaw() = ""; 	
	currentPLU.thirdSelfServLabelTypeRaw() = ""; 	
	currentPLU.primaryPick5LabelTypeRaw() = "";		
	currentPLU.secondPick5LabelTypeRaw() = ""; 		
	currentPLU.thirdPick5LabelTypeRaw() = "";
	currentPLU.primaryOnlineOrderingLabelTypeRaw() = "";		
	currentPLU.secondOnlineOrderingLabelTypeRaw() = ""; 		
	currentPLU.thirdOnlineOrderingLabelTypeRaw() = ""; 	
    currentPLU.shelfLifeMinutesRaw()   = "0";
    currentPLU.productLifeMinutesRaw() = "0";
	currentPLU.labelStockRotation1Raw() = "";		
	currentPLU.labelStockRotation2Raw() = ""; 		
	currentPLU.labelStockRotation3Raw() = "";
    currentPLU.EANprefix()  			= "";
}

// SOF-5067: For FSS scales, automatically enable the ability to print without a GCD
void UiUtilities::checkAllowPrintingNoGcd()
{
    LOG_CLASS_INFO("UiUtilities", "setAllowPrintingNoGcd");

    // if the config value isn't already set (i.e on the very first boot), set it
    if(scaleConfig_->scaleConfig.printWithoutGcdValue() != "1")
    {
        const QString scaleName_ = session_->scaleConfig->scaleConfig.scaleName();

        if (scaleName_.startsWith("FS"))
        {
            LOG_DEBUG("Enabling allow print without customer display.", NULL);

            QString param;
            param = "," + QString::number( fieldOffsetCfgGCDPrintWithoutCustomerDisplay );
            param += ",\"" + QString("1" ) + "\"";
            param += "," + QString::number(0);
            session_->send( ConfigurationDev, CfgSaveSingleField, param);
        }
        else
        {
            LOG_TRACE("Scale is not of FSS Model. Not enabling allow print without customer display.", NULL);
        }
    }
    else
    {
        LOG_TRACE("Allow print without customer display already set", NULL);
    }
}

//! scaleName()
/*!
    Makes a boop sound when a user scans a GS1 barcode with a scanner
    \  param: none
    \  returns: scale name
*/
void UiUtilities::boop()
{
    LOG_CLASS_INFO("UiUtilities", "boop");
    LOG_DEBUG("Playing Beep.", NULL);
#if defined( Q_OS_LINUX)
    isPlaying = true;
    QProcess process;
    QString path = "/usr/local/hobart/audio/scanbeep.wav";
    process.start("aplay", (QStringList() << path));
    process.waitForFinished();
    QTimer::singleShot(1000, this, &UiUtilities::resetPlaying);

#else
    QSound::play("C:/Program Files/Hobart/Audio/scanbeep.wav");
#endif
}

//! scaleName()
/*!
    retrieve scale name
    \  param: none
    \  returns: scale name
*/
QString UiUtilities::scaleName() const
{
    QString result;

    scaleNameEx( &result );

    return result;
}

//! scaleNameEx()
/*!
    retrieve scale name (extended version)
    \  param: scale name
    \  returns: scale flags
*/
scaleFlags UiUtilities::scaleNameEx( QString *name ) const
{
    const QString n( scaleConfig_->scaleConfig.scaleName() );

    scaleFlags nf( (smtPrepack == scaleModelEx( NULL )) ? sfWrap : sfWeigh );
    QString ns;

    // wrapper
    if ( sfWrap & nf )
    {
        /// TODO: what is the HTi Access going to be called?
        //ns = QApplication::tr( "Access" );

        //Trim the scaleName
        ns = n.trimmed();

        // strip out quotes if they exist
        if ( 2 <= ns.length() )
            if (( '\"' == ns.at( 0 ) ) && ( '\"' == ns.at( ns.length() - 1 ) ))
                ns = ns.mid( 1, ns.length() - 2 );

        // the scale name should be in the form EPCP-5HTi...
        QStringList l( ns.split( '-' ) );

        // check for linux/windows
        // the middle digit is either an '5' or '6' which represents OS
        if ( 2 == l.size() )
        {
            // os flags
            if ( l[1].contains( "5" ) )
                nf = (scaleFlags)(nf | sfLinux);
            else if ( l[1].contains( "6" ) )
                nf = (scaleFlags)(nf | sfWindows);
        }

        // the HTs scale (the cheap one) is a reduced feature set
        if ( n.contains( "hts", Qt::CaseInsensitive ) )
            nf = (scaleFlags)(nf | sfReducedFeatureSet);
    }

    // counter scale
    else if ( sfWeigh & nf )
    {
        // trim the string
        ns = n.trimmed();

        // strip out quotes if they exist
        if ( 2 <= ns.length() )
            if (( '\"' == ns.at( 0 ) ) && ( '\"' == ns.at( ns.length() - 1 ) ))
                ns = ns.mid( 1, ns.length() - 2 );

        // the scale name should be in the form HTi-7LS...
        QStringList l( ns.split( '-' ) );

        // check for linux/windows
        // the middle digit is either an 'L' or 'W' which represents OS
        if ( 2 == l.size() )
        {
            // os flags
            if ( l[1].contains( "L" ) )
                nf = (scaleFlags)(nf | sfLinux);
            else if ( l[1].contains( "W" ) )
                nf = (scaleFlags)(nf | sfWindows);
        }

        // the HTs scale (the cheap one) is a reduced feature set
        if ( n.contains( "hts", Qt::CaseInsensitive ) )
            nf = (scaleFlags)(nf | sfReducedFeatureSet);
    }

    // give back name in human readable form (if requested)
    if ( name )
    {
        if ( ns.isEmpty() )
            *name = QApplication::tr( "Unknown" );
        else
            *name = QApplication::tr( "Hobart" ) + " " + ns;
    }

    return nf;
}

//! scaleModel()
/*!
    retrieve scale model
    \  param: none
    \  returns: scale model
*/
QString UiUtilities::scaleModel() const
{
    QString result;

    scaleModelEx( &result );

    return result;
}

//! scaleModelEx()
/*!
    retrieve scale model (extended version)
    \  param: scale model
    \  returns: scale model flags
*/
scaleModelType UiUtilities::scaleModelEx( QString *model ) const
{
    const QString m( scaleConfig_->scaleConfig.scaleModel() );

    scaleModelType mt( smtUnknown );
    QString ms;

    if ( !m.isEmpty() )
    {
        // wrapper
        if ( 2 == m.toInt() )
        {
            mt = smtPrepack;
            ms = QApplication::tr( "Prepack" );
        }

        // counter scale
        else
        {
            // Note:
            // The concept of a "self-service" model has been eliminated starting in HTi... self
            // service is now a mode of operation.
            mt = smtServiceCounter;
            ms = (( 1 == m.toInt() ) ? QApplication::tr( "Self Service" ) : QApplication::tr( "Service Counter" ));
        }
    }

    // give back model in human readable form (if requested)
    if ( model )
        *model = ms;

    return mt;
}

//! scaleCopyright()
/*!
    retrieve scale copyright
    \  param: none
    \  returns: scale copyright
*/
QString UiUtilities::scaleCopyright() const
{
#ifdef ENCODING_CHECK		//	This worked for Windows but not Linux. Test this new change under Windows soon.
    // Changed back to QStringLiteral ... Todd's change did not work.
    return QStringLiteral( "© " ) + QApplication::tr( "Hobart, A division of ITW Food Equipment Group LLC 2013" );
#else
    QChar copyright(0x00a9);
    QString copyrightStr = copyright + QApplication::tr( " Hobart, A division of ITW Food Equipment Group LLC 2013" );
    return copyrightStr;
#endif
}

//! isFeatureLicensed()
/*!
    check if feature is licensed for scale
    \  param: feature to check
    \  returns: true or false
*/
bool UiUtilities::isFeatureLicensed( scaleFeature f, bool blocking ) const
{
    // unknown feature
    if ( !features.contains( f ) )
        return false;

    // these features are auto included (they do not need to be purchased) in full featured product
    if (( SCALE_FPP == f ) ||
        ( SCALE_FULLSCREEN_FLASHKEYS == f ) ||
        ( SCALE_WEB_BROWSER == f ) ||
        ( SCALE_TRANS_GCD == f ))
    {
        scaleFlags flags( scaleNameEx( NULL ) );

        // full featured scale
        if ( !(sfReducedFeatureSet & flags) )
            return true;
    }

    // check for feature
    bool installed( scaleConfig_->getFeatureInstalled( features[f] ) );

    if (( installed ) || ( !blocking ))
        return installed;

    // feature not installed and we are blocking... send off request...
    return scaleConfig_->sendFeatureRequest( features[f] );
}

QString UiUtilities::ascertainHomepage() const
{
    return scaleConfig_->getConfiguredHomepage();
}

// PJR-145: Wrap-only
bool UiUtilities::isWrapOnly()
{
    bool rval = false;

    if( ( session_->isPrepack() ) &&
        ( madpScaleConfig.scaleName().trimmed().left( 3 ).toUpper() == "NGS" ) )

        rval = true;

    return rval;
}

//SOF-5319: see if the current build has a special configuration
QString UiUtilities::checkSpecialConfig()
{
    LOG_CLASS_INFO("UiUtilities", "checkSpecialConfig");
    // if the character has already been found or it has tried to find it more than once
    if(!specialCharacter.isEmpty() || noChecks >= 1)
    {
        return specialCharacter;
    }
    else
    {

        const QString fileName = "/etc/hobart/customver.dat";
        QFile file( fileName );

        // open file and read any characters
        if ( file.open( QIODevice::ReadOnly | QIODevice::Text) )
        {
            QTextStream text(&file);
            while( !text.atEnd() )
            {
                QString line = text.readLine();
                QStringList chars = line.split(" ");

                // ignore if entire line is white space
                if(chars.size() > 0  && line.trimmed()!="" )
                {
                    for (int i = 0; i < chars.size(); i++)
                    {
                       specialCharacter+=chars[i] + " ";
                    }
                }
            }
            file.close();
            noChecks++;
            return specialCharacter;
        }
        noChecks++;
        return "";
    }
}

void UiUtilities::resetPlaying()
{
    isPlaying = false;
}



