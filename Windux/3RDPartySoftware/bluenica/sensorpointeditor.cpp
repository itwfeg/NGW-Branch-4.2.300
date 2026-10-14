/**
 * @file sensorpointeditor.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "dbtables.h"
#include "sensorpointeditor.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorPointEditorWidget::sensorPointEditorWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sensorPointEditorWidget::~sensorPointEditorWidget()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointEditorWidget::insert()
{
    // reset record
    record_.id = 0;
    record_.description.clear();
    record_.profile = 0;
    record_.sensorAddress.clear();
    record_.generateAlerts = false;

    // refresh
    refresh();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointEditorWidget::load( unsigned int id )
{
    // load sensor info
    record_ = db_.querySensorPoint( id );

    // refresh
    refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool sensorPointEditorWidget::remove()
{
    QMessageBox::StandardButton reply(
        QMessageBox::warning(
            this,
            tr( "Confirm Delete" ),
            tr( "This action will remove the sensor point and ALL DATA associated with the sensor point.\n\nAre you sure you want to delete?" ),
            QMessageBox::Yes | QMessageBox::No ) );

    // they chickened out... abort
    if ( QMessageBox::No == reply )
        return false;

    // boom!
    db_.deleteSensorPoint( record_.id );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool sensorPointEditorWidget::save( bool isNewRecord )
{
    // update record
    record_.description = description_->text();
    record_.profile = profile_->currentData().toUInt();
    record_.sensorAddress = sensorAddress_->currentData().toString();
    record_.generateAlerts = generateAlerts_->isChecked();

    // insert!!
    if ( isNewRecord )
        db_.insertSensorPoint( record_ );

    // save!!
    else
        db_.updateSensorPoint( record_ );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointEditorWidget::translate()
{
    descriptionLabel_->setText( tr( "Description:" ) );
    profileLabel_->setText( tr( "Temperature Profile:" ) );
    sensorAddressLabel_->setText( tr( "Sensor Address:" ) );
    generateAlertsLabel_->setText( tr( "Generate Alerts:" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointEditorWidget::resizeEvent( QResizeEvent *e )
{
    description_->setFixedWidth( (50 * width()) / 100 );
    profile_->setFixedWidth( (30 * width()) / 100 );
    sensorAddress_->setFixedWidth( (30 * width()) / 100 );

    _Mybase::resizeEvent( e );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointEditorWidget::refresh()
{
    int index;

    // populate temp profiles
    QList<profileInfo> profiles( db_.queryProfiles() );

    profile_->clear();

    foreach ( const profileInfo &profile, profiles )
        profile_->addItem( profile.description, QVariant( profile.id ) );

    // populate sensors
    QList<sensorInfo> sensors( db_.querySensors() );

    sensorAddress_->clear();
    sensorAddress_->addItem( tr( "None" ), QVariant( QString() ) );

    foreach ( const sensorInfo &sensor, sensors )
        if (( sensor.address == record_.sensorAddress ) || ( !db_.isSensorUsed( sensor.address ) ))
            sensorAddress_->addItem( sensor.address, QVariant( sensor.address ) );

    //
    // populate form
    //

    description_->setText( record_.description );

    if ( -1 == (index = profile_->findData( QVariant( record_.profile ) )) )
        index = 0;

    profile_->setCurrentIndex( index );

    if ( -1 == (index = sensorAddress_->findData( QVariant( record_.sensorAddress ) )) )
        index = 0;

    sensorAddress_->setCurrentIndex( index );

    generateAlerts_->setChecked( record_.generateAlerts );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointEditorWidget::initialize()
{
    descriptionLabel_ = new  QLabel( this );
    descriptionLabel_->setObjectName( "descriptionLabel" );

    description_ = new  QLineEdit( this );
    description_->setObjectName( "description" );

    profileLabel_ = new  QLabel( this );
    profileLabel_->setObjectName( "profileLabel" );

    profile_ = new QComboBox( this );
    profile_->setEditable( false );
    profile_->setObjectName( "profile" );

    sensorAddressLabel_ = new  QLabel( this );
    sensorAddressLabel_->setObjectName( "sensorAddressLabel" );

    sensorAddress_ = new QComboBox( this );
    sensorAddress_->setEditable( false );
    sensorAddress_->setObjectName( "sensorAddress" );

    generateAlertsLabel_ = new  QLabel( this );
    generateAlertsLabel_->setObjectName( "generateAlertsLabel" );

    generateAlerts_ = new QCheckBox( this );
    generateAlerts_->setObjectName( "generateAlerts" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sensorPointEditorWidget::createLayout()
{
    QFormLayout *configLayoutInner( new QFormLayout() );
    configLayoutInner->addRow( descriptionLabel_, description_ );
    configLayoutInner->addItem( new QSpacerItem( 0, 16 ) );
    configLayoutInner->addRow( profileLabel_, profile_ );
    configLayoutInner->addRow( sensorAddressLabel_, sensorAddress_ );
    configLayoutInner->addItem( new QSpacerItem( 0, 16 ) );
    configLayoutInner->addRow( generateAlertsLabel_, generateAlerts_ );

    QHBoxLayout *configLayout( new QHBoxLayout() );
    configLayout->addStretch();
    configLayout->addLayout( configLayoutInner );
    configLayout->addStretch();

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addStretch();
    layout->addLayout( configLayout );
    layout->addStretch();
}
