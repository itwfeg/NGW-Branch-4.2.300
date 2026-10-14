/**
 * @file printercleanerdialog.cpp
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Joseph DiCarlantonio
 */

#include <logmanager.h>

#include "printercleanerdialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <QApplication>
#include <QPushButton>

#include <UiUtilities.h>

#include "ui_printercleanerdialog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterCleanerDialog::PrinterCleanerDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    ui( new Ui::PrinterCleanerDialog )
{
    ui->setupUi(this);

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PrinterCleanerDialog::~PrinterCleanerDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterCleanerDialog::onCleanButtonClicked( )
{
    LOG_CLASS_INFO( "PrinterCleanerDialog", "onButtonClicked" );
    session_->send( Printer, PrTest, "5" );
    ui->btnCleanLabel->setEnabled( false );
    LOG_INFO( "Send clean message", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterCleanerDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PrinterCleanerDialog::initialize()
{
    ui->btnCleanLabel->setEnabled( true );
    connect( ui->btnCleanLabel, SIGNAL( clicked() ), this, SLOT( onCleanButtonClicked() ) );


}
