/**
 * @file manufacturingImageDialog.cpp
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @author Alex Zavala
 */


#include <logmanager.h>

#include "manufacturingImageDialog.h"
#include "ui_manufacturingImageDialog.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include <ScaleConfiguration.h>
#include <Messages.h>
#include <UiUtilities.h>

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>


ManufacturingImageDialog::ManufacturingImageDialog(QWidget *parent): SessionDialog( CenterDialogLocation, parent ),
    ui(new Ui::ManufacturingImageDialog)
{
    setObjectName( "ManufacturingImageDialog" );

    internalConfigData_ = &session_->scaleConfig->internalConfig;

    initialize();
    translate();

    //Setup button slots
    connect(ui->btnAWS, SIGNAL(clicked()), SLOT(awsClicked()));
    connect(ui->btnEWS, SIGNAL(clicked()), SLOT(ewsClicked()));
    connect(ui->btnCLAS, SIGNAL(clicked()), SLOT(clasClicked()));

    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );
}

void ManufacturingImageDialog::initialize()
{
    ui->setupUi(this);
}

QSize ManufacturingImageDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 725) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 250) / SessionMainWindow::scalingDividerY_ );
}

ManufacturingImageDialog::~ManufacturingImageDialog()
{
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );

    delete ui;
}

bool ManufacturingImageDialog::allowOverlayClickDismiss() const
{
    // operator must click the done button to dismiss dialog
    return false;
}

void ManufacturingImageDialog::translate()
{
    ui->retranslateUi(this);

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

void ManufacturingImageDialog::awsClicked()
{
    //Send the message and then close the dialog
    session_->send(Wrapper, WrUIWrapperModel, "0");

    internalConfigData_->isWrapperModelInitialization = true; // SOF-4929

    accept();
}

void ManufacturingImageDialog::ewsClicked()
{
    //Send the message and then close the dialog
    session_->send(Wrapper, WrUIWrapperModel, "1");

    internalConfigData_->isWrapperModelInitialization = true; // SOF-4929

    accept();
}

void ManufacturingImageDialog::clasClicked()
{
    //Send the message and then close the dialog
    session_->send(Wrapper, WrUIWrapperModel, "2");

    internalConfigData_->isClaModelInitialization = true; // SOF-4929
    internalConfigData_->isWrapperModelInitialization = true; // SOF-4929

    accept();
}
