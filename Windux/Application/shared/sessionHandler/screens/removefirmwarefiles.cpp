
/* @file removeFirmwareFiles.cpp
 * removeFirmwareFiles
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Todd

   @Brief removes the wrapper's firmware files.
 */

#include "removefirmwarefiles.h"
#include "ui_removefirmwarefiles.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

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

removeFirmwareFiles::removeFirmwareFiles(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::removeFirmwareFiles)
{
    ui->setupUi(this);
    scaleScreen();

    internalConfigData = &session_->scaleConfig->internalConfig;
    connect(this->ui->removeFirmwareBut , SIGNAL(clicked()), this, SLOT(removeFirwareFiles_clicked()));
    update();
    translate();
}

removeFirmwareFiles::~removeFirmwareFiles()
{
    delete ui;
}

void removeFirmwareFiles::removeFirwareFiles_clicked()
{
    // Per Kraig Wooddell remove both wrapper Sw and CLA SW
    session_->send(VersionManagerDev, RemoveWrapperSWComponents);
    session_->send(VersionManagerDev,RemoveCLASWComponents);
}

void removeFirmwareFiles::initialize()
{
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void removeFirmwareFiles::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool removeFirmwareFiles::onScreenChanging()
{
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void removeFirmwareFiles::onSessionMessage( const SessionMessage& msg )
{
    if (Error == msg[0].toInt() &&  101 == msg[1].toInt() && 101001 == msg[2].toInt())
    {
        ;
    }
}

bool removeFirmwareFiles::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}
