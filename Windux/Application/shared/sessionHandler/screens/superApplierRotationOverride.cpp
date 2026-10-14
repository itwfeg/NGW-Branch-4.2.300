

/* @file superApplierRotationOverride.cpp
 * superApplierRotationOverride
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for overriding the applier rotation for all PLUs
 */

#include "superApplierRotationOverride.h"

#include "ui_superApplierRotationOverride.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtCore>
#include <QComboBox>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
superApplierRotationOverride::superApplierRotationOverride(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superApplierRotationOverride)
{
    ui->setupUi(this);
    scaleScreen();

    //Setup Signals and Slots
    connect(ui->cboxPrimaryRotationOverride, SIGNAL( currentIndexChanged(int)), this, SLOT(primaryApplierRotationOverride(int)));
    connect(ui->cboxSecondaryRotationOverride, SIGNAL( currentIndexChanged(int)), this, SLOT(secondaryApplierRotationOverride(int)));
    connect(ui->cboxTertiaryRotationOverride, SIGNAL( currentIndexChanged(int)), this, SLOT(tertiaryApplierRotationOverride(int)));

    connect(ui->cboxPrimaryRotationDegrees, SIGNAL( currentIndexChanged(int)), this, SLOT(checkIfHasChanged()));
    connect(ui->cboxSecondaryRotationDegrees, SIGNAL( currentIndexChanged(int)), this, SLOT(checkIfHasChanged()));
    connect(ui->cboxTertiaryRotationDegrees, SIGNAL( currentIndexChanged(int)), this, SLOT(checkIfHasChanged()));

    //Update and Initialize the Page
    initialize();
    translate();

    //Check for overrides
    session_->send(ConfigurationDev, CfgRotationOverrideRead);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::initialize()
{
    hasChanged = false;
    initialOverrideValues = NULL;

    //Create cboxes
    //Overrides
    ui->cboxPrimaryRotationOverride->clear();
    ui->cboxSecondaryRotationOverride->clear();
    ui->cboxTertiaryRotationOverride->clear();

    ui->cboxPrimaryRotationOverride->addItem(tr("Do Not Override"));
    ui->cboxPrimaryRotationOverride->addItem(tr("Set Rotation To"));
    ui->cboxPrimaryRotationOverride->addItem(tr("Rotate an Additional"));

    ui->cboxSecondaryRotationOverride->addItem(tr("Do Not Override"));
    ui->cboxSecondaryRotationOverride->addItem(tr("Set Rotation To"));
    ui->cboxSecondaryRotationOverride->addItem(tr("Rotate an Additional"));

    ui->cboxTertiaryRotationOverride->addItem(tr("Do Not Override"));
    ui->cboxTertiaryRotationOverride->addItem(tr("Set Rotation To"));
    ui->cboxTertiaryRotationOverride->addItem(tr("Rotate an Additional"));

    //Degrees
    ui->cboxPrimaryRotationDegrees->clear();
    ui->cboxSecondaryRotationDegrees->clear();
    ui->cboxTertiaryRotationDegrees->clear();

    ui->cboxPrimaryRotationDegrees->addItem("0");
    ui->cboxPrimaryRotationDegrees->addItem("45");
    ui->cboxPrimaryRotationDegrees->addItem("90");
    ui->cboxPrimaryRotationDegrees->addItem("135");
    ui->cboxPrimaryRotationDegrees->addItem("180");
    ui->cboxPrimaryRotationDegrees->addItem("225");
    ui->cboxPrimaryRotationDegrees->addItem("270");
    ui->cboxPrimaryRotationDegrees->addItem("315");

    ui->cboxSecondaryRotationDegrees->addItem("0");
    ui->cboxSecondaryRotationDegrees->addItem("45");
    ui->cboxSecondaryRotationDegrees->addItem("90");
    ui->cboxSecondaryRotationDegrees->addItem("135");
    ui->cboxSecondaryRotationDegrees->addItem("180");
    ui->cboxSecondaryRotationDegrees->addItem("225");
    ui->cboxSecondaryRotationDegrees->addItem("270");
    ui->cboxSecondaryRotationDegrees->addItem("315");

    ui->cboxTertiaryRotationDegrees->addItem("0");
    ui->cboxTertiaryRotationDegrees->addItem("45");
    ui->cboxTertiaryRotationDegrees->addItem("90");
    ui->cboxTertiaryRotationDegrees->addItem("135");
    ui->cboxTertiaryRotationDegrees->addItem("180");
    ui->cboxTertiaryRotationDegrees->addItem("225");
    ui->cboxTertiaryRotationDegrees->addItem("270");
    ui->cboxTertiaryRotationDegrees->addItem("315");

//	Windux does not disable the fields based on whether or not a printer is attached.
//	Maybe they want to set up a database for other cconfigurations that have 2 printers.
//	We will leave them all enabled. [CCD Nov 19, 2014]
//    disableAll();
//    enableConnectedPrinters();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
superApplierRotationOverride::~superApplierRotationOverride()
{
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::translate()
{
    //Translate
    ui->retranslateUi(this);

    //Refresh the cboxes
    //Rotation Overrides
    ui->cboxPrimaryRotationOverride->setItemText(0, tr("Do Not Override"));
    ui->cboxPrimaryRotationOverride->setItemText(1, tr("Set Rotation To"));
    ui->cboxPrimaryRotationOverride->setItemText(2, tr("Rotate an Additional"));

    ui->cboxSecondaryRotationOverride->setItemText(0, tr("Do Not Override"));
    ui->cboxSecondaryRotationOverride->setItemText(1, tr("Set Rotation To"));
    ui->cboxSecondaryRotationOverride->setItemText(2, tr("Rotate an Additional"));

    ui->cboxTertiaryRotationOverride->setItemText(0, tr("Do Not Override"));
    ui->cboxTertiaryRotationOverride->setItemText(1, tr("Set Rotation To"));
    ui->cboxTertiaryRotationOverride->setItemText(2, tr("Rotate an Additional"));

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool superApplierRotationOverride::onScreenChanging()
{
    if( hasChanged )
    {
        QString msg = "";
        msg += QString::number(ui->cboxPrimaryRotationOverride->currentIndex()) + ",";
        msg += QString::number(ui->cboxSecondaryRotationOverride->currentIndex()) + ",";
        msg += QString::number(ui->cboxTertiaryRotationOverride->currentIndex()) + ",";

        msg += ui->cboxPrimaryRotationDegrees->currentText() + ",";		//	HA-79
        msg += ui->cboxSecondaryRotationDegrees->currentText() + ",";	//	HA-79
        msg += ui->cboxTertiaryRotationDegrees->currentText();			//	HA-79

        session_->send(ConfigurationDev, CfgRotationOverrideSave, msg);
    }
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::enableConnectedPrinters()
{
    MadpPeripheralList *perifList = &session_->scaleConfig->peripheralList;

    if(perifList->firstPrinter())
    {
        enablePrinter(PrimaryPrinter);
    }
    if(perifList->secondPrinter())
    {
        enablePrinter(SecondaryPrinter);
    }
    if(perifList->thirdPrinter())
    {
        enablePrinter(TertiaryPrinter);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::disableAll()
{
    disablePrinter(PrimaryPrinter);
    disablePrinter(SecondaryPrinter);
    disablePrinter(TertiaryPrinter);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::enableAll()
{
    enablePrinter(PrimaryPrinter);
    enablePrinter(SecondaryPrinter);
    enablePrinter(TertiaryPrinter);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::enablePrinter(int printerNum)
{
    switch( printerNum )
    {
        case 0://Printer 1
            ui->cboxPrimaryRotationOverride->setEnabled(true);
            ui->cboxPrimaryRotationDegrees->setEnabled(true);
            break;
        case 1://Printer 2
            ui->cboxSecondaryRotationOverride->setEnabled(true);
            ui->cboxSecondaryRotationDegrees->setEnabled(true);
            break;
        case 2://Printer 3
            ui->cboxTertiaryRotationOverride->setEnabled(true);
            ui->cboxTertiaryRotationDegrees->setEnabled(true);
            break;
        default:
            //do nothing
            return;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::disablePrinter(int printerNum)
{
    switch( printerNum )
    {
        case 0://Printer 1
            ui->cboxPrimaryRotationOverride->setEnabled(false);
            ui->cboxPrimaryRotationDegrees->setEnabled(false);

            ui->cboxPrimaryRotationOverride->blockSignals(true);
            ui->cboxPrimaryRotationOverride->setCurrentIndex(0);
            ui->cboxPrimaryRotationOverride->blockSignals(false);

            ui->cboxPrimaryRotationDegrees->blockSignals(true);
            ui->cboxPrimaryRotationDegrees->setCurrentIndex(0);
            ui->cboxPrimaryRotationDegrees->blockSignals(false);
            break;
        case 1://Printer 2
            ui->cboxSecondaryRotationOverride->setEnabled(false);
            ui->cboxSecondaryRotationDegrees->setEnabled(false);

            ui->cboxSecondaryRotationOverride->blockSignals(true);
            ui->cboxSecondaryRotationOverride->setCurrentIndex(0);
            ui->cboxSecondaryRotationOverride->blockSignals(false);

            ui->cboxSecondaryRotationDegrees->blockSignals(true);
            ui->cboxSecondaryRotationDegrees->setCurrentIndex(0);
            ui->cboxSecondaryRotationDegrees->blockSignals(false);
            break;
        case 2://Printer 3
            ui->cboxTertiaryRotationOverride->setEnabled(false);
            ui->cboxTertiaryRotationDegrees->setEnabled(false);

            ui->cboxTertiaryRotationOverride->blockSignals(true);
            ui->cboxTertiaryRotationOverride->setCurrentIndex(0);
            ui->cboxTertiaryRotationOverride->blockSignals(false);

            ui->cboxTertiaryRotationDegrees->blockSignals(true);
            ui->cboxTertiaryRotationDegrees->setCurrentIndex(0);
            ui->cboxTertiaryRotationDegrees->blockSignals(false);
            break;
        default:
            //do nothing
            return;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::primaryApplierRotationOverride(int index)
{
    if( index != 0 )
        enableRotationDegrees(PrimaryPrinter);
    else
        disableRotationDegrees(PrimaryPrinter);
    checkIfHasChanged();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::secondaryApplierRotationOverride(int index)
{
    if( index != 0 )
        enableRotationDegrees(SecondaryPrinter);
    else
        disableRotationDegrees(SecondaryPrinter);
    checkIfHasChanged();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::tertiaryApplierRotationOverride(int index)
{
    if( index != 0 )
        enableRotationDegrees(TertiaryPrinter);
    else
        disableRotationDegrees(TertiaryPrinter);
    checkIfHasChanged();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::checkIfHasChanged()
{
    if(initialOverrideValues)
    {
        if( initialOverrideValues->overrideSetting1().toInt() == ui->cboxPrimaryRotationOverride->currentIndex() &&
            initialOverrideValues->overrideSetting2().toInt() == ui->cboxSecondaryRotationOverride->currentIndex() &&
            initialOverrideValues->overrideSetting3().toInt() == ui->cboxTertiaryRotationOverride->currentIndex() &&
            initialOverrideValues->overrideValue1().toInt() == ui->cboxPrimaryRotationDegrees->currentIndex() &&
            initialOverrideValues->overrideValue2().toInt() == ui->cboxSecondaryRotationDegrees->currentIndex() &&
            initialOverrideValues->overrideValue3().toInt() == ui->cboxTertiaryRotationDegrees->currentIndex() )
            hasChanged = false;
        else
            hasChanged = true;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::disableRotationDegrees(int printerNum)
{
    switch( printerNum )
    {
    case 0:
        ui->cboxPrimaryRotationDegrees->setEnabled(false);
        break;
    case 1:
        ui->cboxSecondaryRotationDegrees->setEnabled(false);
        break;
    case 2:
        ui->cboxTertiaryRotationDegrees->setEnabled(false);
        break;
    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::enableRotationDegrees(int printerNum)
{
    switch( printerNum )
    {
    case 0:
        ui->cboxPrimaryRotationDegrees->setEnabled(true);
        break;
    case 1:
        ui->cboxSecondaryRotationDegrees->setEnabled(true);
        break;
    case 2:
        ui->cboxTertiaryRotationDegrees->setEnabled(true);
        break;
    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::updateRotationOverrideTable(DomRotationOverrides msg)
{
    //Disable Degrees field if no applier override rotation is applied
    if( msg.overrideSetting1().toInt() == 0)
    {
        disableRotationDegrees(PrimaryPrinter);
    }
    if( msg.overrideSetting2().toInt() == 0)
    {
        disableRotationDegrees(SecondaryPrinter);
    }
    if( msg.overrideSetting3().toInt() == 0)
    {
        disableRotationDegrees(TertiaryPrinter);
    }

    //Set the indices for the override settings
    ui->cboxPrimaryRotationOverride->setCurrentIndex(msg.overrideSetting1().toInt());
    ui->cboxSecondaryRotationOverride->setCurrentIndex(msg.overrideSetting2().toInt());
    ui->cboxTertiaryRotationOverride->setCurrentIndex(msg.overrideSetting3().toInt());

	//	Set the rotation overrides.
    setOverrideSetting(ui->cboxPrimaryRotationDegrees, msg.overrideValue1().toInt());		//	HA-79
    setOverrideSetting(ui->cboxSecondaryRotationDegrees, msg.overrideValue2().toInt());		//	HA-79
    setOverrideSetting(ui->cboxTertiaryRotationDegrees, msg.overrideValue3().toInt());		//	HA-79
}

//	HA-79:  We need to find the rotation value in the combo box since that is what is saved
//			to configuration (not the index).
void superApplierRotationOverride::setOverrideSetting(QComboBox *myCBox, int rotationVal)
{
    int index = -1;
    for (int x = 0; x < myCBox->count(); x++)
    {
        if (myCBox->itemText(x).toInt() == rotationVal)
        {
            index = x;
            break;
        }
    }
    if (index == -1)
    {
        index = 0;		//	We have a problem here!
    }
    myCBox->setCurrentIndex(index);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void superApplierRotationOverride::onSessionMessage( const SessionMessage& msg )
{
    if( ( ConfigurationDev == msg[0].toInt() ) && ( CfgRotationOverrideWrite == msg[1].toInt()) )
    {
        DomRotationOverrides m(msg);

        //Save this for later so we can see if the rotation override has changed
        initialOverrideValues = new DomRotationOverrides(msg);

        updateRotationOverrideTable(m);
    }
}
