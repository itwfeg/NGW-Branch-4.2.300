/**
 * @file superSecurityLabels.h
 * @brief ...describe...
 *
 * @copyright Copyright (c) 2015, ITW/Hobart, Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 * @issues:
 * @{
*/

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"
#include "../modes/supervisormode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "TokenParser.h"

#include "superSecurityLabels.h"
#include "ui_superSecurityLabels.h"

static const int MAX_TOTAL_TYPES = 8;
static const int NOT_CHECKED = 0;
static const int CHECKED = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
superSecurityLabels::superSecurityLabels(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superSecurityLabels),
    scaleOpData_(NULL)
{
    LOG_CLASS_INFO( "superSecurityLabels", "superSecurityLabels" );

	currencyData = &session_->scaleConfig->currencySettings;
    scaleOpData_ = &dynamic_cast<SupervisorMode*>( mode_ )->normalScaleOperation_;


    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        scaleOpData_ = &dynamic_cast<SupervisorMode*>( mode_ )->normalScaleOperation_;

    initialize();
    translate();

    session_->send(ConfigurationDev, CfgScaleOperationRead);	// 112,70 to get security label pricing
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
superSecurityLabels::~superSecurityLabels()
{
    LOG_CLASS_INFO( "superSecurityLabels", "~superSecurityLabels" );
    delete ui;
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superSecurityLabels::initialize()
{
    LOG_CLASS_INFO( "superSecurityLabels", "initialize" );

    ui->le_minUnitPrice->setValidator( unitPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    if (currencyData)
	{
		ui->le_minUnitPrice->setDecimal(currencyData->unitSeparator());
    	ui->le_minUnitPrice->setDecimalPlaces(currencyData->primaryPrecision());
	}
	else
	{
	    LOG_TRACE( "No currencyData!", NULL );
    }
	ui->le_minUnitPrice->setInputWrapping(true);
    ui->le_minUnitPrice->setInputTruncation(true);
    if (currencyData)
	    unitPriceValidator_->setTop(makeValidatorTop(currencyData->unitPriceDigits()));

    ui->le_minTotalPrice->setValidator( totalPriceValidator_ = new QIntValidator(0, INT_MAX, this));
    if (currencyData)
	{
	    ui->le_minTotalPrice->setDecimal(currencyData->unitSeparator());
	    ui->le_minTotalPrice->setDecimalPlaces(currencyData->primaryPrecision());
	}
    ui->le_minTotalPrice->setInputWrapping(true);
    ui->le_minTotalPrice->setInputTruncation(true);
    if (currencyData)
	    totalPriceValidator_->setTop(makeValidatorTop(currencyData->totalPriceDigits()));

    // create all of the signals and slots
    // text fields
    connect(ui->le_minUnitPrice,    SIGNAL(editingFinished()),   this, SLOT(minUnitPriceEditingFinished()));
    connect(ui->le_minTotalPrice,   SIGNAL(editingFinished()),   this, SLOT(minTotalPriceEditingFinished()));

    update();
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superSecurityLabels::translate()
{
    LOG_CLASS_INFO( "superSecurityLabels", "translate" );
	ui->retranslateUi( this );

    update();
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superSecurityLabels::helpText( QString& title, QString& description ) const
{
    LOG_CLASS_INFO( "superSecurityLabels", "helpText" );
    Q_UNUSED( title );
    Q_UNUSED( description );

    LOG_TRACE( "Done", NULL );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superSecurityLabels::onScreenChanging()
{
    LOG_CLASS_INFO( "superSecurityLabels", "onScreenChanging" );

    // save every time b\c we dont know what will happen when
    saveSecurityLabelPricing();

    LOG_TRACE( "Done", NULL );
    // base method
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
//                                    PRIVATE METHODS                                               //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
void superSecurityLabels::displaySecurityLabelPricing(const SessionMessage& msg)
{
    LOG_CLASS_INFO( "superSecurityLabels", "displaySecurityLabelPricing" );

    DomNormalScaleOperation m( msg );

    // save the TotalTypeData class to reuse it later
    if ( scaleOpData_ )
        *scaleOpData_ = m;

    ui->le_minUnitPrice->setText( m.unitPriceSecurityLabel() );
    ui->le_minTotalPrice->setText( m.totalPriceSecurityLabel() );
    LOG_TRACE( "Done", NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
//                                   SLOT METHODS                                                   //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void superSecurityLabels::minUnitPriceEditingFinished()
{
    LOG_CLASS_INFO( "superSecurityLabels", "minUnitPriceEditingFinished" );

    LOG_TRACE( "minUnitPrice = %d.", qPrintable(ui->le_minUnitPrice->text()) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superSecurityLabels::minTotalPriceEditingFinished()
{
    LOG_CLASS_INFO( "superSecurityLabels", "minTotalPriceEditingFinished" );

    LOG_TRACE( "minTotalPrice = %d.", qPrintable(ui->le_minTotalPrice->text()) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superSecurityLabels::saveSecurityLabelPricing()
{
    LOG_CLASS_INFO( "superSecurityLabels", "saveSecurityLabelPricing" );

    LOG_TRACE( "saving...", NULL );

    QString msg = scaleOpData_->collectTransactionsRaw() + "," +
    scaleOpData_->daysToKeepTransactions() + "," +
    scaleOpData_->coolTrackingNumRaw() + "," +
    scaleOpData_->useAutoLookupRaw() + "," +
    scaleOpData_->numOfDigits() + "," +
    scaleOpData_->soundBeepOnStableWtRaw() + "," +
    scaleOpData_->autoDisplayProductNotesRaw() + "," +
    scaleOpData_->accessBrowserModeRaw() + "," +
    scaleOpData_->autoLoadOldFirmwareRaw() + "," +
    scaleOpData_->uniqueNumbers() + "," +
    scaleOpData_->popupKeypad() + "," +
    scaleOpData_->popupKeypadStyleRaw() + "," +
    scaleOpData_->opLoginKeyboardStyleRaw() + "," +
    scaleOpData_->enablePowerSaver() + "," +
    scaleOpData_->powerSaveIdleTime() + "," +
    scaleOpData_->logoutRemoteIdleTime() + "," +
    "\"" + scaleOpData_->defaultHomePage() + "\"," +
    scaleOpData_->defaultProductRecord() + "," +
    scaleOpData_->fixedWeightUnits() + ",";
    msg += "\"" + scaleOpData_->maxTare() + "\"" + ",";
    msg += "\"" + scaleOpData_->tareSetback() + "\"" + ",";
    msg += scaleOpData_->beeperVolume() + "," +
    scaleOpData_->byCountCalculationMethodRaw() + "," +
    scaleOpData_->externalWeightPrecision() + "," +
    scaleOpData_->tareEntryPrecision() + "," +
    scaleOpData_->clearPluAfterPrintingRaw() + "," +
    scaleOpData_->operatorRezero() + ",";
    msg += scaleOpData_->filmWidth() + "," +
    scaleOpData_->filmTension() + "," +
    scaleOpData_->tensionFrontToBack() + "," +
    scaleOpData_->tensionLeftToRight() + "," +
    scaleOpData_->intakeMode() + "," +
    scaleOpData_->labelPlacement() + "," +
    scaleOpData_->applierRotation() + ",";

    msg += "\"" + scaleOpData_->timeOnSun() + "\"," +
    "\"" + scaleOpData_->timeOffSun() + "\"," +
    "\"" + scaleOpData_->timeOnMon() + "\"," +
    "\"" + scaleOpData_->timeOffMon() + "\"," +
    "\"" + scaleOpData_->timeOnTue() + "\"," +
    "\"" + scaleOpData_->timeOffTue() + "\"," +
    "\"" + scaleOpData_->timeOnWed() + "\"," +
    "\"" + scaleOpData_->timeOffWed() + "\"," +
    "\"" + scaleOpData_->timeOnThu() + "\"," +
    "\"" + scaleOpData_->timeOffThu() + "\"," +
    "\"" + scaleOpData_->timeOnFri() + "\"," +
    "\"" + scaleOpData_->timeOffFri() + "\"," +
    "\"" + scaleOpData_->timeOnSat() + "\"," +
    "\"" + scaleOpData_->timeOffSat() + "\",";

    msg += scaleOpData_->compressFlagRaw() + "," +
    scaleOpData_->compressTime() + "," +
    scaleOpData_->autoLookupOverrideDigit() + "," +
    scaleOpData_->checkNetConnectivity() + "," +
    "0," +                              // access desktop is always 0 - never allow it anymore
    scaleOpData_->pingPrimaryRaw() + "," +
    scaleOpData_->pingGatewayRaw() + "," +
    scaleOpData_->gcdLayoutType() + "," +
    scaleOpData_->gcdColorScheme() + "," +
    scaleOpData_->accessGcdResetRaw() + "," +
    scaleOpData_->showDeleteAllFilesRaw() + "," +
    scaleOpData_->showPrinterSettingsRaw() + "," +
    scaleOpData_->remoteAccessPermittedRaw() + "," +
    scaleOpData_->remoteAccessLoginMethodRaw() + ",";

    msg += ui->le_minUnitPrice->text() + ",";
    msg += ui->le_minTotalPrice->text() + ",";

    msg += scaleOpData_->backdateByDays() + ",";
    msg += scaleOpData_->pingTarget() + "," +
    scaleOpData_->pingTargetIPAddress() + "," +
    scaleOpData_->pingInterval() + "," +
    scaleOpData_->autoDisplaySuggestiveSellingRaw() + "," +
    scaleOpData_->autoDisplayCookingNotesRaw() + "," +
    scaleOpData_->gcdIdlePlaybackTime() + "," +
    "\"" + scaleOpData_->remoteAccessUserName() + "\"," +
    "\"" + scaleOpData_->remoteAccessPassword() + "\"," +
    "\"" + scaleOpData_->systemFont() + "\"," +
    scaleOpData_->startScreenHome() + "," +
    scaleOpData_->allowAddressBarRaw() + "," +
    scaleOpData_->resetStatisticsDays() + "," +
    scaleOpData_->defaultSearchMethod() + "," +
    "\"" + scaleOpData_->runScreenHomePage() + "\"," +
    scaleOpData_->dcaStartScreenTypeRaw() + "," +
    scaleOpData_->dcaRunScreenTypeRaw() + "," +
    scaleOpData_->dcaKeypadAreaTypeRaw() + "," +
    scaleOpData_->trackingOperatorChangesRaw() + "," +
    scaleOpData_->enableDisplayNetworkDetailsRaw() + "," +
    scaleOpData_->disableOperatorFlashkeyEditorRaw() + "," +
    scaleOpData_->disableStartScreenRaw() + "," +
    scaleOpData_->rightScreenTypeRaw();

    session_->send(ConfigurationDev,CfgScaleOperationSave,msg);	//	112,72,msg
    session_->send(ConfigurationDev,CfgScaleRead);              //	112,20 - to make sure we update

    LOG_TRACE( "Done", NULL );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void superSecurityLabels::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "superSecurityLabels", "onSessionMessage" );

    if(( ConfigurationDev == msg[0].toInt() ) && ( CfgScaleOperationWrite == msg[1].toInt() ))		//	112,71
    {
        LOG_TRACE( "Have CfgScaleOperationWrite message", NULL );

        displaySecurityLabelPricing(msg);
    }
    LOG_TRACE( "Done", NULL );
}

int superSecurityLabels::makeValidatorTop( int digits )
{
    LOG_CLASS_INFO( "superSecurityLabels", "makeValidatorTop" );
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    LOG_TRACE( "Done", NULL );
    return top;
}
