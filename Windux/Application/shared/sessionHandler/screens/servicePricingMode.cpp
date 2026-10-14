/**
 * @file servicePricingMode.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "servicePricingMode.h"
#include "ui_servicePricingMode.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

// Hobart
#include "../../source/HFC/CfgBaseFieldOffsets.h"

servicePricingMode::servicePricingMode(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::servicePricingMode)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();

    //----------------------------------------------------
    // Set the connections for when a checkbox is changed.
    //----------------------------------------------------
    // FT-951 Pounds for pricing needs to be available in metric mode so remove metric test
    //if(!bIsMetricScale)
    //{
        connect(ui->ckPoundsForPricing, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
        connect(ui->ckPoundsForPricing, SIGNAL( clicked() ), this, SLOT( PoundsForPricingChanged() ) );
    //}

    connect(ui->ckFractionalPricing, SIGNAL( clicked() ), this, SLOT( SetupChanged() ) );
    connect(ui->ckFractionalPricing, SIGNAL( clicked() ), this, SLOT( FractionalPricingChanged() ) );

    bSetupChanged = false;

    translate();
}

servicePricingMode::~servicePricingMode()
{
    delete ui;
}

void servicePricingMode::translate()
{
    ui->retranslateUi(this);

    update();
}

bool servicePricingMode::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

//---------------------------------------------------------------
// Set the checkboxes based on the value of
// fractionalPricingOn and poundsForPricingOn in MadpScaleConfig.
// Also dependent on if the scale is set up for
// pounds or kilograms (metric).
//---------------------------------------------------------------
void servicePricingMode::initialize()
{
    //--------------------------------------------------------------
    // NOTE:    These are set from a global Scale Config variable
    //          and NOT as a result of a message requesting data and
    //          then processing of a return message.
    //--------------------------------------------------------------

    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString poundsForPricingSetting = settings->poundsForPricingOnRaw();
    QString fractionalPricingSetting = settings->fractionalPricingOnRaw();

    //    QString scaleModelSetting = settings->scaleModel();
    //    QString countryCodeSetting = settings->countryCode();


    bIsMetricScale = settings->isScaleMetric();

    if( bIsMetricScale )
        ui->ckPoundsForPricing->setText(tr("Allow Kgs for pricing"));
    else
        ui->ckPoundsForPricing->setText(tr("Allow Pounds for pricing"));

    update();

    ui->ckPoundsForPricing->setChecked(false);
    ui->ckPoundsForPricing->setDisabled(false);

    ui->ckFractionalPricing->setChecked(false);
    ui->ckFractionalPricing->setDisabled(false);

    //----------------------------------------------------
    // Set checkbox values, enable/disable checkboxes
    //----------------------------------------------------

    // FT-951 Pounds for pricing needs to be available in metric mode
    // because of FT-951, the entire if metric condition can be removed

    if(poundsForPricingSetting == "1")
    {
        ui->ckPoundsForPricing->setChecked(true);
        ui->ckPoundsForPricing->setStyleSheet("color:black;");

        ui->ckFractionalPricing->setDisabled(true);
        ui->ckFractionalPricing->setStyleSheet("color:gray;");
    }

    if(fractionalPricingSetting == "1")
    {
        ui->ckFractionalPricing->setChecked(true);
        ui->ckFractionalPricing->setStyleSheet("color:black;");

        ui->ckPoundsForPricing->setDisabled(true);
        ui->ckPoundsForPricing->setStyleSheet("color:gray;");
    }

    update();
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void servicePricingMode::SetupChanged()
{
    bSetupChanged = true;
}

//------------------------------------------------------
// Enable/disable (and clear) "Allow Fractional Pricing"
//------------------------------------------------------
void servicePricingMode::PoundsForPricingChanged()
{
    if(ui->ckPoundsForPricing->isChecked())
    {
        ui->ckFractionalPricing->setChecked(false);
        ui->ckFractionalPricing->setDisabled(true);
        ui->ckFractionalPricing->setStyleSheet("color:gray;");
    }
    else
    {
        ui->ckFractionalPricing->setDisabled(false);
        ui->ckFractionalPricing->setStyleSheet("color:black;");
    }
}

//------------------------------------------------------
// Enable/disable (and clear) "Allow Pounds for Pricing"
//------------------------------------------------------
void servicePricingMode::FractionalPricingChanged()
{
    // FT-951 Pounds for pricing needs to be available in metric mode so remove metric condition

    //if(!bIsMetricScale)
    //{
        if(ui->ckFractionalPricing->isChecked())
        {
            ui->ckPoundsForPricing->setChecked(false);
            ui->ckPoundsForPricing->setDisabled(true);
            ui->ckPoundsForPricing->setStyleSheet("color:gray;");
        }
        else
        {
            ui->ckPoundsForPricing->setDisabled(false);
            ui->ckPoundsForPricing->setStyleSheet("color:black;");
        }
    //}
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void servicePricingMode::SaveDataOnExit()
{
    if (bSetupChanged)
    {
        QString param;

        // FT-951 Pounds for pricing needs to be available in metric mode so remove metric test
        //if(!bIsMetricScale)
        //{
            //----------------------------------------
            // Save "Allow Pounds for Pricing" setting
            //----------------------------------------
            param = "," + QString::number( fieldOffsetCfgPoundsForPricingEnabled );
            param += "," + QString(ui->ckPoundsForPricing->isChecked() ? "1" : "0");

            session_->send( ConfigurationDev, CfgSaveSingleField, param );
        //}

        //----------------------------------------
        // Save "Allow Fractional Pricing" setting
        //----------------------------------------
        param = "," + QString::number( fieldOffsetCfgFractionalPricingEnabled );
        param += "," + QString(ui->ckFractionalPricing->isChecked() ? "1" : "0");

        session_->send( ConfigurationDev, CfgSaveSingleField, param );
    }
}
