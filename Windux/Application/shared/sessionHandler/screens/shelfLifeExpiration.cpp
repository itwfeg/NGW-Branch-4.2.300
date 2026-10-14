/**
 * @file shelfLifeExpiration.cpp
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "shelfLifeExpiration.h"
#include "ui_shelfLifeExpiration.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"


shelfLifeExpiration::shelfLifeExpiration(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::shelfLifeExpiration)
{
    ui->setupUi(this);
    scaleScreen();

    bSetupChanged = false;

    //----------------------------------------------------------------
    // Set the connections for when one of the expiration notes is
    // changed.  These all call the same method to set a property that
    // indicates something in the setup changed and the data needs to
    // be saved.
    //----------------------------------------------------------------

    connect(ui->txtPrimaryNote, SIGNAL( textChanged() ), this, SLOT( SetupChanged() ));
    connect(ui->txtSecondaryNote, SIGNAL( textChanged() ), this, SLOT(SetupChanged() ));

    //---------------------------------------
    // Request the Shelf Life Expiration data
    //---------------------------------------
    session_->send( ConfigurationDev, CfgExpirationLabelNoteRead );

    translate();
}

shelfLifeExpiration::~shelfLifeExpiration()
{
    delete ui;
}

void shelfLifeExpiration::translate()
{
    ui->retranslateUi(this);

    update();
}

bool shelfLifeExpiration::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

void shelfLifeExpiration::onSessionMessage( const SessionMessage& msg )
{
    // Shelf Life Expiration data
    if ((ConfigurationDev == msg[0].toInt() ) && ( CfgExpirationLabelNoteWrite == msg[1].toInt() ))
    {
        DomExpiringNote m( msg );

        // Update UI with the Shelf Life Expiration notes
        ui->txtPrimaryNote->setPlainText((m.primaryExpiringNoteText()));
        ui->txtSecondaryNote->setPlainText((m.secondExpiringNoteText()));

        bSetupChanged = false;
    }
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void shelfLifeExpiration::SetupChanged()
{
    bSetupChanged = true;
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void shelfLifeExpiration::SaveDataOnExit()
{
    if (bSetupChanged)
    {
        QString param;

        param = "\"" + ui->txtPrimaryNote->toPlainText() + "\"";
        param += ",\"" + ui->txtSecondaryNote->toPlainText() + "\"";

        session_->send( ConfigurationDev, CfgExpirationLabelNoteSave, param);
    }
}
