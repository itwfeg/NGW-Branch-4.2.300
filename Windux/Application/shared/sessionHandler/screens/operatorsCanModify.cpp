/**
 * @file operatorsCanModify.cpp
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "operatorsCanModify.h"
#include "ui_operatorsCanModify.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

operatorsCanModify::operatorsCanModify(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::operatorsCanModify)
{
    ui->setupUi(this);
    scaleScreen();

    bSetupChanged = false;

    bDateTime = "";                                                 // needs to be initialized to nothing or causes crash
    bDateTimeAccessLevel = "";                                      // needs to be initialized to nothing or causes crash

    //----------------------------------------------------------------
    // Set the connections for when a checkbox is checked/un-checked.
    // These all call the same method to set a property that indicates
    // something in the setup changed and the data needs to be saved.
    //----------------------------------------------------------------
    connect(ui->ckPrice,        SIGNAL(stateChanged(int)), this, SLOT(SetupChangedPrice(int)));
    connect(ui->ckByCount,      SIGNAL(stateChanged(int)), this, SLOT(SetupChangedByCount(int)));
    connect(ui->ckTare,         SIGNAL(stateChanged(int)), this, SLOT(SetupChangedTare(int)));
    connect(ui->ckFixedWeight,  SIGNAL(stateChanged(int)), this, SLOT(SetupChangedFixedWeight(int)));
    connect(ui->ckShelfLife,    SIGNAL(stateChanged(int)), this, SLOT(SetupChangedShelfLife(int)));
    connect(ui->ckProductLife,  SIGNAL(stateChanged(int)), this, SLOT(SetupChangedProductLife(int)));
    connect(ui->ckLabelType,    SIGNAL(stateChanged(int)), this, SLOT(SetupChangedLabelType(int)));
    connect(ui->ckPortions,     SIGNAL(stateChanged(int)), this, SLOT(SetupChangedPortions(int)));

    connect(ui->cboPriceAccessLevel,        SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));
    connect(ui->cboByCountAccessLevel,      SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));
    connect(ui->cboTareAccessLevel,         SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));
    connect(ui->cboFixedWeightAccessLevel,  SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));
    connect(ui->cboShelfLifeAccessLevel,    SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));
    connect(ui->cboProductLifeAccessLevel,  SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));
    connect(ui->cboLabelTypeAccessLevel,    SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));
    connect(ui->cboPortionsAccessLevel,     SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));

    // Set up the combo boxes
    QStringList accessLevelsList;
    accessLevelsList << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9";

    ui->cboPriceAccessLevel->addItems( accessLevelsList );
    ui->cboByCountAccessLevel->addItems( accessLevelsList );
    ui->cboTareAccessLevel->addItems( accessLevelsList );
    ui->cboFixedWeightAccessLevel->addItems( accessLevelsList );
    ui->cboShelfLifeAccessLevel->addItems( accessLevelsList );
    ui->cboProductLifeAccessLevel->addItems( accessLevelsList );
    ui->cboLabelTypeAccessLevel->addItems( accessLevelsList );
    ui->cboPortionsAccessLevel->addItems( accessLevelsList );

    //--------------------------------------
    // Request the Operator Permissions data
    //--------------------------------------
    session_->send( ConfigurationDev, CfgOperatorAccessRead );

    translate();
}

operatorsCanModify::~operatorsCanModify()
{
    delete ui;
}

void operatorsCanModify::translate()
{
    ui->retranslateUi(this);

    update();
}
bool operatorsCanModify::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

void operatorsCanModify::onSessionMessage( const SessionMessage& msg )
{
    // Operator Permissions data
    if ((ConfigurationDev == msg[0].toInt() ) && ( CfgOperatorAccessWrite == msg[1].toInt() ))
    {
        MadpOperatorCanModify m( msg );

        // Update UI with the Operator Permissions settings
        if( m.price() )
        {
            ui->ckPrice->setChecked(true);
            ui->cboPriceAccessLevel->setEnabled( true );
            ui->cboPriceAccessLevel->setCurrentIndex( m.priceAccessLevel() - 1 );
        }

        if( m.byCount() )
        {
            ui->ckByCount->setChecked( true );
            ui->cboByCountAccessLevel->setEnabled( true );
            ui->cboByCountAccessLevel->setCurrentIndex( m.byCountAccessLevel() - 1 );
        }

        if( m.tare() )
        {
            ui->ckTare->setChecked( true );
            ui->cboTareAccessLevel->setEnabled( true );
            ui->cboTareAccessLevel->setCurrentIndex( m.tareAccessLevel() - 1 );
        }

        if( m.fixedWeight() )
        {
            ui->ckFixedWeight->setChecked( true );
            ui->cboFixedWeightAccessLevel->setEnabled( true );
            ui->cboFixedWeightAccessLevel->setCurrentIndex( m.fixedWeightAccessLevel() - 1 );
        }

        if( m.shelfLife() )
        {
            ui->ckShelfLife->setChecked( true );
            ui->cboShelfLifeAccessLevel->setEnabled( true );
            ui->cboShelfLifeAccessLevel->setCurrentIndex( m.shelfLifeAccessLevel() - 1 );
        }

        if( m.productLife() )
        {
            ui->ckProductLife->setChecked( true );
            ui->cboProductLifeAccessLevel->setEnabled( true );
            ui->cboProductLifeAccessLevel->setCurrentIndex( m.productLifeAccessLevel() - 1 );
        }

        if( m.labelType() )
        {
            ui->ckLabelType->setChecked( true );
            ui->cboLabelTypeAccessLevel->setEnabled( true );
            ui->cboLabelTypeAccessLevel->setCurrentIndex( m.labelTypeAccessLevel() - 1 );
        }

        if( m.portions() )
        {
            ui->ckPortions->setChecked( true );
            ui->cboPortionsAccessLevel->setEnabled( true );
            ui->cboPortionsAccessLevel->setCurrentIndex( m.portions() );
        }

        // Not used, but need to save the value for when the settings are saved
        bDateTime = m.dateTimeRaw();
        bDateTimeAccessLevel = m.dateTimeAccessLevelRaw();

        // Set the Access Levels


        bSetupChanged = false;
    }
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void operatorsCanModify::SetupChangedPrice( int state )
{
    if( state )
    {
        ui->cboPriceAccessLevel->setEnabled( true );
    }
    else
    {
        ui->cboPriceAccessLevel->setEnabled( false );
        ui->cboPriceAccessLevel->setCurrentIndex( 0 );
    }

    bSetupChanged = true;
}

void operatorsCanModify::SetupChangedByCount(int state)
{
    if( state )
    {
        ui->cboByCountAccessLevel->setEnabled( true );
    }
    else
    {
        ui->cboByCountAccessLevel->setEnabled( false );
        ui->cboByCountAccessLevel->setCurrentIndex( 0 );
    }

    bSetupChanged = true;
}

void operatorsCanModify::SetupChangedTare(int state)
{
    if( state )
    {
        ui->cboTareAccessLevel->setEnabled( true );
    }
    else
    {
        ui->cboTareAccessLevel->setEnabled( false );
        ui->cboTareAccessLevel->setCurrentIndex( 0 );
    }

    bSetupChanged = true;
}

void operatorsCanModify::SetupChangedFixedWeight(int state)
{
    if( state )
    {
        ui->cboFixedWeightAccessLevel->setEnabled( true );
    }
    else
    {
        ui->cboFixedWeightAccessLevel->setEnabled( false );
        ui->cboFixedWeightAccessLevel->setCurrentIndex( 0 );
    }

    bSetupChanged = true;
}

void operatorsCanModify::SetupChangedShelfLife(int state)
{
    if( state )
    {
        ui->cboShelfLifeAccessLevel->setEnabled( true );
    }
    else
    {
        ui->cboShelfLifeAccessLevel->setEnabled( false );
        ui->cboShelfLifeAccessLevel->setCurrentIndex( 0 );
    }

    bSetupChanged = true;
}

void operatorsCanModify::SetupChangedProductLife(int state)
{
    if( state )
    {
        ui->cboProductLifeAccessLevel->setEnabled( true );
    }
    else
    {
        ui->cboProductLifeAccessLevel->setEnabled( false );
        ui->cboProductLifeAccessLevel->setCurrentIndex( 0 );
    }

    bSetupChanged = true;
}

void operatorsCanModify::SetupChangedLabelType(int state)
{
    if( state )
    {
        ui->cboLabelTypeAccessLevel->setEnabled( true );
    }
    else
    {
        ui->cboLabelTypeAccessLevel->setEnabled( false );
        ui->cboLabelTypeAccessLevel->setCurrentIndex( 0 );
    }

    bSetupChanged = true;
}

void operatorsCanModify::SetupChangedPortions(int state)
{
    if( state )
    {
        ui->cboPortionsAccessLevel->setEnabled( true );
    }
    else
    {
        ui->cboPortionsAccessLevel->setEnabled( false );
        ui->cboPortionsAccessLevel->setCurrentIndex( 0 );
    }

    bSetupChanged = true;
}

void operatorsCanModify::comboboxChanged(int newIndex)
{
    Q_UNUSED(newIndex);

    bSetupChanged = true;
}


//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void operatorsCanModify::SaveDataOnExit()
{
    if (bSetupChanged)
    {
        QString param;

        param = QString(ui->ckPrice->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckByCount->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckTare->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckFixedWeight->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckShelfLife->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckProductLife->isChecked() ? "1" : "0");
        param += "," + QString(ui->ckLabelType->isChecked() ? "1" : "0");
        param += "," + bDateTime;
        param += "," + QString(ui->ckPortions->isChecked() ? "1" : "0");

        param += "," + QString::number(ui->cboPriceAccessLevel->currentIndex() + 1);
        param += "," + QString::number(ui->cboByCountAccessLevel->currentIndex() + 1);
        param += "," + QString::number(ui->cboTareAccessLevel->currentIndex() + 1);
        param += "," + QString::number(ui->cboFixedWeightAccessLevel->currentIndex() + 1);
        param += "," + QString::number(ui->cboShelfLifeAccessLevel->currentIndex() + 1);
        param += "," + QString::number(ui->cboProductLifeAccessLevel->currentIndex() + 1);
        param += "," + QString::number(ui->cboLabelTypeAccessLevel->currentIndex() + 1);
        param += "," + bDateTimeAccessLevel;
        param += "," + QString::number(ui->cboPortionsAccessLevel->currentIndex() + 1);

        session_->send( ConfigurationDev, CfgOperatorAccessSave, param);
    }
}
