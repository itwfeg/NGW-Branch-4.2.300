/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "commclient.h"
#include "configuration.h"
#include "firmwareupgradedialog.h"
#include "mainwindow.h"
#include "sensorselection.h"

#include <message.h>
#include <devicefirmwareprogress.h>
#include <devicefirmwarerequest.h>
#include <deviceflashfirmwareota.h>
#include <nordicfirmware.h>

#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( QWidget *parent ) :
    _Mybase( parent ),
    client_( commClient::instance() ),
    firmwareUpgrade_( nullptr ),
    state_( STATE_UNKNOWN )
{
    // init
    initialize();
    createLayout();

    // proceed to dashboard
    setCurrentState( STATE_DASHBOARD );

    // connect client
    connect( client_, SIGNAL(recvMessage(const message*)), SLOT(onRecvMessage(const message*)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::translate()
{
    if ( back_ )
        back_->setText( tr( "&Back" ) );

    if ( edit_ )
        edit_->setText( tr( "&Edit" ) );

    if ( exit_ )
        exit_->setText( tr( "E&xit" ) );

    if ( save_ )
        save_->setText( tr( "&Save" ) );

    if ( refresh_ )
        refresh_->setText( tr( "&Refresh" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString mainWindow::selectedSensor() const
{
    QList<QString> items( sensorSelection_->selectedItems() );

    if ( !items.size() )
        return 0;

    return items.first();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::setCurrentState( state value )
{
    QPalette p;

    // nothing to do
    if ( value == state_ )
        return;

    // remove all buttons
    removeAllButtons();

    // handle state change
    switch ( (state_ = value) )
    {
    case STATE_DASHBOARD:
        // update buttons
        buttons_->addButton( (edit_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (refresh_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (exit_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // reset widget
        sensorSelection_->clearSelection();

        edit_->setEnabled( false );

        // switch to widget
        stackedWidget_->setCurrentWidget( sensorSelection_ );
        break;

    case STATE_CONFIGURATION:
        // update buttons
        buttons_->addButton( (save_ = new QPushButton( buttons_ )), QDialogButtonBox::ActionRole );
        buttons_->addButton( (back_ = new QPushButton( buttons_ )), QDialogButtonBox::AcceptRole );

        // reset widget
        {
            QList<QString> items( sensorSelection_->selectedItems() );

            if ( items.count() )
                configuration_->connectToDevice( items.first() );
        }

        save_->setEnabled( false );

        // switch to widget
        stackedWidget_->setCurrentWidget( configuration_ );
        break;

    default:
        break;
    }

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::removeAllButtons()
{
    // remove all buttons
    foreach ( QAbstractButton *b, buttons_->buttons() )
    {
        buttons_->removeButton( b );
        b->deleteLater();
    }

    // reset internal pointers
    back_ = edit_ = exit_ = save_ = refresh_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onButtonClicked( QAbstractButton *button )
{
    if (( back_ == button ) && ( configuration_ == stackedWidget_->currentWidget() ))
        configuration_->disconnectFromDevice();

    else if (( edit_ == button ) && ( sensorSelection_ == stackedWidget_->currentWidget() ))
        setCurrentState( STATE_CONFIGURATION );

    else if ( exit_ == button )
        close();

    else if (( save_ == button ) && ( configuration_ == stackedWidget_->currentWidget() ))
    {
        if ( configuration_->save() )
            save_->setEnabled( false );
    }

    else if ( refresh_ == button )
        sensorSelection_->refresh();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onDisconnected()
{
    setCurrentState( STATE_DASHBOARD );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onItemDoubleClicked( const QString &id )
{
    Q_UNUSED( id );

    if ( sensorSelection_ == stackedWidget_->currentWidget() )
        setCurrentState( STATE_CONFIGURATION );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onItemSelectionChanged()
{
    QList<QString> items;

    if ( sensorSelection_ == stackedWidget_->currentWidget() )
        items = sensorSelection_->selectedItems();

    if ( edit_ )
        edit_->setEnabled( !items.isEmpty() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onPendingConfigUpdated( int count )
{
    if ( !save_ )
        return;
    else if ( configuration_->savePending() )
        return;

    save_->setEnabled( count ? true : false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onRecvMessage( const message *msg )
{
    // device firmware progress
    if ( message::HFPD_DEV_FIRMWARE_PROGRESS == msg->type_ )
    {
        const deviceFirmwareProgress *request( dynamic_cast<const deviceFirmwareProgress*>( msg ) );

        if ( !request )
            return;

        qDebug() << "firmware progress" << request->progress_;

        // disable main window
        setEnabled( false );

        // show progress dialog
        if ( !firmwareUpgrade_ )
        {
            firmwareUpgrade_ = new firmwareUpgradeDialog( this );

            firmwareUpgrade_->setMinimumHeight( height() / 2 );
            firmwareUpgrade_->setMinimumWidth( width() / 2 );

            // SOF-3503
            // return to dashboard
            setCurrentState( STATE_DASHBOARD );
        }

        firmwareUpgrade_->setProgress( request->progress_ );
        firmwareUpgrade_->show();

        if ( request->finish_ )
        {
            delete firmwareUpgrade_;
            firmwareUpgrade_ = nullptr;

            // enable main window
            setEnabled( true );
        }
    }

    // device firmware request
    else if ( message::HFPD_DEV_FIRMWARE_REQUEST == msg->type_ )
    {
        const deviceFirmwareRequest *request( dynamic_cast<const deviceFirmwareRequest*>( msg ) );

        if ( !request )
            return;

        const QString address( QString::fromStdString( request->address_ ) );

        // ignore request when
        // a) not on sensor selction screen
        // b) we are pending firmware
        // c) we are explicitly ignoring the device
        if ( STATE_DASHBOARD != state_ )
            return;
        else if (( pendingFirmware_.isValid() ) && ( pendingFirmware_.msecsTo( QDateTime::currentDateTime() ) <= 60000 ))
            return;
        else if ( ignoredFirmwareRequests_.contains( address ) )
            return;

        // prompt to load firmware
        int result =
            QMessageBox::critical(
                this,
                tr( "Firmware Needed" ),
                tr( "Device"  ) + " " + address + " " + tr( "Requires Firmware" ) + "\n" +
                    "\n" +
                    tr( "Would you like to choose firmware and flash this device?" ),
                QMessageBox::StandardButtons( QMessageBox::Yes | QMessageBox::No | QMessageBox::Ignore ),
                QMessageBox::No );

        if ( QMessageBox::Ignore == result )
            ignoredFirmwareRequests_.append( address );
        else if ( QMessageBox::Yes == result )
        {
            QString startDirStr( "/lib/firmware/hfpd" );

            // create dialog
            QFileDialog d( this, tr( "Open Firmware" ), startDirStr, tr( "Zip Files (*.zip)" ) );
            d.setOption( QFileDialog::DontUseNativeDialog, true );

            d.setAcceptMode( QFileDialog::AcceptOpen );
            d.setFileMode( QFileDialog::ExistingFile );
            d.setOption( QFileDialog::ReadOnly, true );

            const QRect drect( QApplication::desktop()->screenGeometry( this ) );

            QRect r( d.geometry() );
            r.setTop( drect.height() / 2 );
            r.setHeight( drect.height() / 2 );
            r.moveLeft( (drect.width() - r.width()) / 2 );

            d.setGeometry( r );

            // prompt firmware to load
            if ( QDialog::Accepted != d.exec() )
                return;

            const QStringList filenames( d.selectedFiles() );
            const QString filename( (1 == filenames.size()) ? filenames.at( 0 ) : QString() );

            if ( !filename.length() )
                return;

            std::string image;
            int version;
            int crc;

            // validate firmware
            if ( !nordicDeviceFirmware::validate( filename.toStdString(), image, version, crc ) )
            {
                QMessageBox::critical(
                    this,
                    tr( "Invalid Firmware" ),
                    tr( "The selected file is invalid. Please select a valid firmware file." ),
                    QMessageBox::StandardButtons( QMessageBox::Ok ),
                    QMessageBox::Ok );

                return;
            }

            QString versionStr;

            if ( version <= 0 )
                versionStr = tr( "UNKNOWN" );
            else
                versionStr = QString::number( version / 100 ) + "." + QString::number( version % 100 );

            // make sure this is what they want
            result = QMessageBox::question(
                this,
                tr( "Upgrade Firmware" ),
                tr( "You are about to update device" ) + " " + address + " " + tr( "to firmware version" ) + " " + versionStr + ".\n" +
                    "\n" +
                    tr( "Do you wish to continue?" ),
                QMessageBox::StandardButtons( QMessageBox::Yes | QMessageBox::No ),
                QMessageBox::Yes );

            if ( QMessageBox::Yes == result )
            {
                // lets go!
                deviceFlashFirmwareOTA request( address.toStdString(), 0, filename.toStdString() );
                client_->sendMessage( &request );

                pendingFirmware_ = QDateTime::currentDateTime();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onUpdateRefreshPending( bool pending )
{
    if ( !refresh_ )
        return;

    QPalette p( refresh_->palette() );

    if ( pending )
    {
        p.setBrush( QPalette::Disabled, QPalette::Button, GREEN );

        refresh_->setEnabled( false );
    }
    else
    {
        const QPushButton temp( this );
        const QPalette tempPalette( temp.palette() );
        p.setBrush( QPalette::Disabled, QPalette::Button, tempPalette.brush( QPalette::Disabled, QPalette::Button ) );

        refresh_->setEnabled( true );
    }

    refresh_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::initialize()
{
    // increase font size
    QFont f( font() );
    f.setPointSize( 14 );

    setFont( f );

    // create central widget
    centralWidget_ = new QWidget( this );
    centralWidget_->setObjectName( "centralWidget" );

    setCentralWidget( centralWidget_ );

    stackedWidget_ = new QStackedWidget( centralWidget_ );
    stackedWidget_->setObjectName( "stackedWidget" );

    buttons_ = new QDialogButtonBox( Qt::Horizontal, centralWidget_ );
    buttons_->setCenterButtons( true );
    buttons_->setObjectName( "buttons" );

    connect( buttons_, SIGNAL(clicked(QAbstractButton*)), SLOT(onButtonClicked(QAbstractButton*)) );

    // ---- //

    configuration_ = new configurationWidget();
    configuration_->setObjectName( "configuration" );

    connect( configuration_, SIGNAL(pendingConfigUpdated(int)), SLOT(onPendingConfigUpdated(int)) );
    connect( configuration_, SIGNAL(disconnected()), SLOT(onDisconnected()) );

    stackedWidget_->addWidget( configuration_ );

    sensorSelection_ = new sensorSelectionWidget();
    sensorSelection_->setObjectName( "sensorSelection" );

    connect( sensorSelection_, SIGNAL(itemDoubleClicked(QString)), SLOT(onItemDoubleClicked(QString)) );
    connect( sensorSelection_, SIGNAL(itemSelectionChanged()), SLOT(onItemSelectionChanged()) );
    connect( sensorSelection_, SIGNAL(updateRefreshPending(bool)), SLOT(onUpdateRefreshPending(bool)) );

    stackedWidget_->addWidget( sensorSelection_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::createLayout()
{
    QVBoxLayout *layout( new QVBoxLayout( centralWidget_ ) );
    layout->setContentsMargins( QMargins( 0, 0, 0, 6 ) );
    layout->addWidget( stackedWidget_, 1 );
    layout->addWidget( buttons_ );
}
