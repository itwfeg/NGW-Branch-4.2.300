//! Messages.h
/*!
 *  Messages contains all of the events and devices for all messages in the scale.
 *  Also contains the methods that use to exist in mainAppletDataPage.asp.
 *  In the Scale Application multiple classes represent multiple devices and events all spread
 *  across multiple classes. Those same events and devices are represented here in a single class
 *  file. The events and devices in this file match the Applications structure concerning numbers
 *  and names for easy mapping and ease of use. This file replaces all of the forms that previously
 *  existed in domOperate, domSuper, domService, and mainAppletDataPage. This specific file contains
 *  all of the methods from MADP. All of the devices and events are represented as enums in the .h file.
 * @Author: Joe E. Crew
 * @bugs: TBD
 * @addtogroup thinClientMessagingCsu
*/

#ifndef MESSAGES_H
#define MESSAGES_H

#include "SessionMessageMap.h"

#include "Devices.h"
#include "Events.h"

class DomNewWrapperAllTrays;


///////////////////////////////////////////////////////////////////////////////////////////////////

// All of the following structs came as forms from mainAppletDataPage.asp

// Global Variables about everything (250,250). Many of these were once standalone global vars and I pulled them into here.
class MadpInternalConfig
{
public:
    int scaleMode;                              // 1=operate, 2=super, 3=service
    int currentIsScaleMetric;                   // 0 - pounds, 1 - kg
    int isRunMode;

    float labelWidth;
    float labelHeight;
    float defaultLabelWidth;
    float defaultLabelHeight;
    int defaultFontSize;

    QString defaultFontName;
    QString statusLineError;
    QString brDatastream;
    QString brFilename;

    bool scaleBootComplete;
    bool changeLangIsEnabled;
    bool isFractionalPricing;
    bool isProportionalTareOn;
    bool ilaAttached;
    bool isSecondCLAAttached;
    bool isMissingPerifAtBoot;
    bool changeProductActive;					// do we clear the PLU when leaving Change Product
    bool hasExternalPrinterSettingsChanged;     // do we have 2nd printers (or 3rd) and have settings changed?
    bool wrUpgradeInProgress;
    bool ssactive_lock;                         // indicates if we go to self service on boot
    bool activate_selfservice;                  // we checked the checkbox and launched self service mode
    bool ssactive_tenative_lock;                // indicates we are going to go to SS once scalebootcomplete is true
    bool didNotSendInitialReadWeighMode;        //
    bool isLeftPanelActivated;
    bool isScaleCalibrating;
    int  fixedFlashkeyDefaultGroup;				// pjr420 - Keep track of the default flashkey group in the Fixed Flashkey Group. Need it to request Base set.

    // Flashkey Area
    bool loadAllSoftKeyGroups;
    bool updateSelfServiceFlashkeys;            // if we updated the Self Serv Chile Flashkey grid
    bool updateNavigateHomeFlashkeys;           // if going to Operate from Service or Supervisor - update flashkeys
    bool selfServicePreventClicks;              // existing fast click condition needs to be handeled
    bool clickedPrintLabelFromFinishScreen;     // on option3 Print to Label and Screen, and now print and display video
    bool selfServiceSendReimageLabelsMsg;       // for when we display the default label but want to print the PLU label
    bool updateOperateFlashkeyData;             // update all flashkeys, in case of COMM, backup\restore
    bool updateOperateLabelData;                // Indicates the need to request the label graphics in Operate mode. Occurs if B\R, COMM, delete all
    bool insyncSoftKeyCtr;                      // Indicates the need to update flashkeys. Occurs if B\R, COMM, delete all

    // Prepack Area
    bool haveWrUpgradeList;                     // default = false;
    bool remoteAccessIsDenied;                  // default = false;
    bool unavailablePageLoaded;                 // default = false;
    bool remoteAccessLoginSuccessful;           // default = false;
    bool needsWrapperUpgrade;                   // indicates when we need a wrapper upgrade
    bool needsCLAUpgrade;                       // indicates when we need a CLA upgrade
    bool needsNextGenWrapperUpgrade;            // indicates when we need a CLA upgrade
    bool needsKPCLoaderUpgrade;
    bool needsKPCUpgraderUpgrade;
    bool stifleWriteAll;                        // if we are in Super and click Depts panel in Operate
    bool allowWriteOnes;                        // if in Super, and we pop Operate Depts and change the Current
    bool timingsUpdatesInProgress;              // indicates the wrapper is being updated with Timings changes
    bool timingsUpdatesAreComplete;             // indicates the wrapper is done with Timings updates
    bool dialogSuppressed;                      // indicates the Timings screen suppressed the Wrapper is updating ..." dialog
    bool messagesSuppressed;                    // indicates we want supress incoming messages
    bool sysInfoUpdatesInProgress;              // indicates the wrapper is being updated with System Info changes
    bool sysInfoUpdatesAreComplete;             // indicates the wrapper is done with System Info updates
    bool sysInfoDialogSuppressed;               // indicates the System Info screen suppressed the Wrapper is updating ..." dialog
    bool scaleWentOutOfTilt;                    // indicates if a scale is more than 3 degrees out of tilt
    bool selfservInvalidPlu;
    bool isClaModelInitialization;              // SOF-4929: Indicates user selected 'CLA' button on Model Selection dialog
    bool isWrapperModelInitialization;          // SOF-4929: Indicates user selected a model on the Model Selection dialog
    bool isOkToLookUpPlu;                       // we can look up a plu on two printer system bc we have printed any previous plu
    bool poppedRezero;                          // did we pop a rezero dialog before looking up the PLU?
    int  isOkDroppedPLU;                        // the plu we could not look up becuase we didnt receive a product complete yet
    QVector<QString> printerCycleCounts;        // What are the cycle counts of the printers on wrappers

};

class MadpCutTestPlu
{
public:
    int pluNumber;
    int meatSpecSheet;
    int cutTestNumber;
};

class MadpForcedFieldFlags         // This is the struct tag
{
public:
    bool useTheForced;
    bool backDatingIsForced;
    bool priceIsForced;
    bool byCountIsForced;
    bool tareIsForced;
    bool netWeightIsForced;
    bool coolIsForced;
    bool labelsIsForced;
    bool pluIsForcedCool;                       // used as an individual check in checkForForcedCool()
    bool expirationDaysIsForced;
};                         // This is the struct variable

// (USED AS INTERNAL GLOBAL VAR)
class MadpPageLocData
{
public:
    QString currentPage;
    QString totalPages;
    QString tmpCurrPage;
};

// (xxx,xxx)
class DomLocalVars
{
public:
    QString weigherCapacityString;
    QString weigherCapacityStringDR;
    QString showDays;                           // show days = true if showing days, false if its date (best by, sell by etc)
    QString launchTextEditOK;                   // enabled if we have operators present
    QString newPluForCool;                      // too many COOL variables and flags, no idea what each do. Simplify!
    //bool pluIsForcedCool;                     // moved to scaleconfig.MadpForcedFieldFlags
};

// xxxx,xxxx - Global placeholder
class DomTrayParams
{
public:
    QString precedence;
    QString trayName;
    QString pluException;
    QString loadTrays;
    QString viewCurrentTimings;
    QString canEditTrayName;
};

// (xxx,xxx)
class DomBackupRestoreData
{
public:
   QString pluData;
   QString storeName;
   QString labelData;
   QString scaleConfig;
   QString operatorData;
   QString wrapperSettings;
   QString softkeyData;
   QString synchronizeConfig;
};

// (xxx,xxx)
class DomClickedLabel
{
public:
    QString textName;
};

// (xxx,xxx)
class DomLocalSyncCheckboxDom
{
public:
    QString productData;
    QString configData;
    QString coolData;
    QString labelData;
    QString operatorData;
    QString flashkeyData;
};

// (xxx,xxx)
class DomProdPlanning
{
public:
    QString prodPlanningFrame;
    QString prodPlanningUpdate;
};


class WrapParameter
{
public:

    typedef enum wrTimingsFieldType
//	enum
    {
        typeUnsignedInt,
        typeSignedInt,
        typeBoolean,
        typeSideUnderfolderReturnHomeStates,
        typeOutsideGripperReleaseStates,
        typeForceOutsideElevatorSelect,
        typeFilmRollToUse
    } wrTimingsFieldType_et;

    typedef enum heightBoxParameters
    {
        PERCENT_STRETCH_CLAMP_ON,       //	0
        KNIFE_HEIGHT,
        CLAMP_HEIGHT,                   //  2: NGW2=Side to Side wrap tension
        DISTANCE,
        FORCE_OUTSIDE_ELEVATOR,         //  on NGW this is leading edge label offset - Now the Secondary label offset
        GRIPPER_ADJUST,                 //	5: NGW2=Front to Back wrap tension
        FRONT_UF_ADJUST,                //  on NGW this is trailing edge label  offset - Now the Primary label offset
        WRAP_SPEED,
        DISCHARGE_SPEED,
        SIDE_UF_ADJUST,
        HEIGHT_RESTRICT,                //	10
        SIDE_UF_RETURN,                 // no longer used = 0
        UNUSED_12,                      // no longer used = 0
        SIDE_CLAMP_GAP,                 // no longer used = 0
        FILM_ROLL_TO_USE,
        REAR_UF_TRIP_DELAY,             //	15
        BRAKE_TIMING,
        OUTSIDE_GRIPPER_RELEASE,        // no longer used = 0
        INSIDE_GRIPPER_RELEASE,         // no longer used = 0
        //LEADING_EDGE_OFFSET,          // value put in FRONT_UF_ADJUST. The App does not have this so deleting it here
        PRIMARY_LABEL_VERT_OFFSET,
        SECONDARY_LABEL_VERT_OFFSET     // 20

    } heightBoxParameters_et;

    WrapParameter(QString& pName, wrTimingsFieldType fieldType, int fieldValue = 0)
         : type(fieldType), data(fieldValue), name(pName) { }
    virtual ~WrapParameter() {}

    virtual wrTimingsFieldType getType() const { return type; }

    virtual void setData(int value) { data = value; }
    virtual int getData() { return data; }

    virtual void setName(const QString& newName) { name = newName; }
    virtual QString getName() const { return name; }

    virtual void setDecreaseName(const QString& newName) { decrease = newName; }
    virtual QString getDecreaseName() const { return decrease; }

    virtual void setIncreaseName(const QString& newName) { increase = newName; }
    virtual QString getIncreaseName() const { return increase; }

private:
    wrTimingsFieldType  type;
    int					data;
    QString             name;

    QString             decrease;
    QString             increase;
};

//	This class will take care of the ASP variables we used to pass between the wrapper timings pages.
class WrapperTimingsData
{
public:

    WrapperTimingsData() : precedence(-1), pageMode(0), wrapperModelNum(-1), loadTrays(true),
                    viewCurrentTimings(true), updateActualPkg(false), canEditTrayName(false),
                    exitByMenu(true), trayName(""),
                    pluException(""), heightBoxSelected(""), cycledTrayName(""), trayList(NULL) { }

    virtual ~WrapperTimingsData() { }

    enum wrPageMode
    {
        pmUnknown = 0,
        pmViewEdit = 1,			//	View/Edit a tray
        pmAutoConfigure = 2, 	//	Auto-configure a new tray
        pmAutoDetect = 3,		//	Auto detect a tray
        pmPLUException = 4		//	New tray with a PLUException
    };

    int 	precedence;
    int		pageMode;
    int		wrapperModelNum;
    bool	loadTrays;
    bool	viewCurrentTimings;
    bool	updateActualPkg;
    bool	canEditTrayName;
    bool	exitByMenu;
    QString trayName;
    QString pluException;
    QString	heightBoxSelected;
    QString cycledTrayName;

    DomNewWrapperAllTrays	*trayList;
    QList<WrapParameter*>	wrapParameterList;
};


//======================== START NUMBERED MESSAGES AND CLASSES =========================================
//======================================================================================================
//======================================================================================================
// (1,2)                                        // existed in all 3 DOMs, so I deleted the Super and Service versions
class DomWeigherConfigRead : public SessionMessageMap<Weigher,WgInquire>
{
    typedef DomWeigherConfigRead _Myt;
    typedef SessionMessageMap<Weigher,WgInquire> _Mybase;
public:

    /// Weigher models.
    enum weigherModels
    {
        wmNone = 0,
        wmESS = 1,
        wmU2000S1 = 2,
        wmU2000S3 = 3,
        wmUWS = 4,
        wmQuantHC = 5,
        wmESC = 6,
        wmEPSA = 7,
        wmHLX1 = 8,                                 /// Current and typical attached weigher for HT scales (dual-range)
        wmEzUSB = 9,                                ///< NOT SUPPORTED - Cypress-based Ez-USB weigher for the AWS
        wmAWS = 10,                                 ///< Atmel-based USB weigher for the AWS, AWS Weighing Device (dual range) (Atmel-based SAM-BA device) (haberrc)
        wmEPSA2 = 11,                               ///< Atmel-based USB weigher for the AWS, AWS Weighing Device, Non-Dual-Range Load Cell (Atmel-based SAM-BA device) (haberrc)
        wmAWS2 = 12,                                ///< Atmel-based USB weigher for the AWS, AWS Weighing Device, Non-Dual-Range Load Cell (Atmel-based SAM-BA device) (haberrc)
        WmEPSAC2 = 13,                              ///	New USB Compact Label Applier
        WmSASCALE1 = 14,                            /// SASCALE-1 for SASCALE:  HT-374
        WmSASCALE2 = 15,                            /// SASCALE-2 for SASCALE (dual-range):  HT-374
        WmKPCWeigher = 16							/// New CAN Weigher (daul-range / CAT3)
    };

    DomWeigherConfigRead() : _Mybase() {}
    DomWeigherConfigRead( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWeigherConfigRead( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWeigherConfigRead() {}

    virtual bool isDualRangeCapable() const {
        return ( ( wmHLX1 == weigherModel() ) || ( wmAWS == weigherModel() ) || ( WmSASCALE2 == weigherModel() ) || ( WmKPCWeigher == weigherModel() ) );
    }

    QString& weigherFilterSpeed() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( bool, isScaleMetric, setIsScaleMetric, isScaleMetricRaw, 3 )
    DEFINE_FIELD( int, weigherScaleFactor, setWeigherScaleFactor, weigherScaleFactorRaw, 4 )
    QString& weigherMaximumWt() {return _Mybase::operator [] (5);}
    QString& weigherMininumWt() {return _Mybase::operator [] (6);}
    DEFINE_FIELD( QDate, lastDateCalibrated, setLastDateCalibrated, lastDateCalibratedRaw, 7 )
    DEFINE_FIELD( int, timesCalibrated, setTimesCalibrated, timesCalibratedRaw, 8 )
    DEFINE_FIELD( QDate, lastDateConfigured, setLastDateConfigured, lastDateConfiguredRaw, 9 )
    DEFINE_FIELD( int, timesConfigured, setTimesConfigured, timesConfiguredRaw, 10 )
    DEFINE_FIELD( int, weigherModel, setWeigherModel, weigherModelRaw, 11 )
    QString& netWeightDRLimitCheck() {return _Mybase::operator [] (12);}
    DEFINE_FIELD( int, dualRangeLimit, setDualRangeLimit, dualRangeLimitRaw, 13 )
    DEFINE_FIELD( bool, isScaleValuemax, setIsScaleValuemax, isScaleValuemaxRaw, 14 )
    QString& gcdLanguage() {return _Mybase::operator [] (15);}              // jhc 7-8-19 why is this here?  The wginquire message and weigherConfiguration in the application doesnt even support it
};

//  (1,4)
class MadpWeightData : public SessionMessageMap<Weigher,WgStatus>
{
    typedef MadpWeightData _Myt;
    typedef SessionMessageMap<Weigher,WgStatus> _Mybase;
public:
    MadpWeightData() : _Mybase() {}
    MadpWeightData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpWeightData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpWeightData() {}

    QString& netWeight() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( bool, zeroFlag, setZeroFlag, zeroFlagRaw, 3 )
    DEFINE_FIELD( QString, dualRangeIndicator, setDualRangeIndicator, dualRangeIndicatorRaw, 4 )
    DEFINE_FIELD( QString, tiltX, setTiltX, tiltXRaw, 5 )
    DEFINE_FIELD( QString, tiltY, setTiltY, tiltYRaw, 6 )
};

//  (1,23)
// SOF-1359
class DomWeigherResetFactoryDefaultsStatus : public SessionMessageMap<Weigher,WgResetFactoryDefaultsComplete>
{
    typedef DomWeigherResetFactoryDefaultsStatus _Myt;
    typedef SessionMessageMap<Weigher,WgResetFactoryDefaultsComplete> _Mybase;
public:
    DomWeigherResetFactoryDefaultsStatus() : _Mybase() {}
    DomWeigherResetFactoryDefaultsStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWeigherResetFactoryDefaultsStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWeigherResetFactoryDefaultsStatus() {}

    QString& resetStatus() {return _Mybase::operator [] (2);}
};

// FORM (1,100)
class DomCalibrateScale : public SessionMessageMap<Weigher,WgCalibrationStep>
{
    typedef DomCalibrateScale _Myt;
    typedef SessionMessageMap<Weigher,WgCalibrationStep> _Mybase;
public:
    DomCalibrateScale() : _Mybase() {}
    DomCalibrateScale( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCalibrateScale( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCalibrateScale() {}

    QString& currentStep() {return _Mybase::operator [] (2);}
    QString& zeroes() {return _Mybase::operator [] (3);}
    QString& delta() {return _Mybase::operator [] (4);}
    QString& nonzeroes() {return _Mybase::operator [] (5);}
    QString& zeroesForDisplay() {return _Mybase::operator [] (6);}
};

// FORM (1,101)
class DomCompleteCalibrate : public SessionMessageMap<Weigher,WgCalibrationEnd>
{
    typedef DomCompleteCalibrate _Myt;
    typedef SessionMessageMap<Weigher,WgCalibrationEnd> _Mybase;
public:
    DomCompleteCalibrate() : _Mybase() {}
    DomCompleteCalibrate( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCompleteCalibrate( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCompleteCalibrate() {}

    QString& completeJob() {return _Mybase::operator [] (2);}
};

// (1,103)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomWeigherInfoRead : public SessionMessageMap<Weigher,WgInfo>
{
    typedef DomWeigherInfoRead _Myt;
    typedef SessionMessageMap<Weigher,WgInfo> _Mybase;
public:
    DomWeigherInfoRead() : _Mybase() {}
    DomWeigherInfoRead( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWeigherInfoRead( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWeigherInfoRead() {}

    QString& address() {return _Mybase::operator [] (2);}
};

// (1,104)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomWeigherInfoCountsRead : public SessionMessageMap<Weigher,WgStatusCounts>
{
    typedef DomWeigherInfoCountsRead _Myt;
    typedef SessionMessageMap<Weigher,WgStatusCounts> _Mybase;
public:
    DomWeigherInfoCountsRead() : _Mybase() {}
    DomWeigherInfoCountsRead( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWeigherInfoCountsRead( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWeigherInfoCountsRead() {}

    QString& nonZeroes() {return _Mybase::operator [] (2);}
    QString& zeroes() {return _Mybase::operator [] (3);}
    QString& weight() {return _Mybase::operator [] (4);}
    QString& tiltX() {return _Mybase::operator [] (5);}
    QString& tiltY() {return _Mybase::operator [] (6);}
};

// FORM (1,111)
class DomWeigherSwitch : public SessionMessageMap<Weigher,WgConfigurationEnd>
{
    typedef DomWeigherSwitch _Myt;
    typedef SessionMessageMap<Weigher,WgConfigurationEnd> _Mybase;
public:
    DomWeigherSwitch() : _Mybase() {}
    DomWeigherSwitch( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWeigherSwitch( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWeigherSwitch() {}

    QString& switchPressed() {return _Mybase::operator [] (2);} 		// switchPressed is not contained in the message. setSwitchToPressed() will set it when it is called (CCD).
};

// FORM (1,117)
class DomAuditLogInformation : public SessionMessageMap<Weigher,WgWriteAuditTrail>
{
    typedef DomAuditLogInformation _Myt;
    typedef SessionMessageMap<Weigher,WgWriteAuditTrail> _Mybase;
public:
    DomAuditLogInformation() : _Mybase() {}
    DomAuditLogInformation( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomAuditLogInformation( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomAuditLogInformation() {}

    DEFINE_FIELD( int, numOfEvents, setNumOfEvents, numOfEventsRaw, 2 )
    QString& auditEventList()    {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numOfTypes, setNumOfTypes, numOfTypesRaw, 4 )
    QString& auditTypeList()     {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, numOfParams, setNumOfParams, numOfParamsRaw, 6 )
    QString& parameterIdList()   {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, numOfDates, setNumOfDate, numOfDateRaw, 8 )
    QString& dateTimeList()      {return _Mybase::operator [] (9);}
    DEFINE_FIELD( int, numOfNewVals, setNumOfNewVals, numOfNewValsRaw, 10 )
    QString& newValueList()      {return _Mybase::operator [] (11);}
    DEFINE_FIELD( int, numOfOldVals, setNumOfOldVals, numOfOldValsRaw, 12 )
    QString& previousValueList() {return _Mybase::operator [] (13);}
};

// SOF-3244: ValueMax FIT Parameters
// (1,128)
class DomWeigherFitParamsRead : public SessionMessageMap<Weigher,WgValueMaxFitParams>
{
    typedef DomWeigherFitParamsRead _Myt;
    typedef SessionMessageMap<Weigher,WgValueMaxFitParams> _Mybase;
public:
    DomWeigherFitParamsRead() : _Mybase() {}
    DomWeigherFitParamsRead( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWeigherFitParamsRead( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWeigherFitParamsRead() {}

    DEFINE_FIELD( long, gravity,               set_gravity,               gravityRaw,               2 )
    DEFINE_FIELD( long, m_LbsCountsConvFactor, set_m_LbsCountsConvFactor, m_LbsCountsConvFactorRaw, 3 )
    DEFINE_FIELD( long, zeroValue,             set_zeroValue,             zeroValueRaw,             4 )
    DEFINE_FIELD( long, spanValue,             set_spanValue,             spanValueRaw,             5 )
    DEFINE_FIELD( long, z0ElectronicOffset,    set_z0ElectronicOffset,    z0ElectronicOffsetRaw,    6 )
    DEFINE_FIELD( long, deadWeight,            set_deadWeight,            deadWeightRaw,            7 )
    DEFINE_FIELD( long, x2,                    set_x2,                    x2Raw,                    8 )
    DEFINE_FIELD( long, x1,                    set_x1,                    x1Raw,                    9 )
    DEFINE_FIELD( long, x0,                    set_x0,                    x0Raw,                   10 )
    DEFINE_FIELD( long, y2,                    set_y2,                    y2Raw,                   11 )
    DEFINE_FIELD( long, y1,                    set_y1,                    y1Raw,                   12 )
    DEFINE_FIELD( long, y0,                    set_y0,                    y0Raw,                   13 )
    DEFINE_FIELD( long, xRot_0,                set_xRot_0,                xRot_0Raw,               14 )
    DEFINE_FIELD( long, xRot_1,                set_xRot_1,                xRot_1Raw,               15 )
    DEFINE_FIELD( long, xRot_2,                set_xRot_2,                xRot_2Raw,               16 )
    DEFINE_FIELD( long, yRot_0,                set_yRot_0,                yRot_0Raw,               17 )
    DEFINE_FIELD( long, yRot_1,                set_yRot_1,                yRot_1Raw,               18 )
    DEFINE_FIELD( long, yRot_2,                set_yRot_2,                yRot_2Raw,               19 )
    DEFINE_FIELD( long, zRot_0,                set_zRot_0,                zRot_0Raw,               20 )
    DEFINE_FIELD( long, zRot_1,                set_zRot_1,                zRot_1Raw,               21 )
    DEFINE_FIELD( long, zRot_2,                set_zRot_2,                zRot_2Raw,               22 )
    DEFINE_FIELD( long, xGain,                 set_xGain,                 xGainRaw,                23 )
    DEFINE_FIELD( long, yGain,                 set_yGain,                 yGainRaw,                24 )
    DEFINE_FIELD( long, gCoeff,                set_gCoeff,                gCoeffRaw,               25 )
    DEFINE_FIELD( long, checkSum,              set_checkSum,              checkSumRaw,             26 )
};

// (2,4)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomUltimaWeigherPowerupTests : public SessionMessageMap<WeigherSystem,WgSysStatus>
{
    typedef DomUltimaWeigherPowerupTests _Myt;
    typedef SessionMessageMap<WeigherSystem,WgSysStatus> _Mybase;
public:
    DomUltimaWeigherPowerupTests() : _Mybase() {}
    DomUltimaWeigherPowerupTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomUltimaWeigherPowerupTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomUltimaWeigherPowerupTests() {}

    QString& storedChecksum() {return _Mybase::operator [] (2);}
    QString& calculatedChecksum() {return _Mybase::operator [] (3);}
    QString& EEPChecksum() {return _Mybase::operator [] (4);}
    QString& wordTest() {return _Mybase::operator [] (5);}
    QString& addressTest() {return _Mybase::operator [] (6);}
};

// (2,20)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomUsbWeigherPowerupTests : public SessionMessageMap<WeigherSystem,SysUsbWgStatus>
{
    typedef DomUsbWeigherPowerupTests _Myt;
    typedef SessionMessageMap<WeigherSystem,SysUsbWgStatus> _Mybase;
public:
    DomUsbWeigherPowerupTests() : _Mybase() {}
    DomUsbWeigherPowerupTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomUsbWeigherPowerupTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomUsbWeigherPowerupTests() {}

    QString& systemEepromError() {return _Mybase::operator [] (2);}
    QString& weigherEepromError() {return _Mybase::operator [] (3);}
    QString& serialNumber() {return _Mybase::operator [] (4);}
    QString& softwareVersion() {return _Mybase::operator [] (5);}
    QString& hardwareVersion() {return _Mybase::operator [] (6);}
    QString& weigherSoftwareVersion() {return _Mybase::operator [] (7);}
};


// (4,21)
class DomOpModeLabelsPrintedCounts : public SessionMessageMap<OperationMode,OpModeLabelsPrintedCounts>
{
    typedef DomOpModeLabelsPrintedCounts _Myt;
    typedef SessionMessageMap<OperationMode,OpModeLabelsPrintedCounts> _Mybase;
public:
    DomOpModeLabelsPrintedCounts() : _Mybase() {}
    DomOpModeLabelsPrintedCounts( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOpModeLabelsPrintedCounts( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOpModeLabelsPrintedCounts() {}

    DEFINE_FIELD( int, currentPrintedCount, setCurrentPrintedCount, currentPrintedCountRaw, 2 )
    DEFINE_FIELD( int, totalLabelsCount, setTotalLabelsCount, totalLabelsCountRaw, 3 )
};


// (5,1)														// Also existed in domService, so I deleted the domService version
class DomLabelApplierConfig : public SessionMessageMap<LabelApplier,LaConfig>
{
    typedef DomLabelApplierConfig _Myt;
    typedef SessionMessageMap<LabelApplier,LaConfig> _Mybase;
public:
    DomLabelApplierConfig() : _Mybase() {}
    DomLabelApplierConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelApplierConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelApplierConfig() {}

    DEFINE_FIELD( bool, weigherStationInstalled, setWeigherStationInstalled, weigherStationInstalledRaw, 2 )
    DEFINE_FIELD( bool, primaryStationInstalled, setPrimaryStationInstalled, primaryStationInstalledRaw, 3 )
    DEFINE_FIELD( bool, secondaryStationInstalled, setSecondaryStationInstalled, secondaryStationInstalledRaw, 4 )
    DEFINE_FIELD( int, packageDirection, setPackageDirection, packageDirectionRaw, 5 )
    QString& dischargePosition() {return _Mybase::operator [] (6);}
    QString& primaryPlacementOffset() {return _Mybase::operator [] (7);}
    QString& secondPlacementOffset() {return _Mybase::operator [] (8);}
    DEFINE_FIELD( int, primaryLabelPlacement, setPrimaryLabelPlacement, primaryLabelPlacementRaw, 9 )
    QString& overdrive() {return _Mybase::operator [] (10);}
};

// (5,2)
// Also existed in domService, so I deleted the domService version
class DomLabelApplierInquire : public SessionMessageMap<LabelApplier,LaInquire>
    {
    typedef DomLabelApplierInquire _Myt;
    typedef SessionMessageMap<LabelApplier,LaInquire> _Mybase;
public:
    DomLabelApplierInquire() : _Mybase() {}
    DomLabelApplierInquire( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelApplierInquire( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelApplierInquire() {}

    QString& transferStationType() {return _Mybase::operator [] (2);}
    QString& weigherStationInstalled() {return _Mybase::operator [] (3);}
    QString& secondaryStationInstalled() {return _Mybase::operator [] (4);}
    QString& packageDirection() {return _Mybase::operator [] (5);}
    QString& dischargePosition() {return _Mybase::operator [] (6);}
    QString& weigherAccelerationRate() {return _Mybase::operator [] (7);}
    QString& primaryAccelerationRate() {return _Mybase::operator [] (8);}
    QString& secondAccelerationRate() {return _Mybase::operator [] (9);}
    QString& primaryPlacementOffset() {return _Mybase::operator [] (10);}
    QString& primaryJustificationDistance() {return _Mybase::operator [] (11);}
    QString& secondPlacementOffset() {return _Mybase::operator [] (12);}
    QString& primaryLabelPlacement() {return _Mybase::operator [] (13);}
    QString& diagonalLabelPlacement() {return _Mybase::operator [] (14);}
    QString& labelPosition() {return _Mybase::operator [] (15);}                      // did not exist in domService version
    QString& hasWeigherWakeupSensor() {return _Mybase::operator [] (16);}             // did not exist in domService version
};

// FORM (5,12)
class DomLabelApplierSensors : public SessionMessageMap<LabelApplier,LaSensors>
{
    typedef DomLabelApplierSensors _Myt;
    typedef SessionMessageMap<LabelApplier,LaSensors> _Mybase;
public:
    DomLabelApplierSensors() : _Mybase() {}
    DomLabelApplierSensors( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelApplierSensors( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelApplierSensors() {}

    /*QString& wakeupSensor() {return _Mybase::operator [] (2);}
    QString& scaleSensor() {return _Mybase::operator [] (3);}
    QString& firstLabelApplierSensor() {return _Mybase::operator [] (4);}
    QString& firstPrinterPositionSwitch() {return _Mybase::operator [] (5);}
    QString& firstPrinterPositionSensor() {return _Mybase::operator [] (6);}
    QString& firstGateSwitch() {return _Mybase::operator [] (7);}
    QString& secondLabelApplierSensor() {return _Mybase::operator [] (8);}
    QString& secondPrinterPositionSwitch() {return _Mybase::operator [] (9);}
    QString& secondPrinterPositionSensor() {return _Mybase::operator [] (10);}
    QString& secondGateSwitch() {return _Mybase::operator [] (11);}*/

    //Alex's updates to the label applier sensors message 5/20/14
    QString& wakeupSensor() {return _Mybase::operator [] (2);}
    QString& weigherEntrySensor() {return _Mybase::operator [] (3);}
    QString& claEntrySensor() {return _Mybase::operator [] (4);}
    QString& pkgEdgeDetectSensor() {return _Mybase::operator [] (5);}
    QString& startSwitch() {return _Mybase::operator [] (6);}
    QString& stopSwitch() {return _Mybase::operator [] (7);}
    QString& claGateSwitch() {return _Mybase::operator [] (8);}
    QString& firstPrinterPostSwitch() {return _Mybase::operator [] (9);}
};

// (5,21)
class DomLabelApplierSystemStatus : public SessionMessageMap<LabelApplier, LaCLASysStatus>
{
    typedef DomLabelApplierSystemStatus _Myt;
    typedef SessionMessageMap<LabelApplier,LaCLASysStatus> _Mybase;
public:
    DomLabelApplierSystemStatus() : _Mybase() {}
    DomLabelApplierSystemStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelApplierSystemStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelApplierSystemStatus() {}

    QString& configChecksum() {return _Mybase::operator [] (2);}
    QString& appVersion() {return _Mybase::operator [] (3);}
    QString& dataVersion() {return _Mybase::operator [] (4);}
    QString& bootVersion() {return _Mybase::operator [] (5);}
};

// (5,25)
class DomLabelApplierSystemInformation : public SessionMessageMap<LabelApplier, LaSysInfo>
{
    typedef DomLabelApplierSystemInformation _Myt;
    typedef SessionMessageMap<LabelApplier,LaSysInfo> _Mybase;
public:

    /// Wrapper models.
    enum labelApplierModel
    {
        NoModel = 0,
        AWS1RL = 1,
        AWS1LR = 2,
        AWS1MLR = 3,                                ///< AWS manual machine left to right
        AWS1LR_I = 4,                               ///< AWS international left to right
        AWS1RL_I = 5,                               ///< AWS international right to left
        AWS1BWRL = 6,                               ///< AWS Lite (defeatured)right to left
        AWS1BWLR = 7,                               ///< AWS Lite (defeatured)left to right
        AWS2RL = 8,                                 ///< AWS with a Wide Elevator only, right to left
        AWS2LR = 9                                  ///< AWS with a Wide Elevator only, left to right
    };

    DomLabelApplierSystemInformation() : _Mybase() {}
    DomLabelApplierSystemInformation( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelApplierSystemInformation( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelApplierSystemInformation() {}

    QString& boardPartNumber() {return _Mybase::operator [] (2);}
    QString& boardRev() {return _Mybase::operator [] (3);}
    QString& boardManufactureDate() {return _Mybase::operator [] (4);}
    QString& boardSerialNumber() {return _Mybase::operator [] (5);}
    QString& boardChecksumStatus() {return _Mybase::operator [] (6);}
    QString& awsSerialNumber() {return _Mybase::operator [] (7);}
    QString& awsManufactureDate() {return _Mybase::operator [] (8);}
    QString& awsInstallDate() {return _Mybase::operator [] (9);}
    DEFINE_FIELD( int, awsModelNumber, setAwsModelNumber, awsModelNumberRaw, 10 )
    QString& awsLabelApplierCycleCount() {return _Mybase::operator [] (11);}
    QString& awsChecksumStatus() {return _Mybase::operator [] (12);}
};

// FORM (5,28)
class DomLabelApplierTestComplete : public SessionMessageMap<LabelApplier, LaTestComplete>
{
    typedef DomLabelApplierTestComplete _Myt;
    typedef SessionMessageMap<LabelApplier,LaTestComplete> _Mybase;
public:
    DomLabelApplierTestComplete() : _Mybase() {}
    DomLabelApplierTestComplete( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelApplierTestComplete( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelApplierTestComplete() {}

    QString& testNum() {return _Mybase::operator [] (2);}
    QString& data1() {return _Mybase::operator [] (3);}
    QString& data2() {return _Mybase::operator [] (4);}
    QString& testPassed() {return _Mybase::operator [] (5);}
};

// FORM (5,86)
class ILACycleCounts : public SessionMessageMap<LabelApplier,LaILASpeed>
{
    typedef ILACycleCounts _Myt;
    typedef SessionMessageMap<LabelApplier,LaILASpeed> _Mybase;
public:
    ILACycleCounts() : _Mybase() {}
    ILACycleCounts( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    ILACycleCounts( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~ILACycleCounts() {}

    QString& speed() {return _Mybase::operator [] (2);}
    QString& counts() {return _Mybase::operator [] (3);}
};

// (5,100)														// Exists in domOperate/domSuper so the domSuper version was deleted.
class DomEvolLabelApplierConfig : public SessionMessageMap<LabelApplier,EvolLaConfig>
{
    typedef DomEvolLabelApplierConfig _Myt;
    typedef SessionMessageMap<LabelApplier,EvolLaConfig> _Mybase;
public:
    DomEvolLabelApplierConfig() : _Mybase() {}
    DomEvolLabelApplierConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolLabelApplierConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolLabelApplierConfig() {}

    DEFINE_FIELD( bool, hasSecondPrinter, setHasSecondPrinter, hasSecondPrinterRaw, 2 )
    DEFINE_FIELD( int, packageDirection, setPackageDirection, packageDirectionRaw, 3 )
};

// FORM (6,4)
class DomLabelApplierPowerUpTests : public SessionMessageMap<LabelApplierSystem,LaSysStatus>
{
    typedef DomLabelApplierPowerUpTests _Myt;
    typedef SessionMessageMap<LabelApplierSystem,LaSysStatus> _Mybase;
public:
    DomLabelApplierPowerUpTests() : _Mybase() {}
    DomLabelApplierPowerUpTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelApplierPowerUpTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelApplierPowerUpTests() {}

    QString& masterCalculatedChecksum() {return _Mybase::operator [] (2);}
    QString& masterStoredChecksum() {return _Mybase::operator [] (3);}
    QString& masterEEPChecksumTest() {return _Mybase::operator [] (4);}
    QString& masterWordTest() {return _Mybase::operator [] (5);}
    QString& masterAddressTest() {return _Mybase::operator [] (6);}
    QString& slave1CalculatedChecksum() {return _Mybase::operator [] (7);}
    QString& slave1StoredChecksum() {return _Mybase::operator [] (8);}
    QString& slave1EEPChecksumTest() {return _Mybase::operator [] (9);}
    QString& slave1WordTest() {return _Mybase::operator [] (10);}
    QString& slave1AddressTest() {return _Mybase::operator [] (11);}
    QString& slave2CalculatedChecksum() {return _Mybase::operator [] (12);}
    QString& slave2StoredChecksum() {return _Mybase::operator [] (13);}
    QString& slave2EEPChecksumTest() {return _Mybase::operator [] (14);}
    QString& slave2WordTest() {return _Mybase::operator [] (15);}
    QString& slave2AddressTest() {return _Mybase::operator [] (16);}
};

// (7,2)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomPrinterConfigRead : public SessionMessageMap<Printer,PrInquire>
{
    typedef DomPrinterConfigRead _Myt;
    typedef SessionMessageMap<Printer,PrInquire> _Mybase;
public:
    DomPrinterConfigRead() : _Mybase() {}
    DomPrinterConfigRead( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterConfigRead( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterConfigRead() {}

    DEFINE_FIELD( int, printerType, setPrinterType, printerTypeRaw, 2 )
    DEFINE_FIELD( int, printContrast, setPrintContrast, printContrastRaw, 3 )
    DEFINE_FIELD( int, vertPrintPosition, setVertPrintPosition, vertPrintPositionRaw, 4 )
    DEFINE_FIELD( int, horizPrintPosition, setHorizPrintPosition, horizPrintPositionRaw, 5 )
    QString& labelWidth() {return _Mybase::operator [] (6);}
    DEFINE_FIELD( int, expelPosition, setExpelPosition, expelPositionRaw, 7 )
    DEFINE_FIELD( int, slaEjectLine, setSlaEjectLine, slaEjectLineRaw, 8 )
};

// FORM (7,8)
class DomPrinterMonitorSensors : public SessionMessageMap<Printer,PrSensors>
{
    typedef DomPrinterMonitorSensors _Myt;
    typedef SessionMessageMap<Printer,PrSensors> _Mybase;
public:
    DomPrinterMonitorSensors() : _Mybase() {}
    DomPrinterMonitorSensors( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterMonitorSensors( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterMonitorSensors() {}

    QString& printerType() {return _Mybase::operator [] (2);}
    QString& isPrintheadDisengaged() {return _Mybase::operator [] (3);}
    QString& printheadTemp() {return _Mybase::operator [] (4);}
    QString& labelWidth() {return _Mybase::operator [] (5);}
    QString& labelTakenSensor() {return _Mybase::operator [] (6);}
    QString& mediaSensor() {return _Mybase::operator [] (7);}
    QString& printheadVoltage() {return _Mybase::operator [] (8);}
    DEFINE_FIELD( int, regulatorTemp, setRegulatorTemp, regulatorTempRaw, 9 )
};

// FORM (7,9)
class DomPrinterPrintheadPower : public SessionMessageMap<Printer,PrPrintheadPower>
{
    typedef DomPrinterPrintheadPower _Myt;
    typedef SessionMessageMap<Printer,PrPrintheadPower> _Mybase;
public:
    DomPrinterPrintheadPower() : _Mybase() {}
    DomPrinterPrintheadPower( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterPrintheadPower( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterPrintheadPower() {}

    QString& printerType() {return _Mybase::operator [] (2);}
    QString& powerState() {return _Mybase::operator [] (3);}
    QString& shortCircuitProtection() {return _Mybase::operator [] (4);}
};

// FORM (7 ,17)
class DomPrinterLabelSize : public SessionMessageMap<Printer,PrNomSize>
{
    typedef DomPrinterLabelSize _Myt;
    typedef SessionMessageMap<Printer,PrNomSize> _Mybase;
public:
    DomPrinterLabelSize() : _Mybase() {}
    DomPrinterLabelSize( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterLabelSize( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterLabelSize() {}

    QString& printerType() {return _Mybase::operator [] (2);}
    QString& actualLabelStockLength() {return _Mybase::operator [] (3);}
    QString& measuredLabelStockLength() {return _Mybase::operator [] (4);}
};

// (7,38)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomPrinterLocations : public SessionMessageMap<Printer,PrPrinterType>
{
    typedef DomPrinterLocations _Myt;
    typedef SessionMessageMap<Printer,PrPrinterType> _Mybase;
public:
    DomPrinterLocations() : _Mybase() {}
    DomPrinterLocations( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterLocations( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterLocations() {}

    QString& printerType() {return _Mybase::operator [] (2);}
    QString& printerAddress() {return _Mybase::operator [] (3);} // *IMPORTANT NOTE* in domSuper/Service this was named printerNode, will have to change all of those references to printerAddress
};

// 7,41															// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomPrinterStockMetrics : public SessionMessageMap<Printer,PrStockMetrics>
{
    typedef DomPrinterStockMetrics _Myt;
    typedef SessionMessageMap<Printer,PrStockMetrics> _Mybase;
public:
    DomPrinterStockMetrics() : _Mybase() {}
    DomPrinterStockMetrics( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterStockMetrics( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterStockMetrics() {}

    DEFINE_FIELD( int, printerType, setPrinterType, printerTypeRaw, 2 )
    DEFINE_FIELD( int, stockWidthInDots, setStockWidthInDots, stockWidthInDotsRaw, 3 )
    DEFINE_FIELD( int, stockLengthInLines, setStockLengthInLines, stockLengthInLinesRaw, 4 )
    QString& stockWidth() {return _Mybase::operator [] (5);}
    QString& stockLength() {return _Mybase::operator [] (6);}
};

//  (7,58)
// SOF-1358
class DomPrinterResetFactoryDefaultsStatus : public SessionMessageMap<Printer,PrResetFactoryDefaultsComplete>
{
    typedef DomPrinterResetFactoryDefaultsStatus _Myt;
    typedef SessionMessageMap<Printer,PrResetFactoryDefaultsComplete> _Mybase;
public:
    DomPrinterResetFactoryDefaultsStatus() : _Mybase() {}
    DomPrinterResetFactoryDefaultsStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterResetFactoryDefaultsStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterResetFactoryDefaultsStatus() {}

    QString& resetStatus() {return _Mybase::operator [] (2);}
};

//  (7,60)
class DomPrinterHeadType : public SessionMessageMap<Printer,PrHeadType>
{
    typedef DomPrinterHeadType _Myt;
    typedef SessionMessageMap<Printer,PrHeadType> _Mybase;
public:
    DomPrinterHeadType() : _Mybase() {}
    DomPrinterHeadType( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterHeadType( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterHeadType() {}

    DEFINE_FIELD( int, printerHeadType, setPrinterHeadType, printerHeadTypeRaw, 2 )
    QString& printerDotSize() {return _Mybase::operator [] (3);}
};

//  (7,64)
// SOF-1358
class DomPrinterCutterStatus : public SessionMessageMap<Printer,PrCutterStatus>
{
    typedef DomPrinterCutterStatus _Myt;
    typedef SessionMessageMap<Printer,PrCutterStatus> _Mybase;
public:
    DomPrinterCutterStatus() : _Mybase() {}
    DomPrinterCutterStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterCutterStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterCutterStatus() {}

    DEFINE_FIELD( bool, home, setHome, HomeRaw, 2 )
    DEFINE_FIELD( bool, installed, setInstalled, installedRaw, 3 )
    DEFINE_FIELD( bool, interlock, setInterlock, interlockRaw, 4 )
    DEFINE_FIELD( bool, jammed, setJammed, jammedRaw, 5 )
};


// 7,108
class DomPrStatusClear : public SessionMessageMap<Printer,PrStatusClear>
{
    typedef DomPrStatusClear _Myt;
    typedef SessionMessageMap<Printer,PrStatusClear> _Mybase;
public:
    DomPrStatusClear() : _Mybase() {}
    DomPrStatusClear( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrStatusClear( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrStatusClear() {}
};

// 7,112
class DomPrWriteAllUsbDriverNames : public SessionMessageMap<Printer,PrWriteAllUsbDriverNames>
{
    typedef DomPrWriteAllUsbDriverNames _Myt;
    typedef SessionMessageMap<Printer,PrWriteAllUsbDriverNames> _Mybase;
public:
    DomPrWriteAllUsbDriverNames() : _Mybase() {}
    DomPrWriteAllUsbDriverNames( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrWriteAllUsbDriverNames( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrWriteAllUsbDriverNames() {}

    DEFINE_FIELD( int, usbPrinterNamesListCount, setUsbPrinterNamesListCount, usbPrinterNamesListCountRaw, 2 )
    QString& usbPrinterNamesList() {return _Mybase::operator [] (3);}
};

// 7,114
class DomPrWriteAllNetworkPrinterDriverNames : public SessionMessageMap<Printer,PrWriteAllNetworkPrinterDriverNames>
{
    typedef DomPrWriteAllNetworkPrinterDriverNames _Myt;
    typedef SessionMessageMap<Printer,PrWriteAllNetworkPrinterDriverNames> _Mybase;
public:
    DomPrWriteAllNetworkPrinterDriverNames() : _Mybase() {}
    DomPrWriteAllNetworkPrinterDriverNames( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrWriteAllNetworkPrinterDriverNames( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrWriteAllNetworkPrinterDriverNames() {}

    DEFINE_FIELD( int, networkPrinterNamesListCount, setNetworkPrinterNamesListCount, networkPrinterNamesListCountRaw, 2 )
    QString& networkPrinterNamesList() {return _Mybase::operator [] (3);}
};

// 7,123
class DomPrWriteAllExternalPrinter : public SessionMessageMap<Printer,PrWriteAllExternalPrinter>
{
    typedef DomPrWriteAllExternalPrinter _Myt;
    typedef SessionMessageMap<Printer,PrWriteAllExternalPrinter> _Mybase;
public:
    DomPrWriteAllExternalPrinter() : _Mybase() {}
    DomPrWriteAllExternalPrinter( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrWriteAllExternalPrinter( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrWriteAllExternalPrinter() {}

    DEFINE_FIELD( int, extPrinterNumListCount, setextPrinterNumListCount, extPrinterNumListCountRaw, 2 )
    QString& extPrinterNumList() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, extPrinterNamesListCount, setextPrinterNamesListCount, extPrinterNamesListCountRaw, 4 )
    QString& extPrinterNamesList() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, extPrinterTypeCount, setextPrinterTypeCount, extPrinterTypeCountRaw, 6 )
    QString& extPrinterTypeList() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, extPrinterDriverCount, setextPrinterDriverCount, extextPrinterDriverCountRaw, 8 )
    QString& extPrinterDriverList() {return _Mybase::operator [] (9);}
    DEFINE_FIELD( int, extPrinterIpAddressCount, setextPrinterIpAddressCount, extPrinterIpAddressCountRaw, 10 )
    QString& extPrinterIpAddressList() {return _Mybase::operator [] (11);}
};

// 7,133
class DomPrWriteAllProductTextPrintParams : public SessionMessageMap<Printer,PrWriteAllProductTextPrintParams>
{
    typedef DomPrWriteAllProductTextPrintParams _Myt;
    typedef SessionMessageMap<Printer,PrWriteAllProductTextPrintParams> _Mybase;
public:
    DomPrWriteAllProductTextPrintParams() : _Mybase() {}
    DomPrWriteAllProductTextPrintParams( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrWriteAllProductTextPrintParams( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrWriteAllProductTextPrintParams() {}

    DEFINE_FIELD( int, extPrinterDataNamesNumCount, setextPrinterDataNamesNumCount, extPrinterDataNamesNumCountRaw, 2 )
    QString& extPrinterDataNamesNumList() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, extPrinterDataLabelsPromptCount, setextPrinterDataLabelsPromptCount, extPrinterDataLabelsPromptCountRaw, 4 )
    QString& extPrinterDataLabelsPrompt() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, extPrinterDataLabelNumCount, setextPrinterDataLabelNumCount, extPrinterDataLabelNumCountRaw, 6 )
    QString& extPrinterDataLabelNumList() {return _Mybase::operator [] (7);}
};

// 7,164
class DomPrAreWeSizingLabels : public SessionMessageMap<Printer,PrAreWeSizingLabels>
{
    typedef DomPrAreWeSizingLabels _Myt;
    typedef SessionMessageMap<Printer,PrAreWeSizingLabels> _Mybase;
public:
    DomPrAreWeSizingLabels() : _Mybase() {}
    DomPrAreWeSizingLabels( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrAreWeSizingLabels( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrAreWeSizingLabels() {}

    DEFINE_FIELD( int, sizingLabels, setSizingLabels, sizingLabelsRaw, 2 )
};


// 7,175
class DomPrHeadDotWear : public SessionMessageMap<Printer,PrHeadDotWear>
{
    typedef DomPrHeadDotWear _Myt;
    typedef SessionMessageMap<Printer,PrHeadDotWear> _Mybase;
public:
    DomPrHeadDotWear() : _Mybase() {}
    DomPrHeadDotWear( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrHeadDotWear( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrHeadDotWear() {}

    // CURRENT CODE SUPPORT - No counts of list sizes
    QString& dotsSizeList() {return _Mybase::operator [] (2);}
    QString& dotsStatusList() {return _Mybase::operator [] (3);}
    QString& dotsCountsList() {return _Mybase::operator [] (3);}
};

// 7,179
class DomPrHeadDotStatus : public SessionMessageMap<Printer,PrHeadDotStatus>
{
    typedef DomPrHeadDotStatus _Myt;
    typedef SessionMessageMap<Printer,PrHeadDotStatus> _Mybase;
public:
    DomPrHeadDotStatus() : _Mybase() {}
    DomPrHeadDotStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrHeadDotStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrHeadDotStatus() {}

    DEFINE_FIELD( int, source, setSource, sourceRaw, 2 )
    DEFINE_FIELD( int, destination, setDestination, destinationRaw, 3 )
    DEFINE_FIELD( int, status, setStatus, statusRaw, 4 )
};

//---------------------------------
// SOF-3763 - Printer Cassette Test
//---------------------------------
// 7,183
class DomPrCassetteStatus : public SessionMessageMap<Printer,PrTestCassette>
{
    typedef DomPrCassetteStatus _Myt;
    typedef SessionMessageMap<Printer,PrTestCassette> _Mybase;
public:
    DomPrCassetteStatus() : _Mybase() {}
    DomPrCassetteStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrCassetteStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrCassetteStatus() {}

    QString& measuredLines() {return _Mybase::operator [] (2);}
    QString& actualLines()   {return _Mybase::operator [] (3);}
    QString& testResult()    {return _Mybase::operator [] (4);}
};


// (8,4)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomPrinterPowerupTests : public SessionMessageMap<PrinterSystem,PrSysStatus>
{
    typedef DomPrinterPowerupTests _Myt;
    typedef SessionMessageMap<PrinterSystem,PrSysStatus> _Mybase;
public:
    DomPrinterPowerupTests() : _Mybase() {}
    DomPrinterPowerupTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterPowerupTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterPowerupTests() {}

    QString& printerType() {return _Mybase::operator [] (2);}
    QString& storedChecksum() {return _Mybase::operator [] (3);}
    QString& calculatedChecksum() {return _Mybase::operator [] (4);}
    QString& EEPChecksum() {return _Mybase::operator [] (5);}
    QString& wordTest() {return _Mybase::operator [] (6);}
    QString& addressTest() {return _Mybase::operator [] (7);}
    QString& softwareVersion() {return _Mybase::operator [] (8);}
    QString& hardwareVersion() {return _Mybase::operator [] (9);}
};

// FORM (9,2)	Not supported in Windux or Fastrack
//class DomUltimaWrapperConfig : public SessionMessageMap<Wrapper,WrInquire>
//{
//    typedef DomUltimaWrapperConfig _Myt;
//    typedef SessionMessageMap<Wrapper,WrInquire> _Mybase;
//public:
//    DomUltimaWrapperConfig() : _Mybase() {}
//    DomUltimaWrapperConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
//    DomUltimaWrapperConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
//    virtual ~DomUltimaWrapperConfig() {}
//
//    QString& configType() {return _Mybase::operator [] (2);}
//    QString& pauseTimeout() {return _Mybase::operator [] (3);}
//    QString& idleTimeout() {return _Mybase::operator [] (4);}
//    QString& filmTension() {return _Mybase::operator [] (5);}
//    QString& packWidth() {return _Mybase::operator [] (6);}
//    QString& packageGrandTotal() {return _Mybase::operator [] (7);}
//    QString& totalCycles() {return _Mybase::operator [] (8);}
//    QString& totalElevatorCollapse() {return _Mybase::operator [] (9);}
//    QString& totalIntakeJam() {return _Mybase::operator [] (10);}
//    QString& totalLengthJam() {return _Mybase::operator [] (11);}
//    QString& totalWidthJam() {return _Mybase::operator [] (12);}
//    QString& totalOpenCover() {return _Mybase::operator [] (13);}
//    QString& totalSealerBeltOpen() {return _Mybase::operator [] (14);}
//    QString& totalTooTall() {return _Mybase::operator [] (15);}
//    QString& totalPusherHomeError() {return _Mybase::operator [] (16);}
//    QString& totalFilmPullError() {return _Mybase::operator [] (17);}
//    QString& enableCelsius() {return _Mybase::operator [] (18);}
//    QString& enableWideFilmTest() {return _Mybase::operator [] (19);}
//    QString& enableNarrowFilmTest() {return _Mybase::operator [] (20);}
//    QString& heaterEnable() {return _Mybase::operator [] (21);}
//    QString& heaterSetting() {return _Mybase::operator [] (22);}
//    QString& packageLengthAdjustment() {return _Mybase::operator [] (23);}
//    QString& packageWidthAdjustment() {return _Mybase::operator [] (24);}
//    QString& totalConfigErrors() {return _Mybase::operator [] (25);}
//    QString& packCodeTotals() {return _Mybase::operator [] (26);}
//};

// (9,4)	Not supported in Windux or Fastrack							// existed in all 3 DOMs, so I deleted the Super and Service versions
//class DomUltimaWrapperStatus : public SessionMessageMap<Wrapper,WrStatus>
//{
//    typedef DomUltimaWrapperStatus _Myt;
//    typedef SessionMessageMap<Wrapper,WrStatus> _Mybase;
//public:
//    DomUltimaWrapperStatus() : _Mybase() {}
//    DomUltimaWrapperStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
//    DomUltimaWrapperStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
//    virtual ~DomUltimaWrapperStatus() {}
//
//    QString& filmWidth() {return _Mybase::operator [] (2);}
//    QString& filmTension() {return _Mybase::operator [] (3);}
//    QString& sealerBeltPower() {return _Mybase::operator [] (4);}
//    QString& beltSetting() {return _Mybase::operator [] (5);}
//    QString& beltTemperature() {return _Mybase::operator [] (6);}
//    QString& wrapperRate() {return _Mybase::operator [] (7);}
//    QString& mainMotorOn() {return _Mybase::operator [] (8);}
//};

// FORM (9,15)	Not supported in Windux or Fastrack
//class DomWrapperSensors : public SessionMessageMap<Wrapper,WrErrorHistory>
//{
//    typedef DomWrapperSensors _Myt;
//    typedef SessionMessageMap<Wrapper,WrErrorHistory> _Mybase;
//public:
//    DomWrapperSensors() : _Mybase() {}
//    DomWrapperSensors( const SessionMessage& rhs ) : _Mybase( rhs ) {}
//    DomWrapperSensors( const _Myt& rhs ) : _Mybase( rhs ) {}
//    virtual ~DomWrapperSensors() {}
//
//    QString& heaterReading() {return _Mybase::operator [] (2);}
//    QString& encoderCount() {return _Mybase::operator [] (3);}
//    QString& intakeCount() {return _Mybase::operator [] (4);}
//    QString& stopSwitch() {return _Mybase::operator [] (5);}
//    QString& startSwitch() {return _Mybase::operator [] (6);}
//    QString& pkgLengthSwitch() {return _Mybase::operator [] (7);}
//    QString& pkgSenseSwitch() {return _Mybase::operator [] (8);}
//    QString& pkgWidthSwitch() {return _Mybase::operator [] (9);}
//    QString& pusherHomeSwitch() {return _Mybase::operator [] (10);}
//    QString& pkgJamSwitch() {return _Mybase::operator [] (11);}
//    QString& wideFilmSwitch() {return _Mybase::operator [] (12);}
//    QString& narrowFilmSwitch() {return _Mybase::operator [] (13);}
//    QString& slideBlockState() {return _Mybase::operator [] (14);}
//    QString& heaterState() {return _Mybase::operator [] (15);}
//    QString& compressorState() {return _Mybase::operator [] (16);}
//    QString& mainMotorState() {return _Mybase::operator [] (17);}
//    QString& dischargeMotorState() {return _Mybase::operator [] (18);}
//    QString& filmLengthState() {return _Mybase::operator [] (19);}
//    QString& filmWidthState() {return _Mybase::operator [] (20);}
//    QString& knifeState() {return _Mybase::operator [] (21);}
//    QString& leftSideClampState() {return _Mybase::operator [] (22);}
//    QString& rightSideClampState() {return _Mybase::operator [] (23);}
//    QString& gripperState() {return _Mybase::operator [] (24);}
//};

// FORM (9,18)
//class DomWrapperRecentErrors : public SessionMessageMap<Wrapper,WrErrorHistory>
//{
//    typedef DomWrapperRecentErrors _Myt;
//    typedef SessionMessageMap<Wrapper,WrErrorHistory> _Mybase;
//public:
//    DomWrapperRecentErrors() : _Mybase() {}
//    DomWrapperRecentErrors( const SessionMessage& rhs ) : _Mybase( rhs ) {}
//    DomWrapperRecentErrors( const _Myt& rhs ) : _Mybase( rhs ) {}
//    virtual ~DomWrapperRecentErrors() {}
//
//    QString& numOfItems() {return _Mybase::operator [] (2);}
//    QString& recentErrList() {return _Mybase::operator [] (3);}
//    QString& numOfPackCodes() {return _Mybase::operator [] (4);}
//    QString& packCodeList() {return _Mybase::operator [] (5);}
// };

// (9,51)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomEvolWrapperStatus : public SessionMessageMap<Wrapper,WrEvolStatus>
{
    typedef DomEvolWrapperStatus _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolStatus> _Mybase;
public:
    DomEvolWrapperStatus() : _Mybase() {}
    DomEvolWrapperStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperStatus() {}

    QString& sealerBeltTemp() {return _Mybase::operator [] (2);}
    QString& filmSupplyTemp() {return _Mybase::operator [] (3);}
    QString& ambientTemp() {return _Mybase::operator [] (4);}
    QString& sealerBeltPower() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( bool, mainPower, setMainPower, mainPowerRaw, 6 )
    DEFINE_FIELD( bool, buildingPressure, setBuildingPressure, buildingPressureRaw, 7 )
};

// (9,55)
class DomEvolWrapperSettings : public SessionMessageMap<Wrapper,WrEvolSettings>
{
    typedef DomEvolWrapperSettings _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolSettings> _Mybase;
public:
    DomEvolWrapperSettings() : _Mybase() {}
    DomEvolWrapperSettings( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperSettings( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperSettings() {}

    QString& intakeMode() {return _Mybase::operator [] (2);}
    QString& filmTension() {return _Mybase::operator [] (3);}
    QString& ftFrontToBack() {return _Mybase::operator [] (4);}
    QString& ftLeftToRight() {return _Mybase::operator [] (5);}
    QString& filmSelection() {return _Mybase::operator [] (6);}
    QString& upperRollWidth() {return _Mybase::operator [] (7);}
    QString& lowerRollWidth() {return _Mybase::operator [] (8);}
    QString& sealerBeltPower() {return _Mybase::operator [] (9);}
    QString& sealerBeltSetting() {return _Mybase::operator [] (10);}
    QString& filmSupplyPower() {return _Mybase::operator [] (11);}
    QString& filmSupplySetting() {return _Mybase::operator [] (12);}
};

// (9,56)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomEvolWrapperConfig : public SessionMessageMap<Wrapper,WrEvolInquire>
{
    typedef DomEvolWrapperConfig _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolInquire> _Mybase;
public:
    DomEvolWrapperConfig() : _Mybase() {}
    DomEvolWrapperConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperConfig() {}

    QString& upperRollWidth() {return _Mybase::operator [] (2);}
    QString& lowerRollWidth() {return _Mybase::operator [] (3);}
    QString& sealerBeltSetting() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, packageDirection, setPackageDirection, packageDirectionRaw, 5 )
    QString& primaryPlacementOffset() {return _Mybase::operator [] (6);}
    QString& priRotationCalibOffset() {return _Mybase::operator [] (7);}
    QString& secondCLAAttached() {return _Mybase::operator [] (8);}
    QString& wrapperOperation() {return _Mybase::operator [] (9);}
    DEFINE_FIELD( bool, hardwareAddOn, setHardwareAddOn, hardwareAddOnRaw, 10 )
    QString& secondaryPlacementOffset() {return _Mybase::operator [] (11);}
    DEFINE_FIELD( bool, autoRotation, setAutoRotation, autoRotationRaw, 12 )
    QString& priWandCalibOffset() {return _Mybase::operator [] (13);}
    QString& secWandCalibOffset() {return _Mybase::operator [] (14);}
    QString& sealerBeltPkgSensorType() {return _Mybase::operator [] (15);}
    QString& upperFilmPullTension() {return _Mybase::operator [] (16);}
    QString& lowerFilmPullTension() {return _Mybase::operator [] (17);}
    QString& heaterIsUsed() {return _Mybase::operator [] (18);}
    QString& priRotationCalibrationOffset_90Deg() {return _Mybase::operator [] (19);}
    QString& priRotationCalibrationOffset_180Deg() {return _Mybase::operator [] (20);}
    QString& priRotationCalibrationOffset_270Deg() {return _Mybase::operator [] (21);}
    QString& secRotationCalibrationOffset_90Deg() {return _Mybase::operator [] (22);}
    QString& secRotationCalibrationOffset_180Deg() {return _Mybase::operator [] (23);}
    QString& secRotationCalibrationOffset_270Deg() {return _Mybase::operator [] (24);}
    QString& secRotationCalibOffset() {return _Mybase::operator [] (25);}
    QString& priApplierHandAdjust() {return _Mybase::operator [] (26);}
    QString& secApplierHandAdjust() {return _Mybase::operator [] (27);}
};

// (9,94)
class DomEvolWrapperPause : public SessionMessageMap<Wrapper,WrWrapperPause>
{
    typedef DomEvolWrapperPause _Myt;
    typedef SessionMessageMap<Wrapper,WrWrapperPause> _Mybase;
public:
    DomEvolWrapperPause() : _Mybase() {}
    DomEvolWrapperPause( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperPause( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperPause() {}

    QString& intakeMode() {return _Mybase::operator [] (2);}
    QString& filmTension() {return _Mybase::operator [] (3);}
    QString& ftFrontToBack() {return _Mybase::operator [] (4);}
    QString& ftLeftToRight() {return _Mybase::operator [] (5);}
    QString& filmSelection() {return _Mybase::operator [] (6);}
    QString& upperRollWidth() {return _Mybase::operator [] (7);}
};

// FORM (9,61) (9,66) - In the past these two messages shared this <form> in legacy code.
// HOWEVER, in the future there will be NO SHARING. ALL CLASSES MUST BE UNIQUE
// This is set up to be 9,61. 9,66 will have to be set up in the future when needed.
class DomEvolWrapperErrors : public SessionMessageMap<Wrapper,WrEvolErrorHistory>
{
    typedef DomEvolWrapperErrors _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolErrorHistory> _Mybase;
public:
    DomEvolWrapperErrors() : _Mybase() {}
    DomEvolWrapperErrors( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperErrors( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperErrors() {}

    DEFINE_FIELD( int, totalErrors, setTotalErrors, totalErrorsRaw, 2 )
    QString& firstErrorNum() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numOfErrCode, setNumOfErrCode, numOfErrCodeRaw, 4 )
    QString& evolErrCode() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, numOfErrDesc, setNumOfErrDesc, numOfErrDescRaw, 6 )
    QString& evolErrDesc() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, numOfErrPkgLength, setNumOfErrPkgLength, numOfErrPkgLengthRaw, 8 )
    QString& evolErrPkgLength() {return _Mybase::operator [] (9);}
    DEFINE_FIELD( int, numOfErrPkgWidth, setNumOfErrPkgWidth, numOfErrPkgWidthRaw, 10 )
    QString& evolErrPkgWidth() {return _Mybase::operator [] (11);}
    DEFINE_FIELD( int, numOfErrPkgHeight, setNumOfErrPkgHeight, numOfErrPkgHeightRaw, 12 )
    QString& evolErrPkgHeight() {return _Mybase::operator [] (13);}
    DEFINE_FIELD( int, numOfErrFilmUsed, setNumOfErrFilmUsed, numOfErrFilmUsedRaw, 14 )
    QString& evolErrFilmUsed() {return _Mybase::operator [] (15);}
    DEFINE_FIELD( int, numOfErrBestFit, setNumOfErrBestFit, numOfErrBestFitRaw, 16 )
    QString& evolErrBestFit() {return _Mybase::operator [] (17);}
    DEFINE_FIELD( int, numOfErrPerNet, setNumOfErrPerNet, numOfErrPerNetRaw, 18 )
    QString& evolErrPerNet() {return _Mybase::operator [] (19);}
    DEFINE_FIELD( int, numOfErrTrayName, setNumOfErrTrayName, numOfErrTrayNameRaw, 20 )
    QString& evolErrTrayName() {return _Mybase::operator [] (21);}
    DEFINE_FIELD( int, numOfErrDateTime, setNumOfErrDateTime, numOfErrDateTimeRaw, 22 )
    QString& evolErrDateTime() {return _Mybase::operator [] (23);}
};

// FORM (9,61) (9,66) - In the past these two messages shared this <form> in legacy code.
// HOWEVER, in the future there will be NO SHARING. ALL CLASSES MUST BE UNIQUE
// This is set up to be 9,61. 9,66 will have to be set up in the future when needed.
class DomEvolWrapperErrorsList : public SessionMessageMap<Wrapper,WrEvolErrorHistoryList>
{
    typedef DomEvolWrapperErrorsList _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolErrorHistoryList> _Mybase;
public:
    DomEvolWrapperErrorsList() : _Mybase() {}
    DomEvolWrapperErrorsList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperErrorsList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperErrorsList() {}

    DEFINE_FIELD( int, totalErrors, setTotalErrors, totalErrorsRaw, 2 )
    QString& firstErrorNum() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numOfErrCode, setNumOfErrCode, numOfErrCodeRaw, 4 )
    QString& evolErrCode() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, numOfErrDesc, setNumOfErrDesc, numOfErrDescRaw, 6 )
    QString& evolErrDesc() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, numOfErrPkgLength, setNumOfErrPkgLength, numOfErrPkgLengthRaw, 8 )
    QString& evolErrPkgLength() {return _Mybase::operator [] (9);}
    DEFINE_FIELD( int, numOfErrPkgWidth, setNumOfErrPkgWidth, numOfErrPkgWidthRaw, 10 )
    QString& evolErrPkgWidth() {return _Mybase::operator [] (11);}
    DEFINE_FIELD( int, numOfErrPkgHeight, setNumOfErrPkgHeight, numOfErrPkgHeightRaw, 12 )
    QString& evolErrPkgHeight() {return _Mybase::operator [] (13);}
    DEFINE_FIELD( int, numOfErrFilmUsed, setNumOfErrFilmUsed, numOfErrFilmUsedRaw, 14 )
    QString& evolErrFilmUsed() {return _Mybase::operator [] (15);}
    DEFINE_FIELD( int, numOfErrBestFit, setNumOfErrBestFit, numOfErrBestFitRaw, 16 )
    QString& evolErrBestFit() {return _Mybase::operator [] (17);}
    DEFINE_FIELD( int, numOfErrPerNet, setNumOfErrPerNet, numOfErrPerNetRaw, 18 )
    QString& evolErrPerNet() {return _Mybase::operator [] (19);}
    DEFINE_FIELD( int, numOfErrTrayName, setNumOfErrTrayName, numOfErrTrayNameRaw, 20 )
    QString& evolErrTrayName() {return _Mybase::operator [] (21);}
    DEFINE_FIELD( int, numOfErrDateTime, setNumOfErrDateTime, numOfErrDateTimeRaw, 22 )
    QString& evolErrDateTime() {return _Mybase::operator [] (23);}
};

// FORM (9,115)
class DomEvolWrapperSensors : public SessionMessageMap<Wrapper,WrEvolSensors>
{
    typedef DomEvolWrapperSensors _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolSensors> _Mybase;
public:
    DomEvolWrapperSensors() : _Mybase() {}
    DomEvolWrapperSensors( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperSensors( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperSensors() {}

    DEFINE_FIELD( bool, primaryHome, setPrimaryHome, primaryHomeRaw, 2 )
    DEFINE_FIELD( bool, shiftHome, setShiftHome, shiftHomeRaw, 3 )
    DEFINE_FIELD( bool, shiftLeftLimit, setShiftLeftLimit, shiftLeftLimitRaw, 4 )
    DEFINE_FIELD( bool, gripperHome, setGripperHome, gripperHomeRaw, 5 )
    DEFINE_FIELD( bool, sideFolderHome, setSideFolderHome, sideFolderHomeRaw, 6 )
    DEFINE_FIELD( bool, pusherHome, setPusherHome, pusherHomeRaw, 7 )
    DEFINE_FIELD( bool, frontFolderHome, setFrontFolderHome, frontFolderHomeRaw, 8 )
    DEFINE_FIELD( bool, rearFolderHome, setRearFolderHome, rearFolderHomeRaw, 9 )
    DEFINE_FIELD( bool, elevatorHome, setElevatorHome, elevatorHomeRaw, 10 )
    DEFINE_FIELD( bool, labelRotationHome, setLabelRotationHome, labelRotationHomeRaw, 11 )
    DEFINE_FIELD( bool, elevatorPkg, setElevatorPkg, elevatorPkgRaw, 12 )
    DEFINE_FIELD( bool, transferPkg, setTransferPkg, transferPkgRaw, 13 )
    DEFINE_FIELD( bool, start, setStart, startRaw, 14 )
    DEFINE_FIELD( bool, stop, setStop, stopRaw, 15 )
    DEFINE_FIELD( bool, sideClampRight, setSideClampRight, sideClampRightRaw, 16 )
    DEFINE_FIELD( bool, sideClampHome, setSideClampHome, sideClampHomeRaw, 17 )
    DEFINE_FIELD( bool, filmRoll, setFilmRoll, filmRollRaw, 18 )
    DEFINE_FIELD( bool, knifeUp, setKnifeUp, knifeUpRaw, 19 )

    DEFINE_FIELD( bool, interlockStop, setInterlockStop, interlockStopRaw, 20 )
    DEFINE_FIELD( bool, interlockRightCanopy, setInterlockRightCanopy, interlockRightCanopyRaw, 21 )
    DEFINE_FIELD( bool, interlockRightSideCover, setInterlockRightSideCover, interlockRightSideCoverRaw, 22 )
    DEFINE_FIELD( bool, interlockFilmSupplyDoor, setInterlockFilmSupplyDoor, interlockFilmSupplyDoorRaw, 23 )
    DEFINE_FIELD( bool, interlockLeftSideCover, setInterlockLeftSideCover, interlockLeftSideCoverRaw, 24 )
    DEFINE_FIELD( bool, interlockIntakeCleanout, setInterlockIntakeCleanout, interlockIntakeCleanoutRaw, 25 )

    // SOF-3838  no longer used repurpose for second label applier
    // DEFINE_FIELD( bool, switched24V, setSwitched24V, switched24VRaw, 26 )
    // DEFINE_FIELD( bool, highVoltageInrush, setHighVoltageInrush, highVoltageInrushRaw, 27 )
    DEFINE_FIELD( bool, secWandUp, setSecWandUp, secWandUpRaw, 26 )
    DEFINE_FIELD( bool, secWandDown, setSecWandDown, secWandDownRaw, 27 )

    DEFINE_FIELD( bool, filmSelectorLever, setFilmSelectorLever, filmSelectorLeverRaw, 28 )
    // SOF-3838  no longer used repurpose for second label applier
    // DEFINE_FIELD( bool, printerPlate, setPrinterPlate, printerPlateRaw, 29 )
    DEFINE_FIELD( bool, secApplierHand, setSecApplierHand, secApplierHandRaw, 29 )

    DEFINE_FIELD( bool, filmSelectorUp, setFilmSelectorUp, filmSelectorUpRaw, 30 )
    DEFINE_FIELD( bool, filmSelectorDown, setFilmSelectorDown, filmSelectorDownRaw, 31 )
    DEFINE_FIELD( bool, wandUp, setWandUp, wandUpRaw, 32 )
    DEFINE_FIELD( bool, wandDown, setWandDown, wandDownRaw, 33 )
    DEFINE_FIELD( bool, knifeDown, setKnifeDown, knifeDownRaw, 34 )
    DEFINE_FIELD( bool, applierHand, setApplierHand, applierHandRaw, 35 )
    DEFINE_FIELD( bool, gripperNarrow, setGripperNarrow, gripperNarrowRaw, 36 )
    DEFINE_FIELD( bool, gripperRight, setGripperRight, gripperRightRaw, 37 )
    DEFINE_FIELD( bool, gripperLeft, setGripperLeft, gripperLeftRaw, 38 )
    DEFINE_FIELD( bool, sideClampLeft, setSideClampLeft, sideClampLeftRaw, 39 )

    DEFINE_FIELD( bool, secLabelRotationHome, setSecLabelRotationHome, secLabelRotationHomeRaw, 40 )
    DEFINE_FIELD( bool, secLabelApplierSlideHome, setSecLabelApplierSlideHome, secLabelApplierSlideHomeRaw, 41 )     // Is the 2nd label applier physically at home?

};

// (9,125)														// exists in domOperate and domSuper so the domSuper version deleted
class DomEvolWrapperPkgDesc : public SessionMessageMap<Wrapper,WrEvolPackageDesc>
{
    typedef DomEvolWrapperPkgDesc _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolPackageDesc> _Mybase;
public:
    DomEvolWrapperPkgDesc() : _Mybase() {}
    DomEvolWrapperPkgDesc( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperPkgDesc( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperPkgDesc() {}

    QString& id() {return _Mybase::operator [] (2);}
    QString& length() {return _Mybase::operator [] (3);}
    QString& width() {return _Mybase::operator [] (4);}
    QString& height() {return _Mybase::operator [] (5);}
    QString& lip() {return _Mybase::operator [] (6);}
    QString& filmWidthUsedToWrap() {return _Mybase::operator [] (7);}
    QString& wrapBoxIndex() {return _Mybase::operator [] (8);}
    QString& heightBoxIndex() {return _Mybase::operator [] (9);}
    QString& trayName() {return _Mybase::operator [] (10);}
    QString& usedWrapBoxBestFit() {return _Mybase::operator [] (11);}
    DEFINE_FIELD( bool, modifiedNetStretch, setModifiedNetStretch, modifiedNetStretchRaw, 12 )
};

// FORM (9,139)
class DomEvolWrapperCameraImage : public SessionMessageMap<Wrapper,WrCameraScan>
{
    typedef DomEvolWrapperCameraImage _Myt;
    typedef SessionMessageMap<Wrapper,WrCameraScan> _Mybase;
public:
    DomEvolWrapperCameraImage() : _Mybase() {}
    DomEvolWrapperCameraImage( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperCameraImage( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperCameraImage() {}

    DEFINE_FIELD( int, cameraNum, setCameraNum, cameraNumRaw, 2 )
    DEFINE_FIELD( integerList, cameraImage, setCameraImage, cameraImageRaw, 3 )
};

// FORM (9,140)
class DomEvolWrapperCameraHeightScan : public SessionMessageMap<Wrapper,WrCameraHeightScan>
{
    typedef DomEvolWrapperCameraHeightScan _Myt;
    typedef SessionMessageMap<Wrapper,WrCameraHeightScan> _Mybase;
public:
    DomEvolWrapperCameraHeightScan() : _Mybase() {}
    DomEvolWrapperCameraHeightScan( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperCameraHeightScan( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperCameraHeightScan() {}

    QString& bit050() {return _Mybase::operator [] (2);}
    QString& bit075() {return _Mybase::operator [] (3);}
    QString& bit100() {return _Mybase::operator [] (4);}
    QString& bit125() {return _Mybase::operator [] (5);}
    QString& bit150() {return _Mybase::operator [] (6);}
    QString& bit175() {return _Mybase::operator [] (7);}
    QString& bit200() {return _Mybase::operator [] (8);}
    QString& bit225() {return _Mybase::operator [] (9);}
    QString& bit250() {return _Mybase::operator [] (10);}
    QString& bit300() {return _Mybase::operator [] (11);}
    QString& bit350() {return _Mybase::operator [] (12);}
    QString& bit400() {return _Mybase::operator [] (13);}
    QString& bit450() {return _Mybase::operator [] (14);}
    QString& bit500() {return _Mybase::operator [] (15);}
    QString& bit600() {return _Mybase::operator [] (16);}
};

// FORM (9,141)
class DomEvolWrapperCameraCalibration : public SessionMessageMap<Wrapper,WrCameraCalibration>
{
    typedef DomEvolWrapperCameraCalibration _Myt;
    typedef SessionMessageMap<Wrapper,WrCameraCalibration> _Mybase;
public:
    DomEvolWrapperCameraCalibration() : _Mybase() {}
    DomEvolWrapperCameraCalibration( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperCameraCalibration( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperCameraCalibration() {}

    QString& camera() {return _Mybase::operator [] (2);}
    QString& result() {return _Mybase::operator [] (3);}
    QString& scaleFactor() {return _Mybase::operator [] (4);}
    QString& firstPixel() {return _Mybase::operator [] (5);}
    QString& lastPixel() {return _Mybase::operator [] (6);}
};

// (9,144)														// exists in domOperate and domService, deleted the domService version
class DomEvolWrapperSystemInformation : public SessionMessageMap<Wrapper,WrEvolSystemInfo>
{
    typedef DomEvolWrapperSystemInformation _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolSystemInfo> _Mybase;
public:

    /// Wrapper models.
    enum wrapperModel
    {
        None = 0,
        AWS1RL = 1,
        AWS1LR = 2,
        AWS1MLR = 3,                                ///< AWS manual machine left to right
        AWS1LR_I = 4,                               ///< AWS international left to right
        AWS1RL_I = 5,                               ///< AWS international right to left
        AWS1BWRL = 6,                               ///< AWS Lite (defeatured)right to left
        AWS1BWLR = 7,                               ///< AWS Lite (defeatured)left to right
        AWS2RL = 8,                                 ///< AWS with a Wide Elevator only, right to left
        AWS2LR = 9,                                 ///< AWS with a Wide Elevator only, left to right
        AWS3RL = 10,                                ///< AWS Enhanced, Two roll, right to left
        AWS3LR = 11,                                ///< AWS Enhanced, Two roll, left to right
        AWS3BRL = 12,                               ///< AWS Enhanced, Single roll, right to left
        AWS3BLR = 13,                               ///< AWS Enhanced, Single roll, left to right
        AWS4RL = 14,                                ///< AWS NGW Small Footprint, Two roll, right to left
        AWS4LR = 15,                                ///< AWS NGW Small Footprint, Two roll, left to right
        AWS4BRL = 16,                               ///< AWS NGW Small Footprint, Single roll, right to left
        AWS4BLR = 17,                               ///< AWS NGW Small Footprint, Single roll, left to right
        NGS1BRL = 18,                               ///< NGS Small Footprint, Basic Wrapper, Single roll, right to left (PJR-145 NGW Wrap-Only)
        NGS1BLR = 19,                               ///< NGS Small Footprint, Basic Wrapper, Single roll, left to right (PJR-145 NGW Wrap-Only)
        NGW2_C_RL = 20,                             ///< NGW, Double roll, Full-CAN, Right to Left
        NGW2_C_LR = 21,                             ///< NGW, Double roll, Full-CAN, Left to Right
        NGW1_C_RL = 22,                             ///< NGW, Single roll, Full-CAN, Right to Left
        NGW1_C_LR = 23,                             ///< NGW, Single roll, Full-CAN, Left to Right
        endWrapperModels,

        reducedTimingModels = AWS3RL,
        nextGenModels = AWS4RL
    };

    DomEvolWrapperSystemInformation() : _Mybase() {}
    DomEvolWrapperSystemInformation( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperSystemInformation( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperSystemInformation() {}

    bool isEWS() const {
        return ((AWS1BWRL == awsModelNumber()) || (AWS1BWLR == awsModelNumber()));
    }

    bool isSingleRoll() const {
        return ((AWS1BWRL == awsModelNumber()) || (AWS1BWLR == awsModelNumber()) ||
            (AWS3BRL == awsModelNumber())   || (AWS3BLR == awsModelNumber()) ||
            (AWS4BRL == awsModelNumber())   || (AWS4BLR == awsModelNumber()) ||
            (NGW1_C_RL == awsModelNumber()) || (NGW1_C_LR == awsModelNumber()) ||
            (NGS1BRL == awsModelNumber())   || (NGS1BLR == awsModelNumber()));
    }

    bool isManualLabelApplier() const {
        return ((AWS1BWRL == awsModelNumber()) || (AWS1BWLR == awsModelNumber()));
    }

    bool isDoubleRoll() const {
        return ((None != awsModelNumber()) && (!isSingleRoll()));
    }

    bool isRightToLeft() const {
        return ((AWS1RL == awsModelNumber()) || (AWS1RL_I == awsModelNumber()) || (AWS1BWRL == awsModelNumber()) ||
            (AWS2RL == awsModelNumber())     || (AWS3RL == awsModelNumber()) || (AWS3BRL == awsModelNumber()) ||
            (AWS4RL == awsModelNumber())     || (AWS4BRL == awsModelNumber()) ||
            (NGW2_C_RL == awsModelNumber())  || (NGW1_C_RL == awsModelNumber()) ||
            (NGS1BRL == awsModelNumber()));
    }

    bool isLeftToRight() const {
        return ((None != awsModelNumber()) && (!isRightToLeft()));
    }

    bool isOpenLoop() const {
        return ((None != awsModelNumber()) && (!isClosedLoop()));
    }

    bool isClosedLoop() const {
        return (reducedTimingModels <= awsModelNumber());
    }

    bool isNextGenModel() const {
        return (nextGenModels <= awsModelNumber());
    }

    QString& boardChecksumStatus() {return _Mybase::operator [] (2);}
    QString& awsChecksumStatus() {return _Mybase::operator [] (3);}
    QString& boardPartNumber() {return _Mybase::operator [] (4);}
    QString& boardRev() {return _Mybase::operator [] (5);}
    QString& boardManufactureDate() {return _Mybase::operator [] (6);}
    QString& awsSerialNumber() {return _Mybase::operator [] (7);}
    QString& awsManufactureDate() {return _Mybase::operator [] (8);}
    QString& awsInstallDate() {return _Mybase::operator [] (9);}
    DEFINE_FIELD( int, awsModelNumber, setAwsModelNumber, awsModelNumberRaw, 10 )
    QString& awsWrapperCycleCount() {return _Mybase::operator [] (11);}
};

// FORM (9,165)
class DomEvolWrapperTestComplete : public SessionMessageMap<Wrapper,WrEvolTestComplete>
{
    typedef DomEvolWrapperTestComplete _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolTestComplete> _Mybase;
public:
    DomEvolWrapperTestComplete() : _Mybase() {}
    DomEvolWrapperTestComplete( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperTestComplete( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperTestComplete() {}

    QString& testNum() {return _Mybase::operator [] (2);}
    QString& subTestNum() {return _Mybase::operator [] (3);}
    QString& testPassed() {return _Mybase::operator [] (4);}
};

// (9,171)
class DomEvolWrapperCycleCounter : public SessionMessageMap<Wrapper,WrEvolCycleCount>
{
    typedef DomEvolWrapperCycleCounter _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolCycleCount> _Mybase;
public:
    DomEvolWrapperCycleCounter() : _Mybase() {}
    DomEvolWrapperCycleCounter( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperCycleCounter( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperCycleCounter() {}

    QString& cycleCount() {return _Mybase::operator [] (2);}
};

// FORM (9,175)
class DomEvolWrapperCameraThreshold : public SessionMessageMap<Wrapper,WrEvolCameraThreshold>
{
    typedef DomEvolWrapperCameraThreshold _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolCameraThreshold> _Mybase;
public:
    DomEvolWrapperCameraThreshold() : _Mybase() {}
    DomEvolWrapperCameraThreshold( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperCameraThreshold( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperCameraThreshold() {}

    DEFINE_FIELD( int, threshold, setThreshold, thresholdRaw, 2 )
};

// (9,180)													    // existed in domService as well so I deleted it
class DomEvolWrapperEventCapture : public SessionMessageMap<Wrapper,WrEvolEventCapture>
{
    typedef DomEvolWrapperEventCapture _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolEventCapture> _Mybase;
public:
    DomEvolWrapperEventCapture() : _Mybase() {}
    DomEvolWrapperEventCapture( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperEventCapture( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperEventCapture() {}

    enum eventCaptureType
    {
        KnifeUpEvent,                   // 0
        KnifeDownEvent,
        GripperNarrowCloseEvent,
        GripperNarrowOpenEvent,
        GripperWideLeftCloseEvent,
        GripperWideLeftOpenEvent,       // 5
        GripperWideRightCloseEvent,
        GripperWideRightOpenEvent,
        SideClampLeftCloseEvent,
        SideClampLeftOpenEvent,
        SideClampRightCloseEvent,       // 10
        SideClampRightOpenEvent,
        WandUpEvent,
        WandDownEvent,
        FilmSelectorUpEvent,
        FilmSelectorDownEvent,          // 15
        TankPressureReadingEvent,
        RegulatedPressureReadingEvent,
        ApplierVacuumReadingEvent,
        ApplierHandHomeEvent,
        SystemPressureDropReadingEvent, // 20       // not on actuators screen
        UpperFilmRollEncoderReadingEvent,           // not on actuators screen
        LowerFilmRollEncoderReadingEvent,           // not on actuators screen
        MainACVoltageReadingEvent,      // 23
        // SOF-3838 Support second label applier
        SecondWandUpEvent,
        SecondWandDownEvent,            // 25
        SecondApplierHandHomeEvent,
        SecondApplierVacuumReadingEvent,
        SecondApplierHomeSlideEvent,
        SecondApplierOutSlideEvent,      // 29

        // these are the same exact copy of the items above, but are used for saving
        KnifeUpSavedEvent,              // 30
        KnifeDownSavedEvent,            // 31
        GripperNarrowCloseSavedEvent,
        GripperNarrowOpenSavedEvent,
        GripperWideLeftCloseSavedEvent,
        GripperWideLeftOpenSavedEvent,   // 35
        GripperWideRightCloseSavedEvent, // 36
        GripperWideRightOpenSavedEvent,
        SideClampLeftCloseSavedEvent,
        SideClampLeftOpenSavedEvent,
        SideClampRightCloseSavedEvent,  // 40
        SideClampRightOpenSavedEvent,   // 41
        WandUpSavedEvent,
        WandDownSavedEvent,
        FilmSelectorUpSavedEvent,
        FilmSelectorDownSavedEvent,     // 45
        TankPressureReadingSavedEvent,  // 46
        RegulatedPressureReadingSavedEvent,
        ApplierVacuumReadingSavedEvent,
        ApplierHandHomeSavedEvent,
        SystemPressureDropReadingSavedEvent,   // 50               // not on actuators screen
        UpperFilmRollEncoderReadingSavedEvent, // 41        // not on actuators screen
        LowerFilmRollEncoderReadingSavedEvent,              // not on actuators screen
        MainACVoltageReadingSavedEvent,
        // SOF-3838 Support second label applier
        SecondWandUpSavedEvent,
        SecondWandDownSavedEvent,               // 55
        SecondApplierHandHomeSavedEvent,
        SecondApplierVacuumReadingSavedEvent,
        SecondApplierHomeSlideSavedEvent,
        SecondApplierOutSlideSavedEvent,      // 59

        endCaptureType
    };

    DEFINE_FIELD( int, captureType, setCaptureType, captureTypeRaw, 2 )
    DEFINE_FIELD( int, packageId, setPackageId, packageIdRaw, 3 )
    DEFINE_FIELD( int, value, setValue, valueRaw, 4 )
    DEFINE_FIELD( int, errorNum, setErrorNum, errorNumRaw, 5 )
    QString& eventMonth() {return _Mybase::operator [] (6);}
    QString& eventDay() {return _Mybase::operator [] (7);}
    QString& eventYear() {return _Mybase::operator [] (8);}
    QString& eventHour() {return _Mybase::operator [] (9);}
    QString& eventMinutes() {return _Mybase::operator [] (10);}
    QString& eventSeconds() {return _Mybase::operator [] (11);}
};

// FORM (9,183)
class DomEvolWrapperKnifeHist : public SessionMessageMap<Wrapper,WrKnifeTimeHistory>
{
    typedef DomEvolWrapperKnifeHist _Myt;
    typedef SessionMessageMap<Wrapper,WrKnifeTimeHistory> _Mybase;
public:
    DomEvolWrapperKnifeHist() : _Mybase() {}
    DomEvolWrapperKnifeHist( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperKnifeHist( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperKnifeHist() {}

    DEFINE_FIELD( int, knifeUpTimeInstall, setKnifeUpTimeInstall, knifeUpTimeInstallRaw, 2 )
    DEFINE_FIELD( QString, knifeUpTimeInstallDate, setKnifeUpTimeInstallDate, knifeUpTimeInstallDateRaw, 3 )
    DEFINE_FIELD( int, knifeUpTimeMin, setKnifeUpTimeMin, knifeUpTimeMinRaw, 4 )
    DEFINE_FIELD( QString, knifeUpTimeMinDate, setKnifeUpTimeMinDate, knifeUpTimeMinDateRaw, 5 )
    DEFINE_FIELD( int, knifeUpTimeMax, setKnifeUpTimeMax, knifeUpTimeMaxRaw, 6 )
    DEFINE_FIELD( QString, knifeUpTimeMaxDate, setKnifeUpTimeMaxDate, knifeUpTimeMaxDateRaw, 7 )

    DEFINE_FIELD( int, knifeDownTimeInstall, setKnifeDownTimeInstall, knifeDownTimeInstallRaw, 8 )
    DEFINE_FIELD( QString, knifeDownTimeInstallDate, setKnifeDownTimeInstallDate, knifeDownTimeInstallDateRaw, 9 )
    DEFINE_FIELD( int, knifeDownTimeMin, setKnifeDownTimeMin, knifeDownTimeMinRaw, 10 )
    DEFINE_FIELD( QString, knifeDownTimeMinDate, setKnifeDownTimeMinDate, knifeDownTimeMinDateRaw, 11 )
    DEFINE_FIELD( int, knifeDownTimeMax, setKnifeDownTimeMax, knifeDownTimeMaxRaw, 12 )
    DEFINE_FIELD( QString, knifeDownTimeMaxDate, setKnifeDownTimeMaxDate, knifeDownTimeMaxDateRaw, 13 )

};

// (9,192)
class MadpEvolWrapperMotorProfile : public SessionMessageMap<Wrapper,WrEvolMotorProfile>
{
    typedef MadpEvolWrapperMotorProfile _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolMotorProfile> _Mybase;
public:
    MadpEvolWrapperMotorProfile() : _Mybase() {}
    MadpEvolWrapperMotorProfile( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpEvolWrapperMotorProfile( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpEvolWrapperMotorProfile() {}

    QString& evolMechanism() {return _Mybase::operator [] (2);}
    QString& mechStatus() {return _Mybase::operator [] (3);}
    QString& motorCapacityUsage() {return _Mybase::operator [] (4);}
};

// FORM (9,198)
class DomOneParamAllHeightBoxes : public SessionMessageMap<Wrapper,WrEvolWriteAllForAttribute>
{
    typedef DomOneParamAllHeightBoxes _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolWriteAllForAttribute> _Mybase;
public:
    DomOneParamAllHeightBoxes() : _Mybase() {}
    DomOneParamAllHeightBoxes( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOneParamAllHeightBoxes( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOneParamAllHeightBoxes() {}

    QString& attribute() {return _Mybase::operator [] (2);}
    QString& pluException() {return _Mybase::operator [] (3);}
    QString& numOfItems() {return _Mybase::operator [] (4);}
    QString& precedenceList() {return _Mybase::operator [] (5);}
    QString& num1() {return _Mybase::operator [] (6);}
    QString& heightList() {return _Mybase::operator [] (7);}
    QString& num2() {return _Mybase::operator [] (8);}
    QString& attributeValueList() {return _Mybase::operator [] (9);}
};

// (9,201)
class DomUltimaWrapperSettings : public SessionMessageMap<Wrapper,WrSettings>     // *IMPORTANT NOTE* - existed in all 3 DOMs, so I deleted the domSuper.asp version
{
    typedef DomUltimaWrapperSettings _Myt;
    typedef SessionMessageMap<Wrapper,WrSettings> _Mybase;
public:
    DomUltimaWrapperSettings() : _Mybase() {}
    DomUltimaWrapperSettings( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomUltimaWrapperSettings( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomUltimaWrapperSettings() {}
                                                                                    // I looked do delete the domService.asp version but I COULD NOT because the
    QString& filmWidth() {return _Mybase::operator [] (2);}                              // domService version, while having the same name and message number (9,201),
    QString& filmTension() {return _Mybase::operator [] (3);}                            // the fields for the domService version had a different number of elements
    QString& intakeMode() {return _Mybase::operator [] (4);}                             // and different names. I also grepped all through SERVICE mode itself and
    QString& sealerBeltPower() {return _Mybase::operator [] (5);}                        // tried to find where the domService version of the form was used, and its
    QString& sealerBeltSetting() {return _Mybase::operator [] (6);}                      // NOT. So therefore I finally did delete it domService version of this form
    QString& isCelsius() {return _Mybase::operator [] (7);}                              // though it has a different structure, element names, etc.
};

// FORM (9,203)
class DomWrapperTests : public SessionMessageMap<Wrapper,WrTestSense>
{
    typedef DomWrapperTests _Myt;
    typedef SessionMessageMap<Wrapper,WrTestSense> _Mybase;
public:
    DomWrapperTests() : _Mybase() {}
    DomWrapperTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperTests() {}

    QString& encoderCounts() {return _Mybase::operator [] (2);}
    QString& intakePosition() {return _Mybase::operator [] (3);}
    QString& slideBlock() {return _Mybase::operator [] (4);}
    QString& filmWidth() {return _Mybase::operator [] (5);}
    QString& filmLength() {return _Mybase::operator [] (6);}
    QString& gripper() {return _Mybase::operator [] (7);}
    QString& leftSideClamp() {return _Mybase::operator [] (8);}
    QString& rightSideClamp() {return _Mybase::operator [] (9);}
    QString& knife() {return _Mybase::operator [] (10);}
};

// FORM (9,205)
class DomWrapperErrorTotals : public SessionMessageMap<Wrapper,WrErrorTotals>
{
    typedef DomWrapperErrorTotals _Myt;
    typedef SessionMessageMap<Wrapper,WrErrorTotals> _Mybase;
public:
    DomWrapperErrorTotals() : _Mybase() {}
    DomWrapperErrorTotals( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperErrorTotals( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperErrorTotals() {}

    QString& elevatorCollapse() {return _Mybase::operator [] (2);}
    QString& intakeJams() {return _Mybase::operator [] (3);}
    QString& lengthJams() {return _Mybase::operator [] (4);}
    QString& widthJams() {return _Mybase::operator [] (5);}
    QString& coverOpen() {return _Mybase::operator [] (6);}
    QString& sealerBeltOpen() {return _Mybase::operator [] (7);}
    QString& pkgTooTall() {return _Mybase::operator [] (8);}
    QString& pusherHome() {return _Mybase::operator [] (9);}
    QString& noFilmPulled() {return _Mybase::operator [] (10);}
    QString& dateReset() {return _Mybase::operator [] (11);}
};

// FORM (9,208)
class DomWrapperTotals : public SessionMessageMap<Wrapper,WrPackageTotals>
{
    typedef DomWrapperTotals _Myt;
    typedef SessionMessageMap<Wrapper,WrPackageTotals> _Mybase;
public:
    DomWrapperTotals() : _Mybase() {}
    DomWrapperTotals( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperTotals( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperTotals() {}

    QString& totalPackages() {return _Mybase::operator [] (2);}
    QString& totalJams() {return _Mybase::operator [] (3);}
    QString& totalCycles() {return _Mybase::operator [] (4);}
    QString& dateReset() {return _Mybase::operator [] (5);}
    QString& nameCount() {return _Mybase::operator [] (6);}
    QString& pcNameList() {return _Mybase::operator [] (7);}
    QString& packageCount() {return _Mybase::operator [] (8);}
    QString& pcPackagesList() {return _Mybase::operator [] (9);}
    QString& jamCount() {return _Mybase::operator [] (10);}
    QString& pcJamsList() {return _Mybase::operator [] (11);}
};

// FORM (9,212)
class DomWrapperPackCodeNames : public SessionMessageMap<Wrapper,WrAllPackCodeNames>
{
    typedef DomWrapperPackCodeNames _Myt;
    typedef SessionMessageMap<Wrapper,WrAllPackCodeNames> _Mybase;
public:
    DomWrapperPackCodeNames() : _Mybase() {}
    DomWrapperPackCodeNames( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperPackCodeNames( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperPackCodeNames() {}

    QString& nameCount() {return _Mybase::operator [] (2);}
    QString& pcNameList() {return _Mybase::operator [] (3);}
};

// FORM (9,214)
class DomWrapperTimings : public SessionMessageMap<Wrapper,WrWritePackCode>
{
    typedef DomWrapperTimings _Myt;
    typedef SessionMessageMap<Wrapper,WrWritePackCode> _Mybase;
public:
    DomWrapperTimings() : _Mybase() {}
    DomWrapperTimings( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperTimings( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperTimings() {}

    QString& packCodeValue() {return _Mybase::operator [] (2);}
    QString& description() {return _Mybase::operator [] (3);}
    QString& filmWidth() {return _Mybase::operator [] (4);}
    QString& filmLength() {return _Mybase::operator [] (5);}
    QString& widthMin() {return _Mybase::operator [] (6);}
    QString& widthMax() {return _Mybase::operator [] (7);}
    QString& lengthMin() {return _Mybase::operator [] (8);}
    QString& lengthMax() {return _Mybase::operator [] (9);}
    QString& sideClampOnMin() {return _Mybase::operator [] (10);}
    QString& sideClampOnValue() {return _Mybase::operator [] (11);}
    QString& sideClampOnMax() {return _Mybase::operator [] (12);}
    QString& sideClampOffMin() {return _Mybase::operator [] (13);}
    QString& sideClampOffValue() {return _Mybase::operator [] (14);}
    QString& sideClampOffMax() {return _Mybase::operator [] (15);}
    QString& gripperOnMin() {return _Mybase::operator [] (16);}
    QString& gripperOnValue() {return _Mybase::operator [] (17);}
    QString& gripperOnMax() {return _Mybase::operator [] (18);}
    QString& gripperOffMin() {return _Mybase::operator [] (19);}
    QString& gripperOffValue() {return _Mybase::operator [] (20);}
    QString& gripperOffMax() {return _Mybase::operator [] (21);}
    QString& knifeOffMin() {return _Mybase::operator [] (22);}
    QString& knifeOffValue() {return _Mybase::operator [] (23);}
    QString& knifeOffMax() {return _Mybase::operator [] (24);}
    QString& slideBlock() {return _Mybase::operator [] (25);}
    QString& packageBooster() {return _Mybase::operator [] (26);}
};

// FORM (9,217)
class DomWrapperPackageSize : public SessionMessageMap<Wrapper,WrIntakePkgSize>
{
    typedef DomWrapperPackageSize _Myt;
    typedef SessionMessageMap<Wrapper,WrIntakePkgSize> _Mybase;
public:
    DomWrapperPackageSize() : _Mybase() {}
    DomWrapperPackageSize( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperPackageSize( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperPackageSize() {}

    QString& packageWidth() {return _Mybase::operator [] (2);}
    QString& packageLength() {return _Mybase::operator [] (3);}
};

// FORM (9,222)
class DomWrapperPackageHistory : public SessionMessageMap<Wrapper,WrPackageHistory>
{
    typedef DomWrapperPackageHistory _Myt;
    typedef SessionMessageMap<Wrapper,WrPackageHistory> _Mybase;
public:
    DomWrapperPackageHistory() : _Mybase() {}
    DomWrapperPackageHistory( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperPackageHistory( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperPackageHistory() {}

    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    QString& packCodeNums() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numOfWidths, setNumOfWidths, numOfWidthsRaw, 4 )
    QString& widthsList() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, numOfLengths, setNumOfLengths, numOfLengthsRaw, 6 )
    QString& lengthsList() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, numOfCodes, setNumOfCodes, numOfCodesRaw, 8 )
    QString& packCodesList() {return _Mybase::operator [] (9);}
};

// FORM (9,237)
class DomNewWrapperAllTrays : public SessionMessageMap<Wrapper,WrEvolWriteAllTrayTypes>     // orig b4 costco merge: WrNewWrTimings
{
    typedef DomNewWrapperAllTrays _Myt;
    typedef SessionMessageMap<DEVICE,WrEvolWriteAllTrayTypes> _Mybase;                      // orig b4 costco merge: WrNewWrTimings
public:
    DomNewWrapperAllTrays() : _Mybase() {}
    DomNewWrapperAllTrays( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNewWrapperAllTrays( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNewWrapperAllTrays() {}

    QString& numOfTrays() {return _Mybase::operator [] (2);}
    QString& trayTypes() {return _Mybase::operator [] (3);}
    QString& numOfExceptions() {return _Mybase::operator [] (4);}
    QString& pluExceptions() {return _Mybase::operator [] (5);}
    QString& num3() {return _Mybase::operator [] (6);}
    QString& canChange() {return _Mybase::operator [] (7);}
};

// FORM (9,241)
class DomWrapBoxParams : public SessionMessageMap<Wrapper,WrEvolWriteWrapBox>             // orig b4 costco merge: WrWrapBoxParams
{
    typedef DomWrapBoxParams _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolWriteWrapBox> _Mybase;                       // orig b4 costco merge: WrWrapBoxParams
public:
    DomWrapBoxParams() : _Mybase() {}
    DomWrapBoxParams( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapBoxParams( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapBoxParams() {}

    QString& precedence() {return _Mybase::operator [] (2);}
    QString& trayName() {return _Mybase::operator [] (3);}
    QString& pluException() {return _Mybase::operator [] (4);}
    QString& minLength() {return _Mybase::operator [] (5);}
    QString& maxLength() {return _Mybase::operator [] (6);}
    QString& minWidth() {return _Mybase::operator [] (7);}
    QString& maxWidth() {return _Mybase::operator [] (8);}
    QString& maxLip() {return _Mybase::operator [] (9);}
    QString& numHeightBoxes() {return _Mybase::operator [] (10);}
    QString& heightBoxes() {return _Mybase::operator [] (11);}
};

// FORM (9,247)
class DomHeightBox : public SessionMessageMap<Wrapper,WrHeightBox>
{
    typedef DomHeightBox _Myt;
    typedef SessionMessageMap<Wrapper,WrHeightBox> _Mybase;
public:
    DomHeightBox() : _Mybase() {}
    DomHeightBox( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomHeightBox( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomHeightBox() {}

    QString& precedence() {return _Mybase::operator [] (2);}
    QString& maxHeight() {return _Mybase::operator [] (3);}
    QString& numOfAttributes() {return _Mybase::operator [] (4);}
    QString& wrapAttributes() {return _Mybase::operator [] (5);}
    QString& canDelete() {return _Mybase::operator [] (6);}
};

// FORM (9,252)
class DomEvolWrapperPackageHistory : public SessionMessageMap<Wrapper,WrEvolPackageHistory>
{
    typedef DomEvolWrapperPackageHistory _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolPackageHistory> _Mybase;
public:
    DomEvolWrapperPackageHistory() : _Mybase() {}
    DomEvolWrapperPackageHistory( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperPackageHistory( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperPackageHistory() {}

    DEFINE_FIELD( int, numOfItems, setnumOfItems, numOfItemsRaw, 2 )
    QString& idList() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, num1, setnum1, num1Raw, 4 )
    QString& widthList() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, num2, setnum2, num2Raw, 6 )
    QString& lengthList() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, num3, setnum3, num3Raw, 8 )
    QString& heightList() {return _Mybase::operator [] (9);}
    DEFINE_FIELD( int, num4, setnum4, num4Raw, 10 )
    QString& lipList() {return _Mybase::operator [] (11);}
    DEFINE_FIELD( int, num5, setnum5, num5Raw, 12 )
    QString& filmWidthList() {return _Mybase::operator [] (13);}
    DEFINE_FIELD( int, num6, setnum6, num6Raw, 14 )
    QString& trayNameList() {return _Mybase::operator [] (15);}
    DEFINE_FIELD( int, num7, setnum7, num7Raw, 16 )
    QString& usedWrapBoxBestFitList() {return _Mybase::operator [] (17);}
    DEFINE_FIELD( int, num8, setnum8, num8Raw, 18 )
    QString& modifiedNetStretchList() {return _Mybase::operator [] (19);}
};

// (9,256)
/*
class DomEvolWrapperHeaterConfig : public SessionMessageMap<Wrapper, WrEvolFilmHeater>
{
    typedef DomEvolWrapperHeaterConfig _Myt;
    typedef SessionMessageMap<Wrapper,WrEvolFilmHeater> _Mybase;
public:
    DomEvolWrapperHeaterConfig() : _Mybase() {}
    DomEvolWrapperHeaterConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperHeaterConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperHeaterConfig() {}

    DEFINE_FIELD( int, heaterIsUsed,      setHeaterIsUsed,      heaterIsUsedRaw, 2 )
    DEFINE_FIELD( int, heaterMaxTemp,     setHeaterMaxTemp,     heaterMaxTempRaw, 3 )
    DEFINE_FIELD( int, heaterMinTemp,     setHeaterMinTemp,     heaterMinTempRaw, 4 )
    DEFINE_FIELD( int, heaterDefaultTemp, setHeaterDefaultTemp, heaterDefaultTempRaw, 5 )
    DEFINE_FIELD( int, sealerIsUsed,      setSealerIsUsed,      sealerIsUsedRaw, 6 )
    DEFINE_FIELD( int, sealerMaxTemp,     setSealerMaxTemp,     sealerMaxTempRaw, 7 )
    DEFINE_FIELD( int, sealerMinTemp,     setSealerMinTemp,     sealerMinTempRaw, 8 )
    DEFINE_FIELD( int, sealerDefaultTemp, setSealerDefaultTemp, sealerDefaultTempRaw, 9 )
};
*/
// FORM (10,4)
class DomWrapperPowerUpTests : public SessionMessageMap<WrapperSystem,WrSysStatus>
{
    typedef DomWrapperPowerUpTests _Myt;
    typedef SessionMessageMap<WrapperSystem,WrSysStatus> _Mybase;
public:
    DomWrapperPowerUpTests() : _Mybase() {}
    DomWrapperPowerUpTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperPowerUpTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperPowerUpTests() {}

    QString& storedChecksum() {return _Mybase::operator [] (2);}
    QString& calculatedChecksum() {return _Mybase::operator [] (3);}
    QString& eepChecksumTest() {return _Mybase::operator [] (4);}
    QString& wordTest() {return _Mybase::operator [] (5);}
    QString& addressTest() {return _Mybase::operator [] (6);}
};

// FORM (10,21)
class DomEvolWrapperSystemStatus : public SessionMessageMap<WrapperSystem,SysEvolWrSysStatus>
{
    typedef DomEvolWrapperSystemStatus _Myt;
    typedef SessionMessageMap<WrapperSystem,SysEvolWrSysStatus> _Mybase;
public:
    DomEvolWrapperSystemStatus() : _Mybase() {}
    DomEvolWrapperSystemStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEvolWrapperSystemStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEvolWrapperSystemStatus() {}

    QString& reportSysError() {return _Mybase::operator [] (2);}
    QString& numOfItems() {return _Mybase::operator [] (3);}
    QString& nodeList() {return _Mybase::operator [] (4);}
    QString& num1() {return _Mybase::operator [] (5);}
    QString& appVersionList() {return _Mybase::operator [] (6);}
    QString& num2() {return _Mybase::operator [] (7);}
    QString& dataVersionList() {return _Mybase::operator [] (8);}
    QString& num3() {return _Mybase::operator [] (9);}
    QString& bootVersionList() {return _Mybase::operator [] (10);}
    QString& num4() {return _Mybase::operator [] (11);}
    QString& bitErrorList() {return _Mybase::operator [] (12);}
    QString& num5() {return _Mybase::operator [] (13);}
    QString& deviceInitFlagsList() {return _Mybase::operator [] (14);}
    QString& num6() {return _Mybase::operator [] (15);}
    QString& errorFlagsList() {return _Mybase::operator [] (16);}
    QString& num7() {return _Mybase::operator [] (17);}
    QString& wrapBoxChecksumList() {return _Mybase::operator [] (18);}
};

// (11,100)
class MadpPeripheralList : public SessionMessageMap<System,SysPeripheralList>
{
    typedef MadpPeripheralList _Myt;
    typedef SessionMessageMap<System,SysPeripheralList> _Mybase;
public:

    /// Printer types.
    enum printerType
    {
        ptNone = 0,                                 ///< None.
        ptEPP = 1,                                  ///< EPP2 Evolution Prepack Printer (without the LSMN board)
        ptESS = 2,                                  ///< Evolution Self Service (Not supported)
        ptESC = 3,                                  ///< Evolution Service Counter (Not supported)
        ptAPC = 4,                                  ///< Service counter printer
        ptEPP2 = 5,                                 ///< EPP3 Evolution Prepack Printer (Atmel-based SAM-BA device)
        ptEPP4 = 6,                                 ///< EPP4 Evolution CAN Prepack Printer (Atmel-based SAM-BA device)
        ptKPC  = 9,									///< Service counter printer ARK board
        ptFSSS = 12									///< Free Standing Self-Serve
    };

    /// Label applier types.
    enum labelApplierType
    {
        latNone = 0,                                ///< None.
        latILA = 1,                                 ///< Index label applier (Not supported).
        latCLA = 2,                                 ///< Original Compact label applier (Not supported)
        latEvolLA = 3,                              ///< Evolution label applier for AWS or EWS
        latSLA = 4,                                 ///< Security label applier
        latCLA_EPSAC2 = 5                           ///< USB Compact label applier (New 2013)
    };

    /// Wrapper types.
    enum wrapperType
    {
        wtNone = 0,                                 ///< None.
        wtUltimaWrapper = 1,                        ///< Ultima.
        wtEvolWrapper = 2                           ///< Evolution.
    };

    MadpPeripheralList() : _Mybase() {}
    MadpPeripheralList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpPeripheralList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpPeripheralList() {}

    virtual bool isLabelApplierAttached() const {
        return ( 0 < labelApplier() );
    }

    virtual bool isPrinterAttached() const {
        return (( 0 < firstPrinter() ) || ( 0 < secondPrinter() ) || ( 0 < thirdPrinter() ));
    }

    virtual bool isWeigherAttached() const {
        return (( 0 < firstWeigher() ) || ( 0 < secondWeigher() ));
    }

    virtual bool isWrapperAttached() const {
        return ( 0 < wrapper() );
    }

    DEFINE_FIELD( int, firstWeigher, setFirstWeigher, firstWeigherRaw, 2 )
    DEFINE_FIELD( int, secondWeigher, setSecondWeigher, secondWeigherRaw, 3 )
    DEFINE_FIELD( int, firstPrinter, setFirstPrinter, firstPrinterRaw, 4 )
    DEFINE_FIELD( int, secondPrinter, setSecondPrinter, secondPrinterRaw, 5 )
    DEFINE_FIELD( int, thirdPrinter, setThirdPrinter, thirdPrinterRaw, 6 )
    DEFINE_FIELD( int, labelApplier, setLabelApplier, labelApplierRaw, 7 )
    DEFINE_FIELD( int, wrapper, setWrapper, wrapperRaw, 8 )
    DEFINE_FIELD( int, numeric, setNumeric, numericRaw, 9 )
    DEFINE_FIELD( int, marquee, setMarquee, marqueeRaw, 10 )
    DEFINE_FIELD( int, gcd, setGcd, gcdRaw, 11 )
};

// (14,2)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomScreenConfig : public SessionMessageMap<TouchScreen,TsCalibrateNextStep>
{
    typedef DomScreenConfig _Myt;
    typedef SessionMessageMap<TouchScreen,TsCalibrateNextStep> _Mybase;
public:
    DomScreenConfig() : _Mybase() {}
    DomScreenConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomScreenConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomScreenConfig() {}

    QString& nextStep() {return _Mybase::operator [] (2);}
};

//  (14,5)
class DomScreenCoordinates : public SessionMessageMap<TouchScreen,screenCoordinates>
{
    typedef DomScreenCoordinates _Myt;
    typedef SessionMessageMap<TouchScreen,screenCoordinates> _Mybase;
public:
    DomScreenCoordinates() : _Mybase() {}
    DomScreenCoordinates( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomScreenCoordinates( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomScreenCoordinates() {}

    QString& xValue() {return _Mybase::operator [] (2);}
    QString& yValue() {return _Mybase::operator [] (3);}
};

//  (15,20)													    // existed in domSuper as well so I deleted it in domService
class DomRfidCurrentId : public SessionMessageMap<RFIDReader,RFIDCurrentID>
{
    typedef DomRfidCurrentId _Myt;
    typedef SessionMessageMap<RFIDReader,RFIDCurrentID> _Mybase;
public:
    DomRfidCurrentId() : _Mybase() {}
    DomRfidCurrentId( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRfidCurrentId( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRfidCurrentId() {}

    QString& rfid() {return _Mybase::operator [] (2);}
};

//  (16,2)
class DomSingleScannedCode : public SessionMessageMap<BarcodeScanner,ScannerWrite>
{
    typedef DomSingleScannedCode _Myt;
    typedef SessionMessageMap<BarcodeScanner,ScannerWrite> _Mybase;
public:
    DomSingleScannedCode() : _Mybase() {}
    DomSingleScannedCode( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSingleScannedCode( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSingleScannedCode() {}

    QString& scannedCode() {return _Mybase::operator [] (2);}
    QString& scannedProductNum() {return _Mybase::operator [] (3);}
    QString& scannedDescription() {return _Mybase::operator [] (4);}
};

//  (16,3)
class DomScannerCodeList : public SessionMessageMap<BarcodeScanner,ScannerWriteAll>
{
    typedef DomScannerCodeList _Myt;
    typedef SessionMessageMap<BarcodeScanner,ScannerWriteAll> _Mybase;
public:
    DomScannerCodeList() : _Mybase() {}
    DomScannerCodeList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomScannerCodeList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomScannerCodeList() {}

    QString& numOfCodes() {return _Mybase::operator [] (2);}
    QString& allGs1Codes() {return _Mybase::operator [] (3);}
    QString& numOfProds() {return _Mybase::operator [] (4);}
    QString& allProductNums() {return _Mybase::operator [] (5);}
    QString& numOfDescs() {return _Mybase::operator [] (6);}
    QString& allDescriptions() {return _Mybase::operator [] (7);}
};

//  (16,12)
class DomNewScannedCode : public SessionMessageMap<BarcodeScanner, ScannerCreateNew>
{
    typedef DomNewScannedCode _Myt;
    typedef SessionMessageMap<BarcodeScanner,ScannerCreateNew> _Mybase;
public:
    DomNewScannedCode() : _Mybase() {}
    DomNewScannedCode( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNewScannedCode( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNewScannedCode() {}

    QString& newScannedCode() {return _Mybase::operator [] (2);}
};

//  (16,22)
class DomNewScannedCodeReadPlu : public SessionMessageMap<BarcodeScanner, ScannerReadProduct>
{
    typedef DomNewScannedCodeReadPlu _Myt;
    typedef SessionMessageMap<BarcodeScanner,ScannerReadProduct> _Mybase;
public:
    DomNewScannedCodeReadPlu() : _Mybase() {}
    DomNewScannedCodeReadPlu( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNewScannedCodeReadPlu( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNewScannedCodeReadPlu() {}

    QString& itemNumber() {return _Mybase::operator [] (2);}
};

//  (16,23)
class DomNewScannedCodePlu : public SessionMessageMap<BarcodeScanner, ScannerReadPlu>
{
    typedef DomNewScannedCodePlu _Myt;
    typedef SessionMessageMap<BarcodeScanner,ScannerReadPlu> _Mybase;
public:
    DomNewScannedCodePlu() : _Mybase() {}
    DomNewScannedCodePlu( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNewScannedCodePlu( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNewScannedCodePlu() {}

    QString& itemNumber() {return _Mybase::operator [] (2);}
};

// 17,0
BEGIN_SESSION_MESSAGE( NGWUpgradeBeginMsg, NextGenWrapper, NGWUpgradeBegin )
    DEFINE_RO_FIELD( 2, int, numberOfNodes )
END_SESSION_MESSAGE

// 17,1
BEGIN_SESSION_MESSAGE( NGWUpgradeStatusMsg, NextGenWrapper, NGWUpgradeStatus )
    DEFINE_RO_FIELD( 2, int, nodeId )
    DEFINE_RO_FIELD( 3, int, state )
    DEFINE_RO_FIELD( 4, QString, version )
    DEFINE_RO_FIELD( 5, int, error )

    enum State
    {
        Version = 0,
        Upgrade = 1,
        Error = 2,
        Execute = 3
    };

    enum Error
    {
        None = 0,
        NoResponseFromNode = 1,
        NodeTimeout = 2,
        NodeWriteFailure = 3,
        NodeChecksumFailure = 4,
        NodeEraseBank1Failure = 5,
        NodeEraseBank2Failure = 6,
        NodeIdFailure = 7,
        NodeWriteAddressOutOfRange = 8
    };

END_SESSION_MESSAGE

// 17,2
BEGIN_SESSION_MESSAGE( NGWUpgradeProgressMsg, NextGenWrapper, NGWUpgradeProgress )
    DEFINE_RO_FIELD( 2, int, nodeId )
    DEFINE_RO_FIELD( 3, int, progress )
END_SESSION_MESSAGE

// 17,3
BEGIN_SESSION_MESSAGE( NGWUpgradeEndMsg, NextGenWrapper, NGWUpgradeEnd )
END_SESSION_MESSAGE

// 17,4
BEGIN_SESSION_MESSAGE( NGWUpgradeRequiredMsg, NextGenWrapper, NGWUpgradeRequired )
END_SESSION_MESSAGE

// 17,9
BEGIN_SESSION_MESSAGE( NGWUpgradeInfoMsg, NextGenWrapper, NGWUpgradeInfo )
    DEFINE_RO_FIELD( 2, int, nodeCount )
    DEFINE_RO_FIELD( 3, QString, nodes )
    DEFINE_RO_FIELD( 4, int, nodeVersionCount )
    DEFINE_RO_FIELD( 5, QString, nodeVersions )
    DEFINE_RO_FIELD( 6, int, firmwareVersionCount )
    DEFINE_RO_FIELD( 7, QString, firmwareVersions )
    DEFINE_RO_FIELD( 8, bool, upgradeRequired )
END_SESSION_MESSAGE

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
/// LOADER

// (20,24 (see 17,0) )
class DomKpcLoaderStartUpgradeMsg : public SessionMessageMap<KPCBootloader,KpcBootLoaderStartUpgrade>
{
    typedef DomKpcLoaderStartUpgradeMsg _Myt;
    typedef SessionMessageMap<KPCBootloader,KpcBootLoaderStartUpgrade> _Mybase;
public:
    DomKpcLoaderStartUpgradeMsg() : _Mybase() {}
    DomKpcLoaderStartUpgradeMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcLoaderStartUpgradeMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcLoaderStartUpgradeMsg() {}
};

// (20,25 (also see 17,1) )
class DomKpcLoaderUpgradeStatusMsg : public SessionMessageMap<KPCBootloader,KpcBootLoaderUpgradeStatus>
{
    typedef DomKpcLoaderUpgradeStatusMsg _Myt;
    typedef SessionMessageMap<KPCBootloader,KpcBootLoaderUpgradeStatus> _Mybase;
public:
    enum KPCState               // go to m.state
    {
        BOOT_NO_STATUS,         // Version = 0,
        BOOT_INIT,              // Upgrade = 1,
        BOOT_READY,             // Error = 2,
        BOOT_UPGRADING,         // Execute = 3
        BOOT_FAULT,
        BOOT_VERIFY,
        BOOT_DONE
    };

    enum KPCFaults              // go to m.fault - was KPCError
    {
        NOFAULT,                		// KPCNone = 0,
        TIMEOUT,                		// KPCNoResponseFromNode = 1,
        WRITE_FAILURE,          		// KPCNodeTimeout = 2,
        CHECKSUM,               		// KPCNodeWriteFailure = 3,
        UNKNOWN_RECORD_TYPE,    		// KPCNodeChecksumFailure = 4,
        ERASE_FAILURE_APP,      		// KPCNodeEraseBank1Failure = 5,
        ERASE_FAILURE_BOOT,     		// KPCNodeEraseBank2Failure = 6,
        COMPONENT_NOT_SET,      		// KPCNodeIdFailure = 7,
        IMAGE_MISMATCH,         		// KPCNodeWriteAddressOutOfRange = 8
        IMAGE_CHECKSUM_MISMATCH,
        MODEL_TYPE_ERROR,
        WRITE_ADDRESS_OUT_OF_RANGE,
        FRAME_OUT_OF_RANGE
    };

    DomKpcLoaderUpgradeStatusMsg() : _Mybase() {}
    DomKpcLoaderUpgradeStatusMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcLoaderUpgradeStatusMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcLoaderUpgradeStatusMsg() {}

    DEFINE_FIELD( QString, nodeId, setNodeId, nodeIdRaw, 2 )
    DEFINE_FIELD( QString, state, setState, stateRaw, 3 )
    DEFINE_FIELD( QString, fault, setFault, faultRaw, 4 )                   // version
    DEFINE_FIELD( QString, faultInfo, setFaultInfo, faultInfoRaw, 5 )       // error
};

// (20,26 (see 17,2) )
class DomKpcLoaderUpgradeProgressMsg : public SessionMessageMap<KPCBootloader,KpcBootLoaderUpgradeProgress>
{
    typedef DomKpcLoaderUpgradeProgressMsg _Myt;
    typedef SessionMessageMap<KPCBootloader,KpcBootLoaderUpgradeProgress> _Mybase;
public:
    DomKpcLoaderUpgradeProgressMsg() : _Mybase() {}
    DomKpcLoaderUpgradeProgressMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcLoaderUpgradeProgressMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcLoaderUpgradeProgressMsg() {}

    DEFINE_FIELD( QString, progress, setProgress, progressRaw, 2 )
};


// (20,28 (see 17,4) )
class DomKpcLoaderUpgradeRequiredMsg : public SessionMessageMap<KPCBootloader, KpcBootLoaderUpgradeRequired>
{
    typedef DomKpcLoaderUpgradeRequiredMsg _Myt;
    typedef SessionMessageMap<KPCBootloader, KpcBootLoaderUpgradeRequired> _Mybase;
public:
    DomKpcLoaderUpgradeRequiredMsg() : _Mybase() {}
    DomKpcLoaderUpgradeRequiredMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcLoaderUpgradeRequiredMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcLoaderUpgradeRequiredMsg() {}
};

// 20,31 (like 17,9)
class DomKpcLoaderUpgradeInfoMsg : public SessionMessageMap<KPCBootloader,KpcBootLoaderUpgradeInfo>
{
    typedef DomKpcLoaderUpgradeInfoMsg _Myt;
    typedef SessionMessageMap<KPCBootloader,KpcBootLoaderUpgradeInfo> _Mybase;
public:
    DomKpcLoaderUpgradeInfoMsg() : _Mybase() {}
    DomKpcLoaderUpgradeInfoMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcLoaderUpgradeInfoMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcLoaderUpgradeInfoMsg() {}

    DEFINE_FIELD( QString, currentVersion, setCurrentVersion, currentVersionRaw, 2 )
    DEFINE_FIELD( QString, newVersion, setNewVersion, newVersionRaw, 3 )
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UPGRADER

// (21,24 (see 17,0) )
class DomKpcStartUpgradeMsg : public SessionMessageMap<KPCBootUpdater,KpcBootUpgraderStartUpgrade>
{
    typedef DomKpcStartUpgradeMsg _Myt;
    typedef SessionMessageMap<KPCBootUpdater,KpcBootUpgraderStartUpgrade> _Mybase;
public:
    DomKpcStartUpgradeMsg() : _Mybase() {}
    DomKpcStartUpgradeMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcStartUpgradeMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcStartUpgradeMsg() {}
};

// (21,25 (also see 17,1) )
class DomKpcUpgradeStatusMsg : public SessionMessageMap<KPCBootUpdater,KpcBootUpgraderUpgradeStatus>
{
    typedef DomKpcUpgradeStatusMsg _Myt;
    typedef SessionMessageMap<KPCBootUpdater,KpcBootUpgraderUpgradeStatus> _Mybase;
public:
    enum KPCState               // go to m.state
    {
        BOOT_NO_STATUS,         // Version = 0,
        BOOT_INIT,              // Upgrade = 1,
        BOOT_READY,             // Error = 2,
        BOOT_UPGRADING,         // Execute = 3
        BOOT_FAULT,
        BOOT_VERIFY,
        BOOT_DONE
    };

    enum KPCFaults              // go to m.fault - was KPCError
    {
        NOFAULT,                // KPCNone = 0,
        TIMEOUT,                // KPCNoResponseFromNode = 1,
        WRITE_FAILURE,          // KPCNodeTimeout = 2,
        CHECKSUM,               // KPCNodeWriteFailure = 3,
        UNKNOWN_RECORD_TYPE,    // KPCNodeChecksumFailure = 4,
        ERASE_FAILURE_APP,      // KPCNodeEraseBank1Failure = 5,
        ERASE_FAILURE_BOOT,     // KPCNodeEraseBank2Failure = 6,
        COMPONENT_NOT_SET,      // KPCNodeIdFailure = 7,
        IMAGE_MISMATCH,         // KPCNodeWriteAddressOutOfRange = 8
        IMAGE_CHECKSUM_MISMATCH,
        MODEL_TYPE_ERROR,
        WRITE_ADDRESS_OUT_OF_RANGE,
        FRAME_OUT_OF_RANGE
    };

    DomKpcUpgradeStatusMsg() : _Mybase() {}
    DomKpcUpgradeStatusMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcUpgradeStatusMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcUpgradeStatusMsg() {}

    DEFINE_FIELD( QString, nodeId, setNodeId, nodeIdRaw, 2 )
    DEFINE_FIELD( QString, state, setState, stateRaw, 3 )
    DEFINE_FIELD( QString, fault, setFault, faultRaw, 4 )                   // version
    DEFINE_FIELD( QString, faultInfo, setFaultInfo, faultInfoRaw, 5 )       // error
};

// (21,26 (see 17,2) )
class DomKpcUpgradeProgressMsg : public SessionMessageMap<KPCBootUpdater,KpcBootUpgraderUpgradeProgress>
{
    typedef DomKpcUpgradeProgressMsg _Myt;
    typedef SessionMessageMap<KPCBootUpdater,KpcBootUpgraderUpgradeProgress> _Mybase;
public:
    DomKpcUpgradeProgressMsg() : _Mybase() {}
    DomKpcUpgradeProgressMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcUpgradeProgressMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcUpgradeProgressMsg() {}

    DEFINE_FIELD( QString, progress, setProgress, progressRaw, 2 )
};


// (21,28 (see 17,4) )
class DomKPCUpgradeRequiredMsg : public SessionMessageMap<KPCBootUpdater,KpcBootUpgraderUpgradeRequired>
{
    typedef DomKPCUpgradeRequiredMsg _Myt;
    typedef SessionMessageMap<KPCBootUpdater,KpcBootUpgraderUpgradeRequired> _Mybase;
public:
    DomKPCUpgradeRequiredMsg() : _Mybase() {}
    DomKPCUpgradeRequiredMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKPCUpgradeRequiredMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKPCUpgradeRequiredMsg() {}
};

// 21,31 (like 17,9)
class DomKpcUpgradeInfoMsg : public SessionMessageMap<KPCBootUpdater,KpcBootUpgraderUpgradeInfo>
{
    typedef DomKpcUpgradeInfoMsg _Myt;
    typedef SessionMessageMap<KPCBootUpdater,KpcBootUpgraderUpgradeInfo> _Mybase;
public:
    DomKpcUpgradeInfoMsg() : _Mybase() {}
    DomKpcUpgradeInfoMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomKpcUpgradeInfoMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomKpcUpgradeInfoMsg() {}

    DEFINE_FIELD( QString, currentVersion, setCurrentVersion, currentVersionRaw, 2 )
    DEFINE_FIELD( QString, newVersion, setNewVersion, newVersionRaw, 3 )
};


// SOF-3449
// 23,4 - Board Site test results
class DomBoardSiteTestsCompleteMsg : public SessionMessageMap<BoardSite,BrdSiteBoardTestCompleteUI>
{
    typedef DomBoardSiteTestsCompleteMsg _Myt;
    typedef SessionMessageMap<BoardSite,BrdSiteBoardTestCompleteUI> _Mybase;
public:
    DomBoardSiteTestsCompleteMsg() : _Mybase() {}
    DomBoardSiteTestsCompleteMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomBoardSiteTestsCompleteMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomBoardSiteTestsCompleteMsg() {}

    DEFINE_FIELD( int, arkSerialFlash,  set_arkSerialFlash, arkSerialFlashRaw,  2 )
    DEFINE_FIELD( int, arkSDSlot0,      set_arkSDSlot0,     arkSDSlot0Raw,      3 )
    DEFINE_FIELD( int, arkSDSlot1,      set_arkSDSlot1,     arkSDSlot1Raw,      4 )
    DEFINE_FIELD( int, arkRTC,          set_arkRTC,         arkRTCRaw,          5 )
    DEFINE_FIELD( int, arkUSB,          set_arkUSB,         arkUSBRaw,          6 )
    DEFINE_FIELD( int, arkWifi,         set_arkWifi,        arkWifiRaw,         7 )
    DEFINE_FIELD( int, arkBlueTooth,    set_arkBlueTooth,   arkBlueToothRaw,    8 )
    DEFINE_FIELD( int, arkEthernet,     set_arkEthernet,    arkEthernetRaw,     9 )
    DEFINE_FIELD( int, arkMemory,       set_arkMemory,      arkMemoryRaw,       10 )
    DEFINE_FIELD( int, arkCan0,         set_arkCan0,        arkCan0Raw,         11 )
    DEFINE_FIELD( int, kpcCan,          set_kpcCan,         kpcCanRaw,          12 )
    DEFINE_FIELD( int, kpcModel,        set_kpcModel,       kpcModelRaw,        13 )
    DEFINE_FIELD( int, kpcSerialFlash,  set_kpcSerialFlash, kpcSerialFlashRaw,  14 )
    DEFINE_FIELD( int, kpcValueMax,     set_kpcValueMax,    kpcValueMaxRaw,     15 )
    DEFINE_FIELD( int, kpcCutter,       set_kpcCutter,      kpcCutterRaw,       16 )
    DEFINE_FIELD( int, kpcDotWear,      set_kpcDotWear,     kpcDotWearwRaw,     17 )
};

// SOF-3516
// 23,8 - Board Site test progress
class DomBoardSiteTestsProgressMsg : public SessionMessageMap<BoardSite,BrdSitePercentCompete>
{
    typedef DomBoardSiteTestsProgressMsg _Myt;
    typedef SessionMessageMap<BoardSite,BrdSitePercentCompete> _Mybase;
public:
    DomBoardSiteTestsProgressMsg() : _Mybase() {}
    DomBoardSiteTestsProgressMsg( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomBoardSiteTestsProgressMsg( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomBoardSiteTestsProgressMsg() {}

    DEFINE_FIELD( QString,  percentComplete,    setPercentComplete, percentCompleteRaw, 2 )
    DEFINE_FIELD( int,      testID,             setTestID,          testIDRaw,          3 )
    DEFINE_FIELD( QString,  testDescription,    setTestDescription, testDescriptionRaw, 4 )
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


// (100,2)
class DomCurrentScaleSetup : public SessionMessageMap<SessionDev,SnVersionInfo>
{
    typedef DomCurrentScaleSetup _Myt;
    typedef SessionMessageMap<SessionDev,SnVersionInfo> _Mybase;
public:
    DomCurrentScaleSetup() : _Mybase() {}
    DomCurrentScaleSetup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCurrentScaleSetup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCurrentScaleSetup() {}

    QString& buildNumber() {return _Mybase::operator [] (2);}
    QString& version() {return _Mybase::operator [] (3);}
    QString& revision() {return _Mybase::operator [] (4);}
    QString& osVersion() {return _Mybase::operator [] (5);}
    QString& scaleIpAddress() {return _Mybase::operator [] (6);}
    QString& gatewayAddress() {return _Mybase::operator [] (7);}
    QString& scaleHostName() {return _Mybase::operator [] (8);}
    QString& scaleDNSAddress() {return _Mybase::operator [] (9);}
    QString& lastComm() {return _Mybase::operator [] (10);}
    QString& lastItemComm() {return _Mybase::operator [] (11);}
    QString& smBackendVersion() {return _Mybase::operator [] (12);}
    QString& hwRev() {return _Mybase::operator [] (13);}
    DEFINE_FIELD( QString, features, setFeatures, featuresRaw, 14 )

};

// (100,3)
class DomInactivityTimeout : public SessionMessageMap<SessionDev,SnTimedOut>
{
    typedef DomInactivityTimeout _Myt;
    typedef SessionMessageMap<SessionDev,SnTimedOut> _Mybase;
public:
    DomInactivityTimeout() : _Mybase() {}
    DomInactivityTimeout( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomInactivityTimeout( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomInactivityTimeout() {}
};

// (100,4)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomTerminateScale : public SessionMessageMap<SessionDev,SnTerminate>
{
    typedef DomTerminateScale _Myt;
    typedef SessionMessageMap<SessionDev,SnTerminate> _Mybase;
public:
    DomTerminateScale() : _Mybase() {}
    DomTerminateScale( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTerminateScale( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTerminateScale() {}

    QString& endSession() {return _Mybase::operator [] (2);}
};

// (100,6)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomDateAndTime : public SessionMessageMap<SessionDev,SnWriteDateTime>
{
    typedef DomDateAndTime _Myt;
    typedef SessionMessageMap<SessionDev,SnWriteDateTime> _Mybase;
public:
    DomDateAndTime() : _Mybase() {}
    DomDateAndTime( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomDateAndTime( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomDateAndTime() {}

    DEFINE_FIELD( int, hobYear, setHobYear, hobYearRaw, 2 )
    DEFINE_FIELD( int, hobMonth, setHobMonth, hobMonthRaw, 3 )
    DEFINE_FIELD( int, hobDay, setHobDay, hobDayRaw, 4 )
    DEFINE_FIELD( int, hobHour, setHobHour, hobHourRaw, 5 )
    DEFINE_FIELD( int, hobMinute, setHobMinute, hobMinuteRaw, 6 )
    DEFINE_FIELD( QString, hobAMPM, setHobAMPM, hobAMPMRaw, 7 )
};

// (100,10)
class DomWeighModeTotalType : public SessionMessageMap<SessionDev,SnWriteWeighModeTransactionType>
{
    typedef DomWeighModeTotalType _Myt;
    typedef SessionMessageMap<SessionDev,SnWriteWeighModeTransactionType> _Mybase;
public:
    DomWeighModeTotalType() : _Mybase() {}
    DomWeighModeTotalType( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWeighModeTotalType( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWeighModeTotalType() {}

    DEFINE_FIELD( int, weighMode, setWeighMode, weighModeRaw, 2 )
    DEFINE_FIELD( int, transTypeNum, setTransTypeNum, transTypeNumRaw, 3 )
    DEFINE_FIELD( QString, totalTypeName, setTotalTypeName, totalTypeNameRaw, 4 )
    DEFINE_FIELD( QString, totalTypeName2, setTotalTypeName2, totalTypeName2Raw, 5 )
    DEFINE_FIELD( bool, manualUPCDigitChange, setManualUPCDigitChange, manualUPCDigitChangeRaw, 6 )
    DEFINE_FIELD( bool, allowProductBackdating, setAllowProductBackdating, allowProductBackdatingRaw, 7 )
    DEFINE_FIELD( bool, useBackgroundColor, setUseBackgroundColor, useBackgroundColorRaw, 8 )
    DEFINE_FIELD( int, backgroundColor, setBackgroundColor, backgroundColorRaw, 9 )
};

// (100,23)
class MadpTouchScreenDriver : public SessionMessageMap<SessionDev,SnTouchTypeSetting>
{
    typedef MadpTouchScreenDriver _Myt;
    typedef SessionMessageMap<SessionDev,SnTouchTypeSetting> _Mybase;
public:
    MadpTouchScreenDriver() : _Mybase() {}
    MadpTouchScreenDriver( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpTouchScreenDriver( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpTouchScreenDriver() {}

    QString& touchTypeSetting() {return _Mybase::operator [] (2);}
};

// (100,25)
class DomPrintRequestUpdateMode : public SessionMessageMap<SessionDev,SnPrintKeyPressed>
{
    typedef DomPrintRequestUpdateMode _Myt;
    typedef SessionMessageMap<SessionDev,SnPrintKeyPressed> _Mybase;
public:
    DomPrintRequestUpdateMode() : _Mybase() {}
    DomPrintRequestUpdateMode( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrintRequestUpdateMode( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrintRequestUpdateMode() {}

};

// 100,27
class DomLangChangeData : public SessionMessageMap<SessionDev,SnSelectCurrentLanguage>
{
    typedef DomLangChangeData _Myt;
    typedef SessionMessageMap<SessionDev,SnSelectCurrentLanguage> _Mybase;
public:
    DomLangChangeData() : _Mybase() {}
    DomLangChangeData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLangChangeData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLangChangeData() {}

    QString& lang() {return _Mybase::operator [] (2);}
};

// 100,33														// existed in domOperate and domSuper, so I deleted the Super version
class DomSelfservLogo : public SessionMessageMap<SessionDev,SnWriteStreamingGraphic>
{
    typedef DomSelfservLogo _Myt;
    typedef SessionMessageMap<SessionDev,SnWriteStreamingGraphic> _Mybase;
public:
    DomSelfservLogo() : _Mybase() {}
    DomSelfservLogo( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSelfservLogo( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSelfservLogo() {}

    QString& sgNum() {return _Mybase::operator [] (2);}
    QString& nextsgNum() {return _Mybase::operator [] (3);}
    QString& imageFileName() {return _Mybase::operator [] (4);}
    QString& displayTime() {return _Mybase::operator [] (5);}
    QString& actionType() {return _Mybase::operator [] (6);}
    QString& actionParameter() {return _Mybase::operator [] (7);}
    QString& indexData() {return _Mybase::operator [] (8);}
    QString& imageType() {return _Mybase::operator [] (9);}
};

//  (100,51)
class DomStatistics : public SessionMessageMap<SessionDev,SnWriteStatistics>
{
    typedef DomStatistics _Myt;
    typedef SessionMessageMap<SessionDev,SnWriteStatistics> _Mybase;
public:
    DomStatistics() : _Mybase() {}
    DomStatistics( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomStatistics( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomStatistics() {}

    QString& flashMemoryUsed() {return _Mybase::operator [] (2);}
    QString& flashMemoryFree() {return _Mybase::operator [] (3);}
    QString& ramMemoryUsed() {return _Mybase::operator [] (4);}
    QString& ramMemoryFree() {return _Mybase::operator [] (5);}
    QString& ramDiskUsed() {return _Mybase::operator [] (6);}
    QString& ramDiskFree() {return _Mybase::operator [] (7);}
    QString& evmMemoryUsed() {return _Mybase::operator [] (8);}
    QString& evmMemoryFree() {return _Mybase::operator [] (9);}
    QString& productCount() {return _Mybase::operator [] (10);}
    QString& classCount() {return _Mybase::operator [] (11);}
    QString& expandedTextCount() {return _Mybase::operator [] (12);}
    QString& specialMessageCount() {return _Mybase::operator [] (13);}
    QString& productNoteCount() {return _Mybase::operator [] (14);}
    QString& labelTypeCount() {return _Mybase::operator [] (15);}
    QString& graphicsCount() {return _Mybase::operator [] (16);}
    QString& operatorCount() {return _Mybase::operator [] (17);}
    QString& howManyProducts() {return _Mybase::operator [] (18);}
    QString& nutrifactCount() {return _Mybase::operator [] (19);}
    QString& flashkeyCount() {return _Mybase::operator [] (20);}
    QString& transactionCount() {return _Mybase::operator [] (21);}
    QString& cuttingTestCount() {return _Mybase::operator [] (22);}
};

// (100,63)
class MadpKeyActivation : public SessionMessageMap<SessionDev,SnKeyActivationStates>
{
    typedef MadpKeyActivation _Myt;
    typedef SessionMessageMap<SessionDev,SnKeyActivationStates> _Mybase;
public:
    MadpKeyActivation() : _Mybase() {zero();}
    MadpKeyActivation( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpKeyActivation( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpKeyActivation() {}

    _Myt& operator = ( const _Myt& rhs ) {assignFrom( rhs ); return *this;}
    _Myt& operator &= ( const _Myt& rhs ) {andOp( rhs ); return *this;}
    _Myt& operator |= ( const _Myt& rhs ) {orOp( rhs ); return *this;}

    DEFINE_FIELD( bool, isSupervisorKey, setSupervisorKey, isSupervisorKeyRaw, 2 )
    DEFINE_FIELD( bool, isSupervisorF10, setSupervisorF10, isSupervisorF10Raw, 3 )
    DEFINE_FIELD( bool, isServiceKey, setServiceKey, isServiceKeyRaw, 4 )
    DEFINE_FIELD( bool, isSelfServeKey, setSelfServeKey, isSelfServeKeyRaw, 5 )
    DEFINE_FIELD( bool, isQATestKey, setQATestKey, isQATestKeyRaw, 6 )
    DEFINE_FIELD( bool, isManfTestKey, setManfTestKey, isManfTestKeyRaw, 7 )

protected:
    virtual void zero() {
        setSupervisorKey( false );
        setSupervisorF10( false );
        setServiceKey( false );
        setSelfServeKey( false );
        setQATestKey( false );
        setManfTestKey( false );
    }

    virtual void assignFrom( const _Myt& other ) {
        setSupervisorKey( other.isSupervisorKey() );
        setSupervisorF10( other.isSupervisorF10() );
        setServiceKey( other.isServiceKey() );
        setSelfServeKey( other.isSelfServeKey() );
        setQATestKey( other.isQATestKey() );
        setManfTestKey( other.isManfTestKey() );
    }

    virtual void andOp( const _Myt& other ) {
        setSupervisorKey( (isSupervisorKey() && other.isSupervisorKey()) ? true : false );
        setSupervisorF10( (isSupervisorF10() && other.isSupervisorF10()) ? true : false );
        setServiceKey( (isServiceKey() && other.isServiceKey()) ? true : false );
        setSelfServeKey( (isSelfServeKey() && other.isSelfServeKey()) ? true : false );
        setQATestKey( (isQATestKey() && other.isQATestKey()) ? true : false );
        setManfTestKey( (isManfTestKey() && other.isManfTestKey()) ? true : false );
    }

    virtual void orOp( const _Myt& other ) {
        setSupervisorKey( (isSupervisorKey() || other.isSupervisorKey()) ? true : false );
        setSupervisorF10( (isSupervisorF10() || other.isSupervisorF10()) ? true : false );
        setServiceKey( (isServiceKey() || other.isServiceKey()) ? true : false );
        setSelfServeKey( (isSelfServeKey() || other.isSelfServeKey()) ? true : false );
        setQATestKey( (isQATestKey() || other.isQATestKey()) ? true : false );
        setManfTestKey( (isManfTestKey() || other.isManfTestKey()) ? true : false );
    }

};

// (100,70)
class MadpSetCurrentFrame : public SessionMessageMap<SessionDev,SnSelectActiveWebPageFrame>
{
    typedef MadpSetCurrentFrame _Myt;
    typedef SessionMessageMap<SessionDev,SnSelectActiveWebPageFrame> _Mybase;
public:
    MadpSetCurrentFrame() : _Mybase() {}
    MadpSetCurrentFrame( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpSetCurrentFrame( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpSetCurrentFrame() {}

    QString& currentFrame() {return _Mybase::operator [] (2);}
};

// (100,82)
class DomBrowseInternetSite : public SessionMessageMap<SessionDev,SnSetFlashkeyInternetWebPage>
{
    typedef DomBrowseInternetSite _Myt;
    typedef SessionMessageMap<SessionDev,SnSetFlashkeyInternetWebPage> _Mybase;
public:
    DomBrowseInternetSite() : _Mybase() {}
    DomBrowseInternetSite( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomBrowseInternetSite( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomBrowseInternetSite() {}

    QString& flashkeyPage() {return _Mybase::operator [] (2);}
};

// (100,100)
class DomTestMyMessage : public SessionMessageMap<SessionDev,SnTestMessage>
{
    typedef DomTestMyMessage _Myt;
    typedef SessionMessageMap<SessionDev,SnTestMessage> _Mybase;
public:
    DomTestMyMessage() : _Mybase() {}
    DomTestMyMessage( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTestMyMessage( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTestMyMessage() {}

    QString& message() {return _Mybase::operator [] (2);}
};

// (100,133)
class MadpNetworkInfo : public SessionMessageMap<SessionDev, SnWriteNetworkInfo>
{
    typedef MadpNetworkInfo _Myt;
    typedef SessionMessageMap<SessionDev,SnWriteNetworkInfo> _Mybase;
public:
    MadpNetworkInfo() : _Mybase() {}
    MadpNetworkInfo( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpNetworkInfo( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpNetworkInfo() {}

    QString& message() {return _Mybase::operator [] (2);}
};

// (100,135)
class MadpLicensedPackages : public SessionMessageMap<SessionDev, SnWriteLicensedPackages>
{
    typedef MadpLicensedPackages _Myt;
    typedef SessionMessageMap<SessionDev,SnWriteLicensedPackages> _Mybase;
public:
    MadpLicensedPackages() : _Mybase() {}
    MadpLicensedPackages( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpLicensedPackages( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpLicensedPackages() {}

    DEFINE_FIELD( QString, features, setFeatures, featuresRaw, 2 )
};

// (100,146)
class NowServingQueueManagement : public SessionMessageMap<SessionDev, SnNowServingQueueManagement>
{
    typedef NowServingQueueManagement _Myt;
    typedef SessionMessageMap<SessionDev,SnNowServingQueueManagement> _Mybase;
public:
    NowServingQueueManagement() : _Mybase() {}
    NowServingQueueManagement( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    NowServingQueueManagement( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~NowServingQueueManagement() {}

    DEFINE_FIELD( int, action, setAction, actionRaw, 2 )
    DEFINE_FIELD( int, resultCode, setResultCode, resultCodeRaw, 3 )
    DEFINE_FIELD( QString, value1, setValue1, value1Raw, 4 )
    DEFINE_FIELD( QString, value2, setValue2, value2Raw, 5 )
    DEFINE_FIELD( QString, value3, setValue3, value3Raw, 6 )
    DEFINE_FIELD( QString, value4, setValue4, value4Raw, 7 )

    /// Message action types.
    enum actionType
    {
        Unknown = 0,								///< Unknown (or invalid) action type.

        // customer queue actions
        ServingNextCustomer = 1,					///< Display on GCD now serving customer, pop customer off the queue.
        GetNextCustomerInLine,						///< Retrieve next customer in line, do not adjust the queue.
        DeleteAllCustomersInLine,					///< Delete all queued customers.
        GetCustomerQueueSize,						///< Get customer queue size.
        ServeSpecificCustomer,                      ///< Serve customer X.
        DismissCustomerAttractDialog,				///< Dismiss customer attract dialog box on GCD.
        CancelServeCustomer,						///< Cancel serve of customer, add them to front of queue.

        // printer actions
        PrintCustomerNumber = 26,					///< PRINT next customer number, increment number.
        SetNextCustomerNumber,						///< Set next printed customer number to X.
        GetNextCustomerNumber,						///< Get next printed customer number, DONT INCREMENT, DONT PRINT.
        IncrementCustomerNumber,					///< PULL customer number, increment number. Used for paper rolls.

        // reset action
        ResetAllQueues = 51,						///< Resets all queues.
        ResetQueue									///< Reset next printed customer to X, delete all queued customers.
    };

};

// (100,147)
class MadpUpgradeFirmwareStatus : public SessionMessageMap<SessionDev, SnUpgradeFirmwareStatus>
{
    typedef MadpUpgradeFirmwareStatus _Myt;
    typedef SessionMessageMap<SessionDev,SnUpgradeFirmwareStatus> _Mybase;
public:
    MadpUpgradeFirmwareStatus() : _Mybase() {}
    MadpUpgradeFirmwareStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpUpgradeFirmwareStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpUpgradeFirmwareStatus() {}

    QString& status() {return _Mybase::operator [] (2);}
};

// (100,148)
class MadpUpgradeSFTPBadAuthentication : public SessionMessageMap<SessionDev, SnUpgradeSFTPBadAuthentication>
{
    typedef MadpUpgradeSFTPBadAuthentication _Myt;
    typedef SessionMessageMap<SessionDev,SnUpgradeSFTPBadAuthentication> _Mybase;
public:
    MadpUpgradeSFTPBadAuthentication() : _Mybase() {}
    MadpUpgradeSFTPBadAuthentication( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpUpgradeSFTPBadAuthentication( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpUpgradeSFTPBadAuthentication() {}

    QString& reply() {return _Mybase::operator [] (2);}
};

// 101,3
BEGIN_SESSION_MESSAGE( ProductPrintLabelsPrinted, ProductDev, ProductComplete )
    DEFINE_RO_FIELD( 2, int, recordNumber )
    DEFINE_RO_FIELD( 3, int, pProductTransaction )
    DEFINE_RO_FIELD( 4, QString, isPrintOk )
END_SESSION_MESSAGE

// 101,4
BEGIN_SESSION_MESSAGE( ProductPrintRequestUpdateMode, ProductDev, ProductPrint )
    DEFINE_RO_FIELD( 2, int, isAnimateMyPrint )
    DEFINE_RO_FIELD( 3, QString, bitmapToDisplay )
END_SESSION_MESSAGE

// (101,7)														// existed in domOperate and domSuper, so I deleted the Super version
class DomCurrentPLU : public SessionMessageMap<ProductDev,ProductWrite>
{
    typedef DomCurrentPLU _Myt;
    typedef SessionMessageMap<ProductDev,ProductWrite> _Mybase;
public:
    DomCurrentPLU() : _Mybase() {}
    DomCurrentPLU( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCurrentPLU( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCurrentPLU() {}

    QString& itemNumber() {return _Mybase::operator [] (2);}                    // any integer number
    DEFINE_FIELD( int, productType, setProductType, productTypeRaw, 3 )         // RW=1,FW=2,BC=3,FL=4
    QString& description() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, barcodeType, setBarcodeType, barcodeTypeRaw, 5 )         // UPCA,UPCD3,UPCE(N/A),EAN13,EAN128C,RSS14E,RSS14E_GTIN_13,RSS14E_GTIN_12, RSS14E_GTIN_8,QR_LEVELM,QR_LEVELL,QR_LEVELQ,QR_LEVELH,DATAMATRIX
    DEFINE_FIELD( int, barcodeFields, setBarcodeFields, barcodeFieldsRaw, 6 )   // UseNoFields(x0),UseProductNumber(x01),UseVendorNumber(x02),UseProductAndVendorNumber(3),UseEanCode(x04)
    QString& numberSystem() {return _Mybase::operator [] (7);}                  // UPC Type or EAN Prefix
    QString& productNumber() {return _Mybase::operator [] (8);}                 // product Number or UPC Number or EAN Number
    QString& vendorNumber() {return _Mybase::operator [] (9);}                  // vendor# is vendor#
    QString& eanCode() {return _Mybase::operator [] (10);}                      // The EAN Code
    DEFINE_FIELD( int, byCount, setByCount, byCountRaw, 11 )
    QString& exceptionPrice() {return _Mybase::operator [] (12);}
    QString& unitPrice() {return _Mybase::operator [] (13);}
    QString& discountType() {return _Mybase::operator [] (14);}
    QString& discountValue1() {return _Mybase::operator [] (15);}                 // valid nums: 0 to 6, 0=none
    QString& discountValue2() {return _Mybase::operator [] (16);}
    QString& discountValue3() {return _Mybase::operator [] (17);}
    QString& discountLabel1() {return _Mybase::operator [] (18);}
    QString& discountLabel2() {return _Mybase::operator [] (19);}
    QString& discountLabel3() {return _Mybase::operator [] (20);}
    QString& tare() {return _Mybase::operator [] (21);}
    QString& proportionalTare() {return _Mybase::operator [] (22);}
    QString& fixedWeight() {return _Mybase::operator [] (23);}
    DEFINE_FIELD( int, shelfLifeDays, setShelfLifeDays, shelfLifeDaysRaw, 24 )
    DEFINE_FIELD( int, shelfLifeHours, setShelfLifeHours, shelfLifeHoursRaw, 25 )
    DEFINE_FIELD( int, productLifeDays, setProductLifeDays, productLifeDaysRaw, 26 )
    DEFINE_FIELD( int, productLifeHours, setProductLifeHours, productLifeHoursRaw, 27 )
    QString& priceModifier() {return _Mybase::operator [] (28);}
    QString& reportingClass() {return _Mybase::operator [] (29);}
    DEFINE_FIELD( int, primaryLabelType, setPrimaryLabelType, primaryLabelTypeRaw, 30 )
    DEFINE_FIELD( int, primaryLabelRotation, setPrimaryLabelRotation, primaryLabelRotationRaw, 31 )
    DEFINE_FIELD( int, secondLabelType, setSecondLabelType, secondLabelTypeRaw, 32 )
    DEFINE_FIELD( int, secondLabelRotation, setSecondLabelRotation, secondLabelRotationRaw, 33 )
    DEFINE_FIELD( int, thirdLabelType, setThirdLabelType, thirdLabelTypeRaw, 34 )
    DEFINE_FIELD( int, thirdLabelRotation, setThirdLabelRotation, thirdLabelRotationRaw, 35 )
    DEFINE_FIELD( int, backDateDays, setBackDateDays, backDateDaysRaw, 36 )
    QString& points() {return _Mybase::operator [] (37);}
    QString& graphic1Name() {return _Mybase::operator [] (38);}
    QString& graphic2Name() {return _Mybase::operator [] (39);}
    QString& graphic3Name() {return _Mybase::operator [] (40);}
    QString& graphic4Name() {return _Mybase::operator [] (41);}
    QString& primaryLabelTypeWidth() {return _Mybase::operator [] (42);}
    QString& primaryLabelTypeLength() {return _Mybase::operator [] (43);}
    QString& secondLabelTypeWidth() {return _Mybase::operator [] (44);}
    QString& secondLabelTypeLength() {return _Mybase::operator [] (45);}
    QString& thirdLabelTypeWidth() {return _Mybase::operator [] (46);}
    QString& thirdLabelTypeLength() {return _Mybase::operator [] (47);}
    DEFINE_FIELD( bool, forcedByCount, setForcedByCount, forcedByCountRaw, 48 )
    DEFINE_FIELD( bool, forcedTare, setForcedTare, forcedTareRaw, 49 )
    DEFINE_FIELD( int, fractionalPriceUnit, setFractionalPriceUnit, fractionalPriceUnitRaw, 50 ) // vals: 1, 25, 50, 100, where 1 = 1 lb, 25 = 1/4 lb, 50 = 1/2 lb, , 100 = 100g
    DEFINE_FIELD( int, poundsForAmount, setPoundsForAmount, poundsForAmountRaw, 51 ) // valid vals: 1-9
    DEFINE_FIELD( bool, forcedUnitPrice, setForcedUnitPrice, forcedUnitPriceRaw, 52 )
    QString& tareType() {return _Mybase::operator [] (53);}                       // valid vals: 0,1
    DEFINE_FIELD( bool, applySecurity, setApplySecurity, applySecurityRaw, 54 )   //QString& applySecurity() {return _Mybase::operator [] (54);}
    DEFINE_FIELD( bool, dontPrintPackedOn, setDontPrintPackedOn, dontPrintPackedOnRaw, 55 )
    DEFINE_FIELD( bool, dontPrintSellBy, setDontPrintSellBy, dontPrintSellByRaw, 56 )
    DEFINE_FIELD( bool, dontPrintBestBefore, setDontPrintBestBefore, dontPrintBestBeforeRaw, 57 )
    DEFINE_FIELD( int, primaryLabelPlacement, setPrimaryLabelPlacement, primaryLabelPlacementRaw, 58 )
    DEFINE_FIELD( int, secondLabelPlacement, setSecondLabelPlacement, secondLabelPlacementRaw, 59 )
    DEFINE_FIELD( int, thirdLabelPlacement, setThirdLabelPlacement, thirdLabelPlacementRaw, 60 )
    QString& fractionalPrice() {return _Mybase::operator [] (61);}
    DEFINE_FIELD( bool, forcedFractionalPrice, setForcedFractionalPrice, forcedFractionalPriceRaw, 62 )
    QString& poundsForPrice() {return _Mybase::operator [] (63);}
    DEFINE_FIELD( bool, forcedPoundsFor, setForcedPoundsFor, forcedPoundsForRaw, 64 )
    DEFINE_FIELD( QDate, sellByDate, setSellByDate, sellByDateRaw, 65 )
    QString& sellByTime() {return _Mybase::operator [] (66);}
    DEFINE_FIELD( QDate, bestBeforeDate, setBestBeforeDate, bestBeforeDateRaw, 67 )
    QString& bestBeforeTime() {return _Mybase::operator [] (68);}
    QString& hasNutrifacts() {return _Mybase::operator [] (69);}
    QString& intakeMode() {return _Mybase::operator [] (70);}
    DEFINE_FIELD( bool, hasFreqShopper, setHasFreqShopper, hasFreqShopperRaw, 71 )
    QString& productGraphic() {return _Mybase::operator [] (72);}
    QString& productName1() {return _Mybase::operator [] (73);}
    QString& productName2() {return _Mybase::operator [] (74);}
    QString& categoryNumber() {return _Mybase::operator [] (75);}
    QString& specialType() {return _Mybase::operator [] (76);}
    DEFINE_FIELD( int, coolCategoryNum, setCoolCategoryNum, coolCategoryNumRaw, 77 )
    DEFINE_FIELD( int, coolPretextNum, setCoolPretextNum, coolPretextNumRaw, 78 )
    DEFINE_FIELD( int, coolDefaultTextNum, setCoolDefaultTextNum, coolDefaultTextNumRaw, 79 )
    DEFINE_FIELD( int, coolTextNum, setCoolTextNum, coolTextNumRaw, 80 )
    DEFINE_FIELD( int, coolShortListNum, setCoolShortListNum, coolShortListNumRaw, 81 )
    DEFINE_FIELD( bool, forcedCool, setForcedCool, forcedCoolRaw, 82 )
    DEFINE_FIELD( QString, coolTrackingText, setCoolTrackingText, coolTrackingTextRaw, 83 )
    DEFINE_FIELD( bool, coolRequired, setCoolRequired, coolRequiredRaw, 84 )
    QString& keyboardNetWeight() {return _Mybase::operator [] (85);}
    DEFINE_FIELD( bool, acceptKbdNetWeight, setAcceptKbdNetWeight, acceptKbdNetWeightRaw, 86 )
    DEFINE_FIELD( bool, numOfLabelsForced, setNumOfLabelsForced, numOfLabelsForcedRaw, 87 )
    DEFINE_FIELD( int, numOfLabels, setNumOfLabels, numOfLabelsRaw, 88 )
    DEFINE_FIELD( int, productionPlanningType, setProductionPlanningType, productionPlanningTypeRaw, 89 )
    DEFINE_FIELD( bool, forcedBackDateDays, setForcedBackDateDays, forcedBackDateDaysRaw, 90 )
    DEFINE_FIELD( bool, expirationDaysForced, setExpirationDaysForced, expirationDaysForcedRaw, 91 )
    DEFINE_FIELD( int, expirationDays, setExpirationDays, expirationDaysRaw, 92 )
    QString& eanFormatString() {return _Mybase::operator [] (93);}
    QString& ean128CFormatString() {return _Mybase::operator [] (94);}
    QString& gs1DatabarGTIN() {return _Mybase::operator [] (95);}
    QString& gs1DatabarAI() {return _Mybase::operator [] (96);}
    DEFINE_FIELD( QDate, backdateDate, setBackdateDate, backdateDateRaw, 97 )
    QString& portions() {return _Mybase::operator [] (98);}
    QString& showPortions() {return _Mybase::operator [] (99);}
    DEFINE_FIELD( QString, sideItemsGroupNum, setSideItemsGroupNum, sideItemsGroupNumRaw, 100 )
    DEFINE_FIELD( bool, mealSideItemsFree, setMealSideItemsFree, mealSideItemsFreeRaw, 101 )
    DEFINE_FIELD( QString, sideItemsIncludedInMeal, setSideItemsIncludedInMeal, sideItemsIncludedInMealRaw, 102 )
    // fieldOffsetItemIsBuildAMealSideItem internal Application flag - not used or recogonizes on this side
    DEFINE_FIELD( QString, meatSpecificationSheet, setMeatSpecificationSheet, meatSpecificationSheetRaw, 103 )      // enums start matching here now
    QString& tareB() {return _Mybase::operator [] (104);}
    QString& qrCodeFormatString() {return _Mybase::operator [] (105);}
    QString& dataMatrixFormatString() {return _Mybase::operator [] (106);}
    QString& minWeight() {return _Mybase::operator [] (107);}
    QString& maxWeight() {return _Mybase::operator [] (108);}
    QString& minPrice() {return _Mybase::operator [] (109);}
    QString& maxPrice() {return _Mybase::operator [] (110);}
    DEFINE_FIELD( int, primaryDefaultLabelType, setPrimaryDefaultLabelType, primaryDefaultLabelTypeRaw, 111 )
    DEFINE_FIELD( int, secondDefaultLabelType, setSecondDefaultLabelType, secondDefaultLabelTypeRaw, 112 )
    DEFINE_FIELD( int, thirdDefaultLabelType, setThirdDefaultLabelType, thirdDefaultLabelTypeRaw, 113 )
    DEFINE_FIELD( int, primaryManualLabelType, setPrimaryManualLabelType, primaryManualLabelTypeRaw, 114 )
    DEFINE_FIELD( int, secondManualLabelType, setSecondManualLabelType, secondManualLabelTypeRaw, 115 )
    DEFINE_FIELD( int, thirdManualLabelType, setThirdManualLabelType, thirdManualLabelTypeRaw, 116 )
    DEFINE_FIELD( int, primaryPrepackLabelType, setPrimaryPrepackLabelType, primaryPrepackLabelTypeRaw, 117 )
    DEFINE_FIELD( int, secondPrepackLabelType, setSecondPrepackLabelType, secondPrepackLabelTypeRaw, 118 )
    DEFINE_FIELD( int, thirdPrepackLabelType, setThirdPrepackLabelType, thirdPrepackLabelTypeRaw, 119 )
    DEFINE_FIELD( int, primaryProdEntryLabelType, setPrimaryProdEntryLabelType, primaryProdEntryLabelTypeRaw, 120 )
    DEFINE_FIELD( int, secondProdEntryLabelType, setSecondProdEntryLabelType, secondProdEntryLabelTypeRaw, 121 )
    DEFINE_FIELD( int, thirdProdEntryLabelType, setThirdProdEntryLabelType, thirdProdEntryLabelTypeRaw, 122 )
    DEFINE_FIELD( int, primarySelfServLabelType, setPrimarySelfServLabelType, primarySelfServLabelTypeRaw, 123 )
    DEFINE_FIELD( int, secondSelfServLabelType, setSecondSelfServLabelType, secondSelfServLabelTypeRaw, 124 )
    DEFINE_FIELD( int, thirdSelfServLabelType, setThirdSelfServLabelType, thirdSelfServLabelTypeRaw, 125 )
    DEFINE_FIELD( int, primaryPick5LabelType, setPrimaryPick5LabelType, primaryPick5LabelTypeRaw, 126 )
    DEFINE_FIELD( int, secondPick5LabelType, setSecondPick5LabelType, secondPick5LabelTypeRaw, 127 )
    DEFINE_FIELD( int, thirdPick5LabelType, setThirdPick5LabelType, thirdPick5LabelTypeRaw, 128 )
    DEFINE_FIELD( int, primaryOnlineOrderingLabelType, setPrimaryOnlineOrderingLabelType, primaryOnlineOrderingLabelTypeRaw, 129 )
    DEFINE_FIELD( int, secondOnlineOrderingLabelType, setSecondOnlineOrderingLabelType, secondOnlineOrderingLabelTypeRaw, 130 )
    DEFINE_FIELD( int, thirdOnlineOrderingLabelType, setThirdOnlineOrderingLabelType, thirdOnlineOrderingLabelTypeRaw, 131 )
    DEFINE_FIELD( int, shelfLifeMinutes, setShelfLifeMinutes, shelfLifeMinutesRaw, 132 )
    DEFINE_FIELD( int, productLifeMinutes, setProductLifeMinutes, productLifeMinutesRaw, 133 )
    DEFINE_FIELD( int, labelStockRotation1, setLabelStockRotation1, labelStockRotation1Raw, 134 )
    DEFINE_FIELD( int, labelStockRotation2, setLabelStockRotation2, labelStockRotation2Raw, 135 )
    DEFINE_FIELD( int, labelStockRotation3, setLabelStockRotation3, labelStockRotation3Raw, 136 )
    // Keep EANprefix at the end of this list ALWAYS!
    // EANprefix is for ProductDefaults only. If you want to add something for Product, add it ahead of it.
    QString& EANprefix() {return _Mybase::operator [] (137);}
};

// (101,10)										                // existed in domOperate and domSuper, so I deleted the Super version
class DomPluSupport : public SessionMessageMap<ProductDev,ProductWriteSupportData>
{
    typedef DomPluSupport _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteSupportData> _Mybase;
public:
    DomPluSupport() : _Mybase() {}
    DomPluSupport( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPluSupport( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPluSupport() {}

    QString& pluNum() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( int, textType, setTextType, textTypeRaw, 3 )
    QString& usedByMany() {return _Mybase::operator [] (4);}
    QString& textFunction() {return _Mybase::operator [] (5);}
    QString& supportPLU() {return _Mybase::operator [] (6);}
    QString& supportText() {return _Mybase::operator [] (7);}
    QString& supportText2() {return _Mybase::operator [] (8);}
    QString& supportText3() {return _Mybase::operator [] (9);}
    QString& supportText4() {return _Mybase::operator [] (10);}
    QString& supportText5() {return _Mybase::operator [] (11);}

    QString marqueeSupportTextStripped() const {
        QString result = const_cast<_Myt*>( this )->supportText();
        result.replace( "\\S", "" );
        result.replace( "\\F", "" );
        result.replace( "\\R", "" );
        result.replace( "\\H", "" );
        result.replace( "\\C", "" );
        result.replace( "\\B", "" );
        result.replace( "\\L", "" );
        result.replace( "\\N", "" );
        result.replace( "\\Q", "" );
        result.replace( "\\X", "" );
        return result;
    }
};

// (101,15)
class MadpUpdateCurrentPLUTotals : public SessionMessageMap<ProductDev,ProductWriteCurrentTotalValues>
{
    typedef MadpUpdateCurrentPLUTotals _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteCurrentTotalValues> _Mybase;
public:
    MadpUpdateCurrentPLUTotals() : _Mybase() {}
    MadpUpdateCurrentPLUTotals( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpUpdateCurrentPLUTotals( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpUpdateCurrentPLUTotals() {}

    QString& itemNumber() {return _Mybase::operator [] (2);}
    QString& totalTare() {return _Mybase::operator [] (3);}
    QString& totalNetWeight() {return _Mybase::operator [] (4);}
    QString& totalPrice() {return _Mybase::operator [] (5);}
    QString& unitPrice() {return _Mybase::operator [] (6);}
    //QString& zeroFlag() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( bool, zeroFlag, setZeroFlag, zeroFlagRaw, 7 )
    QString& dualRangeIndicator() {return _Mybase::operator [] (8);}
    QString& poundsForDisplay() {return _Mybase::operator [] (9);}
    QString& fsTotalPrice() {return _Mybase::operator [] (10);}
    QString& fsSavings() {return _Mybase::operator [] (11);}
    QString& fsUnitPrice() {return _Mybase::operator [] (12);}
};

// (101,16)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomUpdateRunModeField : public SessionMessageMap<ProductDev,ProductWriteSingleField>
{
    typedef DomUpdateRunModeField _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteSingleField> _Mybase;
public:
    DomUpdateRunModeField() : _Mybase() {}
    DomUpdateRunModeField( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomUpdateRunModeField( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomUpdateRunModeField() {}

    DEFINE_FIELD( bool, isUpdateLegal, setUpdateLegal, isUpdateLegalRaw, 2 )
    DEFINE_FIELD( int, offsetIndex, setOffsetIndex, offsetIndexRaw, 3 )
    QString& newValue() {return _Mybase::operator [] (4);}
    QString& prevDomElem() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( bool, shouldAlsoPrint, setShouldAlsoPrint, shouldAlsoPrintRaw, 6 )
};

// (101,19)														// existed in domOperate and domSuper, so I deleted the Super version
class DomDefaultCurrentPLU : public SessionMessageMap<ProductDev,ProductWriteDefault>
{
    typedef DomDefaultCurrentPLU _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteDefault> _Mybase;
public:
    DomDefaultCurrentPLU() : _Mybase() {}
    DomDefaultCurrentPLU( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomDefaultCurrentPLU( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomDefaultCurrentPLU() {}

    QString& itemNumber() {return _Mybase::operator [] (2);}                     // any integer number
    DEFINE_FIELD( int, productType, setProductType, productTypeRaw, 3 )          // RW=1,FW=2,BC=3,FL=4
    QString& description() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, barcodeType, setBarcodeType, barcodeTypeRaw, 5 )
    DEFINE_FIELD( int, barcodeFields, setBarcodeFields, barcodeFieldsRaw, 6 )
    QString& numberSystem() {return _Mybase::operator [] (7);}
    QString& productNumber() {return _Mybase::operator [] (8);}
    QString& vendorNumber() {return _Mybase::operator [] (9);}
    QString& eanCode() {return _Mybase::operator [] (10);}
    DEFINE_FIELD( int, byCount, setByCount, byCountRaw, 11 )
    QString& exceptionPrice() {return _Mybase::operator [] (12);}
    QString& unitPrice() {return _Mybase::operator [] (13);}
    DEFINE_FIELD( int, discountType, setDiscountType, discountTypeRaw, 14 )
    QString& discountValue1() {return _Mybase::operator [] (15);}                 // valid nums: 0 to 6, 0=none
    QString& discountValue2() {return _Mybase::operator [] (16);}
    QString& discountValue3() {return _Mybase::operator [] (17);}
    QString& discountLabel1() {return _Mybase::operator [] (18);}
    QString& discountLabel2() {return _Mybase::operator [] (19);}
    QString& discountLabel3() {return _Mybase::operator [] (20);}
    QString& tare() {return _Mybase::operator [] (21);}
    QString& proportionalTare() {return _Mybase::operator [] (22);}
    QString& fixedWeight() {return _Mybase::operator [] (23);}
    DEFINE_FIELD( int, shelfLifeDays, setShelfLifeDays, shelfLifeDaysRaw, 24 )
    DEFINE_FIELD( int, shelfLifeHours, setShelfLifeHours, shelfLifeHoursRaw, 25 )
    DEFINE_FIELD( int, productLifeDays, setProductLifeDays, productLifeDaysRaw, 26 )
    DEFINE_FIELD( int, productLifeHours, setProductLifeHours, productLifeHoursRaw, 27 )
    QString& priceModifier() {return _Mybase::operator [] (28);}
    QString& reportingClass() {return _Mybase::operator [] (29);}
    DEFINE_FIELD( int, primaryLabelType, setPrimaryLabelType, primaryLabelTypeRaw, 30 )
    DEFINE_FIELD( int, primaryLabelRotation, setPrimaryLabelRotation, primaryLabelRotationRaw, 31 )
    DEFINE_FIELD( int, secondLabelType, setSecondLabelType, secondLabelTypeRaw, 32 )
    DEFINE_FIELD( int, secondLabelRotation, setSecondLabelRotation, secondLabelRotationRaw, 33 )
    DEFINE_FIELD( int, thirdLabelType, setThirdLabelType, thirdLabelTypeRaw, 34 )
    DEFINE_FIELD( int, thirdLabelRotation, setThirdLabelRotation, thirdLabelRotationRaw, 35 )
    DEFINE_FIELD( int, backDateDays, setBackDateDays, backDateDaysRaw, 36 )
    QString& points() {return _Mybase::operator [] (37);}
    QString& graphic1Name() {return _Mybase::operator [] (38);}
    QString& graphic2Name() {return _Mybase::operator [] (39);}
    QString& graphic3Name() {return _Mybase::operator [] (40);}
    QString& graphic4Name() {return _Mybase::operator [] (41);}
    QString& primaryLabelTypeWidth() {return _Mybase::operator [] (42);}
    QString& primaryLabelTypeLength() {return _Mybase::operator [] (43);}
    QString& secondLabelTypeWidth() {return _Mybase::operator [] (44);}
    QString& secondLabelTypeLength() {return _Mybase::operator [] (45);}
    QString& thirdLabelTypeWidth() {return _Mybase::operator [] (46);}
    QString& thirdLabelTypeLength() {return _Mybase::operator [] (47);}
    DEFINE_FIELD( bool, forcedByCount, setForcedByCount, forcedByCountRaw, 48 )
    DEFINE_FIELD( bool, forcedTare, setForcedTare, forcedTareRaw, 49 )
    DEFINE_FIELD( int, fractionalPriceUnit, setFractionalPriceUnit, fractionalPriceUnitRaw, 50 )  // vals: 0,1,2, where 0=0.25,1=0.50,2=1.00
    DEFINE_FIELD( int, poundsForAmount, setPoundsForAmount, poundsForAmountRaw, 51 ) 		      // valid vals: 2-9
    DEFINE_FIELD( bool, forcedUnitPrice, setForcedUnitPrice, forcedUnitPriceRaw, 52 )
    QString& tareType() {return _Mybase::operator [] (53);}                       				  // valid vals: 0,1
    DEFINE_FIELD( bool, applySecurity, setApplySecurity, applySecurityRaw, 54 )   //QString& applySecurity() {return _Mybase::operator [] (54);}
    DEFINE_FIELD( bool, dontPrintPackedOn, setDontPrintPackedOn, dontPrintPackedOnRaw, 55 )
    DEFINE_FIELD( bool, dontPrintSellBy, setDontPrintSellBy, dontPrintSellByRaw, 56 )
    DEFINE_FIELD( bool, dontPrintBestBefore, setDontPrintBestBefore, dontPrintBestBeforeRaw, 57 )
    DEFINE_FIELD( int, primaryLabelPlacement, setPrimaryLabelPlacement, primaryLabelPlacementRaw, 58 )
    DEFINE_FIELD( int, secondLabelPlacement, setSecondLabelPlacement, secondLabelPlacementRaw, 59 )
    DEFINE_FIELD( int, thirdLabelPlacement, setThirdLabelPlacement, thirdLabelPlacementRaw, 60 )
    QString& fractionalPrice() {return _Mybase::operator [] (61);}
    DEFINE_FIELD( bool, forcedFractionalPrice, setForcedFractionalPrice, forcedFractionalPriceRaw, 62 )
    QString& poundsForPrice() {return _Mybase::operator [] (63);}
    DEFINE_FIELD( bool,forcedPoundsFor, setForcedPoundsFor, forcedPoundsForRaw, 64 )
    DEFINE_FIELD( QDate, sellByDate, setSellByDate, sellByDateRaw, 65 )
    QString& sellByTime() {return _Mybase::operator [] (66);}
    DEFINE_FIELD( QDate, bestBeforeDate, setBestBeforeDate, bestBeforeDateRaw, 67 )
    QString& bestBeforeTime() {return _Mybase::operator [] (68);}
    QString& hasNutrifacts() {return _Mybase::operator [] (69);}
    QString& intakeMode() {return _Mybase::operator [] (70);}
    DEFINE_FIELD( bool, hasFreqShopper, setHasFreqShopper, hasFreqShopperRaw, 71 )
    QString& productGraphic() {return _Mybase::operator [] (72);}
    QString& productName1() {return _Mybase::operator [] (73);}
    QString& productName2() {return _Mybase::operator [] (74);}
    QString& categoryNumber() {return _Mybase::operator [] (75);}
    QString& specialType() {return _Mybase::operator [] (76);}
    DEFINE_FIELD( int, coolCategoryNum, setCoolCategoryNum, coolCategoryNumRaw, 77 )
    DEFINE_FIELD( int, coolPretextNum, setCoolPretextNum, coolPretextNumRaw, 78 )
    DEFINE_FIELD( int, coolDefaultTextNum, setCoolDefaultTextNum, coolDefaultTextNumRaw, 79 )
    DEFINE_FIELD( int, coolTextNum, setCoolTextNum, coolTextNumRaw, 80 )
    DEFINE_FIELD( int, coolShortListNum, setCoolShortListNum, coolShortListNumRaw, 81 )
    DEFINE_FIELD( bool, forcedCool, setForcedCool, forcedCoolRaw, 82 )
    DEFINE_FIELD( QString, coolTrackingText, setCoolTrackingText, coolTrackingTextRaw, 83 )
    DEFINE_FIELD( bool, coolRequired, setCoolRequired, coolRequiredRaw, 84 )
    QString& keyboardNetWeight() {return _Mybase::operator [] (85);}
    DEFINE_FIELD( bool, acceptKbdNetWeight, setAcceptKbdNetWeight, acceptKbdNetWeightRaw, 86 )
    DEFINE_FIELD( bool, numOfLabelsForced, setNumOfLabelsForced, numOfLabelsForcedRaw, 87 )
    DEFINE_FIELD( int, numOfLabels, setNumOfLabels, numOfLabelsRaw, 88 )
    QString& productionPlanningType() {return _Mybase::operator [] (89);}
    DEFINE_FIELD( bool, forcedBackDateDays, setForcedBackDateDays, forcedBackDateDaysRaw, 90 )
    DEFINE_FIELD( bool, expirationDaysForced, setExpirationDaysForced, expirationDaysForcedRaw, 91 )
    DEFINE_FIELD( int, expirationDays, setExpirationDays, expirationDaysRaw, 92 )
    QString& eanFormatString() {return _Mybase::operator [] (93);}
    QString& ean128CFormatString() {return _Mybase::operator [] (94);}
    QString& gs1DatabarGTIN() {return _Mybase::operator [] (95);}
    QString& gs1DatabarAI() {return _Mybase::operator [] (96);}
    DEFINE_FIELD( QDate, backdateDate, setBackdateDate, backdateDateRaw, 97 )
    QString& portions() {return _Mybase::operator [] (98);}
    QString& showPortions() {return _Mybase::operator [] (99);}
    DEFINE_FIELD( QString, sideItemsGroupNum, setSideItemsGroupNum, sideItemsGroupNumRaw, 100 )
    DEFINE_FIELD( bool, mealSideItemsFree, setMealSideItemsFree, mealSideItemsFreeRaw, 101 )
    DEFINE_FIELD( QString, sideItemsIncludedInMeal, setSideItemsIncludedInMeal, sideItemsIncludedInMealRaw, 102 )
    // fieldOffsetItemIsBuildAMealSideItem internal Application flag - not used or recogonizes on this side
    DEFINE_FIELD( QString, meatSpecificationSheet, setMeatSpecificationSheet, meatSpecificationSheetRaw, 103 )
    QString& tareB() {return _Mybase::operator [] (104);}
    QString& qrCodeFormatString() {return _Mybase::operator [] (105);}
    QString& dataMatrixFormatString() {return _Mybase::operator [] (106);}
    QString& minWeight() {return _Mybase::operator [] (107);}
    QString& maxWeight() {return _Mybase::operator [] (108);}
    QString& minPrice() {return _Mybase::operator [] (109);}
    QString& maxPrice() {return _Mybase::operator [] (110);}
    DEFINE_FIELD( int, primaryDefaultLabelType, setPrimaryDefaultLabelType, primaryDefaultLabelTypeRaw, 111 )
    DEFINE_FIELD( int, secondDefaultLabelType,  setSecondDefaultLabelType,  secondDefaultLabelTypeRaw, 112 )
    DEFINE_FIELD( int, thirdDefaultLabelType,  setThirdDefaultLabelType,  thirdDefaultLabelTypeRaw, 113 )
    DEFINE_FIELD( int, primaryManualLabelType, setPrimaryManualLabelType, primaryManualLabelTypeRaw, 114 )
    DEFINE_FIELD( int, secondManualLabelType, setSecondManualLabelType, secondManualLabelTypeRaw, 115 )
    DEFINE_FIELD( int, thirdManualLabelType, setThirdManualLabelType, thirdManualLabelTypeRaw, 116 )
    DEFINE_FIELD( int, primaryPrepackLabelType, setPrimaryPrepackLabelType, primaryPrepackLabelTypeRaw, 117 )
    DEFINE_FIELD( int, secondPrepackLabelType, setSecondPrepackLabelType, secondPrepackLabelTypeRaw, 118 )
    DEFINE_FIELD( int, thirdPrepackLabelType, setThirdPrepackLabelType, thirdPrepackLabelTypeRaw, 119 )
    DEFINE_FIELD( int, primaryProdEntryLabelType, setPrimaryProdEntryLabelType, primaryProdEntryLabelTypeRaw, 120 )
    DEFINE_FIELD( int, secondProdEntryLabelType, setSecondProdEntryLabelType, secondProdEntryLabelTypeRaw, 121 )
    DEFINE_FIELD( int, thirdProdEntryLabelType, setThirdProdEntryLabelType, thirdProdEntryLabelTypeRaw, 122 )
    DEFINE_FIELD( int, primarySelfServLabelType, setPrimarySelfServLabelType, primarySelfServLabelTypeRaw, 123 )
    DEFINE_FIELD( int, secondSelfServLabelType, setSecondSelfServLabelType, secondSelfServLabelTypeRaw, 124 )
    DEFINE_FIELD( int, thirdSelfServLabelType, setThirdSelfServLabelType, thirdSelfServLabelTypeRaw, 125 )
    DEFINE_FIELD( int, primaryPick5LabelType, setPrimaryPick5LabelType, primaryPick5LabelTypeRaw, 126 )
    DEFINE_FIELD( int, secondPick5LabelType, setSecondPick5LabelType, secondPick5LabelTypeRaw, 127 )
    DEFINE_FIELD( int, thirdPick5LabelType, setThirdPick5LabelType, thirdPick5LabelTypeRaw, 128 )
    DEFINE_FIELD( int, primaryOnlineOrderingLabelType, setPrimaryOnlineOrderingLabelType, primaryOnlineOrderingLabelTypeRaw, 129 )
    DEFINE_FIELD( int, secondOnlineOrderingLabelType, setSecondOnlineOrderingLabelType, secondOnlineOrderingLabelTypeRaw, 130 )
    DEFINE_FIELD( int, thirdOnlineOrderingLabelType, setThirdOnlineOrderingLabelType, thirdOnlineOrderingLabelTypeRaw, 131 )
    DEFINE_FIELD( int, shelfLifeMinutes, setShelfLifeMinutes, shelfLifeMinutesRaw, 132 )
    DEFINE_FIELD( int, productLifeMinutes, setProductLifeMinutes, productLifeMinutesRaw, 133 )
    // Keep EANprefix at the end ALWAYS! EANprefix is for ProductDefaults only. If you want to add something for Product, add it ahead of it.
    QString& EANprefix() {return _Mybase::operator [] (134);}
};

//  (101,23)
class DomUseNameList : public SessionMessageMap<ProductDev,ProductShowLinksToSupportData>
{
    typedef DomUseNameList _Myt;
    typedef SessionMessageMap<ProductDev,ProductShowLinksToSupportData> _Mybase;
public:
    DomUseNameList() : _Mybase() {}
    DomUseNameList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomUseNameList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomUseNameList() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& useList() {return _Mybase::operator [] (3);}
};

//  (101,24)
class DomCreateNewPlu : public SessionMessageMap<ProductDev,ProductCreate>
{
    typedef DomCreateNewPlu _Myt;
    typedef SessionMessageMap<ProductDev,ProductCreate> _Mybase;
public:
    DomCreateNewPlu() : _Mybase() {}
    DomCreateNewPlu( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCreateNewPlu( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCreateNewPlu() {}

    QString& productNumber() {return _Mybase::operator [] (2);}
    QString& recordType() {return _Mybase::operator [] (3);}
    QString& OKresponse() {return _Mybase::operator [] (4);}
};

// (101,29)
class DomPluTreeRecords : public SessionMessageMap<ProductDev,ProductGetTree>
{
    typedef DomPluTreeRecords _Myt;
    typedef SessionMessageMap<ProductDev,ProductGetTree> _Mybase;
public:
    DomPluTreeRecords() : _Mybase() {}
    DomPluTreeRecords( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPluTreeRecords( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPluTreeRecords() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& pluList() {return _Mybase::operator [] (3);}
};

// (101,30)
class DomOperateSearch : public SessionMessageMap<ProductDev,ProductFindByDescription>
{
    typedef DomOperateSearch _Myt;
    typedef SessionMessageMap<ProductDev,ProductFindByDescription> _Mybase;
public:
    DomOperateSearch() : _Mybase() {}
    DomOperateSearch( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperateSearch( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperateSearch() {}

    //QString& numOfPlues() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( int, numOfPlues, setNumOfPlues, numOfPluesRaw, 2 )
    QString& pluList() {return _Mybase::operator [] (3);}
    //QString& numOfPlueDescs() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, numOfPlueDescs, setNumOfPlueDescs, numOfPlueDescsRaw, 4 )
    QString& plueDescList() {return _Mybase::operator [] (5);}
    QString& searchString() {return _Mybase::operator [] (6);}
    QString& searchType() {return _Mybase::operator [] (7);}
};

// (101,34 )													// existed in domOperate and domSuper, so I deleted the Super version
class DomLabelSizeInfo : public SessionMessageMap<ProductDev,ProductWriteLabelFormSize>
{
    typedef DomLabelSizeInfo _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteLabelFormSize> _Mybase;
public:
    DomLabelSizeInfo() : _Mybase() {}
    DomLabelSizeInfo( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelSizeInfo( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelSizeInfo() {}

    QString& pluNumber() {return _Mybase::operator [] (2);}
    QString& textType() {return _Mybase::operator [] (3);}
    QString& labelHeight() {return _Mybase::operator [] (4);} // switched these according to domSuper.asp
    QString& labelWidth() {return _Mybase::operator [] (5);}  // switched these according to domSuper.asp
    QString& labelFontName() {return _Mybase::operator [] (6);}
    QString& labelFontSize() {return _Mybase::operator [] (7);}
};

//(101,42)
class DomProdEntryPrintBegin : public SessionMessageMap<ProductDev,ProductStartPrintAllLabels>
{
    typedef DomProdEntryPrintBegin _Myt;
    typedef SessionMessageMap<ProductDev,ProductStartPrintAllLabels> _Mybase;
public:
    DomProdEntryPrintBegin() : _Mybase() {}
    DomProdEntryPrintBegin( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProdEntryPrintBegin( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProdEntryPrintBegin() {}

};

//(101,43)
class DomProdEntryPrintStop : public SessionMessageMap<ProductDev,ProductStopPrintAllLabels>
{
    typedef DomProdEntryPrintStop _Myt;
    typedef SessionMessageMap<ProductDev,ProductStopPrintAllLabels> _Mybase;
public:
    DomProdEntryPrintStop() : _Mybase() {}
    DomProdEntryPrintStop( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProdEntryPrintStop( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProdEntryPrintStop() {}

};


//  (101,47)
class DomProductClearUI : public SessionMessageMap<ProductDev,ProductClearUI>
{
    typedef DomProductClearUI _Myt;
    typedef SessionMessageMap<ProductDev,ProductClearUI> _Mybase;
public:
    DomProductClearUI() : _Mybase() {}
    DomProductClearUI( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProductClearUI( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProductClearUI() {}

    DEFINE_FIELD( bool, returnToStartScreen, setReturnToStartScreen, returnToStartScreenRaw, 2 )
};

//  (101,53)
class DomSelfServProdGraphics : public SessionMessageMap<ProductDev,ProductWriteAllSoftKeyGraphicNames>
{
    typedef DomSelfServProdGraphics _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteAllSoftKeyGraphicNames> _Mybase;
public:
    DomSelfServProdGraphics() : _Mybase() {}
    DomSelfServProdGraphics( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSelfServProdGraphics( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSelfServProdGraphics() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& graphicsList() {return _Mybase::operator [] (3);}
};

// (101,63)
class DomShelfLifeExpiration : public SessionMessageMap<ProductDev,ProductWriteAllExpirationLabels>
{
    typedef DomShelfLifeExpiration _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteAllExpirationLabels> _Mybase;
public:
    DomShelfLifeExpiration() : _Mybase() {}
    DomShelfLifeExpiration( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomShelfLifeExpiration( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomShelfLifeExpiration() {}

    //QString& numOfPLUs() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( int, numOfPLUs, setNumOfPLUs, numOfPLUsRaw, 2 )
    QString& pluNumList() {return _Mybase::operator [] (3);}
    //QString& numOfDaysExpired() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, numOfDaysExpired, setNumOfDaysExpired, numOfDaysExpiredRaw, 4 )
    QString& daysExpiredList() {return _Mybase::operator [] (5);}
    //QString& numOfExpiredTimes() {return _Mybase::operator [] (6);}
    DEFINE_FIELD( int, numOfExpiredTimes, setNumOfExpiredTimes, numOfExpiredTimesRaw, 6 )
    QString& expiredTimeList() {return _Mybase::operator [] (7);}
    //QString& numOfDescs() {return _Mybase::operator [] (8);}
    DEFINE_FIELD( int, numOfDescs, setNumOfDescs, numOfDescsRaw, 8 )
    QString& descList() {return _Mybase::operator [] (9);}
};

// (101,80)
class DomAccumulatedItems : public SessionMessageMap<ProductDev,ProductWriteAllAccumulatedItems>
{
    typedef DomAccumulatedItems _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteAllAccumulatedItems> _Mybase;
public:
    DomAccumulatedItems() : _Mybase() {}
    DomAccumulatedItems( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomAccumulatedItems( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomAccumulatedItems() {}

    QString& totalPrice() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( int, numberLineNums, setNumberLineNums, numberLineNumsRaw, 3 )
    QString& listLineNums() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, numberPluNumbers, setNumberPluNumbers, numberPluNumbersRaw, 5 )
    QString& listPluNumbers() {return _Mybase::operator [] (6);}
    DEFINE_FIELD( int, numberPluDescs, setNumberPluDescs, numberPluDescsRaw, 7 )
    QString& listPluDescs() {return _Mybase::operator [] (8);}
    DEFINE_FIELD( int, numberByCountWeights, setNumberByCountWeights, numberByCountWeightsRaw, 9 )
    QString& listByCountWeights() {return _Mybase::operator [] (10);}
    DEFINE_FIELD( int, numberUnitPrices, setNumberUnitPrices, numberUnitPricesRaw, 11 )
    QString& listUnitPrices() {return _Mybase::operator [] (12);}
    DEFINE_FIELD( int, numberTotalPrices, setNumberTotalPrices, numberTotalPricesRaw, 13 )
    QString& listTotalPrices() {return _Mybase::operator [] (14);}
};

//(101,95)
class DomProdEntryPrintPause : public SessionMessageMap<ProductDev,ProductPausePrintAllLabels>
{
    typedef DomProdEntryPrintPause _Myt;
    typedef SessionMessageMap<ProductDev,ProductPausePrintAllLabels> _Mybase;
public:
    DomProdEntryPrintPause() : _Mybase() {}
    DomProdEntryPrintPause( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProdEntryPrintPause( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProdEntryPrintPause() {}

};

//101,97
class DomOnlineOrderingProductTotals : public SessionMessageMap<ProductDev,ProductWriteOnlineOrderingProductTotals>
{
    typedef DomOnlineOrderingProductTotals _Myt;
    typedef SessionMessageMap<ProductDev,ProductWriteOnlineOrderingProductTotals> _Mybase;
public:
    DomOnlineOrderingProductTotals() : _Mybase() {}
    DomOnlineOrderingProductTotals( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOnlineOrderingProductTotals( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOnlineOrderingProductTotals() {}

    QString& orderNumber() {return _Mybase::operator [] (2);}
    QString& orderProductId() {return _Mybase::operator [] (3);}
    QString& pluNumber() {return _Mybase::operator [] (4);}
    QString& totalNetWeight() {return _Mybase::operator [] (5);}
    QString& totalByCount() {return _Mybase::operator [] (6);}
    QString& totalPrice() {return _Mybase::operator [] (7);}

};

//  (102,2)
class DomSingleOperator : public SessionMessageMap<OperatorDev,OpDtWrite>
{
    typedef DomSingleOperator _Myt;
    typedef SessionMessageMap<OperatorDev,OpDtWrite> _Mybase;
public:
    DomSingleOperator() : _Mybase() {}
    DomSingleOperator( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSingleOperator( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSingleOperator() {}

    DEFINE_RW_FIELD( 2, QString, operatorID, setOperatorID )
    DEFINE_RW_FIELD( 3, QString, operatorRFID, setOperatorRFID )
    DEFINE_RW_FIELD( 4, QString, operatorName, setOperatorName )
    DEFINE_RW_FIELD( 5, int, operatorAccessLevel, setOperatorAccessLevel )
    DEFINE_RW_FIELD( 6, int, preferredLanguage, setPreferredLanguage )

};

//  (102,3)
class DomOperatorInfomation : public SessionMessageMap<OperatorDev,OpDtWriteAll>
{
    typedef DomOperatorInfomation _Myt;
    typedef SessionMessageMap<OperatorDev,OpDtWriteAll> _Mybase;
public:
    DomOperatorInfomation() : _Mybase() {}
    DomOperatorInfomation( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorInfomation( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorInfomation() {}

    DEFINE_RO_FIELD( 2, int, numOfIDs )
    DEFINE_RO_FIELD( 3, QString, allOperatorIDs )
    DEFINE_RO_FIELD( 4, int, numOfRFIDs )
    DEFINE_RO_FIELD( 5, QString, allOperatorRFIDs )
    DEFINE_RO_FIELD( 6, int, numOfNames )
    DEFINE_RO_FIELD( 7, QString, allOperatorNames )
    DEFINE_RO_FIELD( 8, int, numOfAccess )
    DEFINE_RO_FIELD( 9, QString, allAccess )
    DEFINE_RO_FIELD( 10, int, numOfLangs )
    DEFINE_RO_FIELD( 11, QString, allLanguages )
};

//(102,20)
class DomOperatorLoginData : public SessionMessageMap<OperatorDev,OpLogin>
{
    typedef DomOperatorLoginData _Myt;
    typedef SessionMessageMap<OperatorDev,OpLogin> _Mybase;
public:
    DomOperatorLoginData() : _Mybase() {}
    DomOperatorLoginData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorLoginData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorLoginData() {}

    QString& operatorID() {return _Mybase::operator [] (2);}
    QString& operatorRFID() {return _Mybase::operator [] (3);}
    QString& operatorName() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, operatorAccessLevel, setOperatorAccessLevel, operatorAccessLevelRaw, 5 )
    QString& preferredLanguage() {return _Mybase::operator [] (6);}
    QString& operatorSawNote() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, operatorMode, setOperatorMode, operatorModeRaw, 8 )  // 1=operator,2=super,3=service
};

// 102,27
class DomOperatorRFIDExists : public SessionMessageMap<OperatorDev,OpRFIDExists>
{
    typedef DomOperatorRFIDExists _Myt;
    typedef SessionMessageMap<OperatorDev,OpRFIDExists> _Mybase;
public:
    DomOperatorRFIDExists() : _Mybase() {}
    DomOperatorRFIDExists( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorRFIDExists( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorRFIDExists() {}
};

// 102,28
class DomOperatorRFIDDoesntExist : public SessionMessageMap<OperatorDev,OpRFIDDoesntExist>
{
    typedef DomOperatorRFIDDoesntExist _Myt;
    typedef SessionMessageMap<OperatorDev,OpRFIDDoesntExist> _Mybase;
public:
    DomOperatorRFIDDoesntExist() : _Mybase() {}
    DomOperatorRFIDDoesntExist( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorRFIDDoesntExist( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorRFIDDoesntExist() {}
};

//  (102,31)
class DomOperatorChanges : public SessionMessageMap<OperatorDev,OpOperatorChanges>
{
    typedef DomOperatorChanges _Myt;
    typedef SessionMessageMap<OperatorDev,OpOperatorChanges> _Mybase;
public:
    DomOperatorChanges() : _Mybase() {}
    DomOperatorChanges( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorChanges( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorChanges() {}

    QString& numOfRecordIds() {return _Mybase::operator [] (2);}
    QString& allRecordIds() {return _Mybase::operator [] (3);}
    QString& numOfFieldsChanged() {return _Mybase::operator [] (4);}
    QString& allFieldsChanged() {return _Mybase::operator [] (5);}
    QString& numOfDateTimes() {return _Mybase::operator [] (6);}
    QString& allDateTimes() {return _Mybase::operator [] (7);}
    QString& numOfPluNums() {return _Mybase::operator [] (8);}
    QString& allPluNums() {return _Mybase::operator [] (9);}
    QString& numOfOldValues() {return _Mybase::operator [] (10);}
    QString& allOldValues() {return _Mybase::operator [] (11);}
    QString& numOfNewValues() {return _Mybase::operator [] (12);}
    QString& allNewValues() {return _Mybase::operator [] (13);}
    QString& numOfOpIDs() {return _Mybase::operator [] (14);}
    QString& allOpIDs() {return _Mybase::operator [] (15);}
    QString& numOfOpNames() {return _Mybase::operator [] (16);}
    QString& allOpNames() {return _Mybase::operator [] (17);}
    QString& numOfTPs() {return _Mybase::operator [] (18);}
    QString& allTotalPrices() {return _Mybase::operator [] (19);}
    QString& numOfWeights() {return _Mybase::operator [] (20);}
    QString& allWeights() {return _Mybase::operator [] (21);}
    QString& currentOperator() {return _Mybase::operator [] (22);}
};

//(102,31)
class DomRemoteAccessCredentialsReq : public SessionMessageMap<OperatorDev,OpRemoteAccessCredentialsReq>
{
    typedef DomRemoteAccessCredentialsReq _Myt;
    typedef SessionMessageMap<OperatorDev,OpRemoteAccessCredentialsReq> _Mybase;
public:
    DomRemoteAccessCredentialsReq() : _Mybase() {}
    DomRemoteAccessCredentialsReq( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRemoteAccessCredentialsReq( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRemoteAccessCredentialsReq() {}

    DEFINE_FIELD( int, loginType, setLoginType, loginTypeRaw, 2 )
};

//(102,32)
class DomRemoteAccessCredentials : public SessionMessageMap<OperatorDev,OpRemoteAccessCredentials>
{
    typedef DomRemoteAccessCredentials _Myt;
    typedef SessionMessageMap<OperatorDev,OpRemoteAccessCredentials> _Mybase;
public:
    DomRemoteAccessCredentials() : _Mybase() {}
    DomRemoteAccessCredentials( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRemoteAccessCredentials( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRemoteAccessCredentials() {}

    DEFINE_FIELD( QString, userName, setUserName, userNameRaw, 2 )
    DEFINE_FIELD( QString, password, setPassword, passwordRaw, 3 )
};

//(102,52)													    // existed in domOperate and domSuper, so I deleted the Super version
class DomOperatorNoteData : public SessionMessageMap<OperatorDev,OpWriteNote>
{
    typedef DomOperatorNoteData _Myt;
    typedef SessionMessageMap<OperatorDev,OpWriteNote> _Mybase;
public:
    DomOperatorNoteData() : _Mybase() {}
    DomOperatorNoteData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorNoteData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorNoteData() {}

    QString& operatorNote() {return _Mybase::operator [] (2);}
};

// See ERROR MESSAGE CLASSES - 103,# at the bottom of this file

//  (105,2)
class DomGraphic : public SessionMessageMap<GraphicDev,GrDtWrite>
{
    typedef DomGraphic _Myt;
    typedef SessionMessageMap<GraphicDev,GrDtWrite> _Mybase;
public:
    DomGraphic() : _Mybase() {}
    DomGraphic( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphic( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphic() {}

    QString& name() {return _Mybase::operator [] (2);}
    QString& custom() {return _Mybase::operator [] (3);}
    QString& data() {return _Mybase::operator [] (4);}
};

//  (105,20)
class DomGraphicNameList : public SessionMessageMap<GraphicDev,GraphicReadAllNames>
{
    typedef DomGraphicNameList _Myt;
    typedef SessionMessageMap<GraphicDev,GraphicReadAllNames> _Mybase;
public:
    DomGraphicNameList() : _Mybase() {}
    DomGraphicNameList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicNameList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicNameList() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& graphicList() {return _Mybase::operator [] (3);}
};

//  (105,55)
class DomGraphicNameListProduct : public SessionMessageMap<GraphicDev,ReadAllProductGraphicNames>
{
    typedef DomGraphicNameListProduct _Myt;
    typedef SessionMessageMap<GraphicDev,ReadAllProductGraphicNames> _Mybase;
public:
    DomGraphicNameListProduct() : _Mybase() {}
    DomGraphicNameListProduct( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicNameListProduct( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicNameListProduct() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& graphicList() {return _Mybase::operator [] (3);}
};

//  (105,75)
class DomGraphicNameListCategory : public SessionMessageMap<GraphicDev,ReadAllCategoryGraphicNames>
{
    typedef DomGraphicNameListCategory _Myt;
    typedef SessionMessageMap<GraphicDev,ReadAllCategoryGraphicNames> _Mybase;
public:
    DomGraphicNameListCategory() : _Mybase() {}
    DomGraphicNameListCategory( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicNameListCategory( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicNameListCategory() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& graphicList() {return _Mybase::operator [] (3);}
};

//  (105,95)
class DomGraphicNameListSpecials : public SessionMessageMap<GraphicDev,ReadAllSpecialsGraphicNames>
{
    typedef DomGraphicNameListSpecials _Myt;
    typedef SessionMessageMap<GraphicDev,ReadAllSpecialsGraphicNames> _Mybase;
public:
    DomGraphicNameListSpecials() : _Mybase() {}
    DomGraphicNameListSpecials( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicNameListSpecials( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicNameListSpecials() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& graphicList() {return _Mybase::operator [] (3);}
};

//  (105,135)
class DomGraphicNameListSelfServ : public SessionMessageMap<GraphicDev,ReadAllSelfServeGraphicNames>
{
    typedef DomGraphicNameListSelfServ _Myt;
    typedef SessionMessageMap<GraphicDev,ReadAllSelfServeGraphicNames> _Mybase;
public:
    DomGraphicNameListSelfServ() : _Mybase() {}
    DomGraphicNameListSelfServ( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicNameListSelfServ( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicNameListSelfServ() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& graphicList() {return _Mybase::operator [] (3);}
};

//  (105,155)
class DomGraphicNameListSoftkeys : public SessionMessageMap<GraphicDev,ReadAllSoftkeyGraphicNames>
{
    typedef DomGraphicNameListSoftkeys _Myt;
    typedef SessionMessageMap<GraphicDev,ReadAllSoftkeyGraphicNames> _Mybase;
public:
    DomGraphicNameListSoftkeys() : _Mybase() {}
    DomGraphicNameListSoftkeys( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicNameListSoftkeys( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicNameListSoftkeys() {}

    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    QString& graphicList() {return _Mybase::operator [] (3);}
};

// (105,175)
class DomGraphicReadAllPlanogramGraphicNames : public SessionMessageMap<GraphicDev,ReadAllPlanogramGraphicNames>
{
    typedef DomGraphicReadAllPlanogramGraphicNames _Myt;
    typedef SessionMessageMap<GraphicDev,ReadAllPlanogramGraphicNames> _Mybase;
public:
    DomGraphicReadAllPlanogramGraphicNames() : _Mybase() {}
    DomGraphicReadAllPlanogramGraphicNames( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicReadAllPlanogramGraphicNames( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicReadAllPlanogramGraphicNames() {}

    DEFINE_FIELD( int, numOfGraphics, setNumOfGraphics, numOfGraphicsRaw, 2 )
    QString& graphicList() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numOfDates, setNumOfDates, numOfDatesRaw, 4 )
    QString& dateList() {return _Mybase::operator [] (5);}
};


// (105,179)
class DomGraphicNameWriteMostRecentPlanogramGraphic : public SessionMessageMap<GraphicDev,WriteMostRecentPlanogramGraphic>
{
    typedef DomGraphicNameWriteMostRecentPlanogramGraphic _Myt;
    typedef SessionMessageMap<GraphicDev,WriteMostRecentPlanogramGraphic> _Mybase;
public:
    DomGraphicNameWriteMostRecentPlanogramGraphic() : _Mybase() {}
    DomGraphicNameWriteMostRecentPlanogramGraphic( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicNameWriteMostRecentPlanogramGraphic( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicNameWriteMostRecentPlanogramGraphic() {}

    DEFINE_FIELD( QString, graphicName, setGraphicName, graphicNameRaw, 2 )
};

//  (105,195)
class DomGraphicNameListVideos : public SessionMessageMap<GraphicDev,ReadAllVideoNames>
{
    typedef DomGraphicNameListVideos _Myt;
    typedef SessionMessageMap<GraphicDev,ReadAllVideoNames> _Mybase;
public:
    DomGraphicNameListVideos() : _Mybase() {}
    DomGraphicNameListVideos( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGraphicNameListVideos( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGraphicNameListVideos() {}

    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    QString& graphicList() {return _Mybase::operator [] (3);}
};


//  (106,2)
class DomExpandedTextData : public SessionMessageMap<ExpandedTextDev,ExpandedDtWrite>
{
    typedef DomExpandedTextData _Myt;
    typedef SessionMessageMap<ExpandedTextDev,ExpandedDtWrite> _Mybase;
public:
    DomExpandedTextData() : _Mybase() {}
    DomExpandedTextData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomExpandedTextData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomExpandedTextData() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}
};

//  (107,2)
class DomSpecialMessageData : public SessionMessageMap<SpecialMessageDev,SpecialDtWrite>
{
    typedef DomSpecialMessageData _Myt;
    typedef SessionMessageMap<SpecialMessageDev,SpecialDtWrite> _Mybase;
public:
    DomSpecialMessageData() : _Mybase() {}
    DomSpecialMessageData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSpecialMessageData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSpecialMessageData() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}
};

//  (108,2)
class DomProductNoteData : public SessionMessageMap<ProductNotesDev,ProductDtWrite>
{
    typedef DomProductNoteData _Myt;
    typedef SessionMessageMap<ProductNotesDev,ProductDtWrite> _Mybase;
public:
    DomProductNoteData() : _Mybase() {}
    DomProductNoteData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProductNoteData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProductNoteData() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}
};

//  (109,5)
class MadpBackupDone : public SessionMessageMap<BackupRestoreDev,BRCancel>
{
    typedef MadpBackupDone _Myt;
    typedef SessionMessageMap<BackupRestoreDev,BRCancel> _Mybase;
public:
    MadpBackupDone() : _Mybase() {}
    MadpBackupDone( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpBackupDone( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpBackupDone() {}

    QString& defaultArg() {return _Mybase::operator [] (2);}
};

//  (109,6)
class DomBackupRestoreStatusComplete : public SessionMessageMap<BackupRestoreDev,Terminate>
{
    typedef DomBackupRestoreStatusComplete _Myt;
    typedef SessionMessageMap<BackupRestoreDev,Terminate> _Mybase;
public:
    DomBackupRestoreStatusComplete() : _Mybase() {}
    DomBackupRestoreStatusComplete( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomBackupRestoreStatusComplete( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomBackupRestoreStatusComplete() {}

    QString& groupIndicator() {return _Mybase::operator [] (2);}
    QString& fileIndicator() {return _Mybase::operator [] (3);}
    QString& fileRecordCnt() {return _Mybase::operator [] (4);}
    QString& totFileRecordCnt() {return _Mybase::operator [] (5);}
    QString& taskRecordCnt() {return _Mybase::operator [] (6);}
    QString& totTaskRecordCnt() {return _Mybase::operator [] (7);}
    QString& errorCount() {return _Mybase::operator [] (8);}
    QString& freeMemory() {return _Mybase::operator [] (9);}
    QString& filesIgnoredRecordCnt() {return _Mybase::operator [] (10);}
    QString& totFilesIgnoredRecCnt() {return _Mybase::operator [] (11);}
};

//  (109,7)
class DomBackupRestoreStatus : public SessionMessageMap<BackupRestoreDev,Progress>
{
    typedef DomBackupRestoreStatus _Myt;
    typedef SessionMessageMap<BackupRestoreDev,Progress> _Mybase;
public:
    DomBackupRestoreStatus() : _Mybase() {}
    DomBackupRestoreStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomBackupRestoreStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomBackupRestoreStatus() {}

    QString& groupIndicator() {return _Mybase::operator [] (2);}
    QString& fileIndicator() {return _Mybase::operator [] (3);}
    QString& fileRecordCnt() {return _Mybase::operator [] (4);}
    QString& totFileRecordCnt() {return _Mybase::operator [] (5);}
    QString& taskRecordCnt() {return _Mybase::operator [] (6);}
    QString& totTaskRecordCnt() {return _Mybase::operator [] (7);}
    QString& errorCount() {return _Mybase::operator [] (8);}
    QString& freeMemory() {return _Mybase::operator [] (9);}
    QString& filesIgnoredRecordCnt() {return _Mybase::operator [] (10);}
    QString& totFilesIgnoredRecCnt() {return _Mybase::operator [] (11);}
};

//  (109,8)
class DomRecordScanStatus : public SessionMessageMap<BackupRestoreDev,ProgressBadRecords>
{
    typedef DomRecordScanStatus _Myt;
    typedef SessionMessageMap<BackupRestoreDev,ProgressBadRecords> _Mybase;
public:
    DomRecordScanStatus() : _Mybase() {}
    DomRecordScanStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRecordScanStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRecordScanStatus() {}

    QString& numOfIDs() {return _Mybase::operator [] (2);}
    QString& errorIDs() {return _Mybase::operator [] (3);}
    QString& numOfRecs() {return _Mybase::operator [] (4);}
    QString& recordNum() {return _Mybase::operator [] (5);}
};

//(110,2)														// existed in domOperate and domSuper, so I deleted the Super version
class DomClassTreeNode : public SessionMessageMap<ClassDev,ClassDtWrite>
{
    typedef DomClassTreeNode _Myt;
    typedef SessionMessageMap<ClassDev,ClassDtWrite> _Mybase;
public:
    DomClassTreeNode() : _Mybase() {}
    DomClassTreeNode( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomClassTreeNode( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomClassTreeNode() {}

    QString& classNumber() {return _Mybase::operator [] (2);}
    QString& reportingClass() {return _Mybase::operator [] (3);}
    QString& classDescription() {return _Mybase::operator [] (4);}
};

//(110,21)														// existed in domOperate and domSuper, so I deleted the Super version
class DomClassTreeRecords : public SessionMessageMap<ClassDev,ClassGetTree>
{
    typedef DomClassTreeRecords _Myt;
    typedef SessionMessageMap<ClassDev,ClassGetTree> _Mybase;
public:
    DomClassTreeRecords() : _Mybase() {}
    DomClassTreeRecords( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomClassTreeRecords( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomClassTreeRecords() {}

    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    DEFINE_FIELD( QString, treeList, setTreeList, treeListRaw, 3 )
};

//(110,22)														// existed in domOperate and domSuper, so I deleted the Super version
class DomBadClassRecords : public SessionMessageMap<ClassDev,ClassGetOrphanTree>
{
    typedef DomBadClassRecords _Myt;
    typedef SessionMessageMap<ClassDev,ClassGetOrphanTree> _Mybase;
public:
    DomBadClassRecords() : _Mybase() {}
    DomBadClassRecords( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomBadClassRecords( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomBadClassRecords() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& treeList() {return _Mybase::operator [] (3);}
};

// (111,2)
class DomLabelType : public SessionMessageMap<LabelTypeDev,LabelTypeDtWrite>
{
    typedef DomLabelType _Myt;
    typedef SessionMessageMap<LabelTypeDev,LabelTypeDtWrite> _Mybase;
public:
    DomLabelType() : _Mybase() {}
    DomLabelType( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLabelType( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLabelType() {}

    QString& id() {return _Mybase::operator [] (2);}
    QString& width() {return _Mybase::operator [] (3);}
    QString& height() {return _Mybase::operator [] (4);}
    QString& custom() {return _Mybase::operator [] (5);}
    QString& rotationOnLabelStock() {return _Mybase::operator [] (6);}
    QString& data() {return _Mybase::operator [] (7);}
};

//  (112,17)
class DomConfigWriteSingleField : public SessionMessageMap<ConfigurationDev,CfgWriteSingleField>
{
    typedef DomConfigWriteSingleField _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgWriteSingleField> _Mybase;
public:
    DomConfigWriteSingleField() : _Mybase() {}
    DomConfigWriteSingleField( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomConfigWriteSingleField( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomConfigWriteSingleField() {}

    QString& isUpdateLegal() {return _Mybase::operator [] (2);}
    QString& offsetIndex() {return _Mybase::operator [] (3);}
    QString& newValue() {return _Mybase::operator [] (4);}
    QString& origValue() {return _Mybase::operator [] (5);}
    QString& shouldAlsoPrint() {return _Mybase::operator [] (6);}
};

// (112,21)
class MadpScaleConfig : public SessionMessageMap<ConfigurationDev,CfgScaleWrite>
{
    typedef MadpScaleConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgScaleWrite> _Mybase;
public:
    MadpScaleConfig() : _Mybase() {}
    MadpScaleConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpScaleConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpScaleConfig() {}

    QString& countryCode() {return _Mybase::operator [] (2);}
    QString& languageCode() {return _Mybase::operator [] (3);}
    QString& secCountryCode() {return _Mybase::operator [] (4);}
    QString& secLanguageCode() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, dateStyle, setDateStyle, dateStyleRaw, 6 )
    DEFINE_FIELD( int, timeFormat, setTimeFormat, timeFormatRaw, 7 )
    DEFINE_FIELD( bool, fractionalPricingOn, setFractionalPricingOn, fractionalPricingOnRaw, 8 )
    DEFINE_FIELD( bool, poundsForPricingOn, setPoundsForPricingOn, poundsForPricingOnRaw, 9 )
    DEFINE_FIELD( int, fixedWtUnits, setFixedWtUnits, fixedWtUnitsRaw, 10 )
    QString& isPlatterTareOn() {return _Mybase::operator [] (11);}
    QString& changeZeroTareOnlyOnce() {return _Mybase::operator [] (12);}
    QString& changeForcedTareOnlyOnce() {return _Mybase::operator [] (13);}
    DEFINE_FIELD( bool, isScaleMetric, setIsScaleMetric, isScaleMetricRaw, 14 )
    DEFINE_FIELD( bool, forcedLogin, setForcedLogin, forcedLoginRaw, 15 )
    DEFINE_FIELD( bool, superForcedLogin, setSuperForcedLogin, superForcedLoginRaw, 16 )
    DEFINE_FIELD( bool, autoLookupProduct, setAutoLookupProduct, autoLookupProductRaw, 17 )
    DEFINE_FIELD( int, autoLookupNumOfDigits, setAutoLookupNumOfDigits, autoLookupNumOfDigitsRaw, 18 )
    DEFINE_FIELD( bool, autoDisplayProductNotes, setAutoDisplayProductNotes, autoDisplayProductNotesRaw, 19 )
    QString& isRemoteKeyboardAttached() {return _Mybase::operator [] (20);}
    QString& isUSBKeyboardAttached() {return _Mybase::operator [] (21);}
    DEFINE_FIELD( bool, weightOnByCount, setWeightOnByCount, weightOnByCountRaw, 22 )
    QString& selfServ() {return _Mybase::operator [] (23);}
    QString& weigherAttached() {return _Mybase::operator [] (24);}
    QString& operCanAccessBrowser() {return _Mybase::operator [] (25);}
    QString& defaultHomePage() {return _Mybase::operator [] (26);}
    QString& ProductGraphicsDirName() {return _Mybase::operator [] (27);}
    QString& ProductGraphicsDefExt() {return _Mybase::operator [] (28);}
    QString& CategoryGraphicsDirName() {return _Mybase::operator [] (29);}
    QString& CategoryGraphicsDefExt() {return _Mybase::operator [] (30);}
    QString& SpecialsGraphicsDirName() {return _Mybase::operator [] (31);}
    QString& SpecialsGraphicsDefExt() {return _Mybase::operator [] (32);}
    DEFINE_FIELD( QString, autoLookupOverrideDigit, setAutoLookupOverrideDigit, autoLookupOverrideDigitRaw, 33 )
    QString& isLeapAvailable() {return _Mybase::operator [] (34);}
    QString& isServiceJumperSet() {return _Mybase::operator [] (35);}
    QString& hasWirelessCard() {return _Mybase::operator [] (36);}
    QString& isTestMsgsEnabled() {return _Mybase::operator [] (37);}
    QString& scaleModel() {return _Mybase::operator [] (38);}
    QString& scaleName() {return _Mybase::operator [] (39);}
    DEFINE_FIELD( bool, secondCheckDigit, setSecondCheckDigit, secondCheckDigitRaw, 40 ) // false - 6 UPC digits, true - 5 UPC digits
    QString& isPopupKeypadOn() {return _Mybase::operator [] (41);}
    DEFINE_FIELD( bool, coolTrackingNum, setCoolTrackingNum, coolTrackingNumRaw, 42 )
    DEFINE_FIELD( int, defaultLabelPlacement, setDefaultLabelPlacement, defaultLabelPlacementRaw, 43 )
    QString& SlideShowGraphicsDirName() {return _Mybase::operator [] (44);}
    QString& SlideShowGraphicsDefExt() {return _Mybase::operator [] (45);}
    QString& SelfServGraphicsDirName() {return _Mybase::operator [] (46);}
    QString& SelfServGraphicsDefExt() {return _Mybase::operator [] (47);}
    QString& SoftkeyGraphicsDirName() {return _Mybase::operator [] (48);}
    QString& SoftkeyGraphicsDefExt() {return _Mybase::operator [] (49);}
    QString& operCanAccessDesktop() {return _Mybase::operator [] (50);}
    DEFINE_FIELD( int, tarePrecision, setTarePrecision, tarePrecisionRaw, 51 )
    DEFINE_FIELD( bool, isRFIDHWPresent, setRFIDHWPresent, isRFIDHWPresentRaw, 52 )
    DEFINE_FIELD( int, keypadStyle, setKeypadStyle, keypadStyleRaw, 53 ) // 1 = telephone, 2 = PC keypad
    DEFINE_FIELD( int, operatorLoginKeyboardStyle, setOperatorLoginKeyboardStyle, operatorLoginKeyboardStyleRaw, 54 ) // 1 = qwerty, 2 = popup numeric keypad
    DEFINE_FIELD( QString, httpQueryModel, setHttpQueryModel, httpQueryModelRaw, 55 )
    DEFINE_FIELD( QString, httpQueryPLU, setHttpQueryPLU, httpQueryPLURaw, 56 )
    DEFINE_FIELD( QString, httpQueryWeight, setHttpQueryWeight, httpQueryWeightRaw, 57 )
    DEFINE_FIELD( QString, httpQueryQuantity, setHttpQueryQuantity, httpQueryQuantityRaw, 58 )
    DEFINE_FIELD( QString, httpQueryLanguage, setHttpQueryLanguage, httpQueryLanguageRaw, 59 )
    DEFINE_FIELD( bool, rebootUnprotectedAllowed, setRebootUnprotectedAllowed, rebootUnprotectedAllowedRaw, 60 )
    QString& customerDisplay() {return _Mybase::operator [] (61);}
    DEFINE_FIELD( bool, autoCreateRFIDOp, setAutoCreateRFIDOp, autoCreateRFIDOpRaw, 62 )
    QString& gcdLayoutNumber() {return _Mybase::operator [] (63);}
    QString& gcdColorSchemeNumber() {return _Mybase::operator [] (64);}
    QString& gcdLanguage() {return _Mybase::operator [] (65);}
    QString& operCanResetGCD() {return _Mybase::operator [] (66);}
    DEFINE_FIELD( bool, showDeleteAllFiles, setShowDeleteAllFiles, showDeleteAllFilesRaw, 67 )
    DEFINE_FIELD( bool, showPrinterSettings, setShowPrinterSettings, showPrinterSettingsRaw, 68 )
    DEFINE_FIELD( bool, remoteAccessPermitted, setRemoteAccessPermitted, remoteAccessPermittedRaw, 69 )
    DEFINE_FIELD( int, remoteAccessLoginMethod, setRemoteAccessLoginMethod, remoteAccessLoginMethodRaw, 70 )
    QString& hasWeigherWakeupSensor() {return _Mybase::operator [] (71);}
    QString& backdateByDaysStyle() {return _Mybase::operator [] (72);}
    DEFINE_FIELD( bool, autoDisplaySuggestiveSelling, setAutoDisplaySuggestiveSelling, autoDisplaySuggestiveSellingRaw, 73 )
    DEFINE_FIELD( bool, autoDisplayCookingNotes, setAutoDisplayCookingNotes, autoDisplayCookingNotesRaw, 74 )
    QString& startScreenHome() {return _Mybase::operator [] (75);}
    DEFINE_FIELD( bool, allowAddressBar, setAllowAddressBar, allowAddressBarRaw, 76 )
    QString& defaultSearchMethod() {return _Mybase::operator [] (77);}
    DEFINE_FIELD( bool, enableAccumulateItems, setEnableAccumulateItems, enableAccumulateItemsRaw, 78 )
    QString& runScreenHome() {return _Mybase::operator [] (79);}
    DEFINE_FIELD( int, dcaStartScreenType, setDcaStartScreenType, dcaStartScreenTypeRaw, 80 )                   // left panel
    DEFINE_FIELD( int, dcaRunScreenType, setDcaRunScreenType, dcaRunScreenTypeRaw, 81 )                         // center panel
    DEFINE_FIELD( int, dcaKeypadAreaType, setDcaKeypadAreaType, dcaKeypadAreaTypeRaw, 82 )                      // numpad area
    QString& planogramGraphicsDirName() {return _Mybase::operator [] (83);}
    QString& planogramGraphicsDefExt() {return _Mybase::operator [] (84);}
    DEFINE_FIELD( bool, operatorsExist, setOperatorsExist, operatorsExistRaw, 85 )
    QString& videosDirName() {return _Mybase::operator [] (86);}
    QString& videosDefExt() {return _Mybase::operator [] (87);}
    DEFINE_FIELD( bool, switchToRunScreenOnStableWeight, setSwitchToRunScreenOnStableWeightt, switchToRunScreenOnStableWeightRaw, 88 )
    DEFINE_FIELD( bool, enableDisplayNetworkDetails, setEnableDisplayNetworkDetails, enableDisplayNetworkDetailsRaw, 89 )
    DEFINE_FIELD( QString, licensedFeatures, setLicensedFeatures, licensedFeaturesRaw, 90 )
    DEFINE_FIELD( bool, disableOperatorFlashkeyEditor, setDisableOperatorFlashkeyEditor, disableOperatorFlashkeyEditorRaw, 91 )
    DEFINE_FIELD( bool, disableStartScreen, setDisableStartScreen, disableStartScreenRaw, 92 )
    DEFINE_FIELD( int, rightScreenType, setRightScreenType, rightScreenTypeRaw, 93 ) // right screen type in operate mode
    DEFINE_FIELD( bool, showTotalTypeOnRunScreen, setShowTotalTypeOnRunScreen, showTotalTypeOnRunScreenRaw, 94 )	//	FT-1466
    DEFINE_FIELD( int, keyboardNetWeightPrecision, setKeyboardNetWeightPrecision, keyboardNetWeightPrecisionRaw, 95 )
    DEFINE_FIELD( int, nowServingSysType, setNowServingSysType, nowServingSysTypeRaw, 96 )
    DEFINE_FIELD( bool, displaySelfServeStartScreen, setDisplaySelfServeStartScreen, displaySelfServeStartScreenRaw, 97 )                               // Big Green GO button
    DEFINE_FIELD( bool, bestSellersOnSelfServeMainScreen, setBestSellersOnSelfServeMainScreen, bestSellersOnSelfServeMainScreenRaw, 98 )
    DEFINE_FIELD( bool, selfServeWeightRequiredToSelectItem, setSelfServeWeightRequiredToSelectItem, selfServeWeightRequiredToSelectItemRaw, 99 )
    DEFINE_FIELD( QString, httpQueryStore, setHttpQueryStore, httpQueryStoreRaw, 100 )
    DEFINE_FIELD( QString, httpQueryDept, setHttpQueryDept, httpQueryDeptRaw, 101 )
    DEFINE_FIELD( int, suggestiveSellingStyle, setSuggestiveSellingStyle, suggestiveSellingStyleRaw, 102 )
    DEFINE_FIELD( int, suggestiveSellingFreq, setSuggestiveSellingFreq, suggestiveSellingFreqRaw, 103 )
    DEFINE_FIELD( bool, alwaysEnableVirtualKeyboard, setAlwaysEnableVirtualKeyboard, alwaysEnableVirtualKeyboardRaw, 104 )
    DEFINE_FIELD( bool, showSelectAllWhenVoidingTrans, setShowSelectAllWhenVoidingTrans, showSelectAllWhenVoidingTransRaw, 105 )
    DEFINE_FIELD( bool, isSingleRangeWeigher, setIsSingleRangeWeigher, isSingleRangeWeigherRaw, 106 )
    DEFINE_FIELD( bool, enableAccumItemsAndFlashkeys, setEnableAccumItemsAndFlashkeys, enableAccumItemsAndFlashkeysRaw, 107 )
    DEFINE_FIELD( int, cuttingTestType, setCuttingTestType, cuttingTestTypeRaw, 108 )
    DEFINE_FIELD( int, cuttestInactivitylogout, setCutTestInactivitylogout, cutTestInactivitylogoutRaw, 109 )
    DEFINE_FIELD( int, bottomScreenType, setBottomScreenType, bottomScreenTypeRaw, 110 ) // bottom screen type in operate mode
    DEFINE_FIELD( bool, enableMeatSpecSheetRequired, setEnableMeatSpecSheetRequireds, enableMeatSpecSheetRequiredRaw, 111 )
    DEFINE_FIELD( bool, enableSearchMenuButton, setEnableSearchMenuButton, enableSearchMenuButtonRaw, 112 )
    DEFINE_FIELD( bool, enableHelpPrompts, setEnableHelpPrompts, enableHelpPromptsRaw, 113 )
    DEFINE_FIELD( bool, enableUseCharacterSearching, setEnableUseCharacterSearching, enableUseCharacterSearchingRaw, 114 )
    DEFINE_FIELD( bool, useSecondTare, setUseSecondTare, useSecondTareRaw, 115 )
    DEFINE_FIELD( QString, allowOperatorUpdateNow, setAllowOperatorUpdateNow,allowOperatorUpdateNowRaw,116)
    DEFINE_FIELD( QString, allowOperatorSendToDept, setAllowOperatorSendToDept,allowOperatorSendToDeptRaw,117)
    DEFINE_FIELD( QString, FTPusername, setFTPusername,FTPusernameRaw,118)
    DEFINE_FIELD( QString, FTPpassword, setFTPpassword,FTPpasswordRaw,119)
    DEFINE_FIELD( QString, FTPIpOrHostname, setAllowOperHostname,FTPIpOrHostnameRaw,120)
    DEFINE_FIELD( QString, FTPdirectory, setFTPdirectory,FTPdirectoryRaw,121)
    DEFINE_FIELD( QString, FTPPLUDataUpdateNow,setFTPPLUDataUpdateNow,FTPPLUDataUpdateNowRaw,122)
    DEFINE_FIELD( QString, FTPLabelDataUpdateNow,setFTPLabelDataUpdateNow,FTPLabelDataUpdateNowRaw,123)
    DEFINE_FIELD( QString, FTPFlashkeyDataUpdateNow,setFTPFlashkeyDataUpdateNow,FTPFlashkeyDataUpdateNowRaw,124)
    DEFINE_FIELD( QString, FTPOperatorDataUpdateNow,setFTPOperatorDataUpdateNow,FTPOperatorDataUpdateNowRaw,125)
    DEFINE_FIELD( QString, FTPPlaylistsUpdateNow,setFTPPlaylistsUpdateNow,FTPPlaylistsUpdateNowRaw,126)
    DEFINE_FIELD( QString, FTPAlertsUpdateNow,setFTPAlertsUpdateNow,FTPAlertsUpdateNowRaw,127)
    DEFINE_FIELD( QString, FTPScaleConfigurationUpdateNow,setFTPScaleConfigurationUpdateNow,FTPScaleConfigurationUpdateNowRaw,128)
    DEFINE_FIELD( QString, FTPSynchronizationConfigurationUpdateNow,setFTPSynchronizationConfigurationUpdateNow,FTPSynchronizationConfigurationUpdateNowRaw,129)
    DEFINE_FIELD( QString, FTPStoreNameNumberUpdateNow,setFTPStoreNameNumberUpdateNow,FTPStoreNameNumberUpdateNowRaw,130)
    DEFINE_FIELD( QString, FTPVideosUpdateNow,setFTPVideosUpdateNow,FTPVideosUpdateNowRaw,131)
    DEFINE_FIELD( QString, FTPImagesUpdateNow,setFTPImagesUpdateNow,FTPImagesUpdateNowRaw,132)
    DEFINE_FIELD( QString, FTPAudioUpdateNow,setFTPAudioUpdateNow,FTPAudioUpdateNowRaw,133)
    DEFINE_FIELD( QString, FTPFirmwareUpdateNow,setFTPFirmwareUpdateNow,FTPFirmwareUpdateNowRaw,134)
    DEFINE_FIELD( QString, FTPPLUDataSendToDept,setFTPPLUDataSendToDept,FTPPLUDataSendToDeptRaw,135)
    DEFINE_FIELD( QString, FTPLabelDataSendToDept,setFTPLabelDataSendToDept,FTPLabelDataSendToDeptRaw,136)
    DEFINE_FIELD( QString, FTPFlashkeyDataSendToDept,setFTPFlashkeyDataSendToDept,FTPFlashkeyDataSendToDeptRaw,137)
    DEFINE_FIELD( QString, FTPOperatorDataSendToDept,setFTPOperatorDataSendToDept,FTPOperatorDataSendToDeptRaw,138)
    DEFINE_FIELD( QString, FTPPlaylistsSendToDept,setFTPPlaylistsSendToDept,FTPPlaylistsSendToDeptRaw,139)
    DEFINE_FIELD( QString, FTPAlertsSendToDept,setFTPAlertsSendToDept,FTPAlertsSendToDeptRaw,140)
    DEFINE_FIELD( QString, FTPScaleConfigurationSendToDept,setFTPScaleConfigurationSendToDept,FTPScaleConfigurationSendToDeptRaw,141)
    DEFINE_FIELD( QString, FTPSynchronizationConfigurationSendToDept,setFTPSynchronizationConfigurationSendToDept,FTPSynchronizationConfigurationSendToDeptRaw,142)
    DEFINE_FIELD( QString, FTPStoreNameNumberSendToDept,setFTPStoreNameNumberSendToDept,FTPStoreNameNumberSendToDeptRaw,143)
    DEFINE_FIELD( QString, FTPVideosSendToDept,setFTPVideosSendToDept,FTPVideosSendToDeptRaw,144)
    DEFINE_FIELD( QString, FTPImagesSendToDept,setFTPImagesSendToDept,FTPImagesSendToDeptRaw,145)
    DEFINE_FIELD( QString, FTPAudioSendToDept,setFTPAudioSendToDept,FTPAudioSendToDeptRaw,146)
    DEFINE_FIELD( QString, FTPFirmwareSendToDept,setFTPFirmwareSendToDept,FTPFirmwareSendToDeptRaw,147)
    DEFINE_FIELD( QString, AutomaticUpdateScaleType, setAutomaticUpdateScaleType, AutomaticUpdatScaleTypeRaw,148)
    DEFINE_FIELD( QString, FTPuseAutomatic, setFTPuseAutomatic, FTPuseAutomaticRaw,149)
    DEFINE_FIELD( QString, FTPmonday, setFTPmonday, FTPmondayRaw,150)
    DEFINE_FIELD( QString, FTPtuesday, setFTPtuesday, FTPtuesdayRaw,151)
    DEFINE_FIELD( QString, FTPwednesday, setFTPwednesday, FTPwednesdayRaw,152)
    DEFINE_FIELD( QString, FTPthursday, setFTPthursday, FTPthursdayRaw,153)
    DEFINE_FIELD( QString, FTPfriday, setFTPfriday, FTPfridayRaw,154)
    DEFINE_FIELD( QString, FTPsaturday, setFTPsaturday, FTPsaturdayRaw,155)
    DEFINE_FIELD( QString, FTPsunday, setFTPsunday, FTPsundayRaw,156)
    DEFINE_FIELD( QString, FTPstartTime, setFTPstartTime, FTPstartTimeRaw,157)
    DEFINE_FIELD( QString, FTPdownloadFirmwareOnly, setFTPdownloadFirmwareOnly, FTPdownloadFirmwareOnlyRaw,158)
    DEFINE_FIELD( QString, FTPdownloadInstallFirmware, setFTPdownloadInstallFirmware, FTPdownloadFirmwareRaw,159)
    DEFINE_FIELD( QString, FTPdailyUpdates, setFTPdailyUpdates, FTPdailyUpdatesRaw,160)
    DEFINE_FIELD( QString, FTPhourlyUpdates, setFTPhourlyUpdates, FTPhourlyUpdates,161)
    DEFINE_FIELD( QString, FTP15MinuteUpdates, setFTP15MinuteUpdates, FTP15MinuteUpdatesRaw,162)
    DEFINE_FIELD( QString, autoBackupDBFrequency, setAutoBackupDBFrequency, autoBackupDBFrequencyRaw,163)
    DEFINE_FIELD( int, defaultFlashkeyFontPointsize, setDefaultFlashkeyFontPointsize, defaultFlashkeyFontPointsizeRaw, 164 )
    DEFINE_FIELD( bool, selfServeDisplayLabelOnScreenGO, setSelfServeDisplayLabelOnScreenGO, selfServeDisplayLabelOnScreenGORaw, 165 )                            // Amazon GO
    DEFINE_FIELD( int, voidTransactionsViewHours, setvoidTransactionsViewHours, voidTransactionsViewHoursRaw, 166 )
    DEFINE_FIELD( bool, selfServeDisplayTareOnRandomWeight, setselfServeDisplayTareOnRandomWeight, selfServeDisplayTareOnRandomWeightRaw, 167 )
    DEFINE_FIELD( bool, autoDoNotPrintNutrifacts, setDoNotPrintNutrifacts, DoNotPrintNutrifactsRaw, 168 )
    DEFINE_FIELD( bool, isPick5Enabled, setIsPick5Enabled, isPick5EnabledRaw, 169 )
    DEFINE_FIELD( bool, isAllowProductionEntryModeOnWrapper, setAllowProductionEntryModeOnWrapper, isAllowProductionEntryModeOnWrapperRaw, 170 )
    DEFINE_FIELD( bool, isCutterEnabled, setIsCutterEnabled, isCutterEnabledRaw, 171 )
    DEFINE_FIELD( bool, isCodeCheckerEnabled, setIsCodeCheckerEnabled, isCodeCheckerEnabledRaw, 172 )
    DEFINE_FIELD( bool, allowSupervisorDotWear, setAllowSupervisorDotWear, allowSupervisorDotWearRaw, 173 )
    DEFINE_FIELD( bool, allowOperatorDotWear, setAllowOperatorDotWear, allowOperatorDotWearRaw, 174 )
    DEFINE_FIELD( bool, selfServeEnablePlaylists, setSelfServeEnablePlaylists, selfServeEnablePlaylistsRaw, 175 )
    DEFINE_FIELD( QString, selfServePlaylistFile, setSelfServePlaylistFile, selfServePlaylistFileRaw, 176 )
    DEFINE_FIELD( bool, enableSearchOnKeypad, setEnableSearchOnKeypad, enableSearchOnKeypadRaw, 177 )
    DEFINE_FIELD( bool, enableEnhancedCenterScreen, setEnableEnhancedCenterScreen, enableEnhancedCenterScreenRaw, 178 )
    QString& enhancedCenterHomePage() {return _Mybase::operator [] (179);}
    DEFINE_FIELD( bool, disableCuttingBetweenLabels, setDisableCuttingBetweenLabels, disableCuttingBetweenLabelsRaw, 180 )
    DEFINE_FIELD( bool, conserveLinerlessStock, setConserveLinerlessStock, conserveLinerlessStockRaw, 181 )
    DEFINE_FIELD( int, selfServeIgnorePrinterErrors, setSelfServeIgnorePrinterErrors, selfServeIgnorePrinterErrorsRaw, 182 )
    DEFINE_FIELD( bool, selfServeCombineSearchKeypadFlashkeys, setSelfServeCombineSearchKeypadFlashkeys, selfServeCombineSearchKeypadFlashkeysRaw, 183 )
    DEFINE_FIELD( bool, selfServeDisplayPluLabel, setSelfServeDisplayPluLabel, selfServeDisplayPluLabelRaw, 184 )

    DEFINE_FIELD( int, selfServeWeightFieldWidth, setSelfServeWeightFieldWidth, selfServeWeightFieldWidthRaw, 185 )
    DEFINE_FIELD( int, selfServeWeightFieldHeight, setSelfServeWeightFieldHeight, selfServeWeightFieldHeightRaw, 186 )
    DEFINE_FIELD( int, selfServeWeightFieldFontSize, setSelfServeWeightFieldFontSize, selfServeWeightFieldFontSizeRaw, 187 )
    DEFINE_FIELD( bool, selfServeDisplayBarcodeLabel, setSelfServeDisplayBarcodeLabel, selfServeDisplayBarcodeLabelRaw, 188 )                               // ORIG: Checkbox, Delhaize: Display Barcode(ONLY) and Print button on screen at same time

    DEFINE_FIELD( int, selfServeStartScreenState, setSelfServeStartScreenState, selfServeStartScreenStateRaw, 189 )
    DEFINE_FIELD( int, selfServeItemEntryScreenState, setSelfServeItemEntryScreenState, selfServeItemEntryScreenStateRaw, 190 )
    DEFINE_FIELD( int, selfServeSearchScreenState, setSelfServeSearchScreenState, selfServeSearchScreenStateRaw, 191 )
    DEFINE_FIELD( int, selfServePrintScreenState, setSelfServePrintScreenState, selfServePrintScreenStateRaw, 192 )
    DEFINE_FIELD( int, selfServeAfterPrintScreenState, setSelfServeAfterPrintScreenState, selfServeAfterPrintScreenStateRaw, 193 )
    DEFINE_FIELD( bool, selfServeStartScreenClickable, setSelfServeStartScreenClickable, selfServeStartScreenClickableRaw, 194 )
    DEFINE_FIELD( bool, selfServeIncludeKeypad, setSelfServeIncludeKeypad, selfServeIncludeKeypadRaw, 195 )                                                 // checkbox include Numpad
    DEFINE_FIELD( bool, selfServeHideButtonBar, setSelfServeHideButtonBar, selfServeHideButtonBarRaw, 196 )
    DEFINE_FIELD( bool, selfServePrintOnStableWeight, setSelfServePrintOnStableWeight, selfServePrintOnStableWeightRaw, 197 )

    DEFINE_FIELD( bool, onlineOrderingEnabled, setOnlineOrderingEnabled, onlineOrderingEnabledRaw, 198 )
    DEFINE_FIELD( QString, onlineOrderingServer, setOnlineOrderingServer, onlineOrderingServerRaw, 199 )

    DEFINE_FIELD( bool, enableVariableShelfLifeTime, setEnableVariableShelfLifeTime, enableVariableShelfLifeTimeRaw, 200 )
    DEFINE_FIELD( int, secondaryLabelPlacement, setSecondaryLabelPlacement, secondaryLabelPlacementRaw, 201 )
    DEFINE_FIELD( int, secondaryApplierRotation, setSecondaryApplierRotation, secondaryApplierRotationRaw, 202 )
    DEFINE_FIELD( bool, wrapperCameraEnabled, setWrapperCameraEnabled, wrapperCameraEnabledRaw, 203 )
    DEFINE_FIELD( QString, selfServeFixedPlu, setSelfServeFixedPlu, selfServeFixedPluRaw, 204 )
    DEFINE_FIELD( int, onlineOrderingZone, setOnlineOrderingZone, onlineOrderingZoneRaw, 205 )
    QString& printWithoutGcdValue() {return _Mybase::operator [] (206);}
    DEFINE_FIELD( bool, selfServeEnableFreshServeUI, setSelfServeEnableFreshServeUI, selfServeEnableFreshServeUIRaw, 207 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType0, setSelfServeFreshServeFlashkeyType0, selfServeFreshServeFlashkeyType0Raw, 208 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType1, setSelfServeFreshServeFlashkeyType1, selfServeFreshServeFlashkeyType1Raw, 209 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType2, setSelfServeFreshServeFlashkeyType2, selfServeFreshServeFlashkeyType2Raw, 210 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType3, setSelfServeFreshServeFlashkeyType3, selfServeFreshServeFlashkeyType3Raw, 211 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType4, setSelfServeFreshServeFlashkeyType4, selfServeFreshServeFlashkeyType4Raw, 212 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText0, setSelfServeFreshServeFlashkeyText0, selfServeFreshServeFlashkeyText0Raw, 213 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText1, setSelfServeFreshServeFlashkeyText1, selfServeFreshServeFlashkeyText1Raw, 214 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText2, setSelfServeFreshServeFlashkeyText2, selfServeFreshServeFlashkeyText2Raw, 215 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText3, setSelfServeFreshServeFlashkeyText3, selfServeFreshServeFlashkeyText3Raw, 216 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText4, setSelfServeFreshServeFlashkeyText4, selfServeFreshServeFlashkeyText4Raw, 217 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum0,  setSelfServeFreshServeFlashkeyNum0, selfServeFreshServeFlashkeyNum0Raw, 218 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum1,  setSelfServeFreshServeFlashkeyNum1, selfServeFreshServeFlashkeyNum1Raw, 219 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum2,  setSelfServeFreshServeFlashkeyNum2, selfServeFreshServeFlashkeyNum2Raw, 220 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum3,  setSelfServeFreshServeFlashkeyNum3, selfServeFreshServeFlashkeyNum3Raw, 221 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum4,  setSelfServeFreshServeFlashkeyNum4, selfServeFreshServeFlashkeyNum4Raw, 222 )
    DEFINE_FIELD( QString, selfServeFreshServeLabelOffset, setSelfServeFreshServeLabelOffset, selfServeFreshServeLabelOffsetRaw, 223 )

    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText0, setSelfServeFreshServeFlashkeySecText0, selfServeFreshServeFlashkeySecText0Raw, 224 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText1, setSelfServeFreshServeFlashkeySecText1, selfServeFreshServeFlashkeySecText1Raw, 225 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText2, setSelfServeFreshServeFlashkeySecText2, selfServeFreshServeFlashkeySecText2Raw, 226 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText3, setSelfServeFreshServeFlashkeySecText3, selfServeFreshServeFlashkeySecText3Raw, 227 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText4, setSelfServeFreshServeFlashkeySecText4, selfServeFreshServeFlashkeySecText4Raw, 228 )
};

// FORM (112,31)
class DomCountrySettings : public SessionMessageMap<ConfigurationDev,CfgClassGetOrphanTree>
{
    typedef DomCountrySettings _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgClassGetOrphanTree> _Mybase;
public:
    DomCountrySettings() : _Mybase() {}
    DomCountrySettings( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCountrySettings( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCountrySettings() {}

    QString& country() {return _Mybase::operator [] (2);}
    QString& primaryLanguage() {return _Mybase::operator [] (3);}
    QString& secondLanguage() {return _Mybase::operator [] (4);}
    QString& unitLength() {return _Mybase::operator [] (5);}
    QString& unitTemperature() {return _Mybase::operator [] (6);}
    QString& unitCurrency() {return _Mybase::operator [] (7);}
};

// (112,41)
class MadpFrequentShopper : public SessionMessageMap<ConfigurationDev,CfgFrequentShopperWrite>
{
    typedef MadpFrequentShopper _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgFrequentShopperWrite> _Mybase;
public:
    MadpFrequentShopper() : _Mybase() {}
    MadpFrequentShopper( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpFrequentShopper( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpFrequentShopper() {}

    QString& roundingMethod() {return _Mybase::operator [] (2);}
    QString& perProductOrScale() {return _Mybase::operator [] (3);}
    QString& discountType() {return _Mybase::operator [] (4);}
    QString& discountFormula() {return _Mybase::operator [] (5);}
    QString& totalPriceText1() {return _Mybase::operator [] (6);}
    QString& totalPriceText2() {return _Mybase::operator [] (7);}
    QString& savingsText1() {return _Mybase::operator [] (8);}
    QString& savingsText2() {return _Mybase::operator [] (9);}
    QString& unitPriceText1() {return _Mybase::operator [] (10);}
    QString& unitPriceText2() {return _Mybase::operator [] (11);}
    QString& roundingWeightPrecision() {return _Mybase::operator [] (12);}
    QString& enableGlobalPercentOff() {return _Mybase::operator [] (13);}
    QString& globalPercentageOff() {return _Mybase::operator [] (14);}
};

//  (112,51)
class DomPrintSpecialFields : public SessionMessageMap<ConfigurationDev,CfgPrintedFieldsWrite>
{
    typedef DomPrintSpecialFields _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgPrintedFieldsWrite> _Mybase;
public:
    DomPrintSpecialFields() : _Mybase() {}
    DomPrintSpecialFields( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrintSpecialFields( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrintSpecialFields() {}

    QString& sellByDate() {return _Mybase::operator [] (2);}
    QString& sellByDateFormat() {return _Mybase::operator [] (3);}
    QString& packedOnDate() {return _Mybase::operator [] (4);}
    QString& packedOnDateFormat() {return _Mybase::operator [] (5);}
    QString& productLifeDate() {return _Mybase::operator [] (6);}
    QString& productLifeDateFormat() {return _Mybase::operator [] (7);}
    QString& unitPrice() {return _Mybase::operator [] (8);}
    QString& byCount() {return _Mybase::operator [] (9);}
    QString& barcode() {return _Mybase::operator [] (10);}
    QString& currency() {return _Mybase::operator [] (11);}
    QString& textNames() {return _Mybase::operator [] (12);}
    QString& weightOnByCount() {return _Mybase::operator [] (13);}
    QString& oneOnByCount() {return _Mybase::operator [] (14);}
    QString& upcCheckDigit() {return _Mybase::operator [] (15);}
    QString& netWeightOnFixedWt() {return _Mybase::operator [] (16);}
    QString& fixedWeightFormat() {return _Mybase::operator [] (17);}
    QString& priceChangedLabelIndicator() {return _Mybase::operator [] (18);}
    QString& unitPriceFormat() {return _Mybase::operator [] (19);}
};

// 112,56
class MadpSupportedLabels : public SessionMessageMap<ConfigurationDev,CfgSupportedLabelsWrite>
{
    typedef MadpSupportedLabels _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgSupportedLabelsWrite> _Mybase;
public:
    MadpSupportedLabels() : _Mybase() {}
    MadpSupportedLabels( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpSupportedLabels( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpSupportedLabels() {}
    DEFINE_FIELD( bool, labelSize_1_000, setLabelSize_1_000, labelSize_1_000Raw, 2 )
    DEFINE_FIELD( bool, labelSize_1_500, setLabelSize_1_500, labelSize_1_500Raw, 3 )
    DEFINE_FIELD( bool, labelSize_1_750, setLabelSize_1_750, labelSize_1_750Raw, 4 )
    DEFINE_FIELD( bool, labelSize_2_000, setLabelSize_2_000, labelSize_2_000Raw, 5 )
    DEFINE_FIELD( bool, labelSize_2_375, setLabelSize_2_375, labelSize_2_375Raw, 6 )
    DEFINE_FIELD( bool, labelSize_2_500, setLabelSize_2_500, labelSize_2_500Raw, 7 )
    DEFINE_FIELD( bool, labelSize_3_000, setLabelSize_3_000, labelSize_3_000Raw, 8 )
    DEFINE_FIELD( bool, labelSize_3_500, setLabelSize_3_500, labelSize_3_500Raw, 9 )
    DEFINE_FIELD( bool, labelSize_4_000, setLabelSize_4_000, labelSize_4_000Raw, 10 )
    DEFINE_FIELD( bool, labelSize_4_500, setLabelSize_4_500, labelSize_4_500Raw, 11 )
    DEFINE_FIELD( bool, labelSize_5_000, setLabelSize_5_000, labelSize_5_000Raw, 12 )
    DEFINE_FIELD( bool, labelSize_5_500, setLabelSize_5_500, labelSize_5_500Raw, 13 )
    DEFINE_FIELD( bool, labelSize_6_000, setLabelSize_6_000, labelSize_6_000Raw, 14 )
    DEFINE_FIELD( bool, labelSize_6_500, setLabelSize_6_500, labelSize_6_500Raw, 15 )
    DEFINE_FIELD( bool, labelSize_7_000, setLabelSize_7_000, labelSize_7_000Raw, 16 )
    DEFINE_FIELD( bool, labelSize_7_500, setLabelSize_7_500, labelSize_7_500Raw, 17 )
    DEFINE_FIELD( bool, labelSize_8_000, setLabelSize_8_000, labelSize_8_000Raw, 18 )
    DEFINE_FIELD( bool, labelSize_8_500, setLabelSize_8_500, labelSize_8_500Raw, 19 )
    DEFINE_FIELD( bool, labelSize_9_000, setLabelSize_9_000, labelSize_9_000Raw, 20 )
    DEFINE_FIELD( bool, labelSize_9_500, setLabelSize_9_500, labelSize_9_500Raw, 21 )
};

// (112,61)
class MadpOperatorCanModify : public SessionMessageMap<ConfigurationDev,CfgOperatorAccessWrite>
{
    typedef MadpOperatorCanModify _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgOperatorAccessWrite> _Mybase;
public:
    MadpOperatorCanModify() : _Mybase() {}
    MadpOperatorCanModify( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpOperatorCanModify( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpOperatorCanModify() {}

    DEFINE_FIELD( bool, price, setPrice, priceRaw, 2 )
    DEFINE_FIELD( bool, byCount, setByCount, byCountRaw, 3 )
    DEFINE_FIELD( bool, tare, setTare, tareRaw, 4 )
    DEFINE_FIELD( bool, fixedWeight, setFixedWeight, fixedWeightRaw, 5 )
    DEFINE_FIELD( bool, shelfLife, setShelfLife, shelfLifeRaw, 6 )
    DEFINE_FIELD( bool, productLife, setProductLife, productLifeRaw, 7 )
    DEFINE_FIELD( bool, labelType, setLabelType, labelTypeRaw, 8 )
    DEFINE_FIELD( bool, dateTime, setDateTime, dateTimeRaw, 9 )
    DEFINE_FIELD( bool, portions, setPortions, portionsRaw, 10 )
    DEFINE_FIELD( int,  priceAccessLevel, setPriceAccessLevel, priceAccessLevelRaw, 11 )
    DEFINE_FIELD( int,  byCountAccessLevel, setByCountAccessLevel, byCountAccessLevelRaw, 12 )
    DEFINE_FIELD( int,  tareAccessLevel, setTareAccessLevel, tareAccessLevelRaw, 13 )
    DEFINE_FIELD( int,  fixedWeightAccessLevel, setFixedWeightAccessLevel, fixedWeightAccessLevelRaw, 14 )
    DEFINE_FIELD( int,  shelfLifeAccessLevel, setShelfLifeAccessLevel, shelfLifeAccessLevelRaw, 15 )
    DEFINE_FIELD( int,  productLifeAccessLevel, setProductLifeAccessLevel, productLifeAccessLevelRaw, 16 )
    DEFINE_FIELD( int,  labelTypeAccessLevel, setLabelTypeAccessLevel, labelTypeAccessLevelRaw, 17 )
    DEFINE_FIELD( int,  dateTimeAccessLevel, setDateTimeAccessLevel, dateTimeAccessLevelRaw, 18 )
    DEFINE_FIELD( int,  portionsAccessLevel, setPortionsAccessLevel, portionsAccessLevelRaw, 19 )
};

//  (112,71)
class DomNormalScaleOperation : public SessionMessageMap<ConfigurationDev,CfgScaleOperationWrite>
{
    typedef DomNormalScaleOperation _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgScaleOperationWrite> _Mybase;
public:
    DomNormalScaleOperation() : _Mybase() {}
    DomNormalScaleOperation( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNormalScaleOperation( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNormalScaleOperation() {}

    DEFINE_FIELD( bool, collectTransactions, setCollectTransactions, collectTransactionsRaw, 2 )
    QString& daysToKeepTransactions() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( bool, coolTrackingNum, setCoolTrackingNum, coolTrackingNumRaw, 4 )
    DEFINE_FIELD( bool, useAutoLookup, setUseAutoLookup, useAutoLookupRaw, 5 )
    QString& numOfDigits() {return _Mybase::operator [] (6);}
    DEFINE_FIELD( bool, soundBeepOnStableWt, setSoundBeepOnStableWt, soundBeepOnStableWtRaw, 7 )
    DEFINE_FIELD( bool, autoDisplayProductNotes, setAutoDisplayProductNotes, autoDisplayProductNotesRaw, 8 )
    DEFINE_FIELD( bool, accessBrowserMode, setAccessBrowserMode, accessBrowserModeRaw, 9 )
    DEFINE_FIELD( bool, autoLoadOldFirmware, setAutoLoadOldFirmware, autoLoadOldFirmwareRaw, 10 )
    QString& uniqueNumbers() {return _Mybase::operator [] (11);}
    QString& popupKeypad() {return _Mybase::operator [] (12);}			// not used
    DEFINE_FIELD( int, popupKeypadStyle, setPopupKeypadStyle, popupKeypadStyleRaw, 13 ) // keypad style 1=Telephone 2=PC Keyboard
    DEFINE_FIELD( int, opLoginKeyboardStyle, setOpLoginKeyboardStyle, opLoginKeyboardStyleRaw, 14 ) // keyboard style 1=QWERTY 2=numeric
    QString& enablePowerSaver() {return _Mybase::operator [] (15);}
    QString& powerSaveIdleTime() {return _Mybase::operator [] (16);}
    QString& logoutRemoteIdleTime() {return _Mybase::operator [] (17);}
    QString& defaultHomePage() {return _Mybase::operator [] (18);}
    QString& defaultProductRecord() {return _Mybase::operator [] (19);}
    QString& fixedWeightUnits() {return _Mybase::operator [] (20);}
    QString& maxTare() {return _Mybase::operator [] (21);}
    QString& tareSetback() {return _Mybase::operator [] (22);}
    QString& beeperVolume() {return _Mybase::operator [] (23);}			 // we want to remove this
    DEFINE_FIELD( int, byCountCalculationMethod, setByCountCalculationMethod, byCountCalculationMethodRaw, 24 ) // keyboard style 1=QWERTY 2=numeric
    QString& externalWeightPrecision() {return _Mybase::operator [] (25);}
    QString& tareEntryPrecision() {return _Mybase::operator [] (26);}
    DEFINE_FIELD( bool, clearPluAfterPrinting, setClearPluAfterPrinting, clearPluAfterPrintingRaw, 27 )
    QString& operatorRezero() {return _Mybase::operator [] (28);}		// single field write, instant validattion of this field
    QString& filmWidth() {return _Mybase::operator [] (29);}
    QString& filmTension() {return _Mybase::operator [] (30);}
    QString& tensionFrontToBack() {return _Mybase::operator [] (31);}
    QString& tensionLeftToRight() {return _Mybase::operator [] (32);}
    QString& intakeMode() {return _Mybase::operator [] (33);}
    QString& labelPlacement() {return _Mybase::operator [] (34);}
    QString& applierRotation() {return _Mybase::operator [] (35);}
    QString& timeOnSun() {return _Mybase::operator [] (36);}
    QString& timeOffSun() {return _Mybase::operator [] (37);}
    QString& timeOnMon() {return _Mybase::operator [] (38);}
    QString& timeOffMon() {return _Mybase::operator [] (39);}
    QString& timeOnTue() {return _Mybase::operator [] (40);}
    QString& timeOffTue() {return _Mybase::operator [] (41);}
    QString& timeOnWed() {return _Mybase::operator [] (42);}
    QString& timeOffWed() {return _Mybase::operator [] (43);}
    QString& timeOnThu() {return _Mybase::operator [] (44);}
    QString& timeOffThu() {return _Mybase::operator [] (45);}
    QString& timeOnFri() {return _Mybase::operator [] (46);}
    QString& timeOffFri() {return _Mybase::operator [] (47);}
    QString& timeOnSat() {return _Mybase::operator [] (48);}
    QString& timeOffSat() {return _Mybase::operator [] (49);}
    DEFINE_FIELD( bool, compressFlag, setCompressFlag, compressFlagRaw, 50 )
    QString& compressTime() {return _Mybase::operator [] (51);}
    QString& autoLookupOverrideDigit() {return _Mybase::operator [] (52);}
    QString& checkNetConnectivity() {return _Mybase::operator [] (53);}
    QString& accessDesktop() {return _Mybase::operator [] (54);} // hardcoded for now since no session support yet
    DEFINE_FIELD( bool, pingPrimary, setPingPrimary, pingPrimaryRaw, 55 )
    DEFINE_FIELD( bool, pingGateway, setPingGateway, pingGatewayRaw, 56 )
    QString& gcdLayoutType() {return _Mybase::operator [] (57);}
    QString& gcdColorScheme() {return _Mybase::operator [] (58);}
    DEFINE_FIELD( bool, accessGcdReset, setAccessGcdReset, accessGcdResetRaw, 59 )
    DEFINE_FIELD( bool, showDeleteAllFiles, setShowDeleteAllFiles, showDeleteAllFilesRaw, 60 )
    DEFINE_FIELD( bool, showPrinterSettings, setShowPrinterSettings, showPrinterSettingsRaw, 61 )
    DEFINE_FIELD( bool, remoteAccessPermitted, setRemoteAccessPermitted, remoteAccessPermittedRaw, 62 )
    DEFINE_FIELD( int, remoteAccessLoginMethod, setRemoteAccessLoginMethod, remoteAccessLoginMethodRaw, 63 )
    QString& unitPriceSecurityLabel() {return _Mybase::operator [] (64);}
    QString& totalPriceSecurityLabel() {return _Mybase::operator [] (65);}
    QString& backdateByDays() {return _Mybase::operator [] (66);}
    QString& pingTarget() {return _Mybase::operator [] (67);}
    QString& pingTargetIPAddress() {return _Mybase::operator [] (68);}
    QString& pingInterval() {return _Mybase::operator [] (69);}
    DEFINE_FIELD( bool, autoDisplaySuggestiveSelling, setAutoDisplaySuggestiveSelling, autoDisplaySuggestiveSellingRaw, 70 )
    DEFINE_FIELD( bool, autoDisplayCookingNotes, setAutoDisplayCookingNotes, autoDisplayCookingNotesRaw, 71 )
    QString& gcdIdlePlaybackTime() {return _Mybase::operator [] (72);}
    QString& remoteAccessUserName() {return _Mybase::operator [] (73);}
    QString& remoteAccessPassword() {return _Mybase::operator [] (74);}
    QString& systemFont() {return _Mybase::operator [] (75);}
    QString& startScreenHome() {return _Mybase::operator [] (76);}
    DEFINE_FIELD( bool, allowAddressBar, setAllowAddressBar, allowAddressBarRaw, 77 )
    QString& resetStatisticsDays() {return _Mybase::operator [] (78);}
    QString& defaultSearchMethod() {return _Mybase::operator [] (79);}
    QString& runScreenHomePage() {return _Mybase::operator [] (80);}
    DEFINE_FIELD( int, dcaStartScreenType, setDcaStartScreenType, dcaStartScreenTypeRaw, 81 )
    DEFINE_FIELD( int, dcaRunScreenType, setDcaRunScreenType, dcaRunScreenTypeRaw, 82 )
    DEFINE_FIELD( int, dcaKeypadAreaType, setDcaKeypadAreaType, dcaKeypadAreaTypeRaw, 83 )
    DEFINE_FIELD( bool, trackingOperatorChanges, setTrackingOperatorChanges, trackingOperatorChangesRaw, 84 )
    DEFINE_FIELD( bool, enableDisplayNetworkDetails, setEnableDisplayNetworkDetails, enableDisplayNetworkDetailsRaw, 85 )
    DEFINE_FIELD( bool, disableOperatorFlashkeyEditor, setDisableOperatorFlashkeyEditor, disableOperatorFlashkeyEditorRaw, 86 )
    DEFINE_FIELD( bool, disableStartScreen, setDisableStartScreen, disableStartScreenRaw, 87 )
    DEFINE_FIELD( int, rightScreenType, setRightScreenType, rightScreenTypeRaw, 88 ) // right screen type in operate mode
    DEFINE_FIELD( bool, showTotalTypeOnRunScreen, setShowTotalTypeOnRunScreen, showTotalTypeOnRunScreenRaw, 89 )	//	FT-1466
    QString& nowServingQueueIPAddress() {return _Mybase::operator [] (90);}
    DEFINE_FIELD( int, nowServingSysType, setNowServingSysType, nowServingSysTypeRaw, 91 )
    DEFINE_FIELD( bool, displaySelfServeStartScreen, setDisplaySelfServeStartScreen, displaySelfServeStartScreenRaw, 92 )                                   // Big Green GO button
    DEFINE_FIELD( bool, bestSellersOnSelfServeMainScreen, setBestSellersOnSelfServeMainScreen, bestSellersOnSelfServeMainScreenRaw, 93 )
    DEFINE_FIELD( bool, mealsRequireWeight, setMealsRequireWeight, mealsRequireWeightRaw, 94 )
    DEFINE_FIELD( int, suggestiveSellingStyle, setSuggestiveSellingStyle, suggestiveSellingStyleRaw, 95 )
    DEFINE_FIELD( int, suggestiveSellingFreq, setSuggestiveSellingFreq, suggestiveSellingFreqRaw, 96 )
    DEFINE_FIELD( bool, alwaysEnableVirtualKeyboard, setAlwaysEnableVirtualKeyboard, alwaysEnableVirtualKeyboardRaw, 97 )
    DEFINE_FIELD( bool, showSelectAllWhenVoidingTrans, setShowSelectAllWhenVoidingTrans, showSelectAllWhenVoidingTransRaw, 98 )
    QString& periodicOfflineBeep() {return _Mybase::operator [] (99);}
    DEFINE_FIELD( bool, fixedWeightVerification, setFixedWeightVerification, fixedWeightVerificationRaw, 100 )
    DEFINE_FIELD( int, cuttingTestType, setCuttingTestType, cuttingTestTypeRaw, 101 )
    DEFINE_FIELD( int, maxSuspendedTests, setMaxSuspendedTests, maxSuspendedTestsRaw, 102 )
    DEFINE_FIELD( int, maxSuspendedTestsPerOperator, setMaxSuspendedTestsPerOperator, maxSuspendedTestsPerOperatorRaw, 103 )
    DEFINE_FIELD( int, inactivitylogout, setInactivitylogout, inactivitylogoutRaw, 104 )
    DEFINE_FIELD( int, bottomScreenType, setBottomScreenType, bottomScreenTypeRaw, 105 ) // bottom screen type in operate mode
    DEFINE_FIELD( bool, enableSearchMenuButton, setEnableSearchMenuButton, enableSearchMenuButtonRaw, 106 )
    DEFINE_FIELD( bool, enableHelpPrompts, setEnableHelpPrompts, enableHelpPromptsRaw, 107 )
    DEFINE_FIELD( bool, enableUseCharacterSearching, setEnableUseCharacterSearching, enableUseCharacterSearchingRaw, 108 )
    DEFINE_FIELD( bool, useSecondTare, setUseSecondTare, useSecondTareRaw, 109 )
    DEFINE_FIELD( bool, trackingSupervisorChanges, setTrackingSupervisorChanges, trackingSupervisorChangesRaw, 110 )
    QString& variableShelfLifeTime() {return _Mybase::operator [] (111);}
    DEFINE_FIELD( bool, selfServeWeightRequiredToSelectItem, setSelfServeWeightRequiredToSelectItem, selfServeWeightRequiredToSelectItemRaw, 112 )
    DEFINE_FIELD( bool, selfServeByCountItemsAllowed, setSelfServeByCountItemsAllowed, selfServeByCountItemsAllowedRaw, 113 )
    DEFINE_FIELD( bool, selfServeDisplayLabelOnScreenGO, setSelfServeDisplayLabelOnScreenGO, selfServeDisplayLabelOnScreenGORaw, 114 )                            // Amazon GO - terrible name, should be label instead of barcode
    DEFINE_FIELD( int, voidTransactionsViewHours, setvoidTransactionsViewHours, voidTransactionsViewHoursRaw, 115 )
    DEFINE_FIELD( bool, selfServeDisplayTareOnRandomWeight, setselfServeDisplayTareOnRandomWeight, selfServeDisplayTareOnRandomWeightRaw, 116 )
    DEFINE_FIELD( bool, autoDoNotPrintNutrifacts, setDoNotPrintNutrifacts, autoDoNotPrintNutrifactsRaw, 117 )
    DEFINE_FIELD( bool, selfServeEnablePlaylists, setSelfServeEnablePlaylists, selfServeEnablePlaylistsRaw, 118 )
    DEFINE_FIELD( QString, selfServePlaylistFile, setSelfServePlaylistFile, selfServePlaylistFileRaw, 119 )
    DEFINE_FIELD( bool, enableSearchOnKeypad, setEnableSearchOnKeypad, enableSearchOnKeypadRaw, 120 )
    DEFINE_FIELD( bool, enableEnhancedCenterScreen, setEnableEnhancedCenterScreen, enableEnhancedCenterScreenRaw, 121 )
    QString& enhancedCenterHomePage() {return _Mybase::operator [] (122);}
    DEFINE_FIELD( bool, autoUpdateUPCBarcodeOver99, setAutoUpdateUPCBarcodeOver99, autoUpdateUPCBarcodeOver99Raw, 123 )
    DEFINE_FIELD( int, barcodeTypeOver99, setBarcodeTypeOver99, barcodeTypeOver99Raw, 124 )
    DEFINE_FIELD( bool, disableCuttingBetweenLabels, setDisableCuttingBetweenLabels, disableCuttingBetweenLabelsRaw, 125 )
    DEFINE_FIELD( bool, conserveLinerlessStock, setConserveLinerlessStock, conserveLinerlessStockRaw, 126 )
    DEFINE_FIELD( int, selfServeIgnorePrinterErrors, setSelfServeIgnorePrinterErrors, selfServeIgnorePrinterErrorsRaw, 127 )
    DEFINE_FIELD( bool, selfServeCombineSearchKeypadFlashkeys, setSelfServeCombineSearchKeypadFlashkeys, selfServeCombineSearchKeypadFlashkeysRaw, 128 )
    DEFINE_FIELD( bool, selfServeDisplayPluLabel, setSelfServeDisplayPluLabel, selfServeDisplayPluLabelRaw, 129 )
    QString& minSweetheartWeight() {return _Mybase::operator [] (130);}

    DEFINE_FIELD( bool, selfServeDisplayBarcodeLabel, setSelfServeDisplayBarcodeLabel, selfServeDisplayBarcodeLabelRaw, 131 )                               // ORIG: Checkbox, Delhaize: Display Barcode(ONLY) and Print button on screen at same time
    DEFINE_FIELD( int, selfServeStartScreenState, setSelfServeStartScreenState, selfServeStartScreenStateRaw, 132 )
    DEFINE_FIELD( int, selfServeItemEntryScreenState, setSelfServeItemEntryScreenState, selfServeItemEntryScreenStateRaw, 133 )
    DEFINE_FIELD( int, selfServeSearchScreenState, setSelfServeSearchScreenState, selfServeSearchScreenStateRaw, 134 )
    DEFINE_FIELD( int, selfServePrintScreenState, setSelfServePrintScreenState, selfServePrintScreenStateRaw, 135 )
    DEFINE_FIELD( int, selfServeAfterPrintScreenState, setSelfServeAfterPrintScreenState, selfServeAfterPrintScreenStateRaw, 136 )
    DEFINE_FIELD( bool, selfServeStartScreenClickable, setSelfServeStartScreenClickable, selfServeStartScreenClickableRaw, 137 )
    DEFINE_FIELD( bool, selfServeIncludeKeypad, setSelfServeIncludeKeypad, selfServeIncludeKeypadRaw, 138 )
    DEFINE_FIELD( bool, selfServeHideButtonBar, setSelfServeHideButtonBar, selfServeHideButtonBarRaw, 139 )
    DEFINE_FIELD( bool, selfServePrintOnStableWeight, setSelfServePrintOnStableWeight, selfServePrintOnStableWeightRaw, 140 )
    DEFINE_FIELD( bool, enableVariableShelfLifeTime, setEnableVariableShelfLifeTime, enableVariableShelfLifeTimeRaw, 141 )
    DEFINE_FIELD( int, secondaryLabelPlacement, setSecondaryLabelPlacement, secondaryLabelPlacementRaw, 142 )
    DEFINE_FIELD( int, secondaryApplierRotation, setSecondaryApplierRotation, secondaryApplierRotationRaw, 143 )
    DEFINE_FIELD( bool, wrapperCameraEnabled, setWrapperCameraEnabled, wrapperCameraEnabledRaw, 144 )
    DEFINE_FIELD( QString, selfServeFixedPlu, setSelfServeFixedPlu, selfServeFixedPluRaw, 145 )
    DEFINE_FIELD(int, selfServeTimeout, setSelfServeTimeout, selfServeTimeoutRaw, 146)
    DEFINE_FIELD( bool, selfServeEnableFreshServeUI, setSelfServeEnableFreshServeUI, selfServeEnableFreshServeUIRaw, 147 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType0, setSelfServeFreshServeFlashkeyType0, selfServeFreshServeFlashkeyType0Raw, 148 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType1, setSelfServeFreshServeFlashkeyType1, selfServeFreshServeFlashkeyType1Raw, 149 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType2, setSelfServeFreshServeFlashkeyType2, selfServeFreshServeFlashkeyType2Raw, 150 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType3, setSelfServeFreshServeFlashkeyType3, selfServeFreshServeFlashkeyType3Raw, 151 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyType4, setSelfServeFreshServeFlashkeyType4, selfServeFreshServeFlashkeyType4Raw, 152 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText0, setSelfServeFreshServeFlashkeyText0, selfServeFreshServeFlashkeyText0Raw, 153 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText1, setSelfServeFreshServeFlashkeyText1, selfServeFreshServeFlashkeyText1Raw, 154 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText2, setSelfServeFreshServeFlashkeyText2, selfServeFreshServeFlashkeyText2Raw, 155 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText3, setSelfServeFreshServeFlashkeyText3, selfServeFreshServeFlashkeyText3Raw, 156 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyText4, setSelfServeFreshServeFlashkeyText4, selfServeFreshServeFlashkeyText4Raw, 157 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum0,  setSelfServeFreshServeFlashkeyNum0, selfServeFreshServeFlashkeyNum0Raw, 158 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum1,  setSelfServeFreshServeFlashkeyNum1, selfServeFreshServeFlashkeyNum1Raw, 159 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum2,  setSelfServeFreshServeFlashkeyNum2, selfServeFreshServeFlashkeyNum2Raw, 160 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum3,  setSelfServeFreshServeFlashkeyNum3, selfServeFreshServeFlashkeyNum3Raw, 161 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeyNum4,  setSelfServeFreshServeFlashkeyNum4, selfServeFreshServeFlashkeyNum4Raw, 162 )
    DEFINE_FIELD( QString, selfServeFreshServeLabelOffset, setSelfServeFreshServeLabelOffset, selfServeFreshServeLabelOffsetRaw, 163 )

    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText0, setSelfServeFreshServeFlashkeySecText0, selfServeFreshServeFlashkeySecText0Raw, 164 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText1, setSelfServeFreshServeFlashkeySecText1, selfServeFreshServeFlashkeySecText1Raw, 165 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText2, setSelfServeFreshServeFlashkeySecText2, selfServeFreshServeFlashkeySecText2Raw, 166 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText3, setSelfServeFreshServeFlashkeySecText3, selfServeFreshServeFlashkeySecText3Raw, 167 )
    DEFINE_FIELD( QString, selfServeFreshServeFlashkeySecText4, setSelfServeFreshServeFlashkeySecText4, selfServeFreshServeFlashkeySecText4Raw, 168 )
};

//  (112,91)
class DomStoreInformation : public SessionMessageMap<ConfigurationDev,CfgStoreInfoWrite>
{
    typedef DomStoreInformation _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgStoreInfoWrite> _Mybase;
public:
    DomStoreInformation() : _Mybase() {}
    DomStoreInformation( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomStoreInformation( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomStoreInformation() {}

    QString& storeName() {return _Mybase::operator [] (2);}
    QString& storeID() {return _Mybase::operator [] (3);}
    QString& storeGraphicName() {return _Mybase::operator [] (4);}
    QString& deptNum() {return _Mybase::operator [] (5);}
};

// (112,101)
class MadpCurrencySettings : public SessionMessageMap<ConfigurationDev,CfgCurrencyWrite>
{
    typedef MadpCurrencySettings _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgCurrencyWrite> _Mybase;
public:
    MadpCurrencySettings() : _Mybase() {}
    MadpCurrencySettings( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpCurrencySettings( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpCurrencySettings() {}

    QString& unitSeparator() {return _Mybase::operator [] (2);}
    QString& unitPlacement() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, totalPriceDigits, setTotalPriceDigits, totalPriceDigitsRaw, 4 )
    DEFINE_FIELD( int, unitPriceDigits, setUnitPriceDigits, unitPriceDigitsRaw, 5 )
    QString& roundingFactor() {return _Mybase::operator [] (6);}
    QString& roundingMethod() {return _Mybase::operator [] (7);}
    QString& priCurrSymbol() {return _Mybase::operator [] (8);}
    DEFINE_FIELD( int, primaryPrecision, setPrimaryPrecision, primaryPrecisionRaw, 9 )
    QString& secCurrSymbol() {return _Mybase::operator [] (10);}
    DEFINE_FIELD( int, secondaryPrecision, setSecondaryPrecision, secondaryPrecisionRaw, 11 )
};

//(112,111)														// existed in domOperate and domSuper, so I deleted the Super version
class DomSelfservConfig : public SessionMessageMap<ConfigurationDev,CfgSelfServiceWrite>
{
    typedef DomSelfservConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgSelfServiceWrite> _Mybase;
public:
    DomSelfservConfig() : _Mybase() {}
    DomSelfservConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSelfservConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSelfservConfig() {}

    QString& gridXElements() {return _Mybase::operator [] (2);}
    QString& gridYElements() {return _Mybase::operator [] (3);}
    QString& numOfMostCommonCategories() {return _Mybase::operator [] (4);}
    QString& imageTypeOnStartScreen() {return _Mybase::operator [] (5);}
    QString& initialStreamingGraphicNum() {return _Mybase::operator [] (6);}
    QString& numOfDescCharsToUse() {return _Mybase::operator [] (7);}
    QString& productStatsHoursMethod() {return _Mybase::operator [] (8);}
    QString& selfServeInactivityTimeout() {return _Mybase::operator [] (9);}
    QString& appendingPluNumberToName() {return _Mybase::operator [] (10);}
    QString& productGraphicsDirectoryName() {return _Mybase::operator [] (11);}
    QString& productGraphicsDefaultExt() {return _Mybase::operator [] (12);}
    QString& categoryGraphicsDirectoryName() {return _Mybase::operator [] (13);}
    QString& categoryGraphicsDefaultExt() {return _Mybase::operator [] (14);}
    QString& slideShowGraphicsDirectoryName() {return _Mybase::operator [] (15);}
    QString& slideShowGraphicsDefaultExt() {return _Mybase::operator [] (16);}
    QString& logoGraphicFileName() {return _Mybase::operator [] (17);}
    QString& noGraphicAvailableFileName1() {return _Mybase::operator [] (18);}
    QString& noGraphicAvailableFileName2() {return _Mybase::operator [] (19);}
    QString& previousGraphicFileName1() {return _Mybase::operator [] (20);}
    QString& previousGraphicFileName2() {return _Mybase::operator [] (21);}
    QString& nextGraphicFileName1() {return _Mybase::operator [] (22);}
    QString& nextGraphicFileName2() {return _Mybase::operator [] (23);}
};

//  (112,121)
class DomScaleUpgrade : public SessionMessageMap<ConfigurationDev,CfgScaleUpgradeWrite>
{
    typedef DomScaleUpgrade _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgScaleUpgradeWrite> _Mybase;
public:
    DomScaleUpgrade() : _Mybase() {}
    DomScaleUpgrade( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomScaleUpgrade( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomScaleUpgrade() {}

    QString& autoLoadOldFirmware() {return _Mybase::operator [] (2);}
    QString& autoSearchNewFirmware() {return _Mybase::operator [] (3);}
    QString& newFirmwareSearchTime() {return _Mybase::operator [] (4);}
};

//  (112,131)
class DomEmailConfig : public SessionMessageMap<ConfigurationDev,CfgEmailWrite>
{
    typedef DomEmailConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgEmailWrite> _Mybase;
public:
    DomEmailConfig() : _Mybase() {}
    DomEmailConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEmailConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEmailConfig() {}

    QString& from() {return _Mybase::operator [] (2);}
    QString& sendTo() {return _Mybase::operator [] (3);}
    QString& mailServer() {return _Mybase::operator [] (4);}
    QString& isDnsEnabled() {return _Mybase::operator [] (5);}
};

// (112,141)
class MadpOperatorDisplayFields : public SessionMessageMap<ConfigurationDev,CfgPrepackWrite>
{
    typedef MadpOperatorDisplayFields _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgPrepackWrite> _Mybase;
public:
    MadpOperatorDisplayFields() : _Mybase() {}
    MadpOperatorDisplayFields( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpOperatorDisplayFields( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpOperatorDisplayFields() {}

    DEFINE_FIELD( bool, enableSellBy, setEnableSellBy, enableSellByRaw, 2 )
    DEFINE_FIELD( bool, enableBestBefore, setEnableBestBefore, enableBestBeforeRaw, 3 )
    DEFINE_FIELD( bool, enableProdImg, setEnableProdImg, enableProdImgRaw, 4 )
    DEFINE_FIELD( bool, enableUpcNumber, setEnableUpcNumber, enableUpcNumberRaw, 5 )
    DEFINE_FIELD( bool, enableFilmWidth, setEnableFilmWidth, enableFilmWidthRaw, 6 )
    DEFINE_FIELD( bool, enableFilmTension, setEnableFilmTension, enableFilmTensionRaw, 7 )
    DEFINE_FIELD( bool, enableWrapTypes, setEnableWrapTypes, enableWrapTypesRaw, 8 )
    DEFINE_FIELD( bool, enableCountryOfOrgin, setEnableCountryOfOrgin, enableCountryOfOrginRaw, 9 )
    DEFINE_FIELD( bool, enableTrackingNumber, setEnableTrackingNumber, enableTrackingNumberRaw, 10 )
    DEFINE_FIELD( bool, enableProdEntry, setEnableProdEntry, enableProdEntryRaw, 11 )
    DEFINE_FIELD( bool, prodEntryContinuousPrint, setProdEntryContinuousPrint, prodEntryContinuousPrintRaw, 12 )
    DEFINE_FIELD( bool, enableProdPlan, setEnableProdPlan, enableProdPlanRaw, 13 )
    DEFINE_FIELD( QString, prodPlanningFrame, setProdPlanningFrame, prodPlanningFrameRaw, 14 )
    DEFINE_FIELD( QString, prodPlanningUpdate, setProdPlanningUpdate, prodPlanningUpdateRaw, 15 )
    DEFINE_FIELD( bool, appendingPluNumberToName, setAppendingPluNumberToName, appendingPluNumberToNameRaw, 16 )
    DEFINE_FIELD( int, numberOfCharacters, setNumberOfCharacters, numberOfCharactersRaw, 17 )
    DEFINE_FIELD( bool, enableRunTotals, setEnableRunTotals, enableRunTotalsRaw, 18 )
    DEFINE_FIELD( bool, enablePackedOn, setEnablePackedOn, enablePackedOnRaw, 19 )
    DEFINE_FIELD( int, dcaStartScreenType, setDcaStartScreenType, dcaStartScreenTypeRaw, 20 )
    DEFINE_FIELD( int, dcaRunScreenType, setDcaRunScreenType, dcaRunScreenTypeRaw, 21 )
    DEFINE_FIELD( int, dcaKeypadAreaType, setDcaKeypadAreaType, dcaKeypadAreaTypeRaw, 22 )
    DEFINE_FIELD( int, capacityBarLocation, setCapacityBarLocation, capacityBarLocationRaw, 23 )
    DEFINE_FIELD( int, keypadLocation, setKeypadLocation, keypadLocationRaw, 24 )
    DEFINE_FIELD( bool, enableSelfService, setEnableSelfService, enableSelfServiceRaw, 25 )
    DEFINE_FIELD( int, rightScreenType, setRightScreenType, rightScreenTypeRaw, 26 ) // right screen type in operate mode
    DEFINE_FIELD( int, bottomScreenType, setBottomScreenType, bottomScreenTypeRaw, 27 ) // bottom screen type in operate mode
    DEFINE_FIELD( int, defaultFlashkeyFontPointsize, setDefaultFlashkeyFontPointsize, defaultFlashkeyFontPointsizeRaw, 28 )
    DEFINE_FIELD( int, priLabelStockRotated, setPriLabelStockRotated, priLabelStockRotatedRaw, 29 )
    DEFINE_FIELD( int, secLabelStockRotated, setSecLabelStockRotated, secLabelStockRotatedRaw, 30 )
};

//(112,151)														// existed in domOperate and domSuper, so I deleted the Super version
class DomExpiringNote : public SessionMessageMap<ConfigurationDev,CfgExpirationLabelNoteWrite>
{
    typedef DomExpiringNote _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgExpirationLabelNoteWrite> _Mybase;
public:
    DomExpiringNote() : _Mybase() {}
    DomExpiringNote( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomExpiringNote( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomExpiringNote() {}

    QString& primaryExpiringNoteText() {return _Mybase::operator [] (2);}
    QString& secondExpiringNoteText() {return _Mybase::operator [] (3);}
};

//  (112,161)
class DomMarqueeConfig : public SessionMessageMap<ConfigurationDev,CfgMarqueeWrite>
{
    typedef DomMarqueeConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgMarqueeWrite> _Mybase;
public:
    DomMarqueeConfig() : _Mybase() {}
    DomMarqueeConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomMarqueeConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomMarqueeConfig() {}

    QString& permanentMarqueeText() {return _Mybase::operator [] (2);}
    QString& displayPluMarquee() {return _Mybase::operator [] (3);}
    QString& howManyTimes() {return _Mybase::operator [] (4);}

    QString permanentMarqueeTextStripped() const {
        QString result = const_cast<_Myt*>( this )->permanentMarqueeText();
        result.replace( "\\S", "" );
        result.replace( "\\F", "" );
        result.replace( "\\R", "" );
        result.replace( "\\H", "" );
        result.replace( "\\C", "" );
        result.replace( "\\B", "" );
        result.replace( "\\L", "" );
        result.replace( "\\N", "" );
        result.replace( "\\Q", "" );
        result.replace( "\\X", "" );
        return result;
    }
};

//  (112,171)
class DomFtpConfig : public SessionMessageMap<ConfigurationDev,CfgFTPWrite>
{
    typedef DomFtpConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgFTPWrite> _Mybase;
public:
    DomFtpConfig() : _Mybase() {}
    DomFtpConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomFtpConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomFtpConfig() {}

    QString& ftpServer() {return _Mybase::operator [] (2);}
    QString& userName() {return _Mybase::operator [] (3);}
    QString& password() {return _Mybase::operator [] (4);}
    QString& subDirectory() {return _Mybase::operator [] (5);}
    QString& scheduleStartTime() {return _Mybase::operator [] (6);}
    QString& useAutomaticUpdates() {return _Mybase::operator [] (7);}
    QString& monday() {return _Mybase::operator [] (8);}
    QString& tuesday() {return _Mybase::operator [] (9);}
    QString& wednesday() {return _Mybase::operator [] (10);}
    QString& thursday() {return _Mybase::operator [] (11);}
    QString& friday() {return _Mybase::operator [] (12);}
    QString& saturday() {return _Mybase::operator [] (13);}
    QString& sunday() {return _Mybase::operator [] (14);}
    QString& downloadFirmwareOnly() {return _Mybase::operator [] (15);}
    QString& downloadInstallFirmware() {return _Mybase::operator [] (16);}
    QString& dailyUpdates() {return _Mybase::operator [] (17);}
    QString& hourlyUpdates() {return _Mybase::operator [] (18);}
    QString& minute15Updates() {return _Mybase::operator [] (19);}
    QString& updatePLUData() {return _Mybase::operator [] (20);}
    QString& updateLabelData() {return _Mybase::operator [] (21);}
    QString& updateFlashkeyData() {return _Mybase::operator [] (22);}
    QString& updateOperatorData() {return _Mybase::operator [] (23);}
    QString& updatePlaylists() {return _Mybase::operator [] (24);}
    QString& updateAlerts() {return _Mybase::operator [] (25);}
    QString& updateScaleConfiguration() {return _Mybase::operator [] (26);}
    QString& updateSynchronizationConfiguration() {return _Mybase::operator [] (27);}
    QString& updateStoreNameNumber() {return _Mybase::operator [] (28);}
    QString& updateVideos() {return _Mybase::operator [] (29);}
    QString& updateImages() {return _Mybase::operator [] (30);}
    QString& updateAudio() {return _Mybase::operator [] (31);}
    QString& updateFirmware() {return _Mybase::operator [] (32);}
    QString& sendPLUData() {return _Mybase::operator [] (33);}
    QString& sendLabelData() {return _Mybase::operator [] (34);}
    QString& sendFlashkeyData() {return _Mybase::operator [] (35);}
    QString& sendOperatorData() {return _Mybase::operator [] (36);}
    QString& sendPlaylists() {return _Mybase::operator [] (37);}
    QString& sendAlerts() {return _Mybase::operator [] (38);}
    QString& sendScaleConfiguration() {return _Mybase::operator [] (39);}
    QString& sendSynchronizationConfiguration() {return _Mybase::operator [] (40);}
    QString& sendStoreNameNumber() {return _Mybase::operator [] (41);}
    QString& sendVideos() {return _Mybase::operator [] (42);}
    QString& sendImages() {return _Mybase::operator [] (43);}
    QString& sendAudio() {return _Mybase::operator [] (44);}
    QString& sendFirmware() {return _Mybase::operator [] (45);}
    QString& allowOperatorSendToDept() {return _Mybase::operator [] (46);}
    QString& allowOperatorUpdateNow() {return _Mybase::operator [] (47);}
    QString& autoBackupDBFrequency() {return _Mybase::operator [] (48);}
};

class DomFtpResponse : public SessionMessageMap<ConfigurationDev, CfgFTPResponseWrite>
{
    typedef DomFtpResponse _Myt;
    typedef SessionMessageMap<ConfigurationDev, CfgFTPResponseWrite> _Mybase;
public:
    DomFtpResponse() : _Mybase() {}
    DomFtpResponse( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomFtpResponse( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomFtpResponse() {}

    QString& FTPServerMessage() {return _Mybase::operator [] (2);}

};

class DomFtpOptionsConfig : public SessionMessageMap<ConfigurationDev,CfgFTPOptionsWrite>
{
    typedef DomFtpOptionsConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgFTPOptionsWrite> _Mybase;
public:
    DomFtpOptionsConfig() : _Mybase() {}
    DomFtpOptionsConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomFtpOptionsConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomFtpOptionsConfig() {}

    QString& PLUData() {return _Mybase::operator [] (2);}
    QString& labelData() {return _Mybase::operator [] (3);}
    QString& flashkeyData() {return _Mybase::operator [] (4);}
    QString& operatorData() {return _Mybase::operator [] (5);}
    QString& playlists() {return _Mybase::operator [] (6);}
    QString& alerts() {return _Mybase::operator [] (7);}
    QString& scaleConfiguration() {return _Mybase::operator [] (8);}
    QString& synchronizationConfiguration() {return _Mybase::operator [] (9);}
    QString& storeNameNumber() {return _Mybase::operator [] (10);}
    QString& videos() {return _Mybase::operator [] (11);}
    QString& images() {return _Mybase::operator [] (12);}
    QString& audio() {return _Mybase::operator [] (13);}
    QString& firmware() {return _Mybase::operator [] (14);}
};

//  (112,181)
class DomAutomaticUpdate : public SessionMessageMap<ConfigurationDev,CfgSynchronizationWrite>
{
    typedef DomAutomaticUpdate _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgSynchronizationWrite> _Mybase;
public:
    DomAutomaticUpdate() : _Mybase() {}
    DomAutomaticUpdate( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomAutomaticUpdate( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomAutomaticUpdate() {}

    DEFINE_FIELD( int, scaleType, setScaleType, scaleTypeRaw, 2 )
    QString& hostName() {return _Mybase::operator [] (3);} 		// can be either a hostName or an IP address
};

//  (112,185)
class DomAutomaticDataTypes : public SessionMessageMap<ConfigurationDev,CfgSyncListWrite>
{
    typedef DomAutomaticDataTypes _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgSyncListWrite> _Mybase;
public:
    DomAutomaticDataTypes() : _Mybase() {}
    DomAutomaticDataTypes( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomAutomaticDataTypes( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomAutomaticDataTypes() {}

    QString& configurations() {return _Mybase::operator [] (2);}
    QString& emailAlerts() {return _Mybase::operator [] (3);}
    QString& productDefaults() {return _Mybase::operator [] (4);}
    QString& classes() {return _Mybase::operator [] (5);}
    QString& expandedTexts() {return _Mybase::operator [] (6);}
    QString& specialMsgs() {return _Mybase::operator [] (7);}
    QString& nutritionFacts() {return _Mybase::operator [] (8);}
    QString& productNotes() {return _Mybase::operator [] (9);}
    QString& marqueeTexts() {return _Mybase::operator [] (10);}
    DEFINE_FIELD( bool, transactions, setTransactions, transactionsRaw, 11 )
    DEFINE_FIELD( bool, transactionTypes, setTransactionTypes, transactionTypesRaw, 12 )
    QString& categories() {return _Mybase::operator [] (13);}
    QString& specials() {return _Mybase::operator [] (14);}
    QString& coolCategories() {return _Mybase::operator [] (15);}
    QString& coolCountries() {return _Mybase::operator [] (16);}
    QString& coolProdSteps() {return _Mybase::operator [] (17);}
    QString& coolPreTexts() {return _Mybase::operator [] (18);}
    QString& coolTexts() {return _Mybase::operator [] (19);}
    QString& coolListTexts() {return _Mybase::operator [] (20);}
    QString& coolLists() {return _Mybase::operator [] (21);}
    QString& coolCatCountries() {return _Mybase::operator [] (22);}
    QString& coolCatProdSteps() {return _Mybase::operator [] (23);}
    QString& coolCatPreTexts() {return _Mybase::operator [] (24);}
    QString& products() {return _Mybase::operator [] (25);}
    DEFINE_FIELD( bool, expirationLabels, setExpirationLabels, expirationLabelsRaw, 26 )
    QString& labels() {return _Mybase::operator [] (27);}
    QString& labelTexts() {return _Mybase::operator [] (28);}
    DEFINE_FIELD( bool, operatorNotes, setOperatorNotes, operatorNotesRaw, 29 )
    DEFINE_FIELD( bool, operators, setOperators, operatorsRaw, 30 )
    QString& softkeyGroups() {return _Mybase::operator [] (31);}
    QString& softkeyStyles() {return _Mybase::operator [] (32);}
    QString& softkeyFeatures() {return _Mybase::operator [] (33);}
    QString& softkeys() {return _Mybase::operator [] (34);}
    QString& labelGraphics() {return _Mybase::operator [] (35);}
    QString& catGraphics() {return _Mybase::operator [] (36);}
    QString& prodGraphics() {return _Mybase::operator [] (37);}
    QString& softkeyGraphics() {return _Mybase::operator [] (38);}
    QString& specialsGraphics() {return _Mybase::operator [] (39);}
};

//  (112,191)
class DomCommInterfaceConfig : public SessionMessageMap<ConfigurationDev,CfgCommInterfaceWrite>
{
    typedef DomCommInterfaceConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgCommInterfaceWrite> _Mybase;
public:
    DomCommInterfaceConfig() : _Mybase() {}
    DomCommInterfaceConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCommInterfaceConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCommInterfaceConfig() {}

    QString& enableClearProductClassTotals() {return _Mybase::operator [] (2);}
    QString& enableClearHourlyTotal() {return _Mybase::operator [] (3);}
    QString& enableClearOperatorTotal() {return _Mybase::operator [] (4);}
    QString& enableClearCoolTotal() {return _Mybase::operator [] (5);}
    QString& enableClearOperatorChange() {return _Mybase::operator [] (6);}
};

//  (112,201)
class DomScaleManagement : public SessionMessageMap<ConfigurationDev,CfgScaleManagementWrite>
{
    typedef DomScaleManagement _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgScaleManagementWrite> _Mybase;
public:
    DomScaleManagement() : _Mybase() {}
    DomScaleManagement( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomScaleManagement( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomScaleManagement() {}

    QString& ignoreUltimaLabels() {return _Mybase::operator [] (2);}
    QString& dontChangeCurrentCool() {return _Mybase::operator [] (3);}
    QString& enableUPTagPrefix() {return _Mybase::operator [] (4);}
};

// SOF-4769: Scheduled Merge
//  (112,206)
class DomCommInterfaceScheduledMergeConfig : public SessionMessageMap<ConfigurationDev,CfgCommIntfaceScheduledWrite>
{
    typedef DomCommInterfaceScheduledMergeConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgCommIntfaceScheduledWrite> _Mybase;
public:
    DomCommInterfaceScheduledMergeConfig() : _Mybase() {}
    DomCommInterfaceScheduledMergeConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCommInterfaceScheduledMergeConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCommInterfaceScheduledMergeConfig() {}

    QString& useAutomaticMerges() {return _Mybase::operator [] (2);}
    QString& monday() {return _Mybase::operator [] (3);}
    QString& tuesday() {return _Mybase::operator [] (4);}
    QString& wednesday() {return _Mybase::operator [] (5);}
    QString& thursday() {return _Mybase::operator [] (6);}
    QString& friday() {return _Mybase::operator [] (7);}
    QString& saturday() {return _Mybase::operator [] (8);}
    QString& sunday() {return _Mybase::operator [] (9);}
    QString& dailyUpdates() {return _Mybase::operator [] (10);}
    QString& scheduleStartTime() {return _Mybase::operator [] (11);}
    QString& hourlyUpdates() {return _Mybase::operator [] (12);}
    QString& execMergeAfterComm() {return _Mybase::operator [] (13);}
    QString& mergeHistoryDateTime() {return _Mybase::operator [] (14);}
};

//  (112,211)
class DomRotationOverrides : public SessionMessageMap<ConfigurationDev,CfgRotationOverrideWrite>
{
    typedef DomRotationOverrides _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgRotationOverrideWrite> _Mybase;
public:
    DomRotationOverrides() : _Mybase() {}
    DomRotationOverrides( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRotationOverrides( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRotationOverrides() {}

    QString& overrideSetting1() {return _Mybase::operator [] (2);}
    QString& overrideSetting2() {return _Mybase::operator [] (3);}
    QString& overrideSetting3() {return _Mybase::operator [] (4);}
    QString& overrideValue1() {return _Mybase::operator [] (5);}
    QString& overrideValue2() {return _Mybase::operator [] (6);}
    QString& overrideValue3() {return _Mybase::operator [] (7);}
};

//  (112,221)
class DomReportsSetup : public SessionMessageMap<ConfigurationDev,CfgReportsSetupWrite>
{
    typedef DomReportsSetup _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgReportsSetupWrite> _Mybase;
public:
    DomReportsSetup() : _Mybase() {}
    DomReportsSetup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomReportsSetup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomReportsSetup() {}

    QString& offsetTop() {return _Mybase::operator [] (2);}
    QString& offsetBottom() {return _Mybase::operator [] (3);}
};

//  (112,231)													// also existed in domService so I deleted it from domService
class DomGcdSettings : public SessionMessageMap<ConfigurationDev,CfgGCDWrite>
{
    typedef DomGcdSettings _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgGCDWrite> _Mybase;
public:
    DomGcdSettings() : _Mybase() {}
    DomGcdSettings( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGcdSettings( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGcdSettings() {}

    QString& gcdLayoutNumber() {return _Mybase::operator [] (2);}
    QString& gcdColorSchemeNumber() {return _Mybase::operator [] (3);}
    QString& gcdAllowOperReset() {return _Mybase::operator [] (4);}
    QString& gcdVolume() {return _Mybase::operator [] (5);}
    QString& gcdPlaybackWaitTime() {return _Mybase::operator [] (6);}
    QString& mediaServerName() {return _Mybase::operator [] (7);}
    QString& mediaServerIP() {return _Mybase::operator [] (8);}
    QString& ftpUserName() {return _Mybase::operator [] (9);}
    QString& ftpPassword() {return _Mybase::operator [] (10);}
    QString& ftpDirectory() {return _Mybase::operator [] (11);}
    QString& playlistFile() {return _Mybase::operator [] (12);}
    QString& autoSearch() {return _Mybase::operator [] (13);}
    QString& searchHour() {return _Mybase::operator [] (14);}
    QString& searchMinute() {return _Mybase::operator [] (15);}
    QString& amOrPm() {return _Mybase::operator [] (16);}
    QString& isDnsEnabled() {return _Mybase::operator [] (17);}
    QString& gcdDownloadProtocol() {return _Mybase::operator [] (18);}
    QString& gcdLanguage() {return _Mybase::operator [] (19);}
    QString& useTransactionalLayout() {return _Mybase::operator [] (20);}
    QString& idleAdvertType() {return _Mybase::operator [] (21);}
    QString& idleWebsiteAddress() {return _Mybase::operator [] (22);}
    QString& productAdvertType() {return _Mybase::operator [] (23);}
};

// (112,241)
class MadpAccumItemsConfig : public SessionMessageMap<ConfigurationDev,CfgAccumulateItemsWrite>
{
    typedef MadpAccumItemsConfig _Myt;
    typedef SessionMessageMap<ConfigurationDev,CfgAccumulateItemsWrite> _Mybase;
public:
    MadpAccumItemsConfig() : _Mybase() {}
    MadpAccumItemsConfig( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpAccumItemsConfig( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpAccumItemsConfig() {}

    DEFINE_FIELD( bool, enableAccumItems, setEnableAccumItems, enableAccumItemsRaw, 2 )
    DEFINE_FIELD( bool, enableItemStripMode, setEnableItemStripMode, enableItemStripModeRaw, 3 )
    DEFINE_FIELD( bool, accumItemsReturnToStartScreen, setAccumItemsReturnToStartScreen, accumItemsReturnToStartScreenRaw, 4 )
    DEFINE_FIELD( bool, enableAccumItemsTotalsOnly, setEnableAccumItemsTotalsOnly, enableAccumItemsTotalsOnlyRaw, 5 )
    QString& accumItemsEANFormatString() {return _Mybase::operator [] (6);}
    QString& accumItemsEANPrefix() {return _Mybase::operator [] (7);}
    QString& accumItemsEANCode() {return _Mybase::operator [] (8);}
    QString& accumItemsScaleId() {return _Mybase::operator [] (9);}
    QString& accumItemsTransNum() {return _Mybase::operator [] (10);}
    DEFINE_FIELD( bool, readTransactionsCompatibility, setReadTransactionsCompatibility, readTransactionsCompatibilityRaw, 11 )
    DEFINE_FIELD( bool, accumBarcodeUseFirstItem, setAccumBarcodeUseFirstItem, accumBarcodeUseFirstItemRaw, 12 )
    DEFINE_FIELD( bool, enableAccumItemsAndFlashkeys, setEnableAccumItemsAndFlashkeys, enableAccumItemsAndFlashkeysRaw, 13 )
    DEFINE_FIELD( bool, readBarcodeFromCloud,     setReadBarcodeFromCloud,    readBarcodeFromCloudRaw, 14)
    QString& barcodeServerAddress(){return _Mybase::operator[](15);}
};

// FORM (112,???)
/*  Can't find, could it be 254?
class DomRndisSettings : public SessionMessageMap<ConfigurationDev,EVENT>
{
    typedef DomRndisSettings _Myt;
    typedef SessionMessageMap<ConfigurationDev,EVENT> _Mybase;
public:
    DomRndisSettings() : _Mybase() {}
    DomRndisSettings( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRndisSettings( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRndisSettings() {}

    QString& hlxIpAddress() {return _Mybase::operator [] (2);}
    QString& gcdIpAddress() {return _Mybase::operator [] (3);}
    QString& subMaskAddress() {return _Mybase::operator [] (4);}
    QString& gcdServerPort() {return _Mybase::operator [] (5);}
    QString& defaultHlxIp() {return _Mybase::operator [] (6);}
    QString& defaultGcdIp() {return _Mybase::operator [] (7);}
    QString& defaultSubMask() {return _Mybase::operator [] (8);}
    QString& defaultServerPort() {return _Mybase::operator [] (9);}
};
*/
// (113,21)												        // existed in all 3 DOMs, so I deleted the Super and Service versions
class MadpFontNameList : public SessionMessageMap<FontDev,FontWriteAllNames>
{
    typedef MadpFontNameList _Myt;
    typedef SessionMessageMap<FontDev,FontWriteAllNames> _Mybase;
public:
    MadpFontNameList() : _Mybase() {}
    MadpFontNameList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpFontNameList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpFontNameList() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& fontsList() {return _Mybase::operator [] (3);}
};

// FORM (116,2)
class DomReportCompleted : public SessionMessageMap<ReportManagerDev,TerminateReport>
{
    typedef DomReportCompleted _Myt;
    typedef SessionMessageMap<ReportManagerDev,TerminateReport> _Mybase;
public:
    DomReportCompleted() : _Mybase() {}
    DomReportCompleted( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomReportCompleted( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomReportCompleted() {}

};

//  (117,21)
class DomNutrifactsData : public SessionMessageMap<NutrifactsDev,NutrifactsUse>
{
    typedef DomNutrifactsData _Myt;
    typedef SessionMessageMap<NutrifactsDev,NutrifactsUse> _Mybase;
public:
    DomNutrifactsData() : _Mybase() {}
    DomNutrifactsData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNutrifactsData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNutrifactsData() {}

    QString& useNutriPLU() {return _Mybase::operator [] (2);}
    QString& doesExist() {return _Mybase::operator [] (3);}
};

//  (119,2)
class DomGenericText : public SessionMessageMap<GenericTextDev,GenericDtWrite>
{
    typedef DomGenericText _Myt;
    typedef SessionMessageMap<GenericTextDev,GenericDtWrite> _Mybase;
public:
    DomGenericText() : _Mybase() {}
    DomGenericText( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGenericText( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGenericText() {}

    QString& recordId() {return _Mybase::operator [] (2);}
    QString& text() {return _Mybase::operator [] (3);}
};

//  (119,3)
class DomGenericTextList : public SessionMessageMap<GenericTextDev,GenericDtWriteAll>
{
    typedef DomGenericTextList _Myt;
    typedef SessionMessageMap<GenericTextDev,GenericDtWriteAll> _Mybase;
public:
    DomGenericTextList() : _Mybase() {}
    DomGenericTextList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGenericTextList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGenericTextList() {}

    QString& numOfRecordIds() {return _Mybase::operator [] (2);}
    QString& allRecordIds() {return _Mybase::operator [] (3);}
    QString& numOfTexts() {return _Mybase::operator [] (4);}
    QString& textList() {return _Mybase::operator [] (5);}
};

//  (119,21)
class DomAppletTextList : public SessionMessageMap<GenericTextDev,AppletViewerWriteAll>
{
    typedef DomAppletTextList _Myt;
    typedef SessionMessageMap<GenericTextDev,AppletViewerWriteAll> _Mybase;
public:
    DomAppletTextList() : _Mybase() {}
    DomAppletTextList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomAppletTextList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomAppletTextList() {}

    QString& numOfRecordIds() {return _Mybase::operator [] (2);}
    QString& allRecordIds() {return _Mybase::operator [] (3);}
    QString& numOfTexts() {return _Mybase::operator [] (4);}
    QString& textList() {return _Mybase::operator [] (5);}
};

//(120,13) 											            // existed in all 3 DOMs, so I deleted the Super and Service versions
class DomUpgradeMessage : public SessionMessageMap<VersionManagerDev,InitiateUpgradeProcess>
{
    typedef DomUpgradeMessage _Myt;
    typedef SessionMessageMap<VersionManagerDev,InitiateUpgradeProcess> _Mybase;
public:
    DomUpgradeMessage() : _Mybase() {}
    DomUpgradeMessage( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomUpgradeMessage( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomUpgradeMessage() {}

    QString& device() {return _Mybase::operator [] (2);}
    QString& subType() {return _Mybase::operator [] (3);}
};

//(120,14)											            // existed in all 3 DOMs, so I deleted the Super and Service versions
class DomUpgradeMessageComplete : public SessionMessageMap<VersionManagerDev,UpgradeComplete>
{
    typedef DomUpgradeMessageComplete _Myt;
    typedef SessionMessageMap<VersionManagerDev,UpgradeComplete> _Mybase;
public:
    DomUpgradeMessageComplete() : _Mybase() {}
    DomUpgradeMessageComplete( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomUpgradeMessageComplete( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomUpgradeMessageComplete() {}

    QString& device() {return _Mybase::operator [] (2);}
    QString& subType() {return _Mybase::operator [] (3);}
};

// (120,15)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class MadpWrUpgradeStatus : public SessionMessageMap<VersionManagerDev,UsbWrapperUpgradeStatus>
{
    typedef MadpWrUpgradeStatus _Myt;
    typedef SessionMessageMap<VersionManagerDev,UsbWrapperUpgradeStatus> _Mybase;
public:
    MadpWrUpgradeStatus() : _Mybase() {}
    MadpWrUpgradeStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpWrUpgradeStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpWrUpgradeStatus() {}

    QString& node() {return _Mybase::operator [] (2);}
    QString& status() {return _Mybase::operator [] (3);}
};

// (120,16)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class MadpUpgradeTransferStatus : public SessionMessageMap<VersionManagerDev,UpgradeXferStatus>
{
    typedef MadpUpgradeTransferStatus _Myt;
    typedef SessionMessageMap<VersionManagerDev,UpgradeXferStatus> _Mybase;
public:
    MadpUpgradeTransferStatus() : _Mybase() {}
    MadpUpgradeTransferStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpUpgradeTransferStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpUpgradeTransferStatus() {}

    QString& bytesSent() {return _Mybase::operator [] (2);}
    QString& totalBytes() {return _Mybase::operator [] (3);}
};

// (120,17)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class MadpUpgradeLists : public SessionMessageMap<VersionManagerDev,PeripheralUpgradeList>
{
    typedef MadpUpgradeLists _Myt;
    typedef SessionMessageMap<VersionManagerDev,PeripheralUpgradeList> _Mybase;
public:
    MadpUpgradeLists() : _Mybase() {}
    MadpUpgradeLists( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpUpgradeLists( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpUpgradeLists() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& componentList() {return _Mybase::operator [] (3);}
    QString& num2() {return _Mybase::operator [] (4);}
    QString& curVersionList() {return _Mybase::operator [] (5);}
    QString& num3() {return _Mybase::operator [] (6);}
    QString& newVersionList() {return _Mybase::operator [] (7);}
    QString& num4() {return _Mybase::operator [] (8);}
    QString& isPendingList() {return _Mybase::operator [] (9);}
};

//  (121,2)
class DomCategoryItem : public SessionMessageMap<CategoryDev,CategoryDtWrite>
{
    typedef DomCategoryItem _Myt;
    typedef SessionMessageMap<CategoryDev,CategoryDtWrite> _Mybase;
public:
    DomCategoryItem() : _Mybase() {}
    DomCategoryItem( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCategoryItem( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCategoryItem() {}

    QString& categorySetupNum() {return _Mybase::operator [] (2);}
    QString& reportingCategory() {return _Mybase::operator [] (3);}
    QString& primaryName() {return _Mybase::operator [] (4);}
    QString& secondName() {return _Mybase::operator [] (5);}
    QString& graphicName() {return _Mybase::operator [] (6);}
    QString& bgColor() {return _Mybase::operator [] (7);}
    QString& textColor() {return _Mybase::operator [] (8);}
    QString& categoryPlaylist() {return _Mybase::operator [] (9);}
};

//  (121,3)
class DomSelfServAllCategories : public SessionMessageMap<CategoryDev,CategoryDtWriteAll>
{
    typedef DomSelfServAllCategories _Myt;
    typedef SessionMessageMap<CategoryDev,CategoryDtWriteAll> _Mybase;
public:
    DomSelfServAllCategories() : _Mybase() {}
    DomSelfServAllCategories( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSelfServAllCategories( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSelfServAllCategories() {}

    QString& maxCategories() {return _Mybase::operator [] (2);}
    QString& numOfNums() {return _Mybase::operator [] (3);}
    QString& CatNumsList() {return _Mybase::operator [] (4);}
    QString& numOfNames() {return _Mybase::operator [] (5);}
    QString& CatNamesList() {return _Mybase::operator [] (6);}
};

//  (121,21)
class DomCategoryGraphics : public SessionMessageMap<CategoryDev,CategoryWriteAllCatGraphicNames>
{
    typedef DomCategoryGraphics _Myt;
    typedef SessionMessageMap<CategoryDev,CategoryWriteAllCatGraphicNames> _Mybase;
public:
    DomCategoryGraphics() : _Mybase() {}
    DomCategoryGraphics( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCategoryGraphics( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCategoryGraphics() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& graphicsList() {return _Mybase::operator [] (3);}
};

//  (121,23)
class DomMainCategoryNames : public SessionMessageMap<CategoryDev,CategoryWriteAllMainCategories>
{
    typedef DomMainCategoryNames _Myt;
    typedef SessionMessageMap<CategoryDev,CategoryWriteAllMainCategories> _Mybase;
public:
    DomMainCategoryNames() : _Mybase() {}
    DomMainCategoryNames( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomMainCategoryNames( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomMainCategoryNames() {}

    QString& maxCategories() {return _Mybase::operator [] (2);}
    QString& numOfNums() {return _Mybase::operator [] (3);}
    QString& CatNumsList() {return _Mybase::operator [] (4);}
    QString& numOfNames() {return _Mybase::operator [] (5);}
    QString& CatNamesList() {return _Mybase::operator [] (6);}
};

//  (121,51)
class DomSelfServFeaturedCategories : public SessionMessageMap<CategoryDev,CategoryWriteSoftKeyList>
{
    typedef DomSelfServFeaturedCategories _Myt;
    typedef SessionMessageMap<CategoryDev,CategoryWriteSoftKeyList> _Mybase;
public:
    DomSelfServFeaturedCategories() : _Mybase() {}
    DomSelfServFeaturedCategories( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSelfServFeaturedCategories( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSelfServFeaturedCategories() {}

    QString& numOfTypes() {return _Mybase::operator [] (2);}
    QString& dataType() {return _Mybase::operator [] (3);}
    QString& numOfRecs() {return _Mybase::operator [] (4);}
    QString& recNums() {return _Mybase::operator [] (5);}
    QString& numOfDescs() {return _Mybase::operator [] (6);}
    QString& description() {return _Mybase::operator [] (7);}
};

//  (122,21)
class DomPingData : public SessionMessageMap<NetworkDev,NetworkPingResponse>
{
    typedef DomPingData _Myt;
    typedef SessionMessageMap<NetworkDev,NetworkPingResponse> _Mybase;
public:
    DomPingData() : _Mybase() {}
    DomPingData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPingData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPingData() {}

    QString& pingAttempts() {return _Mybase::operator [] (2);}
    QString& pingReplies() {return _Mybase::operator [] (3);}
};

//(122,31)														// existed in domOperate and domSuper, so I deleted the Super version
class DomTcpipSetup : public SessionMessageMap<NetworkDev,CfgTcpIpWrite>
{
    typedef DomTcpipSetup _Myt;
    typedef SessionMessageMap<NetworkDev,CfgTcpIpWrite> _Mybase;
public:
    DomTcpipSetup() : _Mybase() {}
    DomTcpipSetup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTcpipSetup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTcpipSetup() {}

    QString& tcpipType() {return _Mybase::operator [] (2);}
    QString& ipAddress() {return _Mybase::operator [] (3);}
    QString& subnetMask() {return _Mybase::operator [] (4);}
    QString& defaultGateway() {return _Mybase::operator [] (5);}
    QString& secondGateway() {return _Mybase::operator [] (6);}
    QString& thirdGateway() {return _Mybase::operator [] (7);}
    QString& secondNetwork() {return _Mybase::operator [] (8);}
    QString& thirdNetwork() {return _Mybase::operator [] (9);}
    QString& wellKnownPort() {return _Mybase::operator [] (10);}
    QString& macAddress() {return _Mybase::operator [] (11);}
    QString& vpnEnabled() {return _Mybase::operator [] (12);}
    QString& vpnIpAddress() {return _Mybase::operator [] (13);}
};

//  (122,41)
class DomDnsSetup : public SessionMessageMap<NetworkDev,CfgDnsWrite>
{
    typedef DomDnsSetup _Myt;
    typedef SessionMessageMap<NetworkDev,CfgDnsWrite> _Mybase;
public:
    DomDnsSetup() : _Mybase() {}
    DomDnsSetup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomDnsSetup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomDnsSetup() {}

    QString& dnsChecked() {return _Mybase::operator [] (2);}
    QString& dnsAutomatic() {return _Mybase::operator [] (3);}
    QString& hostName() {return _Mybase::operator [] (4);}
    QString& invalidName0000() {return _Mybase::operator [] (5);}
    QString& ipAddress0() {return _Mybase::operator [] (6);}
    QString& ipAddress1() {return _Mybase::operator [] (7);}
    QString& ipAddress2() {return _Mybase::operator [] (8);}
};

//  (122,51)
class DomNetworkCard : public SessionMessageMap<NetworkDev,CfgWirelessCardWrite>
{
    typedef DomNetworkCard _Myt;
    typedef SessionMessageMap<NetworkDev,CfgWirelessCardWrite> _Mybase;
public:
    DomNetworkCard() : _Mybase() {}
    DomNetworkCard( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNetworkCard( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNetworkCard() {}

    QString& cardName() {return _Mybase::operator [] (2);}
    QString& macAddress() {return _Mybase::operator [] (3);}
    QString& isCardConnected() {return _Mybase::operator [] (4);}
    QString& systemID() {return _Mybase::operator [] (5);}
    QString& sharedKeyAuth() {return _Mybase::operator [] (6);}
    QString& isWepEnabled() {return _Mybase::operator [] (7);}
    QString& activeKey() {return _Mybase::operator [] (8);}
    QString& key1() {return _Mybase::operator [] (9);}
    QString& key2() {return _Mybase::operator [] (10);}
    QString& key3() {return _Mybase::operator [] (11);}
    QString& key4() {return _Mybase::operator [] (12);}
};

//  (122,62)
class DomWirelessCardSignal : public SessionMessageMap<NetworkDev,NetworkSignalStrength>
{
    typedef DomWirelessCardSignal _Myt;
    typedef SessionMessageMap<NetworkDev,NetworkSignalStrength> _Mybase;
public:
    DomWirelessCardSignal() : _Mybase() {}
    DomWirelessCardSignal( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWirelessCardSignal( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWirelessCardSignal() {}

    QString& signalStrength() {return _Mybase::operator [] (2);}
};

//  (122,65)
class DomWirelessCardConnection : public SessionMessageMap<NetworkDev,NetworkConnectionStatus>
{
    typedef DomWirelessCardConnection _Myt;
    typedef SessionMessageMap<NetworkDev,NetworkConnectionStatus> _Mybase;
public:
    DomWirelessCardConnection() : _Mybase() {}
    DomWirelessCardConnection( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWirelessCardConnection( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWirelessCardConnection() {}

    QString& status() {return _Mybase::operator [] (2);}
};

//  (122,71)
class DomTftpSetup : public SessionMessageMap<NetworkDev,NetworkTftpUpgradeWrite>
{
    typedef DomTftpSetup _Myt;
    typedef SessionMessageMap<NetworkDev,NetworkTftpUpgradeWrite> _Mybase;
public:
    DomTftpSetup() : _Mybase() {}
    DomTftpSetup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTftpSetup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTftpSetup() {}

    QString& isAutomatic() {return _Mybase::operator [] (2);}
    QString& ipAddress() {return _Mybase::operator [] (3);}
    QString& bootFilePath() {return _Mybase::operator [] (4);}
    QString& dnsEnabled() {return _Mybase::operator [] (5);}
};

//  (122,81)
class DomLeapSetup : public SessionMessageMap<NetworkDev,cfgLEAPWrite>
{
    typedef DomLeapSetup _Myt;
    typedef SessionMessageMap<NetworkDev,cfgLEAPWrite> _Mybase;
public:
    DomLeapSetup() : _Mybase() {}
    DomLeapSetup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLeapSetup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLeapSetup() {}

    QString& leapEnabled() {return _Mybase::operator [] (2);}
    QString& leapUserName() {return _Mybase::operator [] (3);}
    QString& leapPassword() {return _Mybase::operator [] (4);}
};

//  (122,84)
class DomLeapStatus : public SessionMessageMap<NetworkDev,cfgLEAPStatusWrite>
{
    typedef DomLeapStatus _Myt;
    typedef SessionMessageMap<NetworkDev,cfgLEAPStatusWrite> _Mybase;
public:
    DomLeapStatus() : _Mybase() {}
    DomLeapStatus( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLeapStatus( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLeapStatus() {}

    QString& status() {return _Mybase::operator [] (2);}
};

//(123,2)
class DomProductRunTotal : public SessionMessageMap<TotalsDev,productRunTotal02>
{
    typedef DomProductRunTotal _Myt;
    typedef SessionMessageMap<TotalsDev,productRunTotal02> _Mybase;
public:
    DomProductRunTotal() : _Mybase() {}
    DomProductRunTotal( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProductRunTotal( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProductRunTotal() {}

    QString& productNum() {return _Mybase::operator [] (2);}
    QString& repClassNum() {return _Mybase::operator [] (3);}
    QString& transTypeNum() {return _Mybase::operator [] (4);}
    QString& totalFlags() {return _Mybase::operator [] (5);}
    QString& totalWeight() {return _Mybase::operator [] (6);}
    QString& totalPrice() {return _Mybase::operator [] (7);}
    QString& totalPackages() {return _Mybase::operator [] (8);}
    QString& totalRuns() {return _Mybase::operator [] (9);}
    QString& totalItems() {return _Mybase::operator [] (10);}
    QString& reportingClass() {return _Mybase::operator [] (11);}
};

//  (123,22)
class DomTotalRecordData22 : public SessionMessageMap<TotalsDev,totalRecordData22>
{
    typedef DomTotalRecordData22 _Myt;
    typedef SessionMessageMap<TotalsDev,totalRecordData22> _Mybase;
public:
    DomTotalRecordData22() : _Mybase() {}
    DomTotalRecordData22( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTotalRecordData22( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTotalRecordData22() {}

    QString& totalNum() {return _Mybase::operator [] (2);}
    QString& totalType() {return _Mybase::operator [] (3);}
    QString& totalWeight() {return _Mybase::operator [] (3);}
    QString& totalPrice() {return _Mybase::operator [] (4);}
    QString& totalPackages() {return _Mybase::operator [] (5);}
    QString& totalRuns() {return _Mybase::operator [] (6);}
    QString& totalItems() {return _Mybase::operator [] (7);}
    QString& reportingClass() {return _Mybase::operator [] (8);}
};

//  (123,42)
class DomTotalRecordData42 : public SessionMessageMap<TotalsDev,totalRecordData42>
{
    typedef DomTotalRecordData42 _Myt;
    typedef SessionMessageMap<TotalsDev,totalRecordData42> _Mybase;
public:
    DomTotalRecordData42() : _Mybase() {}
    DomTotalRecordData42( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTotalRecordData42( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTotalRecordData42() {}

    QString& totalNum() {return _Mybase::operator [] (2);}
    QString& totalType() {return _Mybase::operator [] (3);}
    QString& totalWeight() {return _Mybase::operator [] (3);}
    QString& totalPrice() {return _Mybase::operator [] (4);}
    QString& totalPackages() {return _Mybase::operator [] (5);}
    QString& totalRuns() {return _Mybase::operator [] (6);}
    QString& totalItems() {return _Mybase::operator [] (7);}
    QString& reportingClass() {return _Mybase::operator [] (8);}
};

//  (123,62)
class DomTotalRecordData62 : public SessionMessageMap<TotalsDev,totalRecordData62>
{
    typedef DomTotalRecordData62 _Myt;
    typedef SessionMessageMap<TotalsDev,totalRecordData62> _Mybase;
public:
    DomTotalRecordData62() : _Mybase() {}
    DomTotalRecordData62( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTotalRecordData62( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTotalRecordData62() {}

    QString& totalNum() {return _Mybase::operator [] (2);}
    QString& totalType() {return _Mybase::operator [] (3);}
    QString& totalWeight() {return _Mybase::operator [] (3);}
    QString& totalPrice() {return _Mybase::operator [] (4);}
    QString& totalPackages() {return _Mybase::operator [] (5);}
    QString& totalRuns() {return _Mybase::operator [] (6);}
    QString& totalItems() {return _Mybase::operator [] (7);}
    QString& reportingClass() {return _Mybase::operator [] (8);}
};

//  (123,82)
class DomTotalRecordData82 : public SessionMessageMap<TotalsDev,totalRecordData82>
{
    typedef DomTotalRecordData82 _Myt;
    typedef SessionMessageMap<TotalsDev,totalRecordData82> _Mybase;
public:
    DomTotalRecordData82() : _Mybase() {}
    DomTotalRecordData82( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTotalRecordData82( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTotalRecordData82() {}

    QString& totalNum() {return _Mybase::operator [] (2);}
    QString& totalType() {return _Mybase::operator [] (3);}
    QString& totalWeight() {return _Mybase::operator [] (3);}
    QString& totalPrice() {return _Mybase::operator [] (4);}
    QString& totalPackages() {return _Mybase::operator [] (5);}
    QString& totalRuns() {return _Mybase::operator [] (6);}
    QString& totalItems() {return _Mybase::operator [] (7);}
    QString& reportingClass() {return _Mybase::operator [] (8);}
};

//  (123,37)
class DomProductTotalsList37 : public SessionMessageMap<TotalsDev,productTotalList37>
{
    typedef DomProductTotalsList37 _Myt;
    typedef SessionMessageMap<TotalsDev,productTotalList37> _Mybase;
public:
    DomProductTotalsList37() : _Mybase() {}
    DomProductTotalsList37( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProductTotalsList37( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProductTotalsList37() {}

    QString& prodTotalNum() {return _Mybase::operator [] (2);}
    QString& prodDescription() {return _Mybase::operator [] (3);}
    QString& numOfTypeIds() {return _Mybase::operator [] (4);}
    QString& allTypeIds() {return _Mybase::operator [] (5);}
    QString& numOfWeights() {return _Mybase::operator [] (6);}
    QString& weightsList() {return _Mybase::operator [] (7);}
    QString& numOfPrices() {return _Mybase::operator [] (8);}
    QString& pricesList() {return _Mybase::operator [] (9);}
    QString& numOfPackages() {return _Mybase::operator [] (10);}
    QString& packagesList() {return _Mybase::operator [] (11);}
    QString& numOfRuns() {return _Mybase::operator [] (12);}
    QString& runsList() {return _Mybase::operator [] (13);}
    QString& numOfItems() {return _Mybase::operator [] (14);}
    QString& itemsList() {return _Mybase::operator [] (15);}
};

//  (123,97)
class DomProductTotalsList97 : public SessionMessageMap<TotalsDev,productTotalList97>
{
    typedef DomProductTotalsList97 _Myt;
    typedef SessionMessageMap<TotalsDev,productTotalList97> _Mybase;
public:
    DomProductTotalsList97() : _Mybase() {}
    DomProductTotalsList97( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProductTotalsList97( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProductTotalsList97() {}

    QString& prodTotalNum() {return _Mybase::operator [] (2);}
    QString& prodDescription() {return _Mybase::operator [] (3);}
    QString& numOfTypeIds() {return _Mybase::operator [] (4);}
    QString& allTypeIds() {return _Mybase::operator [] (5);}
    QString& numOfWeights() {return _Mybase::operator [] (6);}
    QString& weightsList() {return _Mybase::operator [] (7);}
    QString& numOfPrices() {return _Mybase::operator [] (8);}
    QString& pricesList() {return _Mybase::operator [] (9);}
    QString& numOfPackages() {return _Mybase::operator [] (10);}
    QString& packagesList() {return _Mybase::operator [] (11);}
    QString& numOfRuns() {return _Mybase::operator [] (12);}
    QString& runsList() {return _Mybase::operator [] (13);}
    QString& numOfItems() {return _Mybase::operator [] (14);}
    QString& itemsList() {return _Mybase::operator [] (15);}
};

//  (123,43)
class DomOperatorTotalsList43 : public SessionMessageMap<TotalsDev,operatorTotalList43>
{
    typedef DomOperatorTotalsList43 _Myt;
    typedef SessionMessageMap<TotalsDev,operatorTotalList43> _Mybase;
public:
    DomOperatorTotalsList43() : _Mybase() {}
    DomOperatorTotalsList43( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorTotalsList43( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorTotalsList43() {}

    QString& numOfOpIds() {return _Mybase::operator [] (2);}
    QString& opIdsList() {return _Mybase::operator [] (3);}
    QString& numOfOpNames() {return _Mybase::operator [] (4);}
    QString& opNamesList() {return _Mybase::operator [] (5);}
    QString& numOfWeights() {return _Mybase::operator [] (6);}
    QString& weightsList() {return _Mybase::operator [] (7);}
    QString& numOfPrices() {return _Mybase::operator [] (8);}
    QString& pricesList() {return _Mybase::operator [] (9);}
    QString& numOfPackages() {return _Mybase::operator [] (10);}
    QString& packagesList() {return _Mybase::operator [] (11);}
    QString& numOfRuns() {return _Mybase::operator [] (12);}
    QString& runsList() {return _Mybase::operator [] (13);}
    QString& numOfItems() {return _Mybase::operator [] (14);}
    QString& itemsList() {return _Mybase::operator [] (15);}
};

//  (123,63)
class DomHourlyTotalsList63 : public SessionMessageMap<TotalsDev,hourlyTotalList63>
{
    typedef DomHourlyTotalsList63 _Myt;
    typedef SessionMessageMap<TotalsDev,hourlyTotalList63> _Mybase;
public:
    DomHourlyTotalsList63() : _Mybase() {}
    DomHourlyTotalsList63( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomHourlyTotalsList63( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomHourlyTotalsList63() {}

    QString& numOfHours() {return _Mybase::operator [] (2);}
    QString& hoursList() {return _Mybase::operator [] (3);}
    QString& numOfWeights() {return _Mybase::operator [] (4);}
    QString& weightsList() {return _Mybase::operator [] (5);}
    QString& numOfPrices() {return _Mybase::operator [] (6);}
    QString& pricesList() {return _Mybase::operator [] (7);}
    QString& numOfPackages() {return _Mybase::operator [] (8);}
    QString& packagesList() {return _Mybase::operator [] (9);}
    QString& numOfRuns() {return _Mybase::operator [] (10);}
    QString& runsList() {return _Mybase::operator [] (11);}
    QString& numOfItems() {return _Mybase::operator [] (12);}
    QString& itemsList() {return _Mybase::operator [] (13);}
};

//  (124,2)
class DomSingleSpecial : public SessionMessageMap<SpecialsDev,SpecialsDtWrite>
{
    typedef DomSingleSpecial _Myt;
    typedef SessionMessageMap<SpecialsDev,SpecialsDtWrite> _Mybase;
public:
    DomSingleSpecial() : _Mybase() {}
    DomSingleSpecial( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSingleSpecial( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSingleSpecial() {}

    QString& specialID() {return _Mybase::operator [] (2);}
    QString& specialDesc() {return _Mybase::operator [] (3);}
    QString& graphicName1() {return _Mybase::operator [] (4);}
    QString& graphicName2() {return _Mybase::operator [] (5);}
    QString& defaultLength() {return _Mybase::operator [] (6);}
};

//  (124,3)
class DomSpecialsList : public SessionMessageMap<SpecialsDev,SpecialsDtWriteAll>
{
    typedef DomSpecialsList _Myt;
    typedef SessionMessageMap<SpecialsDev,SpecialsDtWriteAll> _Mybase;
public:
    DomSpecialsList() : _Mybase() {}
    DomSpecialsList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSpecialsList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSpecialsList() {}

    QString& numOfIds() {return _Mybase::operator [] (2);}
    QString& idList() {return _Mybase::operator [] (3);}
    QString& numOfTypes() {return _Mybase::operator [] (4);}
    QString& typeList() {return _Mybase::operator [] (5);}
    QString& numOf1STGraphics() {return _Mybase::operator [] (6);}
    QString& firstGraphicList() {return _Mybase::operator [] (7);}
    QString& numOf2NDGraphics() {return _Mybase::operator [] (8);}
    QString& secondGraphicList() {return _Mybase::operator [] (9);}
};

//  (124,20)
class DomSpecialsGraphics : public SessionMessageMap<SpecialsDev,ReadAllGraphics>
{
    typedef DomSpecialsGraphics _Myt;
    typedef SessionMessageMap<SpecialsDev,ReadAllGraphics> _Mybase;
public:
    DomSpecialsGraphics() : _Mybase() {}
    DomSpecialsGraphics( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSpecialsGraphics( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSpecialsGraphics() {}

    QString& numOfGraphics() {return _Mybase::operator [] (2);}
    QString& graphicsList() {return _Mybase::operator [] (3);}
};

//  (129,2)
class DomSlideshowSlide : public SessionMessageMap<StreamingGraphicDev,StreamingGraphicDtWrite>
{
    typedef DomSlideshowSlide _Myt;
    typedef SessionMessageMap<StreamingGraphicDev,StreamingGraphicDtWrite> _Mybase;
public:
    DomSlideshowSlide() : _Mybase() {}
    DomSlideshowSlide( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSlideshowSlide( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSlideshowSlide() {}

    QString& recordNum() {return _Mybase::operator [] (2);}
    QString& nextSlide() {return _Mybase::operator [] (3);}
    QString& graphicName() {return _Mybase::operator [] (4);}
    QString& displayTime() {return _Mybase::operator [] (5);}
    QString& actionType() {return _Mybase::operator [] (6);}
    QString& actionParam() {return _Mybase::operator [] (7);}
};

//  (129,3)
class DomSlideshowList : public SessionMessageMap<StreamingGraphicDev,StreamingGraphicDtWriteAll>
{
    typedef DomSlideshowList _Myt;
    typedef SessionMessageMap<StreamingGraphicDev,StreamingGraphicDtWriteAll> _Mybase;
public:
    DomSlideshowList() : _Mybase() {}
    DomSlideshowList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSlideshowList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSlideshowList() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& recordNumList() {return _Mybase::operator [] (3);}
    QString& numOfTimes() {return _Mybase::operator [] (4);}
    QString& durationList() {return _Mybase::operator [] (5);}
    QString& numOfGraphics() {return _Mybase::operator [] (6);}
    QString& graphicList() {return _Mybase::operator [] (7);}
};

//  (129,21)
class DomStreamingGraphicsNames : public SessionMessageMap<StreamingGraphicDev,StreamingGraphicWriteAllGraphicNames>
{
    typedef DomStreamingGraphicsNames _Myt;
    typedef SessionMessageMap<StreamingGraphicDev,StreamingGraphicWriteAllGraphicNames> _Mybase;
public:
    DomStreamingGraphicsNames() : _Mybase() {}
    DomStreamingGraphicsNames( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomStreamingGraphicsNames( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomStreamingGraphicsNames() {}

    QString& numOfGraphics() {return _Mybase::operator [] (2);}
    QString& graphicsList() {return _Mybase::operator [] (3);}
};

//  (131,21)
class DomOperatorChangesSummary : public SessionMessageMap<OperatorChangesDev,WriteOperatorChangeSummary>
{
    typedef DomOperatorChangesSummary _Myt;
    typedef SessionMessageMap<OperatorChangesDev,WriteOperatorChangeSummary> _Mybase;
public:
    DomOperatorChangesSummary() : _Mybase() {}
    DomOperatorChangesSummary( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorChangesSummary( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorChangesSummary() {}

    QString& numOfOpIds() {return _Mybase::operator [] (2);}
    QString& opIdsList() {return _Mybase::operator [] (3);}
    QString& numOfOpNames() {return _Mybase::operator [] (4);}
    QString& opNamesList() {return _Mybase::operator [] (5);}
    QString& numOfTrans() {return _Mybase::operator [] (6);}
    QString& transactionList() {return _Mybase::operator [] (7);}
    QString& numOfOldTotals() {return _Mybase::operator [] (8);}
    QString& oldTotalsList() {return _Mybase::operator [] (9);}
    QString& numOfDeltas() {return _Mybase::operator [] (10);}
    QString& deltaList() {return _Mybase::operator [] (11);}
};

//  (131,31)
class DomOperatorChangesDetails : public SessionMessageMap<OperatorChangesDev,WriteOpertorChangesGroup>
{
    typedef DomOperatorChangesDetails _Myt;
    typedef SessionMessageMap<OperatorChangesDev,WriteOpertorChangesGroup> _Mybase;
public:
    DomOperatorChangesDetails() : _Mybase() {}
    DomOperatorChangesDetails( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorChangesDetails( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorChangesDetails() {}

    QString& operatorName() {return _Mybase::operator [] (2);}
    QString& numOfTransIDs() {return _Mybase::operator [] (3);}
    QString& transIdList() {return _Mybase::operator [] (4);}
    QString& numberDateTimes() {return _Mybase::operator [] (5);}
    QString& dateTimesList() {return _Mybase::operator [] (6);}
    QString& numOfTransTypes() {return _Mybase::operator [] (7);}
    QString& transTypesList() {return _Mybase::operator [] (8);}
    QString& numOfPLUs() {return _Mybase::operator [] (9);}
    QString& pluList() {return _Mybase::operator [] (10);}
    QString& numOfPrices() {return _Mybase::operator [] (11);}
    QString& totalPriceList() {return _Mybase::operator [] (12);}
    QString& numOfChanges() {return _Mybase::operator [] (13);}
    QString& changeFieldList() {return _Mybase::operator [] (14);}
    QString& numOfOldVals() {return _Mybase::operator [] (15);}
    QString& oldValsList() {return _Mybase::operator [] (16);}
    QString& numOfNewVals() {return _Mybase::operator [] (17);}
    QString& newValsList() {return _Mybase::operator [] (18);}
    QString& previousAvailable() {return _Mybase::operator [] (19);}
    QString& nextAvailable() {return _Mybase::operator [] (20);}
};

//  (134,2)
class DomEmailAlertEvent : public SessionMessageMap<EmailAlertsDev,emailAlertDtWrite>
{
    typedef DomEmailAlertEvent _Myt;
    typedef SessionMessageMap<EmailAlertsDev,emailAlertDtWrite> _Mybase;
public:
    DomEmailAlertEvent() : _Mybase() {}
    DomEmailAlertEvent( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEmailAlertEvent( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEmailAlertEvent() {}

QString& recordNum() {return _Mybase::operator [] (2);}
QString& eventText() {return _Mybase::operator [] (3);}
QString& sendEmail() {return _Mybase::operator [] (4);}
QString& eventNotify() {return _Mybase::operator [] (5);}
QString& eventDelay() {return _Mybase::operator [] (6);}
};

//  (134,3)
class DomEmailNotifyEvents : public SessionMessageMap<EmailAlertsDev,emailAlertDtWriteAll>
{
    typedef DomEmailNotifyEvents _Myt;
    typedef SessionMessageMap<EmailAlertsDev,emailAlertDtWriteAll> _Mybase;
public:
    DomEmailNotifyEvents() : _Mybase() {}
    DomEmailNotifyEvents( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomEmailNotifyEvents( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomEmailNotifyEvents() {}

    QString& recordItems() {return _Mybase::operator [] (2);}
    QString& recordList() {return _Mybase::operator [] (3);}
    QString& evtTextItems() {return _Mybase::operator [] (4);}
    QString& evtTextList() {return _Mybase::operator [] (5);}
    QString& evtSelItems() {return _Mybase::operator [] (6);}
    QString& evtSelList() {return _Mybase::operator [] (7);}
    QString& evtNotifyItems() {return _Mybase::operator [] (8);}
    QString& evtNotifyList() {return _Mybase::operator [] (9);}
    QString& evtHFNotifyItems() {return _Mybase::operator [] (10);}     // SOF-2455
    QString& evtHFNotifyList() {return _Mybase::operator [] (11);}      // SOF-2455
    QString& evtHFResolutionItems() {return _Mybase::operator [] (12);} // SOF-2455
    QString& evtHFResolutionList() {return _Mybase::operator [] (13);}  // SOF-2455
    QString& evtDelayItems() {return _Mybase::operator [] (14);}
    QString& evtDelayList() {return _Mybase::operator [] (15);}
};

//(135,22)														// existed in domOperate and domSuper, so I deleted the Super version
class DomCoolCountry : public SessionMessageMap<COOLDev,COOLDevWriteCountry>
{
    typedef DomCoolCountry _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteCountry> _Mybase;
public:
    DomCoolCountry() : _Mybase() {}
    DomCoolCountry( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolCountry( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolCountry() {}

    QString& name1() {return _Mybase::operator [] (2);}
    QString& name2() {return _Mybase::operator [] (3);}
    QString& category() {return _Mybase::operator [] (4);}
};

//(135,23)														// existed in domOperate and domSuper, so I deleted the Super version
class DomCoolCountryList : public SessionMessageMap<COOLDev,COOLDevWriteAllCountries>
{
    typedef DomCoolCountryList _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteAllCountries> _Mybase;
public:
    DomCoolCountryList() : _Mybase() {}
    DomCoolCountryList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolCountryList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolCountryList() {}

    DEFINE_FIELD( int, numOfItems, setnumOfItems, numOfItemsRaw, 2 )
    QString& names() {return _Mybase::operator [] (3);}
};
//(135,34)														// existed in domOperate and domSuper, so I deleted the Super version
class DomCoolCountryCatList : public SessionMessageMap<COOLDev,COOLDevWriteCountriesByCategory>
{
    typedef DomCoolCountryCatList _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteCountriesByCategory> _Mybase;
public:
    DomCoolCountryCatList() : _Mybase() {}
    DomCoolCountryCatList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolCountryCatList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolCountryCatList() {}

    DEFINE_FIELD( int, numOfItems, setnumOfItems, numOfItemsRaw, 2 )
    QString& names() {return _Mybase::operator [] (3);}
};

//(135,42)														// existed in domOperate and domSuper, so I deleted the Super version
class DomCoolProductionStep : public SessionMessageMap<COOLDev,COOLDevWriteProductionStep>
{
    typedef DomCoolProductionStep _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteProductionStep> _Mybase;
public:
    DomCoolProductionStep() : _Mybase() {}
    DomCoolProductionStep( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolProductionStep( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolProductionStep() {}

    QString& name1() {return _Mybase::operator [] (2);}
    QString& name2() {return _Mybase::operator [] (3);}
    QString& category() {return _Mybase::operator [] (4);}
};

//(135,43)														// existed in domOperate and domSuper, so I deleted the Super version
class DomCoolProdStepList : public SessionMessageMap<COOLDev,COOLDevWriteAllProductionSteps>
{
    typedef DomCoolProdStepList _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteAllProductionSteps> _Mybase;
public:
    DomCoolProdStepList() : _Mybase() {}
    DomCoolProdStepList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolProdStepList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolProdStepList() {}

    DEFINE_FIELD( int, numOfItems, setnumOfItems, numOfItemsRaw, 2 )
    QString& names() {return _Mybase::operator [] (3);}
};

//(135,54)														// existed in domOperate and domSuper, so I deleted the Super version
class DomCoolProdStepCatList : public SessionMessageMap<COOLDev,COOLDevWriteProductionStepsByCategory>
{
    typedef DomCoolProdStepCatList _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteProductionStepsByCategory> _Mybase;
public:
    DomCoolProdStepCatList() : _Mybase() {}
    DomCoolProdStepCatList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolProdStepCatList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolProdStepCatList() {}

    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    QString& names() {return _Mybase::operator [] (3);}
};

//  (135,62)
class DomCoolPretext : public SessionMessageMap<COOLDev,COOLDevWritePretext>
{
    typedef DomCoolPretext _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWritePretext> _Mybase;
public:
    DomCoolPretext() : _Mybase() {}
    DomCoolPretext( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolPretext( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolPretext() {}

    QString& recordNum() {return _Mybase::operator [] (2);}
    QString& name1() {return _Mybase::operator [] (3);}
    QString& name2() {return _Mybase::operator [] (4);}
    QString& category() {return _Mybase::operator [] (5);}
};

//  (135,63)
class DomCoolPretextList : public SessionMessageMap<COOLDev,COOLDevWriteAllPretexts>
{
    typedef DomCoolPretextList _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteAllPretexts> _Mybase;
public:
    DomCoolPretextList() : _Mybase() {}
    DomCoolPretextList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolPretextList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolPretextList() {}

    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    QString& recordNums() {return _Mybase::operator [] (3);}
    QString& numOfTexts() {return _Mybase::operator [] (4);}
    QString& names() {return _Mybase::operator [] (5);}
};

//  (135,74)
class DomCoolPretextCatList : public SessionMessageMap<COOLDev,COOLDevWritePretextsByCategory>
{
    typedef DomCoolPretextCatList _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWritePretextsByCategory> _Mybase;
public:
    DomCoolPretextCatList() : _Mybase() {}
    DomCoolPretextCatList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolPretextCatList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolPretextCatList() {}

    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    QString& recordNums() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numOfTexts, setNumOfTexts, numOfOfTextsRaw, 4 )
    QString& names() {return _Mybase::operator [] (5);}
};

//  (135,82)
class DomCoolCategory : public SessionMessageMap<COOLDev,COOLDevWriteCategory>
{
    typedef DomCoolCategory _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteCategory> _Mybase;
public:
    DomCoolCategory() : _Mybase() {}
    DomCoolCategory( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolCategory( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolCategory() {}

    QString& recordNum() {return _Mybase::operator [] (2);}
    QString& name1() {return _Mybase::operator [] (3);}
    QString& name2() {return _Mybase::operator [] (4);}
};

//  (135,83)
class DomCoolCategoryList : public SessionMessageMap<COOLDev,COOLDevWriteAllCategories>
{
    typedef DomCoolCategoryList _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteAllCategories> _Mybase;
public:
    DomCoolCategoryList() : _Mybase() {}
    DomCoolCategoryList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolCategoryList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolCategoryList() {}

    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    QString& recordNums() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numOfNames, setNumOfNames, numOfNamesRaw, 4 )
    QString& names() {return _Mybase::operator [] (5);}
};

//(135,102)											            // existed in domOperate and domSuper, so I deleted the Super version
class DomCoolTextForm : public SessionMessageMap<COOLDev,COOLDevWriteCoolText>
{
    typedef DomCoolTextForm _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevWriteCoolText> _Mybase;
public:
    DomCoolTextForm() : _Mybase() {}
    DomCoolTextForm( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolTextForm( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolTextForm() {}

    QString& recordNum() {return _Mybase::operator [] (2);}
    QString& coolText() {return _Mybase::operator [] (3);}
};

//(135,112)											            // existed in domOperate and domSuper, so I deleted the Super version
class DomCoolTextFormNewCool : public SessionMessageMap<COOLDev,COOLDevCreateNewCoolText>
{
    typedef DomCoolTextFormNewCool _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevCreateNewCoolText> _Mybase;
public:
    DomCoolTextFormNewCool() : _Mybase() {}
    DomCoolTextFormNewCool( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolTextFormNewCool( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolTextFormNewCool() {}

    DEFINE_FIELD( int, recordNum, setRecordNum, recordNumRaw, 2 )
    QString& coolText() {return _Mybase::operator [] (3);}
};

//(135,114)														// existed in domOperate and domSuper, so I deleted the Super version
class DomCoolTextList : public SessionMessageMap<COOLDev,COOLDevShortlistWriteAllCoolText>
{
    typedef DomCoolTextList _Myt;
    typedef SessionMessageMap<COOLDev,COOLDevShortlistWriteAllCoolText> _Mybase;
public:
    DomCoolTextList() : _Mybase() {}
    DomCoolTextList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCoolTextList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCoolTextList() {}

    //QString& numOfItems() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 2 )
    QString& recordNums() {return _Mybase::operator [] (3);}
    //QString& numOfNames() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, numOfNames, setNumOfNames, numOfNamesRaw, 4 )
    QString& names() {return _Mybase::operator [] (5);}
};

//  (136,22)
class DomSoftkey : public SessionMessageMap<SoftkeyDev,WriteSoftkey>
{
    typedef DomSoftkey _Myt;
    typedef SessionMessageMap<SoftkeyDev,WriteSoftkey> _Mybase;
public:
    DomSoftkey() : _Mybase() {}
    DomSoftkey( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSoftkey( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSoftkey() {}

    DEFINE_FIELD( int, featureNum, setFeatureNum, featureNumRaw, 2 )
    DEFINE_FIELD( int, groupNum, setGroupNum, groupNumRaw, 3 )
    DEFINE_FIELD( int, hPosition, setHPosition, hPositionRaw, 4 )
    DEFINE_FIELD( int, vPosition, setVPosition, vPositionRaw, 5 )
    DEFINE_FIELD( int, hSize, setHSize, hSizeRaw, 6 )
    DEFINE_FIELD( int, vSize, setVSize, vSizeRaw, 7 )
    DEFINE_FIELD( int, action, setAction, actionRaw, 8 )
    DEFINE_FIELD( QString, actionParam, setActionParam, actionParamRaw, 9 )
    DEFINE_FIELD( QString, actionStrParam, setActionStrParam, actionStrParamRaw, 10 )
    DEFINE_FIELD( int, actionFeature, setActionFeature, actionFeatureRaw, 11 )
    DEFINE_FIELD( QString, fgImage, setFgImage, fgImageRaw, 12 )
    DEFINE_FIELD( QString, bgColor, setBgColor, bgColorRaw, 13 )
    DEFINE_FIELD( QString, defBgColor, setDefBgColor, defBgColorRaw, 14 )
    DEFINE_FIELD( bool, useDefBgColor, setUseDefBgColor, useDefBgColorRaw, 15 )
    DEFINE_FIELD( QString, bgImage, setBgImage, bgImageRaw, 16 )
    DEFINE_FIELD( QString, defBgImage, setDefBgImage, defBgImageRaw, 17 )
    DEFINE_FIELD( bool, useDefBgImage, setUseDefBgImage, useDefBgImageRaw, 18 )
    DEFINE_FIELD( QString, textColor, setTextColor, textColorRaw, 19 )
    DEFINE_FIELD( QString, defTextColor, setDefTextColor, defTextColorRaw, 20 )
    DEFINE_FIELD( bool, useDefTextColor, setUseDefTextColor, useDefTextColorRaw, 21 )
    DEFINE_FIELD( QString, name1, setName1, name1Raw, 22 )
    DEFINE_FIELD( QString, defName1, setDefName1, defName1Raw, 23 )
    DEFINE_FIELD( bool, useDefName1, setUseDefName1, useDefName1Raw, 24 )
    DEFINE_FIELD( QString, name2, setName2, name2Raw, 25 )
    DEFINE_FIELD( QString, defName2, setDefName2, defName2Raw, 26 )
    DEFINE_FIELD( bool, useDefName2, setUseDefName2, useDefName2Raw, 27 )
    DEFINE_FIELD( bool, forceNoText, setForceNoText, forceNoTextRaw, 28 )
    DEFINE_FIELD( bool, forceNoBgColor, setForceNoBgColor, forceNoBgColorRaw, 29 )
    DEFINE_FIELD( bool, forceNoBgImage, setForceNoBgImage, forceNoBgImageRaw, 30 )
    DEFINE_FIELD( QString, scriptName, setScriptName, scriptNameRaw, 31 )
    DEFINE_FIELD( int, scriptInterval, setScriptInterval, scriptIntervalRaw, 32 )
    DEFINE_FIELD( QString, bgToggledColor, setBgToggledColor, bgToggledColorRaw, 33 )
    DEFINE_FIELD( QString, bgToggledImage, setToggledBgImage, bgToggledImageRaw, 34 )
};

// 136,37
BEGIN_SESSION_MESSAGE( ListSoftkeyScriptsMsg, SoftkeyDev, ListSoftkeyScripts )
    DEFINE_RO_FIELD( 2, int, count )
    DEFINE_FIELD( QString, scriptList, setScriptList, scriptListRaw, 3 )
END_SESSION_MESSAGE

// 136,38
BEGIN_SESSION_MESSAGE( SoftkeyScriptResultMsg, SoftkeyDev, SoftkeyScriptResult )
    DEFINE_FIELD( QString, scriptName, setScriptName, scriptNameRaw, 2)
    DEFINE_FIELD( int, exitStatus, setExitStatus, exitStatusRaw, 3)
    DEFINE_FIELD( QString, scriptOutput, setScriptOutput, scriptOutputRaw, 4)
END_SESSION_MESSAGE

//  (136,42)
class DomSoftkeyFeature : public SessionMessageMap<SoftkeyDev,WriteSoftkeyFeature>
{
    typedef DomSoftkeyFeature _Myt;
    typedef SessionMessageMap<SoftkeyDev,WriteSoftkeyFeature> _Mybase;
public:
    DomSoftkeyFeature() : _Mybase() {}
    DomSoftkeyFeature( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSoftkeyFeature( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSoftkeyFeature() {}

    DEFINE_FIELD( int, featureNum, setFeatureNum, featureNumRaw, 2 )
    DEFINE_FIELD( int, defaultGroupNum, setDefaultGroupNum, defaultGroupNumRaw, 3 )
    DEFINE_FIELD( int, styleNum, setStyleNum, styleNumRaw, 4 )
    DEFINE_FIELD( int, horizGridSize, setHorizGridSize, horizGridSizeRaw, 5 )
    DEFINE_FIELD( int, vertGridSize, setVertGridSize, vertGridSizeRaw, 6 )
    DEFINE_FIELD( int, horizSpacing, setHorizSpacing, horizSpacingRaw, 7 )
    DEFINE_FIELD( int, vertSpacing, setVertSpacing, vertSpacingRaw, 8 )
    DEFINE_FIELD( int, fixedRowColMethod, setFixedRowColMethod, fixedRowColMethodRaw, 9 )
    DEFINE_FIELD( int, defaultButton, setDefaultButton, defaultButtonRaw, 10 )
};

//  (136,62)
class DomSoftkeyStyle : public SessionMessageMap<SoftkeyDev,WriteSoftkeyStyle>
{
    typedef DomSoftkeyStyle _Myt;
    typedef SessionMessageMap<SoftkeyDev,WriteSoftkeyStyle> _Mybase;
public:
    DomSoftkeyStyle() : _Mybase() {}
    DomSoftkeyStyle( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSoftkeyStyle( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSoftkeyStyle() {}

    DEFINE_FIELD( int, styleNum, setStyleNum, styleNumRaw, 2 )
    DEFINE_FIELD( int, textVAlign, setTextVAlign, textVAlignRaw, 3 )
    DEFINE_FIELD( QString, textColor, setTextColor, textColorRaw, 4 )
    DEFINE_FIELD( QString, grayedOutTextColor, setGrayedOutTextColor, grayedOutTextColorRaw, 5 )
    DEFINE_FIELD( QString, backgroundColor, setBackgroundColor, backgroundColorRaw, 6 )
    DEFINE_FIELD( QString, backgroundImage, setBackgroundImage, backgroundImageRaw, 7 )
    DEFINE_FIELD( QString, imageDir, setImageDir, imageDirRaw, 8 )
    DEFINE_FIELD( QString, fgImageLoc, setFgImageLoc, fgImageLocRaw, 9 )
};

//  (136,74)
class DomSoftkeyStyleNumbers : public SessionMessageMap<SoftkeyDev,WriteAllSoftkeyStyleNumbers>
{
    typedef DomSoftkeyStyleNumbers _Myt;
    typedef SessionMessageMap<SoftkeyDev,WriteAllSoftkeyStyleNumbers> _Mybase;
public:
    DomSoftkeyStyleNumbers() : _Mybase() {}
    DomSoftkeyStyleNumbers( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSoftkeyStyleNumbers( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSoftkeyStyleNumbers() {}

    QString& numOfItems() {return _Mybase::operator [] (2);}
    QString& numbers() {return _Mybase::operator [] (3);}
};

//  (136,82)
class DomSoftkeyGroup : public SessionMessageMap<SoftkeyDev,WriteSoftkeyGroup>
{
    typedef DomSoftkeyGroup _Myt;
    typedef SessionMessageMap<SoftkeyDev,WriteSoftkeyGroup> _Mybase;
public:
    DomSoftkeyGroup() : _Mybase() {}
    DomSoftkeyGroup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSoftkeyGroup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSoftkeyGroup() {}

    DEFINE_FIELD( int, groupNum, setGroupNum, groupNumRaw, 2 )
    DEFINE_FIELD( int, horizGridSize, setHorizGridSize, horizGridSizeRaw, 3 )
    DEFINE_FIELD( int, vertGridSize, setVertGridSize, vertGridSizeRaw, 4 )
    DEFINE_FIELD( int, horizSpacing, setHorizSpacing, horizSpacingRaw, 5 )
    DEFINE_FIELD( int, vertSpacing, setVertSpacing, vertSpacingRaw, 6 )
};

// (136,83)
class DomWriteAllSoftkeyGroups : public SessionMessageMap<SoftkeyDev,WriteAllSoftkeyGroups>
{
    typedef DomWriteAllSoftkeyGroups _Myt;
    typedef SessionMessageMap<SoftkeyDev,WriteAllSoftkeyGroups> _Mybase;
public:
    DomWriteAllSoftkeyGroups() : _Mybase() {}
    DomWriteAllSoftkeyGroups( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWriteAllSoftkeyGroups( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWriteAllSoftkeyGroups() {}

    DEFINE_FIELD( int, numGroups, setNumGroups, numGroupsRaw, 2 )
    DEFINE_FIELD( integerList, groupList, setGroupList, groupListRaw, 3 )
};

//(136,93)														// existed in domOperate and domSuper, so I deleted the Super version
class DomLoadSoftkeyGroup : public SessionMessageMap<SoftkeyDev,LoadSoftkeyGroup>
{
    typedef DomLoadSoftkeyGroup _Myt;
    typedef SessionMessageMap<SoftkeyDev,LoadSoftkeyGroup> _Mybase;
public:
    DomLoadSoftkeyGroup() : _Mybase() {}
    DomLoadSoftkeyGroup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLoadSoftkeyGroup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLoadSoftkeyGroup() {}

    DEFINE_FIELD( int, featureNum, setFeatureNum, featureNumRaw, 2 )
    DEFINE_FIELD( int, groupNum, setGroupNum, groupNumRaw, 3 )
    DEFINE_FIELD( int, horizGridSize, setHorizGridSize, horizGridSizeRaw, 4 )
    DEFINE_FIELD( int, vertGridSize, setVertGridSize, vertGridSizeRaw, 5 )
    DEFINE_FIELD( int, horizSpacing, setHorizSpacing, horizSpacingRaw, 6 )
    DEFINE_FIELD( int, vertSpacing, setVertSpacing, vertSpacingRaw, 7 )
    DEFINE_FIELD( int, textVertAlign, setTextVertAlign, textVertAlignRaw, 8 )
    QString& textColor() {return _Mybase::operator [] (9);}
    QString& disabledTextColor() {return _Mybase::operator [] (10);}
    QString& styleBgColor() {return _Mybase::operator [] (11);}
    QString& styleBgImage() {return _Mybase::operator [] (12);}
    QString& fgImageLocation() {return _Mybase::operator [] (13);}
    QString& imageDir() {return _Mybase::operator [] (14);}
    DEFINE_FIELD( int, currentPageFirstSoftkey, setCurrentPageFirstSoftkey, currentPageFirstSoftkeyRaw, 15 )
    DEFINE_FIELD( int, totalNumOfSoftkeys, setTotalNumOfSoftkeys, totalNumOfSoftkeysRaw, 16 )
    DEFINE_FIELD( int, numOfSoftkeys, setNumOfSoftkeys, numOfSoftkeysRaw, 17 )
    QString& hPosList() {return _Mybase::operator [] (18);}
    DEFINE_FIELD( int, count1, setCount1, count1Raw, 19 )
    QString& vPosList() {return _Mybase::operator [] (20);}
    DEFINE_FIELD( int, count2, setCount2, count2Raw, 21 )
    QString& hSizeList() {return _Mybase::operator [] (22);}
    DEFINE_FIELD( int, count3, setCount3, count3Raw, 23 )
    QString& vSizeList() {return _Mybase::operator [] (24);}
    DEFINE_FIELD( int, count4, setCount4, count4Raw, 25 )
    QString& bgColorList() {return _Mybase::operator [] (26);}
    DEFINE_FIELD( int, count5, setCount5, count5Raw, 27 )
    QString& bgImageList() {return _Mybase::operator [] (28);}
    DEFINE_FIELD( int, count6, setCount6, count6Raw, 29 )
    QString& nameList() {return _Mybase::operator [] (30);}
    DEFINE_FIELD( int, count7, setCount7, count7Raw, 31 )
    QString& actionList() {return _Mybase::operator [] (32);}
    DEFINE_FIELD( int, count8, setCount8, count8Raw, 33 )
    QString& actionParamList() {return _Mybase::operator [] (34);}
    DEFINE_FIELD( int, count9, setCount9, count9Raw, 35 )
    QString& actionFeatureNumList() {return _Mybase::operator [] (36);}
    DEFINE_FIELD( int, count10, setCount10, count01Raw, 37 )
    QString& isEnabledList() {return _Mybase::operator [] (38);}
    DEFINE_FIELD( int, count11, setCount11, count11Raw, 39 )
    QString& fgImageList() {return _Mybase::operator [] (40);}
    DEFINE_FIELD( int, count12, setCount12, count12Raw, 41 )
    QString& textColorList() {return _Mybase::operator [] (42);}
    DEFINE_FIELD( int, count13, setCount13, setCount13Raw, 43)
    QString& scriptNameList() {return _Mybase::operator [] (44) ;}
    DEFINE_FIELD( int, count14, setCount14, setCount14Raw, 45)
    QString& scriptIntervalList() {return _Mybase::operator [] (46) ;}
    /////////////////////////////////////////////////////////
};

//(136,255)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomLoadAllSoftkeyGroup : public SessionMessageMap<SoftkeyDev,LoadAllSoftkeyGroups>
{
    typedef DomLoadAllSoftkeyGroup _Myt;
    typedef SessionMessageMap<SoftkeyDev,LoadAllSoftkeyGroups> _Mybase;
public:
    DomLoadAllSoftkeyGroup() : _Mybase() {}
    DomLoadAllSoftkeyGroup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomLoadAllSoftkeyGroup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomLoadAllSoftkeyGroup() {}

    QString& noData() {return _Mybase::operator [] (2);}
};

//  (137,2)
class DomMarqueeTextData : public SessionMessageMap<MarqueeTextDev,MarqueeTextDtWrite>
{
    typedef DomMarqueeTextData _Myt;
    typedef SessionMessageMap<MarqueeTextDev,MarqueeTextDtWrite> _Mybase;
public:
    DomMarqueeTextData() : _Mybase() {}
    DomMarqueeTextData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomMarqueeTextData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomMarqueeTextData() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}

    QString marqueeTextDataStripped() const {
        QString result = const_cast<_Myt*>( this )->textText();
        result.replace( "\\S", "" );
        result.replace( "\\F", "" );
        result.replace( "\\R", "" );
        result.replace( "\\H", "" );
        result.replace( "\\C", "" );
        result.replace( "\\B", "" );
        result.replace( "\\L", "" );
        result.replace( "\\N", "" );
        result.replace( "\\Q", "" );
        result.replace( "\\X", "" );
        return result;
    }
};

// FORM (138,2)
class DomInstallationLogDeviceData : public SessionMessageMap<PeripheralDev,PeripheralDtWrite>
{
    typedef DomInstallationLogDeviceData _Myt;
    typedef SessionMessageMap<PeripheralDev,PeripheralDtWrite> _Mybase;
public:
    DomInstallationLogDeviceData() : _Mybase() {}
    DomInstallationLogDeviceData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomInstallationLogDeviceData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomInstallationLogDeviceData() {}

    QString& deviceType() {return _Mybase::operator [] (2);}
    QString& deviceSubtype() {return _Mybase::operator [] (3);}
    QString& deviceKind() {return _Mybase::operator [] (4);}
    QString& serialNum() {return _Mybase::operator [] (5);}
    QString& deviceHistory() {return _Mybase::operator [] (6);}
    QString& date() {return _Mybase::operator [] (7);}
};

// (138,20)
class MadpLostPeripheralList : public SessionMessageMap<PeripheralDev,WriteLostPeripheralsList>
{
    typedef MadpLostPeripheralList _Myt;
    typedef SessionMessageMap<PeripheralDev,WriteLostPeripheralsList> _Mybase;
public:
    MadpLostPeripheralList() : _Mybase() {}
    MadpLostPeripheralList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpLostPeripheralList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpLostPeripheralList() {}

    // check for any peripheral missing
    virtual bool isMissingPeripheral() const {
        return (( 0 != firstWeigher() ) ||
            ( 0 != secondWeigher() ) ||
            ( 0 != firstPrinter() ) ||
            ( 0 != secondPrinter() ) ||
            ( 0 != thirdPrinter() ) ||
            ( 0 != labelApplier() ) ||
            ( 0 != wrapper() ) ||
            ( 0 != gcd() ));
    }

    DEFINE_FIELD( int, firstWeigher, setFirstWeigher, firstWeigherRaw, 2 )
    DEFINE_FIELD( int, secondWeigher, setSecondWeigher, secondWeigherRaw, 3 )
    DEFINE_FIELD( int, firstPrinter, setFirstPrinter, firstPrinterRaw, 4 )
    DEFINE_FIELD( int, secondPrinter, setSecondPrinter, secondPrinterRaw, 5 )
    DEFINE_FIELD( int, thirdPrinter, setThirdPrinter, thirdPrinterRaw, 6 )
    DEFINE_FIELD( int, labelApplier, setLabelApplier, labelApplierRaw, 7 )
    DEFINE_FIELD( int, wrapper, setWrapper, wrapperRaw, 8 )
    DEFINE_FIELD( int, numeric, setNumeric, numericRaw, 9 )
    DEFINE_FIELD( int, marquee, setMarquee, marqueeRaw, 10 )
    DEFINE_FIELD( int, gcd, setGcd, gcdRaw, 11 )
};

//(142,4)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomNumericPowerupTests : public SessionMessageMap<NumericSystem,NmStatus>
{
    typedef DomNumericPowerupTests _Myt;
    typedef SessionMessageMap<NumericSystem,NmStatus> _Mybase;
public:
    DomNumericPowerupTests() : _Mybase() {}
    DomNumericPowerupTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNumericPowerupTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNumericPowerupTests() {}

    QString& storedChecksum() {return _Mybase::operator [] (2);}
    QString& calculatedChecksum() {return _Mybase::operator [] (3);}
    QString& EEPChecksum() {return _Mybase::operator [] (4);}
    QString& wordTest() {return _Mybase::operator [] (5);}
    QString& addressTest() {return _Mybase::operator [] (6);}
    QString& softwareVersion() {return _Mybase::operator [] (7);}
    QString& hardwareVersion() {return _Mybase::operator [] (8);}
};

//(143,4)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomMarqueePowerupTests : public SessionMessageMap<MarqueeSystem,MqStatus>
{
    typedef DomMarqueePowerupTests _Myt;
    typedef SessionMessageMap<MarqueeSystem,MqStatus> _Mybase;
public:
    DomMarqueePowerupTests() : _Mybase() {}
    DomMarqueePowerupTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomMarqueePowerupTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomMarqueePowerupTests() {}

    QString& storedChecksum() {return _Mybase::operator [] (2);}
    QString& calculatedChecksum() {return _Mybase::operator [] (3);}
    QString& EEPChecksum() {return _Mybase::operator [] (4);}
    QString& wordTest() {return _Mybase::operator [] (5);}
    QString& addressTest() {return _Mybase::operator [] (6);}
    QString& softwareVersion() {return _Mybase::operator [] (7);}
    QString& hardwareVersion() {return _Mybase::operator [] (8);}
};

// (145,23)
class MadpNetworkConnectivity : public SessionMessageMap<NetworkConnectivityDev,CurrentStatus>
{
    typedef MadpNetworkConnectivity _Myt;
    typedef SessionMessageMap<NetworkConnectivityDev,CurrentStatus> _Mybase;
public:
    MadpNetworkConnectivity() : _Mybase() {}
    MadpNetworkConnectivity( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpNetworkConnectivity( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpNetworkConnectivity() {}

    DEFINE_FIELD( bool, isNetworkConnected, setIsNetworkConnected, isNetworkConnectedRaw, 2 )
    DEFINE_FIELD( int, type, setType, typeRaw, 3 ) // 0=wired, 1 = wireless
    DEFINE_FIELD( int, strength, setStrength, strengthRaw, 4 ) // 0-5
};

// (145,35)
class MadpNetworkDeviceType : public SessionMessageMap<NetworkConnectivityDev,NetworkDeviceType>
{
    typedef MadpNetworkDeviceType _Myt;
    typedef SessionMessageMap<NetworkConnectivityDev,NetworkDeviceType> _Mybase;
public:
    MadpNetworkDeviceType() : _Mybase() {}
    MadpNetworkDeviceType( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpNetworkDeviceType( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpNetworkDeviceType() {}

    QString& deviceType() {return _Mybase::operator [] (2);} // 0=wire 1=wifi}
};


//(150,2)
class DomForm1 : public SessionMessageMap<CGICallManagerDev,CGICallPerformed>
{
    typedef DomForm1 _Myt;
    typedef SessionMessageMap<CGICallManagerDev,CGICallPerformed> _Mybase;
public:
    DomForm1() : _Mybase() {}
    DomForm1( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomForm1( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomForm1() {}

    QString& resultCode() {return _Mybase::operator [] (2);}
};

//(151,21)														// existed in domOperate and domSuper, so I deleted the Super version
class DomVoidableTransactionGroup : public SessionMessageMap<TransactionDev,TransactionWriteNonVoidGroup>
{
    typedef DomVoidableTransactionGroup _Myt;
    typedef SessionMessageMap<TransactionDev,TransactionWriteNonVoidGroup> _Mybase;
public:
    DomVoidableTransactionGroup() : _Mybase() {}
    DomVoidableTransactionGroup( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomVoidableTransactionGroup( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomVoidableTransactionGroup() {}

    //QString& numberIds() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( int, numberIds, setNumberIds, numberIdsRaw, 2 )
    QString& listIds() {return _Mybase::operator [] (3);}
    //QString& numberDateTimes() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, numberDateTimes, setNumberDateTimes, numberDateTimesRaw, 4 )
    QString& listDateTimes() {return _Mybase::operator [] (5);}
    //QString& numberOperators() {return _Mybase::operator [] (6);}
    DEFINE_FIELD( int, numberOperators, setNumberOperators, numberOperatorsRaw, 6 )
    QString& listOperators() {return _Mybase::operator [] (7);}
    //QString& numberTransTypes() {return _Mybase::operator [] (8);}
    DEFINE_FIELD( int, numberTransTypes, setNumberTransTypes, numberTransTypesRaw, 8 )
    QString& listTransTypes() {return _Mybase::operator [] (9);}
    //QString& numberPLUs() {return _Mybase::operator [] (10);}
    DEFINE_FIELD( int, numberPLUs, setNumberPLUs, numberPLUsRaw, 10 )
    QString& listPLUs() {return _Mybase::operator [] (11);}
    //QString& numberWeights() {return _Mybase::operator [] (12);}
    DEFINE_FIELD( int, numberWeights, setNumberWeights, numberWeightsRaw, 12 )
    QString& listWeights() {return _Mybase::operator [] (13);}
    //QString& numberPrices() {return _Mybase::operator [] (14);}
    DEFINE_FIELD( int, numberPrices, setNumberPrices, numberPricesRaw, 14 )
    QString& listPrices() {return _Mybase::operator [] (15);}
    //QString& numberItems() {return _Mybase::operator [] (16);}
    DEFINE_FIELD( int, numberItems, setNumberItems, numberItemsRaw, 16 )
    QString& listItems() {return _Mybase::operator [] (17);}
    //QString& previousAvailable() {return _Mybase::operator [] (18);}
    DEFINE_FIELD( bool, previousAvailable, setPreviousAvailable, previousAvailableRaw, 18 )
    //QString& nextAvailable() {return _Mybase::operator [] (19);}
    DEFINE_FIELD( bool, nextAvailable, setNextAvailable, nextAvailableRaw, 19 )
};

//  (151,31)
class DomTransactionDates : public SessionMessageMap<TransactionDev,TransactionWriteDates>
{
    typedef DomTransactionDates _Myt;
    typedef SessionMessageMap<TransactionDev,TransactionWriteDates> _Mybase;
public:
    DomTransactionDates() : _Mybase() {}
    DomTransactionDates( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTransactionDates( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTransactionDates() {}

    QString& numOfWeekDays() {return _Mybase::operator [] (2);}
    QString& weekDaysList() {return _Mybase::operator [] (3);}
    QString& numOfDatesDisplay() {return _Mybase::operator [] (4);}
    QString& datesDisplayList() {return _Mybase::operator [] (5);}
    QString& numOfDates() {return _Mybase::operator [] (6);}
    QString& datesList() {return _Mybase::operator [] (7);}
};

//  (152,2)
class DomTotalTypeData : public SessionMessageMap<TransactionTypeDev,TransactionTypeDtWrite>
{
    typedef DomTotalTypeData _Myt;
    typedef SessionMessageMap<TransactionTypeDev,TransactionTypeDtWrite> _Mybase;
public:
    DomTotalTypeData() : _Mybase() {}
    DomTotalTypeData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTotalTypeData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTotalTypeData() {}

    QString& number() {return _Mybase::operator [] (2);}
    QString& primaryName() {return _Mybase::operator [] (3);}
    QString& secondName() {return _Mybase::operator [] (4);}
    QString& grossWeight() {return _Mybase::operator [] (5);}
    QString& upcChangeDigit() {return _Mybase::operator [] (6);}
    QString& reportsToPrepack() {return _Mybase::operator [] (7);}
    QString& manualChangeUPCDigit() {return _Mybase::operator [] (8);}
    QString& autoChangeUPCDigit() {return _Mybase::operator [] (9);}
    QString& allowBackDating() {return _Mybase::operator [] (10);}
    QString& flags() {return _Mybase::operator [] (11);}
    QString& useBackgroundColor() {return _Mybase::operator [] (12);}
    QString& backgroundColor() {return _Mybase::operator [] (13);}
    QString& textForLabel() {return _Mybase::operator [] (14);}
};

//(152,21)
class DomTotalTypesList : public SessionMessageMap<TransactionTypeDev,TransactionTypeWriteAllNames>
{
    typedef DomTotalTypesList _Myt;
    typedef SessionMessageMap<TransactionTypeDev,TransactionTypeWriteAllNames> _Mybase;
public:
    DomTotalTypesList() : _Mybase() {}
    DomTotalTypesList( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTotalTypesList( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTotalTypesList() {}

    //QString& numOfRecordIds() {return _Mybase::operator [] (2);}
    DEFINE_FIELD( int, numOfRecordIds, setNumOfRecordIds, numOfRecordIdsRaw, 2 )
    QString& allRecordIds() {return _Mybase::operator [] (3);}
    //QString& priNumOfNames() {return _Mybase::operator [] (4);}
    DEFINE_FIELD( int, priNumOfNames, setPriNumOfNames, priNumOfNamesRaw, 4 )
    QString& namesList() {return _Mybase::operator [] (5);}
    //QString& secNumOfNames() {return _Mybase::operator [] (6);}
    DEFINE_FIELD( int, secNumOfNames, setSecNumOfNames, secNumOfNamesRaw, 6 )
    QString& secNamesList() {return _Mybase::operator [] (7);}
};

//(152,31)
class DomTotalTypeNameSetting : public SessionMessageMap<TransactionTypeDev,TransactionTypeWriteCurrentType>
{
    typedef DomTotalTypeNameSetting _Myt;
    typedef SessionMessageMap<TransactionTypeDev,TransactionTypeWriteCurrentType> _Mybase;
public:
    DomTotalTypeNameSetting() : _Mybase() {}
    DomTotalTypeNameSetting( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTotalTypeNameSetting( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTotalTypeNameSetting() {}

    QString& transNum() {return _Mybase::operator [] (2);}
    QString& name1() {return _Mybase::operator [] (3);}
    QString& name2() {return _Mybase::operator [] (4);}
    QString& manualUPCDigitChange() {return _Mybase::operator [] (5);}
    QString& allowProductBackdating() {return _Mybase::operator [] (6);}
    QString& useBackgroundColor() {return _Mybase::operator [] (7);}
    QString& backgroundColor() {return _Mybase::operator [] (8);}
};

//  (153,21)
class DomHourlyTotalsData : public SessionMessageMap<HourlyTotalDev,WriteFilteredHourlyTotals>
{
    typedef DomHourlyTotalsData _Myt;
    typedef SessionMessageMap<HourlyTotalDev,WriteFilteredHourlyTotals> _Mybase;
public:
    DomHourlyTotalsData() : _Mybase() {}
    DomHourlyTotalsData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomHourlyTotalsData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomHourlyTotalsData() {}

    QString& numOfHours() {return _Mybase::operator [] (2);}
    QString& hoursList() {return _Mybase::operator [] (3);}
    QString& numOfWeights() {return _Mybase::operator [] (4);}
    QString& weightsList() {return _Mybase::operator [] (5);}
    QString& numOfPrices() {return _Mybase::operator [] (6);}
    QString& pricesList() {return _Mybase::operator [] (7);}
    QString& numOfItems() {return _Mybase::operator [] (8);}
    QString& itemsList() {return _Mybase::operator [] (9);}
    QString& numOfTrans() {return _Mybase::operator [] (10);}
    QString& transactionList() {return _Mybase::operator [] (11);}
};

//  (154,21)
class DomOperatorTotalsData : public SessionMessageMap<OperatorTotalDev,WriteFilteredOperatorTotals>
{
    typedef DomOperatorTotalsData _Myt;
    typedef SessionMessageMap<OperatorTotalDev,WriteFilteredOperatorTotals> _Mybase;
public:
    DomOperatorTotalsData() : _Mybase() {}
    DomOperatorTotalsData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorTotalsData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorTotalsData() {}

    QString& numOfNames() {return _Mybase::operator [] (2);}
    QString& namesList() {return _Mybase::operator [] (3);}
    QString& numOfWeights() {return _Mybase::operator [] (4);}
    QString& weightsList() {return _Mybase::operator [] (5);}
    QString& numOfPrices() {return _Mybase::operator [] (6);}
    QString& pricesList() {return _Mybase::operator [] (7);}
    QString& numOfItems() {return _Mybase::operator [] (8);}
    QString& itemsList() {return _Mybase::operator [] (9);}
    QString& numOfTrans() {return _Mybase::operator [] (10);}
    QString& transactionList() {return _Mybase::operator [] (11);}
};

//  (156,21)
class DomProductTotalsData : public SessionMessageMap<ProductTotalDev,WriteFilteredProductTotals>
{
    typedef DomProductTotalsData _Myt;
    typedef SessionMessageMap<ProductTotalDev,WriteFilteredProductTotals> _Mybase;
public:
    DomProductTotalsData() : _Mybase() {}
    DomProductTotalsData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomProductTotalsData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomProductTotalsData() {}

    QString& numOfTrans() {return _Mybase::operator [] (2);}
    QString& transList() {return _Mybase::operator [] (3);}
    QString& numOfWeights() {return _Mybase::operator [] (4);}
    QString& weightsList() {return _Mybase::operator [] (5);}
    QString& numOfPrices() {return _Mybase::operator [] (6);}
    QString& pricesList() {return _Mybase::operator [] (7);}
    QString& numOfItems() {return _Mybase::operator [] (8);}
    QString& itemsList() {return _Mybase::operator [] (9);}
    QString& numOfTransTypes() {return _Mybase::operator [] (10);}
    QString& transTypeList() {return _Mybase::operator [] (11);}
};

//(159,2)
class DomCuttingTest : public SessionMessageMap<CutTestDev,CuttingTestDtWrite>
{
    typedef DomCuttingTest _Myt;
    typedef SessionMessageMap<CutTestDev,CuttingTestDtWrite> _Mybase;
public:
    DomCuttingTest() : _Mybase() {}
    DomCuttingTest( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCuttingTest( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCuttingTest() {}

    QString& testNum() {return _Mybase::operator [] (2);}
    QString& operNum() {return _Mybase::operator [] (3);}
    QString& vendorNum() {return _Mybase::operator [] (4);}
    QString& productNum() {return _Mybase::operator [] (5);}
    QString& primalWeight() {return _Mybase::operator [] (6);}
    QString& primalCost() {return _Mybase::operator [] (7);}
    QString& startDate() {return _Mybase::operator [] (8);}
    QString& endDate() {return _Mybase::operator [] (9);}
    QString& lotNumber() {return _Mybase::operator [] (10);}        // new
    QString& packingDate() {return _Mybase::operator [] (11);}      // new
    QString& packingHours() {return _Mybase::operator [] (12);}     // new
    QString& packingMinutes() {return _Mybase::operator [] (13);}   // new
    QString& comments() {return _Mybase::operator [] (14);}         // new
    QString& finalComments() {return _Mybase::operator [] (15);}      // new

};

//(159,24)
class DomCuttingTestTotals : public SessionMessageMap<CutTestDev,CuttingTestDetailsWrite>
{
    typedef DomCuttingTestTotals _Myt;
    typedef SessionMessageMap<CutTestDev,CuttingTestDetailsWrite> _Mybase;
public:
    DomCuttingTestTotals() : _Mybase() {}
    DomCuttingTestTotals( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCuttingTestTotals( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCuttingTestTotals() {}

    QString& testNum() {return _Mybase::operator [] (2);}               // shared - cut test default & option 1
    QString& itemNum() {return _Mybase::operator [] (3);}               // shared - cut test default & option 1
    QString& vendorNum() {return _Mybase::operator [] (4);}             // shared - cut test default & option 1 (supplier number)
    QString& operatorID() {return _Mybase::operator [] (5);}            // was operator, but word operator is keyword - made errors.
    QString& sellWeight() {return _Mybase::operator [] (6);}
    QString& shrinkWeight() {return _Mybase::operator [] (7);}
    QString& itemWeight() {return _Mybase::operator [] (8);}            // shared - cut test default & option 1
    QString& sellYield() {return _Mybase::operator [] (9);}
    QString& shrinkYield() {return _Mybase::operator [] (10);}
    QString& itemYield() {return _Mybase::operator [] (11);}
    QString& avgPrice() {return _Mybase::operator [] (12);}
    QString& sellPrice() {return _Mybase::operator [] (13);}
    QString& itemCost() {return _Mybase::operator [] (14);}             // shared - cut test default & option 1
    QString& imu() {return _Mybase::operator [] (15);}
    QString& startDate() {return _Mybase::operator [] (16);}
    QString& endDate() {return _Mybase::operator [] (17);}
    DEFINE_FIELD( int, numOfItems, setNumOfItems, numOfItemsRaw, 18 )   // shared - cut test default & option 1
    QString& itemsList() {return _Mybase::operator [] (19);}            // shared - cut test default & option 1
    DEFINE_FIELD( int, num2, setNum2, num2Raw, 20 )                     // shared - cut test default & option 1
    QString& descList() {return _Mybase::operator [] (21);}             // shared - cut test default & option 1
    DEFINE_FIELD( int, num3, setNum3, num3Raw, 22 )                     // shared - cut test default & option 1
    QString& weightList() {return _Mybase::operator [] (23);}           // shared - cut test default & option 1
    DEFINE_FIELD( int, num4, setNum4, num4Raw, 24 )
    QString& yieldList() {return _Mybase::operator [] (25);}            // shared - this is % yeild under default, participation in option 1
    DEFINE_FIELD( int, num5, setNum5, num5Raw, 26 )                     // shared - cut test default & option 1
    QString& unitPriceList() {return _Mybase::operator [] (27);}        // shared - cut test default & option 1
    DEFINE_FIELD( int, num6, setNum6, num6Raw, 28 )                     // shared - cut test default & option 1
    QString& totalPriceList() {return _Mybase::operator [] (29);}       // shared - cut test default & option 1
    QString& itemLotNumber() {return _Mybase::operator [] (30);}        // new - cutting details test option 1
    QString& itemPackingDate() {return _Mybase::operator [] (31);}      // new - cutting details test option 1
    QString& operatorName() {return _Mybase::operator [] (32);}         // new - cutting details test option 1
    QString& testStatus() {return _Mybase::operator [] (33);}           // new - cutting details test option 1
    DEFINE_FIELD( int, num7, setNum7, num7Raw, 34 )                     // new - cutting details test option 1
    QString& packagesList() {return _Mybase::operator [] (35);}         // new - Column 3 on details screen. Appendix F says this is an existing field, its not
};

// FORM (159,25)
class DomCuttingTestHistoryTotals : public SessionMessageMap<CutTestDev,CuttingTestHistoryWrite>
{
    typedef DomCuttingTestHistoryTotals _Myt;
    typedef SessionMessageMap<CutTestDev,CuttingTestHistoryWrite> _Mybase;
public:
    DomCuttingTestHistoryTotals() : _Mybase() {}
    DomCuttingTestHistoryTotals( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCuttingTestHistoryTotals( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCuttingTestHistoryTotals() {}

    DEFINE_FIELD( int, numOfTests, setTests, testsRaw, 2 )
    QString& testNumList() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numOfItems, setItems, itemsRaw, 4 )
    QString& itemNumList() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, numOfOps, setOps, opsRaw, 6 )
    QString& operatorIDList() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, numOfstati, setStati, statiRaw, 8 )
    QString& testStatusList() {return _Mybase::operator [] (9);}
    DEFINE_FIELD( int, numOfSups, setSups, supsRaw, 10 )
    QString& supplierNumList() {return _Mybase::operator [] (11);}
    DEFINE_FIELD( int, numOfPluCosts, setCosts, costsRaw, 12 )
    QString& prodCostList() {return _Mybase::operator [] (13);}
    DEFINE_FIELD( int, numOfPluWt, setWeights, weightsRaw, 14 )
    QString& prodWeightList() {return _Mybase::operator [] (15);}
    DEFINE_FIELD( int, numOfLotNums, setLots, lotsRaw, 16 )
    QString& lotNumList() {return _Mybase::operator [] (17);}
    DEFINE_FIELD( int, numOfpackDates, setDates, datesRaw, 18 )
    QString& packDateList() {return _Mybase::operator [] (19);}
};

// FORM (161,4)
class DomGcdPowerupTests : public SessionMessageMap<GCDSessionDev,GcdSessionStatus>
{
    typedef DomGcdPowerupTests _Myt;
    typedef SessionMessageMap<GCDSessionDev,GcdSessionStatus> _Mybase;
public:
    DomGcdPowerupTests() : _Mybase() {}
    DomGcdPowerupTests( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomGcdPowerupTests( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomGcdPowerupTests() {}

    QString& gcdDDRTestPassed() {return _Mybase::operator [] (2);}
    QString& gcdBoardVersion() {return _Mybase::operator [] (3);}
    QString& gcdWinCEVersion() {return _Mybase::operator [] (4);}
    QString& gcdMGRVersion() {return _Mybase::operator [] (5);}
    QString& gcdDLLVersion() {return _Mybase::operator [] (6);}
};

// (164,4)
class DomMfgReadData : public SessionMessageMap<MfgTestDev,ReadMfgData>
{
    typedef DomMfgReadData _Myt;
    typedef SessionMessageMap<MfgTestDev,ReadMfgData> _Mybase;
public:
    DomMfgReadData() : _Mybase() {}
    DomMfgReadData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomMfgReadData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomMfgReadData() {}

    DEFINE_FIELD( QString, scaleStyle, setScaleStyle, scaleStyleRaw, 2 )
    DEFINE_FIELD( QString, boardAssembly, setBoardAssembly, boardAssemblyRaw, 3 )
    DEFINE_FIELD( QString, boardRevision, setBoardRevision, boardRevisionRaw, 4 )
    DEFINE_FIELD( QString, dateOfManufacture, setDateOfManufacture, dateOfManufactureRaw, 5 )
    DEFINE_FIELD( QString, serialNumber, setSerialNumber, serialNumberRaw, 6 )
};

// (164,16)
class DomMfgReadLoaderVersion : public SessionMessageMap<MfgTestDev,ReadLoaderVersion>
{
    typedef DomMfgReadLoaderVersion _Myt;
    typedef SessionMessageMap<MfgTestDev,ReadLoaderVersion> _Mybase;
public:
    DomMfgReadLoaderVersion() : _Mybase() {}
    DomMfgReadLoaderVersion( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomMfgReadLoaderVersion( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomMfgReadLoaderVersion() {}

    DEFINE_FIELD( QString, version, setVersion, versionRaw, 2 )
};

// (164,24)
class DomPrinterLabelCount : public SessionMessageMap<MfgTestDev,PrinterLabelCount>
{
    typedef DomPrinterLabelCount _Myt;
    typedef SessionMessageMap<MfgTestDev,PrinterLabelCount> _Mybase;
public:
    DomPrinterLabelCount() : _Mybase() {}
    DomPrinterLabelCount( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomPrinterLabelCount( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomPrinterLabelCount() {}

    DEFINE_FIELD( int, count, setCount, countRaw, 2 )
};

// (164,27)
class DomWrapperCycleCount : public SessionMessageMap<MfgTestDev,WrapperCycleCount>
{
    typedef DomWrapperCycleCount _Myt;
    typedef SessionMessageMap<MfgTestDev,WrapperCycleCount> _Mybase;
public:
    DomWrapperCycleCount() : _Mybase() {}
    DomWrapperCycleCount( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomWrapperCycleCount( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomWrapperCycleCount() {}

    DEFINE_FIELD( int, count, setCount, countRaw, 2 )
};

//  (165,2)
class DomCookingInstructionsData : public SessionMessageMap<CookingInstructionsDev,CookingInstructionsDtWrite>
{
    typedef DomCookingInstructionsData _Myt;
    typedef SessionMessageMap<CookingInstructionsDev,CookingInstructionsDtWrite> _Mybase;
public:
    DomCookingInstructionsData() : _Mybase() {}
    DomCookingInstructionsData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCookingInstructionsData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCookingInstructionsData() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}
};

//  (166,2)
class DomRecipeData : public SessionMessageMap<RecipeDev,RecipeDtWrite>
{
    typedef DomRecipeData _Myt;
    typedef SessionMessageMap<RecipeDev,RecipeDtWrite> _Mybase;
public:
    DomRecipeData() : _Mybase() {}
    DomRecipeData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRecipeData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRecipeData() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}
};

//  (167,2)
class DomExpandedText2Data : public SessionMessageMap<ExpandedText2Dev,ExpandedText2DtWrite>
{
    typedef DomExpandedText2Data _Myt;
    typedef SessionMessageMap<ExpandedText2Dev,ExpandedText2DtWrite> _Mybase;
public:
    DomExpandedText2Data() : _Mybase() {}
    DomExpandedText2Data( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomExpandedText2Data( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomExpandedText2Data() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}
};

//  (168,2)
class DomSuggestiveSellingData : public SessionMessageMap<SuggestiveSellingDev,SuggestiveSellingDtWrite>
{
    typedef DomSuggestiveSellingData _Myt;
    typedef SessionMessageMap<SuggestiveSellingDev,SuggestiveSellingDtWrite> _Mybase;
public:
    DomSuggestiveSellingData() : _Mybase() {}
    DomSuggestiveSellingData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomSuggestiveSellingData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomSuggestiveSellingData() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}
};

//  (169,2)
class DomAllergensData : public SessionMessageMap<AllergensDev,AllergensDtWrite>
{
    typedef DomAllergensData _Myt;
    typedef SessionMessageMap<AllergensDev,AllergensDtWrite> _Mybase;
public:
    DomAllergensData() : _Mybase() {}
    DomAllergensData( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomAllergensData( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomAllergensData() {}

    QString& textNum() {return _Mybase::operator [] (2);}
    QString& textText() {return _Mybase::operator [] (3);}
    QString& errorCode() {return _Mybase::operator [] (4);}
};

// (170,3)
class DomRecalls : public SessionMessageMap<RecallsDev,RecallsDevWriteAll>
{
    typedef DomRecalls _Myt;
    typedef SessionMessageMap<RecallsDev,RecallsDevWriteAll> _Mybase;
public:
    DomRecalls() : _Mybase() {}
    DomRecalls( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRecalls( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRecalls() {}

    DEFINE_FIELD( int, numIds, setNumIds, numIdsRaw, 2 )
    QString& ids() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numDates, setNumDates, numDatesRaw, 4 )
    QString& dates() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, numTexts, setNumTexts, numTextsRaw, 6 )
    QString& texts() {return _Mybase::operator [] (7);}
    DEFINE_FIELD( int, numWasReadFlags, setNumWasReadFlags, numWasReadFlagsRaw, 8 )
    QString& wasReadFlags() {return _Mybase::operator [] (9);}
};

// (170,20)
class DomRecallsExist : public SessionMessageMap<RecallsDev,RecallsDevNewRecallsExist>
{
    typedef DomRecallsExist _Myt;
    typedef SessionMessageMap<RecallsDev,RecallsDevNewRecallsExist> _Mybase;
public:
    DomRecallsExist() : _Mybase() {}
    DomRecallsExist( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomRecallsExist( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomRecallsExist() {}

    DEFINE_FIELD( bool, existFlag, setExistFlag, existFlagRaw, 2 )
};

// 173,33
BEGIN_SESSION_MESSAGE( DiagConfigurationMsg, Diagnostics, DiagConfiguration )
    DEFINE_RW_FIELD( 2, QString, serviceContactInfo, setServiceContactInfo )
    DEFINE_RW_FIELD( 3, QString, gatewayDeviceAddress, setGatewayDeviceAddress )
    DEFINE_RW_FIELD( 4, int, gatewayDeviceLogLevel, setGatewayDeviceLogLevel )
    DEFINE_RW_FIELD( 5, bool, disableValueMaxDiagCollection, setDisableValueMaxDiagCollection )
END_SESSION_MESSAGE

// 173,35
BEGIN_SESSION_MESSAGE( DiagStatusMsg, Diagnostics, DiagStatus )
    DEFINE_RO_FIELD( 2, bool, gatewayOnline )
    DEFINE_RO_FIELD( 3, bool, gatewayDiscovered )
    DEFINE_RO_FIELD( 4, QString, gatewayAddress )
    DEFINE_RO_FIELD( 5, int, offlineMessages )
END_SESSION_MESSAGE

// 173,49
BEGIN_SESSION_MESSAGE( DiagAuditLogMsg, Diagnostics, DiagAuditLog )
    DEFINE_RO_FIELD( 2, int, count )
    DEFINE_RO_FIELD( 3, QString, auditRecords )
END_SESSION_MESSAGE

// 173,54
BEGIN_SESSION_MESSAGE( DiagAuditLogServiceVisitStatusMsg, Diagnostics, DiagAuditLogServiceVisitStatus )
    DEFINE_RO_FIELD( 2, bool, start )
    DEFINE_RO_FIELD( 3, bool, stop )
END_SESSION_MESSAGE

// 173,51
BEGIN_SESSION_MESSAGE( DiagAuditLogRecordMsg, Diagnostics, DiagAuditLogRecord )
    DEFINE_RO_FIELD( 2, QString, timestamp )
    DEFINE_RO_FIELD( 3, int, severity )
    DEFINE_RO_FIELD( 4, int, source )
    DEFINE_RO_FIELD( 5, int, sourceExt )
    DEFINE_RO_FIELD( 6, QString, description )
END_SESSION_MESSAGE

// 173,64
BEGIN_SESSION_MESSAGE( DiagCheckErrorHandlerReqMsg, Diagnostics, DiagCheckErrorHandlerReq )
    DEFINE_RO_FIELD( 2, int, device )
    DEFINE_RO_FIELD( 3, int, errorCode )
END_SESSION_MESSAGE

// 173,65
BEGIN_SESSION_MESSAGE( DiagCheckErrorHandlerMsg, Diagnostics, DiagCheckErrorHandler )
    DEFINE_RO_FIELD( 2, int, device )
    DEFINE_RO_FIELD( 3, int, errorCode )
    DEFINE_RO_FIELD( 4, bool, available )
    DEFINE_RO_FIELD( 5, bool, force )
    DEFINE_RO_FIELD( 6, QString, event )
END_SESSION_MESSAGE

// 173,67
BEGIN_SESSION_MESSAGE( DiagStartErrorHandlerFromMaintRecordMsg, Diagnostics, DiagStartErrorHandlerFromMaintRecord )
    DEFINE_RW_FIELD( 2, QString, maintRecord, setMaintRecord )
    DEFINE_RW_FIELD( 3, bool, serviceMode, setServiceMode )
END_SESSION_MESSAGE

// 173,80
BEGIN_SESSION_MESSAGE( DiagShowWizardMsg, Diagnostics, DiagShowWizard )
    DEFINE_RO_FIELD( 2, QString, event )
    DEFINE_RO_FIELD( 3, QString, timestamp )
    DEFINE_RO_FIELD( 4, QString, diagEvent )
    DEFINE_RO_FIELD( 5, int, diagErrorCode )
    DEFINE_RO_FIELD( 6, QString, hardwareModel )
    DEFINE_RO_FIELD( 7, QString, hardwareSerial )
    DEFINE_RO_FIELD( 8, int, hardwareCycles )
END_SESSION_MESSAGE

// 173,82
BEGIN_SESSION_MESSAGE( DiagInitWizardMsg, Diagnostics, DiagInitWizard )
    DEFINE_RO_FIELD( 2, QString, diagEvent )
    DEFINE_RO_FIELD( 3, bool, ready )
    DEFINE_RO_FIELD( 4, int, errorCode )
    DEFINE_RO_FIELD( 5, QString, errorMessage )
END_SESSION_MESSAGE

// 173,84
BEGIN_SESSION_MESSAGE( DiagPerformWizardTestingMsg, Diagnostics, DiagPerformWizardTesting )
    DEFINE_RO_FIELD( 2, QString, diagEvent )
    DEFINE_RO_FIELD( 3, bool, result )
    DEFINE_RO_FIELD( 4, int, errorCode )
END_SESSION_MESSAGE

// 173,96
BEGIN_SESSION_MESSAGE( DiagServiceMaintLogReqMsg, Diagnostics, DiagServiceMaintLogReq )
    DEFINE_RW_FIELD( 2, int, filter, setFilter )
    DEFINE_RW_FIELD( 3, int, age, setAge )

    /// Filter options.
    enum Filter
    {
        OpenRecordsOnly = 0,						///< Only open log records.
        ResolvedRecordsOnly,						///< Only resolved log records.
        AllRecords									///< Pull all records.
    };

END_SESSION_MESSAGE

// 173,97
BEGIN_SESSION_MESSAGE( DiagServiceMaintLogMsg, Diagnostics, DiagServiceMaintLog )
    DEFINE_RO_FIELD( 2, int, count )
    DEFINE_RO_FIELD( 3, QString, maintRecords )
END_SESSION_MESSAGE

// 173,98
BEGIN_SESSION_MESSAGE( DiagClearServiceMaintRecordMsg, Diagnostics, DiagClearServiceMaintRecord )
    DEFINE_RW_FIELD( 2, QString, maintRecord, setMaintRecord )
    DEFINE_RW_FIELD( 3, int, reason, setReason )
    DEFINE_RW_FIELD( 4, int, serviceMethod1, setServiceMethod1 )
    DEFINE_RW_FIELD( 5, int, servicePart1, setServicePart1 )
    DEFINE_RW_FIELD( 6, int, serviceMethod2, setServiceMethod2 )
    DEFINE_RW_FIELD( 7, int, servicePart2, setServicePart2 )
    DEFINE_RW_FIELD( 8, int, serviceMethod3, setServiceMethod3 )
    DEFINE_RW_FIELD( 9, int, servicePart3, setServicePart3 )
    DEFINE_RW_FIELD( 10, int, serviceMethod4, setServiceMethod4 )
    DEFINE_RW_FIELD( 11, int, servicePart4, setServicePart4 )
    DEFINE_RW_FIELD( 12, QString, serviceNotes, setServiceNotes )

    /// Reason types.
    enum Reason
    {
        Ignored = 0,								///< Ignored maintenance item.
        ResolvedByService,							///< Resolved maintenance item.
        ResolvedByDiag								///< Resolved by diagnostics over time.
    };

END_SESSION_MESSAGE

// 173,99
BEGIN_SESSION_MESSAGE( DiagPrintServiceMaintLogMsg, Diagnostics, DiagPrintServiceMaintLog )
    DEFINE_RW_FIELD( 2, int, filter, setFilter )
    DEFINE_RW_FIELD( 3, int, age, setAge )

    /// Filter options.
    enum Filter
    {
        OpenRecordsOnly = 0,						///< Only open log records.
        ResolvedRecordsOnly,						///< Only resolved log records.
        AllRecords									///< Pull all records.
    };

END_SESSION_MESSAGE

// 173,101
BEGIN_SESSION_MESSAGE( DiagServiceActionsMsg, Diagnostics, DiagServiceActions )
    DEFINE_RO_FIELD( 2, int, count )
    DEFINE_RO_FIELD( 3, QString, actions )
END_SESSION_MESSAGE

// 173,113
BEGIN_SESSION_MESSAGE( DiagExportDatabaseCompleteMsg, Diagnostics, DiagExportDatabaseComplete )
    DEFINE_RO_FIELD( 2, bool, success )
END_SESSION_MESSAGE

// 173,116
BEGIN_SESSION_MESSAGE( DiagSendDatabaseCompleteMsg, Diagnostics, DiagSendDatabaseComplete )
    DEFINE_RO_FIELD( 2, bool, success )
END_SESSION_MESSAGE

// (175,3)
class DomDepartments : public SessionMessageMap<DeptDev,DeptDevDtWriteAll>
{
    typedef DomDepartments _Myt;
    typedef SessionMessageMap<DeptDev,DeptDevDtWriteAll> _Mybase;
public:
    DomDepartments() : _Mybase() {}
    DomDepartments( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomDepartments( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomDepartments() {}

    DEFINE_FIELD( int, numDepts, setNumDepts, numDeptsRaw, 2 )
    QString& deptsList() {return _Mybase::operator [] (3);}
    DEFINE_FIELD( int, numNames1, setNumNames1, numNames1Raw, 4 )
    QString& names1() {return _Mybase::operator [] (5);}
    DEFINE_FIELD( int, numNames2, setNumNames2, numNames2Raw, 6 )
    QString& names2() {return _Mybase::operator [] (7);}
};

    // (175,2)
    class DomCurrentDept : public SessionMessageMap<DeptDev,DeptDevDtWrite>
    {
        typedef DomCurrentDept _Myt;
        typedef SessionMessageMap<DeptDev,DeptDevDtWrite> _Mybase;
    public:
        DomCurrentDept() : _Mybase() {}
        DomCurrentDept( const SessionMessage& rhs ) : _Mybase( rhs ) {}
        DomCurrentDept( const _Myt& rhs ) : _Mybase( rhs ) {}
        virtual ~DomCurrentDept() {}

        DEFINE_FIELD( int, deptNum, setDeptNum, deptNumRaw, 2 )
        QString& name1() {return _Mybase::operator [] (3);}
        QString& name2() {return _Mybase::operator [] (4);}
    };

//  (250,12)
class DomNetworkReport : public SessionMessageMap<JobsDevice,TestNetworkReport>
{
    typedef DomNetworkReport _Myt;
    typedef SessionMessageMap<JobsDevice,TestNetworkReport> _Mybase;
public:
    DomNetworkReport() : _Mybase() {}
    DomNetworkReport( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomNetworkReport( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomNetworkReport() {}

    QString& numOfDates() {return _Mybase::operator [] (2);}
    QString& dateList() {return _Mybase::operator [] (3);}
    QString& numOfTimes() {return _Mybase::operator [] (4);}
    QString& timeList() {return _Mybase::operator [] (5);}
    QString& numOfIds() {return _Mybase::operator [] (6);}
    QString& idList() {return _Mybase::operator [] (7);}
    QString& numOfRecords() {return _Mybase::operator [] (8);}
    QString& recordList() {return _Mybase::operator [] (9);}
    QString& numOfErrors() {return _Mybase::operator [] (10);}
    QString& errorList() {return _Mybase::operator [] (11);}
};

//  (250,21)
class DomOperatorNames : public SessionMessageMap<JobsDevice,TestOperatorNames>
{
    typedef DomOperatorNames _Myt;
    typedef SessionMessageMap<JobsDevice,EVENT> _Mybase;
public:
    DomOperatorNames() : _Mybase() {}
    DomOperatorNames( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomOperatorNames( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomOperatorNames() {}

    QString& numOfOpIds() {return _Mybase::operator [] (2);}
    QString& operatorIdList() {return _Mybase::operator [] (3);}
    QString& numOfNames() {return _Mybase::operator [] (4);}
    QString& operatorNameList() {return _Mybase::operator [] (5);}
};

// (250,41) digit entry results
class MadpOperatorDataEntry : public SessionMessageMap<JobsDevice,TestOperatorDataEntry>
{
    typedef MadpOperatorDataEntry _Myt;
    typedef SessionMessageMap<JobsDevice,TestOperatorDataEntry> _Mybase;
public:
    MadpOperatorDataEntry() : _Mybase() {}
    MadpOperatorDataEntry( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    MadpOperatorDataEntry( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~MadpOperatorDataEntry() {}

    QString& newValue() {return _Mybase::operator [] (2);}
    QString& isPrintLegal() {return _Mybase::operator [] (3);}
    QString& callUIFunctions() {return _Mybase::operator [] (4);}
};

//  (250,97)
class DomCreateCopiedPlu : public SessionMessageMap<JobsDevice,TestCreateCopiedPLU>
{
    typedef DomCreateCopiedPlu _Myt;
    typedef SessionMessageMap<JobsDevice,TestCreateCopiedPLU> _Mybase;
public:
    DomCreateCopiedPlu() : _Mybase() {}
    DomCreateCopiedPlu( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomCreateCopiedPlu( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomCreateCopiedPlu() {}

    QString& productNumber() {return _Mybase::operator [] (2);}
    QString& copiedPLU() {return _Mybase::operator [] (3);}
};

//(250,98)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomTestMessage : public SessionMessageMap<JobsDevice,TestTestMessage>
{
    typedef DomTestMessage _Myt;
    typedef SessionMessageMap<JobsDevice,TestTestMessage> _Mybase;
public:
    DomTestMessage() : _Mybase() {}
    DomTestMessage( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTestMessage( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTestMessage() {}

    QString& hobMessage() {return _Mybase::operator [] (2);}
};

//(250,99)														// existed in all 3 DOMs, so I deleted the Super and Service versions
class DomTestWeight : public SessionMessageMap<JobsDevice,TestTestWeight>
{
    typedef DomTestWeight _Myt;
    typedef SessionMessageMap<JobsDevice,TestTestWeight> _Mybase;
public:
    DomTestWeight() : _Mybase() {}
    DomTestWeight( const SessionMessage& rhs ) : _Mybase( rhs ) {}
    DomTestWeight( const _Myt& rhs ) : _Mybase( rhs ) {}
    virtual ~DomTestWeight() {}

    QString& hobWeight() {return _Mybase::operator [] (2);}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                     ERROR MESSAGES
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <int _Device, int _Event>
BEGIN_SESSION_MESSAGE( DeviceErrorMsg, _Device, _Event )
    DEFINE_RO_FIELD( 2, int, errorCode )
    DEFINE_RO_FIELD( 3, int, errorData )
    DEFINE_RO_FIELD( 4, int, errorStyle )
    DEFINE_RO_FIELD( 5, int, errorIndex )
    DEFINE_RO_FIELD( 6, int, errorType )
    DEFINE_RO_FIELD( 7, int, errorData2 )
    DEFINE_RO_FIELD( 8, int, numOfErrs )
    DEFINE_RO_FIELD( 9, QString, errorStrings )

    /// Error actions.
    enum Action
    {
        ActionNo = 0,                               ///< No / Cancel
        ActionYes                                   ///< Yes / Okay
    };

END_SESSION_MESSAGE

typedef DeviceErrorMsg<Error, ErrUnknownDevice> returnedErrorsUnknownDev;
typedef DeviceErrorMsg<Error, ErrWeigherDevice> returnedErrorsWeigherDev;
typedef DeviceErrorMsg<Error, ErrEventDevice> returnedErrorsEventDev;
typedef DeviceErrorMsg<Error, ErrOperationModeDevice> returnedErrorsOperationModeDev;
typedef DeviceErrorMsg<Error, ErrPrinterDevice> returnedErrorsPrinterDev;
typedef DeviceErrorMsg<Error, ErrWrapperDevice> returnedErrorsWrapperDev;
typedef DeviceErrorMsg<Error, ErrTouchScreenDevice> returnedErrorsTouchScreenDev;
typedef DeviceErrorMsg<Error, ErrRFIDReader> returnedErrorsRFIDReaderDev;
typedef DeviceErrorMsg<Error, ErrBarcodeScanner> returnedErrorBarcodeScannerDev;
typedef DeviceErrorMsg<Error, ErrSessionDevice> returnedErrorsSessionDev;
typedef DeviceErrorMsg<Error, ErrProductDevice> returnedErrorsProductDev;
typedef DeviceErrorMsg<Error, ErrOperatorDevice> returnedErrorsOperatorDev;
typedef DeviceErrorMsg<Error, ErrTotalTypeDevice> returnedErrorsTotalTypeDev;
typedef DeviceErrorMsg<Error, ErrGraphicDevice> returnedErrorsGraphicDev;
typedef DeviceErrorMsg<Error, ErrExpandedTextDevice> returnedErrorsExpandedTextDev;
typedef DeviceErrorMsg<Error, ErrBackupRestoreDevice> returnedErrorsBackupRestoreDev;
typedef DeviceErrorMsg<Error, ErrClassDevice> returnedErrorsClassDev;
typedef DeviceErrorMsg<Error, ErrConfigurationDevice> returnedErrorsConfigurationDev;
typedef DeviceErrorMsg<Error, ErrVersionManagerDevice> returnedErrorsVersionManagerDev;
typedef DeviceErrorMsg<Error, ErrCategoryDevice> returnedErrorsCategoryDev;
typedef DeviceErrorMsg<Error, ErrNetworkDevice> returnedErrorsNetworkDev;
typedef DeviceErrorMsg<Error, ErrTotalsDevice> returnedErrorsTotalsDev;
typedef DeviceErrorMsg<Error, ErrSpecialsDevice> returnedErrorsSpecialsDev;
typedef DeviceErrorMsg<Error, ErrOperatorChangesDevice> returnedErrorsOperatorChangesDev;
typedef DeviceErrorMsg<Error, ErrCOOLDevice> returnedErrorsCOOLDev;
typedef DeviceErrorMsg<Error, ErrSoftkeyDevice> returnedErrorsSoftkeyDev;
typedef DeviceErrorMsg<Error, ErrImportDevice> returnedErrorsImportDev;
typedef DeviceErrorMsg<Error, ErrNetworkConnectivityDevice> returnedErrorsNetworkConnectivityDev;
typedef DeviceErrorMsg<Error, ErrSynchronizationDevice> returnedErrorsSynchronizationDev;
typedef DeviceErrorMsg<Error, ErrTransactionDevice> returnedErrorsTransactionDev;
typedef DeviceErrorMsg<Error, ErrHourlyTotalDevice> returnedErrorsHourlyTotalDev;
typedef DeviceErrorMsg<Error, ErrOperatorTotalDevice> returnedErrorsOperatorTotalDev;
typedef DeviceErrorMsg<Error, ErrProductTotalDevice> returnedErrorsProductTotalDev;
typedef DeviceErrorMsg<Error, ErrWindowsConfigurationDevice> returnedErrorsWindowsConfigurationDev;
typedef DeviceErrorMsg<Error, ErrGCDSessionDevice> returnedErrorsErrGCDSessionDev;
typedef DeviceErrorMsg<Error, ErrDeviceDev> returnedErrDeviceDev;
typedef DeviceErrorMsg<Error, ErrDepartments> returnedErrDepartments;
typedef DeviceErrorMsg<Error, ErrEOFDevice> returnedErrorsErrEOFDev;
typedef DeviceErrorMsg<Error, ErrRemoveAnyModalDialogDevice> returnedErrorsRemoveAnyModalDialogDev;

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MESSAGES_H
