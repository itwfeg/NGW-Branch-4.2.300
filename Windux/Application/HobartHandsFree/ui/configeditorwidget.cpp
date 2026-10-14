/**
 * @file configeditorwidget.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "configeditorwidget.h"

#include <nordicfirmware.h>

#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

extern bool _expert;

///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline QPalette::ColorRole configEditorWidget::widgetBackgroundRole( const T *widget )
{
    return widget->backgroundRole();
}

template <>
inline QPalette::ColorRole configEditorWidget::widgetBackgroundRole<QComboBox>( const QComboBox* /*widget*/ )
{
    return QPalette::Button;
}

template <>
inline QPalette::ColorRole configEditorWidget::widgetBackgroundRole<QSpinBox>( const QSpinBox* /*widget*/ )
{
    return QPalette::Base;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline QColor configEditorWidget::widgetDefaultBackgroundColor()
{
    const T widget;
    const QPalette p( widget.palette() );

    return p.color( widgetBackgroundRole( &widget ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <>
inline QByteArray configEditorWidget::widgetConfigValue<QComboBox>( const QComboBox *widget, int len )
{
    QByteArray newval;

    while ( newval.length() < len )
    {
        const QVariant data( widget->currentData() );

        int v( data.toInt() );
        v >>= 8 * newval.length();
        v &= 0xFF;

        newval.prepend( (char) v );
    }

    return newval;
}

template <>
inline QByteArray configEditorWidget::widgetConfigValue<QLineEdit>( const QLineEdit *widget, int len )
{
    QByteArray newval( qPrintable( widget->text() ) );

    while ( newval.length() < len )
        newval.append( ' ' );

    if ( len < newval.length() )
        newval.truncate( len );

    return newval;
}

template <>
inline QByteArray configEditorWidget::widgetConfigValue<QSpinBox>( const QSpinBox *widget, int len )
{
    QByteArray newval;

    while ( newval.length() < len )
    {
        int v( widget->value() );
        v >>= 8 * newval.length();
        v &= 0xFF;

        newval.prepend( (char) v );
    }

    return newval;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <>
inline void configEditorWidget::connectFieldSignals<QComboBox>( QComboBox *field )
{
    connect( field, SIGNAL(currentIndexChanged(int)), SLOT(onEditingFinished()) );
}

template <>
inline void configEditorWidget::connectFieldSignals<QLabel>( QLabel* /*field*/ )
{
    // nothing to do
}

template <>
inline void configEditorWidget::connectFieldSignals<QLineEdit>( QLineEdit *field )
{
    connect( field, SIGNAL(textChanged(QString)), SLOT(onEditingFinished()) );
}

template <>
inline void configEditorWidget::connectFieldSignals<QSpinBox>( QSpinBox *field )
{
    connect( field, SIGNAL(valueChanged(int)), SLOT(onEditingFinished()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline void configEditorWidget::updateFieldBackground( T *field, bool changed ) const
{
    // update palette
    QPalette p( field->palette() );
    p.setColor( widgetBackgroundRole( field ), changed ? YELLOW : widgetDefaultBackgroundColor<T>() );

    field->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline bool configEditorWidget::createField( int config, QWidget *parent, QLabel **fieldLabel, T **field, const QString& fieldLabelText )
{
    // check existing fields
    if ( fields_.contains( config ) )
    {
        (*fieldLabel) = fields_[config].first;
        (*field) = qobject_cast<T*>( fields_[config].second );

        return false;
    }

    // create field label
    (*fieldLabel) = new QLabel( parent );
    (*fieldLabel)->setObjectName( "field" + QString::number( config ) + "_label" );
    (*fieldLabel)->setText( fieldLabelText );

    // create field
    (*field) = new T( parent );
    (*field)->setObjectName( "field" + QString::number( config ) );

    connectFieldSignals( (*field) );

    fields_[config] = fieldPair( (*fieldLabel), (*field) );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline bool configEditorWidget::hasFieldChanged( int config, T *field, QByteArray& value ) const
{
    if ( !config_.contains( config ) )
        return false;

    // check new value different from old value
    const QByteArray oldval( config_[config] );

    const bool changed( oldval != (value = widgetConfigValue( field, oldval.length() )) );

    // update background
    updateFieldBackground( field, changed );

    return changed;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
configEditorWidget::configEditorWidget( QWidget *parent ) :
    _Mybase( parent ),
    version_( 0 )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
configEditorWidget::~configEditorWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString configEditorWidget::firmwareVersion() const
{
    return QString::number( version_ / 100 ) + "." + QString::number( version_ % 100 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
configEditorWidget::configMap configEditorWidget::pending() const
{
    return configPending_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::setValue( int id, const QByteArray& value )
{
    config_[id] = value;

    // process firmware version
    if ( 0 == id )
    {
        if ( 2 != value.length() )
            qWarning() << "unable to process firmware";
        else
        {
            version_ = (100 * value[0]) + value[1];
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::clear()
{
    // reset version
    version_ = 0;

    // delete all config
    config_.clear();

    clearPending();

    // delete all config widgets
    foreach ( const fieldPair& field, fields_ )
    {
        delete field.first;
        delete field.second;
    }

    // delete all fields
    fields_.clear();

    // reset form
    setCurrentWidget( operConfig_ );

    message_->clear();

    logoFilename_->clear();

    firmwareFilename_->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::clearPending()
{
    configPending_.clear();
    emit pendingConfigUpdated( configPending_.size() );

    // enable input
    enableUserInput();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool configEditorWidget::process()
{
    // Currently v1.0.0 - v.1.9.9 is supported
    if (( 100 <= version_ ) && ( version_ <= 199 ))
    {
        const bool populated( 0 < fields_.size() );

        // make sure we have all expected fields
        if ( config_.size() != configExpected_v1_.size() )
            return false;

        // make sure each expected field exists
        foreach ( int config, configExpected_v1_.keys() )
            if ( !config_.contains( config ) )
                return false;

        // setup allowed power levels
        QStringList powerLevels;
        powerLevels.append( "-40" );
        powerLevels.append( "-20" );
        powerLevels.append( "-16" );
        powerLevels.append( "-12" );
        powerLevels.append( "-8" );
        powerLevels.append( "-4" );
        powerLevels.append( "0" );
        powerLevels.append( "4" );

        // ---- //

        updateLabel( 1, 2, tr( "Device Type:" ), toHexString( config_[1] ) );
        updateLabel( 2, 4, tr( "OTA Identifier:" ), toHexString( config_[2] ) );

        updateLineEdit( 10, 10, tr( "Active Message (Top):" ), QString( config_[10] ) );
        updateLineEdit( 11, 10, tr( "Active Message (Bottom):" ), QString( config_[11] ) );
        updateLineEdit( 12, 10, tr( "Inactive Message (Top):" ), QString( config_[12] ) );
        updateLineEdit( 13, 10, tr( "Inactive Message (Bottom):" ), QString( config_[13] ) );

        updateLineEdit( 20, 16, tr( "Operator Name:" ), QString( config_[20] ) );
        updateSpinBox( 21, 1, tr( "Shift Duration (hours):" ), toUInt8( config_[21] ), 0, 36 );

        updateComboBox( 50, 1, tr( "TX Power Level (db):" ), toInt8( config_[50] ), powerLevels );
        updateSpinBox( 51, 1, tr( "TX Low Power Count:" ), toUInt8( config_[51] ), 0, 15 );
        updateSpinBox( 52, 2, tr( "TX Delay (ms):" ), toUInt16( config_[52] ), 125, 2000 );

        updateSpinBox( 60, 1, tr( "Beep Time (ms):" ), toUInt8( config_[60] ), 0, 255 );

        // ---- //

        if ( !populated )
        {
            if ( operConfig_->layout() )
                delete operConfig_->layout();

            QFormLayout *operConfigLayout( new QFormLayout( operConfig_  ) );

            operConfigLayout->addRow( fields_[1].first, fields_[1].second );
            operConfigLayout->addRow( fields_[2].first, fields_[2].second );
            operConfigLayout->addItem( new QSpacerItem( 16, 16 ) );

            operConfigLayout->addRow( fields_[20].first, fields_[20].second );
            operConfigLayout->addRow( fields_[21].first, fields_[21].second );
            operConfigLayout->addItem( new QSpacerItem( 16, 16 ) );

            operConfigLayout->addRow( fields_[60].first, fields_[60].second );
            operConfigLayout->addItem( new QSpacerItem( 16, 16 ) );

            // ---- //

            if ( shiftTextConfig_->layout() )
                delete shiftTextConfig_->layout();

            QFormLayout *shiftTextConfigLayout( new QFormLayout( shiftTextConfig_  ) );

            shiftTextConfigLayout->addRow( fields_[10].first, fields_[10].second );
            shiftTextConfigLayout->addRow( fields_[11].first, fields_[11].second );
            shiftTextConfigLayout->addRow( fields_[12].first, fields_[12].second );
            shiftTextConfigLayout->addRow( fields_[13].first, fields_[13].second );
            shiftTextConfigLayout->addItem( new QSpacerItem( 16, 16 ) );

            // ---- //

            if ( advertConfig_->layout() )
                delete advertConfig_->layout();

            QFormLayout *advertConfigLayout( new QFormLayout( advertConfig_   ) );

            advertConfigLayout->addRow( fields_[50].first, fields_[50].second );
            advertConfigLayout->addRow( fields_[51].first, fields_[51].second );
            advertConfigLayout->addRow( fields_[52].first, fields_[52].second );
            advertConfigLayout->addItem( new QSpacerItem( 16, 16 ) );

            // ---- //

            const bool hasDisplayAttached( (config_[1].at( 1 ) & 0x01) ? true : false );

            QList<int> fieldsToDisable;
            fieldsToDisable.push_back( 10 );
            fieldsToDisable.push_back( 11 );
            fieldsToDisable.push_back( 12 );
            fieldsToDisable.push_back( 13 );
            fieldsToDisable.push_back( 20 );

            foreach ( int f, fieldsToDisable )
            {
                fields_[f].first->setEnabled( hasDisplayAttached );
                fields_[f].second->setEnabled( hasDisplayAttached );
            }

            QList<int> tabsToDisable;
            tabsToDisable.push_back( messagingTabIndex_ );
            tabsToDisable.push_back( logoTabIndex_ );

            foreach ( int t, tabsToDisable )
                setTabEnabled( t, hasDisplayAttached );
        }

        return true;

    } // v1.0.0 - v.1.9.9

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::translate()
{
    setTabText( indexOf( operConfig_  ), tr( "Operator Setup" ) );
    setTabText( indexOf( shiftTextConfig_  ), tr( "Shift Text" ) );
    setTabText( indexOf( advertConfig_  ), tr( "Advert Config" ) );
    setTabText( indexOf( messaging_ ), tr( "Messages" ) );
    setTabText( indexOf( logo_ ), tr( "Logo" ) );
    setTabText( indexOf( firmware_  ), tr( "Firmware" ) );

    messageLabel_->setText( tr( "Message Text:  (16 Characters Max)" ) );

    sendMessage_->setText( tr( "Send Message" ) );
    clearAllMessages_->setText( tr( "Clear All Messages" ) );

    logoFilenameLabel_->setText( tr( "Logo Filename:  (128 x 48 px Optimal)" ) );

    sendLogo_->setText( tr( "Upload Logo" ) );

    firmwareFilenameLabel_->setText( tr( "Firmware Filename:" ) );

    updateFirmware_->setText( tr( "Update" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::onClicked()
{
    // send message
    if ( sendMessage_ == sender() )
    {
        emit sendMessage( message_->text() );

        message_->setText( QString() );
    }

    // clear all messages
    else if ( clearAllMessages_ == sender() )
    {
        emit clearAllMessages();
    }

    // find logo
    else if ( findLogo_ == sender() )
    {
        QStringList folders;
        QString startDirStr;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb3" );
        folders.push_back( "/media/thumb2" );
        folders.push_back( "/media/thumb1" );
        folders.push_back( "/media/thumb" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

        // find existing folder
        foreach ( const QString& f, folders )
            if ( QDir( startDirStr = f ).exists() )
                break;

        // create dialog
        QFileDialog d( this, tr( "Open Image" ), startDirStr, tr( "Image Files (*.bmp *.gif *.jpg *.png)" ) );
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

        // show dialog
        if ( QDialog::Accepted == d.exec() )
        {
            const QStringList filenames( d.selectedFiles() );
            const QString filename( (1 == filenames.size()) ? filenames.at( 0 ) : QString() );

            if ( filename.length() )
                logoFilename_->setText( filename );
        }
    }

    // send logo
    else if ( sendLogo_ == sender() )
    {
        // convert image to byte array
        QByteArray a;

        for ( int w( 0 ); w < 128; ++w )
            for ( int h( 40 ); 0 <= h; h -= 8 )
            {
                unsigned char byte( 0 );

                for ( int bit( 8 ); bit--; )
                {
                    byte <<= 1;

                    if ( qGray( logoImage_.pixel( w, h+bit ) ) < 128 )
                        byte |= 1;
                }

                a.append( byte );
            }

        // emit
        emit sendLogo( a );
    }

    // find firmware
    else if ( findFirmware_ == sender() )
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

        // show dialog
        if ( QDialog::Accepted == d.exec() )
        {
            const QStringList filenames( d.selectedFiles() );
            const QString filename( (1 == filenames.size()) ? filenames.at( 0 ) : QString() );

            if ( filename.length() )
                firmwareFilename_->setText( filename );
        }
    }

    // update firmware
    else if ( updateFirmware_ == sender() )
    {
        // emit
        updateFirmware( toUInt32( config_[2] ), firmwareFilename_->text() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::onEditingFinished()
{
    // logo
    if ( logoFilename_ == sender() )
    {
        const QString filename( logoFilename_->text() );

        // convert to b/w image and show preview
        if ( !QFile::exists( filename ) )
            logoPreview_->setPixmap( QPixmap::fromImage( logoImage_ = QImage() ) );
        else
        {
            const QSize s( 128, 48 );
            const QPixmap raw( logoFilename_->text() );

            if ( raw.isNull() )
                return;

            // scale image
            const QPixmap scaled( raw.scaled( s, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

            // center and convert
            QImage centered;

            if ( s == scaled.size() )
                centered = scaled.toImage();
            else
            {
                QPixmap temp( s );
                temp.fill( Qt::white );

                QPainter p;
                p.setRenderHint( QPainter::Antialiasing, true );

                p.begin( &temp );
                p.drawPixmap( (s.width() - scaled.width()) / 2, (s.height() - scaled.height()) / 2, scaled );
                p.end();

                centered = temp.toImage();
            }

            // convert to black and white
            logoImage_ = QImage( s, QImage::Format_Mono );
            logoImage_.fill( 1 );

            for ( int h( s.height() ); h--; )
                for ( int w( s.width() ); w--; )
                    if ( qGray( centered.pixel( w, h ) ) < 128 )
                        logoImage_.setPixel( w, h, 0 );


            // show preview
            logoPreview_->setFixedSize( s );
            logoPreview_->setPixmap( QPixmap::fromImage( logoImage_ ) );
        }
    }

    // firmware
    else if ( firmwareFilename_ == sender() )
    {
        // this will be validated and enabled in enableUserInput()...
        updateFirmware_->setEnabled( false );
        updateFirmwareLabel_->clear();
    }

    // config field
    else
    {
        // locate field
        for ( fieldPairMap::const_iterator i( fields_.begin() ); i != fields_.end(); ++i )
        {
            const fieldPair field( i.value() );

            // check field against sender
            if ( field.second == sender() )
            {
                QByteArray newval;
                bool changed( false );

                // check for field value changed
                QComboBox *comboBox;
                QLineEdit *lineEdit;
                QSpinBox *spinBox;

                if ( (comboBox = qobject_cast<QComboBox*>( sender() )) )
                    changed = hasFieldChanged( i.key(), comboBox, newval );
                else if ( (lineEdit = qobject_cast<QLineEdit*>( sender() )) )
                    changed = hasFieldChanged( i.key(), lineEdit, newval );
                else if ( (spinBox = qobject_cast<QSpinBox*>( sender() )) )
                {
                    changed = hasFieldChanged( i.key(), spinBox, newval );

                    // We need to send high powered advert every 2 seconds due to limitations in
                    // Qt and how BLE works. The BLE specification states a host must connect
                    // to a device within 150 microseconds after advert transmission otherwise
                    // the radio will be powered off. Qt only waits around approx 2 seconds for
                    // such an advert before it gives up and times out.
                    //
                    // So here we enforce this 2 second high power advert rule. If they set the
                    // TX power really low (i.e. -40 db) we will never see the device unless it
                    // sends a high power advert.
                    //
                    // So here if they make modification to one field we will auto adjust the
                    // other field to reflect proper 2 second ratio.

                    // low power count
                    // advert delay
                    if ((( fields_.contains( 51 ) ) && ( spinBox == fields_[51].second )) ||
                        (( fields_.contains( 52 ) ) && ( spinBox == fields_[52].second )))
                    {
                        static const int maxHighPowerDelay( 2000 );

                        QSpinBox *lowPowerCount( qobject_cast<QSpinBox*>( fields_[51].second ) );
                        QSpinBox *advertDelay( qobject_cast<QSpinBox*>( fields_[52].second ) );

                        // calculate current delay
                        const int delay( advertDelay->value() * (lowPowerCount->value() + 1) );

                        if ( maxHighPowerDelay < delay )
                        {
                            const int value( spinBox->value() );

                            // generate mapping of values
                            QMap<int, int> advertMapping;

                            for ( int i( lowPowerCount->minimum() ); i <= lowPowerCount->maximum(); ++i )
                                advertMapping[i] = maxHighPowerDelay / (i+1);

                            // block signals while we change the other
                            spinBox->blockSignals( true );

                            // low power count changed... modify advert delay
                            if (( lowPowerCount == spinBox ) && ( lowPowerCount->minimum() <= value ) && ( value <= lowPowerCount->maximum() ))
                                advertDelay->setValue( advertMapping[value] );

                            // advert delay changed... modify low power count
                            else if (( advertDelay == spinBox ) && ( advertDelay->minimum() <= value ) && ( value <= advertDelay->maximum() ))
                            {
                                for ( int i( lowPowerCount->maximum() ); lowPowerCount->minimum() <= i; --i )
                                    if ( value <= advertMapping[i] )
                                    {
                                        lowPowerCount->setValue( i );
                                        break;
                                    }
                            }

                            // restore signals
                            spinBox->blockSignals( false );
                        }
                    }
                }
                else
                {
                    qWarning() << "unhandled field" << sender();
                    return;
                }

                // update pending map
                if ( changed )
                    configPending_[i.key()] = newval;
                else
                    configPending_.remove( i.key() );

                emit pendingConfigUpdated( configPending_.size() );

                break;
            }
        }
    }

    // enable input
    enableUserInput();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::initialize()
{
    operConfigTabIndex_ = addTab( operConfig_ = new QWidget( this ), QString() );
    shiftTextConfigTabIndex_ = addTab( shiftTextConfig_ = new QWidget( this ), QString() );

    advertConfigTabIndex_ = addTab( advertConfig_ = new QWidget( this ), QString() );

    if ( !_expert )
        removeTab( advertConfigTabIndex_ );

    messagingTabIndex_= addTab( messaging_ = new QWidget( this ), QString() );
    logoTabIndex_ = addTab( logo_ = new QWidget( this ), QString() );
    firmwareTabIndex_ = addTab( firmware_ = new QWidget( this ), QString() );

    // ---- //

    messageLabel_ = new QLabel( messaging_ );
    messageLabel_->setObjectName( "messageLabel" );

    message_ = new QLineEdit( messaging_ );
    message_->setObjectName( "message" );

    message_->setMaxLength( 16 );

    connect( message_, SIGNAL(textChanged(QString)), SLOT(onEditingFinished()) );

    sendMessage_ = new QPushButton( messaging_ );
    sendMessage_->setEnabled( false );
    sendMessage_->setObjectName( "sendMessage" );

    connect( sendMessage_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    clearAllMessages_ = new QPushButton( messaging_ );
    clearAllMessages_->setObjectName( "clearAllMessages" );

    connect( clearAllMessages_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    // ---- //

    logoFilenameLabel_ = new QLabel( logo_ );
    logoFilenameLabel_->setObjectName( "logoFilenameLabel" );

    logoFilename_ = new QLineEdit( logo_ );
    logoFilename_->setObjectName( "logoFilename" );

    connect( logoFilename_, SIGNAL(textChanged(QString)), SLOT(onEditingFinished()) );

    findLogo_ = new QPushButton( "...", logo_ );
    findLogo_->setObjectName( "findLogo" );

    connect( findLogo_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    sendLogo_ = new QPushButton( logo_ );
    sendLogo_->setEnabled( false );
    sendLogo_->setObjectName( "sendLogo" );

    connect( sendLogo_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    logoPreview_ = new QLabel( logo_ );
    logoPreview_->setObjectName( "logoPreview" );

    // ---- //

    firmwareFilenameLabel_ = new QLabel( firmware_ );
    firmwareFilenameLabel_->setObjectName( "firmwareFilenameLabel" );

    firmwareFilename_ = new QLineEdit( firmware_ );
    firmwareFilename_->setObjectName( "firmwareFilename" );

    connect( firmwareFilename_, SIGNAL(textChanged(QString)), SLOT(onEditingFinished()) );

    findFirmware_ = new QPushButton( "...", firmware_ );
    findFirmware_->setObjectName( "findFirmware" );

    connect( findFirmware_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    updateFirmwareLabel_ = new QLabel( firmware_ );
    updateFirmwareLabel_->setObjectName( "updateFirmwareLabel" );

    updateFirmware_ = new QPushButton( firmware_ );
    updateFirmware_->setEnabled( false );
    updateFirmware_->setObjectName( "updateFirmware" );

    connect( updateFirmware_, SIGNAL(clicked(bool)), SLOT(onClicked()) );

    // ---- //

    configExpected_v1_[ 0 ] = nullptr;              // FIRMWARE_VER
    configExpected_v1_[ 1 ] = operConfig_;          // DEVICE_TYPE
    configExpected_v1_[ 2 ] = operConfig_;          // OTA_ID
    configExpected_v1_[ 10 ] = shiftTextConfig_;    // MSG_ACTIVE_TOP
    configExpected_v1_[ 11 ] = shiftTextConfig_;    // MSG_ACTIVE_BOTTOM
    configExpected_v1_[ 12 ] = shiftTextConfig_;    // MSG_INACTIVE_TOP
    configExpected_v1_[ 13 ] = shiftTextConfig_;    // MSG_INACTIVE_BOTTOM
    configExpected_v1_[ 20 ] = operConfig_;         // USER_NAME
    configExpected_v1_[ 21 ] = operConfig_;         // SHIFT_DURATION
    configExpected_v1_[ 50 ] = advertConfig_;       // ADVERT_TX_POWER_LEVEL
    configExpected_v1_[ 51 ] = advertConfig_;       // ADVERT_TX_COUNT
    configExpected_v1_[ 52 ] = advertConfig_;       // ADVERT_TX_DELAY
    configExpected_v1_[ 60 ] = operConfig_;         // BEEP_TIME
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::createLayout()
{
    QHBoxLayout *messageButtons( new QHBoxLayout() );
    messageButtons->addWidget( sendMessage_ );
    messageButtons->addWidget( clearAllMessages_ );
    messageButtons->addStretch();

    QVBoxLayout *messageLayout( new QVBoxLayout( messaging_ ) );
    messageLayout->addWidget( messageLabel_ );
    messageLayout->addWidget( message_ );
    messageLayout->addItem( new QSpacerItem( 16, 16 ) );
    messageLayout->addLayout( messageButtons );
    messageLayout->addStretch();

    // ---- //

    QHBoxLayout *logoFilename( new QHBoxLayout() );
    logoFilename->addWidget( logoFilename_, 1 );
    logoFilename->addWidget( findLogo_ );

    QHBoxLayout *logoButtons( new QHBoxLayout() );
    logoButtons->addWidget( sendLogo_ );
    logoButtons->addWidget( logoPreview_ );
    logoButtons->addStretch();

    QVBoxLayout *logoLayout( new QVBoxLayout( logo_ ) );
    logoLayout->addWidget( logoFilenameLabel_ );
    logoLayout->addLayout( logoFilename );
    logoLayout->addItem( new QSpacerItem( 16, 16 ) );
    logoLayout->addLayout( logoButtons );
    logoLayout->addStretch();

    // ---- //

    QHBoxLayout *firmwareFilename( new QHBoxLayout() );
    firmwareFilename->addWidget( firmwareFilename_, 1 );
    firmwareFilename->addWidget( findFirmware_ );

    QHBoxLayout *firmwareButtons( new QHBoxLayout() );
    firmwareButtons->addWidget( updateFirmware_ );
    firmwareButtons->addStretch();

    QVBoxLayout *firmwareLayout( new QVBoxLayout( firmware_ ) );
    firmwareLayout->addWidget( firmwareFilenameLabel_ );
    firmwareLayout->addLayout( firmwareFilename );
    firmwareLayout->addItem( new QSpacerItem( 16, 16 ) );
    firmwareLayout->addWidget( updateFirmwareLabel_ );
    firmwareLayout->addLayout( firmwareButtons );
    firmwareLayout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::enableUserInput()
{
    const QString messageText( message_->text() );

    int version( 0 );

    sendMessage_->setEnabled( messageText.length() );
    sendLogo_->setEnabled( !logoImage_.isNull() );
    updateFirmware_->setEnabled( validateFirmwareImage( firmwareFilename_->text(), version ) );

    if (( updateFirmware_->isEnabled() ) && ( 0 < version ))
        updateFirmwareLabel_->setText( "Firmware Version: " + QString::number( version / 100 ) + "." + QString::number( version % 100 ) );
    else if ( updateFirmware_->isEnabled() )
        updateFirmwareLabel_->setText( "Firmware Version: UNKNOWN" );
    else
        updateFirmwareLabel_->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::updateComboBox( int config, int configLen, const QString &text, int val, const QStringList& vals )
{
    const QByteArray a( config_[config] );

    QLabel *valueLabel;
    QComboBox *value;

    if ( configLen != a.length() )
        return;

    fieldParentMap *parents( nullptr );

    if (( 100 <= version_ ) && ( version_ <= 199 ))
        parents = &configExpected_v1_;

    if ( !parents )
        return;

    if ( createField( config, parents->value( config ), &valueLabel, &value, text ) )
    {
        value->blockSignals( true );

        foreach ( const QString& val, vals )
            value->addItem( val, val.toInt() );

        value->blockSignals( false );
    }

    int idx( value->findData( val ) );

    value->blockSignals( true );

    if ( 0 <= idx )
        value->setCurrentIndex( idx );
    else
        value->setCurrentIndex( 0 );

    value->blockSignals( false );

    updateFieldBackground( value, false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::updateLabel( int config, int configLen, const QString &text, const QString& val )
{
    const QByteArray a( config_[config] );

    QLabel *valueLabel;
    QLabel *value;

    if ( configLen != a.length() )
        return;

    fieldParentMap *parents( nullptr );

    if (( 100 <= version_ ) && ( version_ <= 199 ))
        parents = &configExpected_v1_;

    if ( !parents )
        return;

    createField( config, parents->value( config ), &valueLabel, &value, text );

    value->blockSignals( true );
    value->setText( val );
    value->blockSignals( false );

    updateFieldBackground( value, false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::updateLineEdit( int config, int configLen, const QString &text, const QString& val )
{
    const QByteArray a( config_[config] );

    QLabel *valueLabel;
    QLineEdit *value;

    if ( configLen != a.length() )
        return;

    fieldParentMap *parents( nullptr );

    if (( 100 <= version_ ) && ( version_ <= 199 ))
        parents = &configExpected_v1_;

    if ( !parents )
        return;

    if ( createField( config, parents->value( config ), &valueLabel, &value, text ) )
        value->setMaxLength( a.size() );

    value->blockSignals( true );
    value->setText( val );
    value->blockSignals( false );

    updateFieldBackground( value, false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configEditorWidget::updateSpinBox( int config, int configLen, const QString &text, int val, int minVal, int maxVal )
{
    const QByteArray a( config_[config] );

    QLabel *valueLabel;
    QSpinBox *value;

    if ( configLen != a.length() )
        return;

    fieldParentMap *parents( nullptr );

    if (( 100 <= version_ ) && ( version_ <= 199 ))
        parents = &configExpected_v1_;

    if ( !parents )
        return;

    if ( createField( config, parents->value( config ), &valueLabel, &value, text ) )
    {
        value->blockSignals( true );
        value->setMinimum( minVal );
        value->setMaximum( maxVal );
        value->blockSignals( false );
    }

    value->blockSignals( true );
    value->setValue( val );
    value->blockSignals( false );

    updateFieldBackground( value, false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString configEditorWidget::toHexString( const QByteArray &a )
{
    QString result;

    for ( int i( a.size() ); i--; )
    {
        result = QString::number( a[i], 16 ) + result;

        while ( 0 != (result.length() % 2 ) )
            result = "0" + result;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int configEditorWidget::toInt8( const QByteArray &a )
{
    if ( 1 != a.length() )
        return 0;

    return (signed char) a[0];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int configEditorWidget::toUInt8( const QByteArray &a )
{
    unsigned int result( toInt8( a ) );
    result <<= 24;
    result >>= 24;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int configEditorWidget::toInt16( const QByteArray &a )
{
    int result( 0 );

    if ( 2 == a.length() )
    {
        result |= (unsigned char) a[0];
        result <<= 8;
        result |= (unsigned char) a[1];
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int configEditorWidget::toUInt16( const QByteArray &a )
{
    unsigned int result( toInt16( a ) );
    result <<= 16;
    result >>= 16;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int configEditorWidget::toInt32( const QByteArray &a )
{
    int result( 0 );

    if ( 4 == a.length() )
    {
        result |= (unsigned char) a[0];
        result <<= 8;
        result |= (unsigned char) a[1];
        result <<= 8;
        result |= (unsigned char) a[2];
        result <<= 8;
        result |= (unsigned char) a[3];
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int configEditorWidget::toUInt32( const QByteArray &a )
{
    unsigned int result( 0 );

    if ( 4 == a.length() )
    {
        result |= (unsigned char) a[0];
        result <<= 8;
        result |= (unsigned char) a[1];
        result <<= 8;
        result |= (unsigned char) a[2];
        result <<= 8;
        result |= (unsigned char) a[3];
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool configEditorWidget::validateFirmwareImage( const QString &filename, int &version )
{
    std::string image;
    int crc;

    // empty string is not valid
    if ( filename.isEmpty() )
        return false;

    // validate file
    return nordicDeviceFirmware::validate( qPrintable( filename ), image, version, crc );
}
