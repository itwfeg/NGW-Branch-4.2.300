/* @file serviceLabelApplierConfig.cpp
 * serviceLabelApplierConfig
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief there is alot of code inthis file that implements the renumber and delete product number. It was decided on 1/25/2013 to eliminate the renumber and delte product number from this page but Larry wanted to maybe reuse it for the all product fields page so it is left in this file. The renumber and delte buttons are hidden so no execution will take place
 */

#include "serviceLabelApplierConfig.h"

#include "ui_serviceLabelApplierConfig.h"

#include "logmanager.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart {
class numericLineEdit;
}

serviceLabelApplierConfig::serviceLabelApplierConfig(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui( new Ui::serviceLabelApplierConfig ),
    labelApplierConfigData( new DomLabelApplierConfig() ),
    init( false ),
    hasChanged( false ),
    prePrintStateChanged( false )
{
    ui->setupUi(this);
    scaleScreen();

    internalConfigData = &session_->scaleConfig->internalConfig;
    connect(ui->leOverdrive, SIGNAL(textChanged()), this, SLOT(doSave()));
    connect(ui->lePrimaryStationOffset, SIGNAL(textChanged()), this, SLOT(doSave()));
    connect(ui->leSecondaryStationOffset, SIGNAL(textChanged()), this, SLOT(doSave()));
    connect(ui->leOverdrive, SIGNAL(textChanged()), this, SLOT(doSave()));
    connect(ui->cboxPackageDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(doSave()));
    connect(ui->cboxPackageDischarge, SIGNAL(currentIndexChanged(int)), this, SLOT(doSave()));
    connect(ui->cboxPackagePlacement, SIGNAL(currentIndexChanged(int)), this, SLOT(doSave()));
    connect(ui->cboxSecondLabelApplierAttached, SIGNAL(currentIndexChanged(int)), this, SLOT(secondLabelApplierAttachedChanged(int)));
    connect(ui->cboxSecondLabelApplierAttached, SIGNAL(currentIndexChanged(int)), this, SLOT(doSave()));

    connect(ui->cbPrimaryLabelPreprint, SIGNAL(clicked()), this, SLOT(prePrintStateChange()));
    connect(ui->cbSecondLabelPreprint, SIGNAL(clicked()), this, SLOT(prePrintStateChange()));

    // Numeric Validators
    ui->leOverdrive->setValidator(overdriveValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leOverdrive->setDecimalPlaces(0);
    ui->leOverdrive->setInputWrapping(true);
    ui->leOverdrive->setInputTruncation(true);

    overdriveValidator_->setTop(99);
    overdriveValidator_->setBottom(0);

    ui->lePrimaryStationOffset->setValidator(primaryOffsetValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->lePrimaryStationOffset->setDecimalPlaces(0);
    ui->lePrimaryStationOffset->setInputWrapping(true);
    ui->lePrimaryStationOffset->setInputTruncation(true);
    primaryOffsetValidator_->setTop(30);
    primaryOffsetValidator_->setBottom(0);

    ui->leSecondaryStationOffset->setValidator(secondaryOffsetValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leSecondaryStationOffset->setDecimalPlaces(0);
    ui->leSecondaryStationOffset->setInputWrapping(true);
    ui->leSecondaryStationOffset->setInputTruncation(true);
    secondaryOffsetValidator_->setTop(30);
    secondaryOffsetValidator_->setBottom(0);

    scaleConfigData = &session_->scaleConfig->scaleConfig;

    update();
    // Set the applier is configuration mode
    session_->send(LabelApplier, LaMode, "5");  // Set label applier mode
    session_->send(LabelApplier, LaReqInquire); // request config from label applier
    session_->send( ConfigurationDev, CfgPrepackRead ); // update ui (sends back a CfgPrepackWrite)

    this->initialize();
    translate();
}

serviceLabelApplierConfig::~serviceLabelApplierConfig()
{
    delete ui;

    if ( labelApplierConfigData )
    {
        delete labelApplierConfigData;
        labelApplierConfigData = NULL;
    }
}

void serviceLabelApplierConfig::prePrintStateChange()
{
    prePrintStateChanged = true;
}

void serviceLabelApplierConfig::secondLabelApplierAttachedChanged(int newIndex)
{
    // nothing to do until we get config
    if ( !init )
        return;

    if(newIndex == 0)
    {
        ui->leSecondaryStationOffset->hide();
        ui->lblSecondaryStation->hide();
    }
    else if(newIndex == 1)
    {
        ui->leSecondaryStationOffset->show();
        ui->lblSecondaryStation->show();
        ui->leSecondaryStationOffset->setEnabled(true);
    }
}

int serviceLabelApplierConfig::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

void serviceLabelApplierConfig::initialize()
{
    hasChanged = false;

    ui->cboxSecondLabelApplierAttached->clear();
    ui->cboxSecondLabelApplierAttached->addItem(tr("No"), "0");
    ui->cboxSecondLabelApplierAttached->addItem(tr("Yes"), "1");
    ui->cboxSecondLabelApplierAttached->setDisabled(true);

    ui->cboxPackageDirection->clear();
    ui->cboxPackageDirection->addItem(tr("Right to left"), "0");
    ui->cboxPackageDirection->addItem(tr("Left to right"), "1");
    ui->cboxPackageDirection->setDisabled(true);

    ui->cboxPackageDischarge->clear();
    ui->cboxPackageDischarge->addItem(tr("Side"), "0");
    ui->cboxPackageDischarge->addItem(tr("Front"), "1");
    ui->cboxPackageDischarge->setDisabled(true);

    ui->cboxPackagePlacement->clear();
    ui->cboxPackagePlacement->addItem(tr("Leading edge"), "0");
    ui->cboxPackagePlacement->addItem(tr("Trailing edge"), "1");
    ui->cboxPackagePlacement->setDisabled(true);

    ui->leOverdrive->setText("");
    ui->leOverdrive->setDisabled(true);

    ui->lePrimaryStationOffset->setText("");
    ui->lePrimaryStationOffset->setDisabled(true);

    ui->leSecondaryStationOffset->setText("");
    ui->leSecondaryStationOffset->setDisabled(true);
}

void serviceLabelApplierConfig::doSave()
{
    // nothing to do until we get config
    if ( !init )
        return;

    hasChanged = true;
    // Not really since if you change it back to what it was then it isn't different, but everyone seems to be doing anyway.
    // set the saving state for the destructor
}

void serviceLabelApplierConfig::saveConfig()
{
    LOG_CLASS_INFO("serviceLabelApplierConfig", "saveConfig()");

    if(hasChanged)
    {
        QString msg = "";
        msg += labelApplierConfigData->weigherStationInstalledRaw() + ",";
        msg += labelApplierConfigData->primaryStationInstalledRaw() + ",";
        msg += QString::number(ui->cboxSecondLabelApplierAttached->currentIndex()) + ",";
        msg += QString::number(ui->cboxPackageDirection->currentIndex()) + ",";
        msg += QString::number(ui->cboxPackageDischarge->currentIndex()) + ",";

        if(ui->lePrimaryStationOffset->text().toInt() > 30)
            msg += "8,";
        else
            msg += ui->lePrimaryStationOffset->text() + ",";

        if(ui->leSecondaryStationOffset->text().toInt() > 30)
            msg += "8,";
        else
            msg += ui->leSecondaryStationOffset->text() + ",";

        msg += QString::number(ui->cboxPackagePlacement->currentIndex()) + ",";

        if(ui->leOverdrive->text().toInt() > 100)
            msg += "10";
        else
            msg += ui->leOverdrive->text();

        session_->send(LabelApplier, LaConfig, msg ); // (5,1)
        session_->send(LabelApplier, LaReqInquire); // request config from label applier to update UI

        hasChanged = false;
    }

    if(prePrintStateChanged)
    {
        MadpOperatorDisplayFields *operatorDisplayFields = &session_->scaleConfig->operatorDisplayFields;

        operatorDisplayFields->setPriLabelStockRotated( ui->cbPrimaryLabelPreprint->isChecked() );
        operatorDisplayFields->setSecLabelStockRotated( ui->cbSecondLabelPreprint->isChecked() );

        LOG_INFO( "Sending Primary Pre-Print: %d", ui->cbPrimaryLabelPreprint->isChecked() );
        LOG_INFO( "Sending Secondary Pre-Print: %d", ui->cbSecondLabelPreprint->isChecked() );

        session_->send( *operatorDisplayFields );
        session_->send( ConfigurationDev, CfgPrepackSave );

        session_->send( ConfigurationDev, CfgPrepackRead ); // update ui

        prePrintStateChanged = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierConfig::translate()
{
    ui->retranslateUi(this);
    ui->cboxSecondLabelApplierAttached->setItemText(0, tr("No"));
    ui->cboxSecondLabelApplierAttached->setItemText(1, tr("Yes"));

    ui->cboxPackageDirection->setItemText(0, tr("Right to left"));
    ui->cboxPackageDirection->setItemText(1, tr("Left to right"));

    ui->cboxPackageDischarge->setItemText(0, tr("Side"));
    ui->cboxPackageDischarge->setItemText(1, tr("Front"));

    ui->cboxPackagePlacement->setItemText(0, tr("Leading edge"));
    ui->cboxPackagePlacement->setItemText(1, tr("Trailing edge"));
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierConfig::onScreenChanging()
{
    this->saveConfig();
    // also set label applier to conveyor mode since we are leaving the screen
    session_->send(LabelApplier, LaMode, "3");
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierConfig::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO("serviceLabelApplierConfig", "onSessionMessage()");
    // Note:
    //
    // In the HLX ASP implementation both the config and inquire messages are handled by the same
    // code... so I am not sure what to expect here.  I suppose lets handle both messages until
    // further testing says otherwise.

    if(ConfigurationDev == msg[0].toInt() && CfgPrepackWrite == msg[1].toInt())
    {
        MadpOperatorDisplayFields m(msg);

        LOG_INFO( "Received Primary Pre-Print: %d", m.priLabelStockRotated() );
        LOG_INFO( "Received Secondary Pre-Print: %d", m.secLabelStockRotated() );

        ui->cbPrimaryLabelPreprint->setChecked( m.priLabelStockRotated() == 0 ? false : true );
        ui->cbSecondLabelPreprint->setChecked( m.secLabelStockRotated() == 0 ? false : true );
    }
    else if(LabelApplier == msg[0].toInt() && (LaConfig == msg[1].toInt() || LaInquire == msg[1].toInt()))
    {
        // we just asked for and received the applier's config so update the GUI controls

        //Safe parsing of comma delimited message in case it ever changes
        DomLabelApplierConfig m(msg);

        // save off label config
        *labelApplierConfigData = m;

        //Secondary Label Applier Attached
        ui->cboxSecondLabelApplierAttached->setCurrentIndex( m.secondaryStationInstalled() ? 1 : 0 );
        ui->cboxSecondLabelApplierAttached->setEnabled(true);

        //Package Direction
        ui->cboxPackageDirection->setCurrentIndex(m.packageDirection());
        ui->cboxPackageDirection->setEnabled(true);

        //Package Discharge
        ui->cboxPackageDischarge->setCurrentIndex(m.dischargePosition().toInt());
        ui->cboxPackageDischarge->setEnabled(true);

        //Primary Label Placement
        ui->cboxPackagePlacement->setCurrentIndex(m.primaryLabelPlacement());
        ui->cboxPackagePlacement->setEnabled(true);

        // Overdrive
        ui->leOverdrive->fromSessionField(m.overdrive());
        ui->leOverdrive->setEnabled(true);

        ///////////////////////////////////////////
        // Offsets

        // Dynamic visibility of primary and secondary label appliers
        if( !m.secondaryStationInstalled() )
        {
            ui->lblSecondaryStation->hide();
            ui->leSecondaryStationOffset->hide();
        }
        else
        {
            ui->lblSecondaryStation->show();
            ui->leSecondaryStationOffset->show();
        }

        //This may be hidden fields
        ui->lePrimaryStationOffset->fromSessionField(m.primaryPlacementOffset());
        ui->lePrimaryStationOffset->setEnabled(true);

        ui->leSecondaryStationOffset->fromSessionField(m.secondPlacementOffset());
        ui->leSecondaryStationOffset->setEnabled(true);

        // we are now initialized!!
        init = true;
    }
}
