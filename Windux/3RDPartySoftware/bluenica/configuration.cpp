/**
 * @file configuration.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "configuration.h"
#include "dbtables.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
configurationWidget::configurationWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
configurationWidget::~configurationWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::load()
{
    tempUnitOfMeasure_->setCurrentIndex( db_.queryConfigValue( CONFIG_TEMP_UNIT_OF_MEASURE ).toInt() );
    maxTempAge_->setText( db_.queryConfigValue( CONFIG_MAX_TEMP_AGE ) );
    batteryNormalFrom_->setText( db_.queryConfigValue( CONFIG_BATTERY_NORMAL_FROM ) );
    batteryWarningFrom_->setText( db_.queryConfigValue( CONFIG_BATTERY_WARNING_FROM ) );
    commNormalFrom_->setText( db_.queryConfigValue( CONFIG_COMM_NORMAL_FROM ) );
    commWarningFrom_->setText( db_.queryConfigValue( CONFIG_COMM_WARNING_FROM ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool configurationWidget::save()
{
    const unsigned int batteryNormalFrom( batteryNormalFrom_->text().toUInt() );
    const unsigned int batteryWarningFrom( batteryWarningFrom_->text().toUInt() );

    const unsigned int commNormalFrom( commNormalFrom_->text().toUInt() );
    const unsigned int commWarningFrom( commWarningFrom_->text().toUInt() );

    // validate
    if (( !validateInput( maxTempAge_ ) ) ||
        ( !validateInput( batteryNormalFrom_ ) ) ||
        ( !validateInput( batteryWarningFrom_ ) ) ||
        ( !validateInput( commNormalFrom_ ) ) ||
        ( !validateInput( commWarningFrom_ ) ) ||
        ( batteryNormalFrom <= batteryWarningFrom ) ||
        ( commWarningFrom <= commNormalFrom ))
    {
        QMessageBox::StandardButton reply(
            QMessageBox::warning(
                this,
                tr( "Invalid Configuration" ),
                tr( "One or more configuration values are invalid.\n\nDo you wish to exit anyway?" ),
                QMessageBox::Yes | QMessageBox::No ) );

        // stay here
        return (QMessageBox::Yes == reply);
    }

    // save!
    db_.updateConfigValue( CONFIG_TEMP_UNIT_OF_MEASURE, QString::number( tempUnitOfMeasure_->currentIndex() ) );
    db_.updateConfigValue( CONFIG_MAX_TEMP_AGE, maxTempAge_->text() );
    db_.updateConfigValue( CONFIG_BATTERY_NORMAL_FROM, batteryNormalFrom_->text() );
    db_.updateConfigValue( CONFIG_BATTERY_WARNING_FROM, batteryWarningFrom_->text() );
    db_.updateConfigValue( CONFIG_COMM_NORMAL_FROM, commNormalFrom_->text() );
    db_.updateConfigValue( CONFIG_COMM_WARNING_FROM, commWarningFrom_->text() );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::translate()
{
    tempUnitOfMeasureLabel_->setText( tr( "Temperature type:" ) );
    maxTempAgeLabel_->setText( tr( "Maximum data age (days):" ) );
    batteryNormalFromLabel_->setText( tr( "Battery normal level (percent):" ) );
    batteryWarningFromLabel_->setText( tr( "Battery warning level (percent):" ) );
    commNormalFromLabel_->setText( tr( "Offline sensor normal level (minutes):" ) );
    commWarningFromLabel_->setText( tr( "Offline sensor warning level (minutes):" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::initialize()
{
    static const QChar degree( 0260 ); // degree character in octal

    tempUnitOfMeasureLabel_ = new  QLabel( this );
    tempUnitOfMeasureLabel_->setObjectName( "tempUnitOfMeasureLabel" );

    tempUnitOfMeasure_ = new QComboBox( this );
    tempUnitOfMeasure_->setEditable( false );
    tempUnitOfMeasure_->setObjectName( "tempUnitOfMeasure" );

    tempUnitOfMeasure_->addItem( QString( "%0%1" ).arg( degree ).arg( "F" ), 0 );
    tempUnitOfMeasure_->addItem( QString( "%0%1" ).arg( degree ).arg( "C" ), 1 );

    maxTempAgeLabel_ = new QLabel( this );
    maxTempAgeLabel_->setObjectName( "maxTempAgeLabel" );

    maxTempAge_ = new QLineEdit( this );
    maxTempAge_->setObjectName( "maxTempAge" );
    maxTempAge_->setValidator( new QIntValidator( 14, 999, this ) );

    batteryNormalFromLabel_ = new QLabel( this );
    batteryNormalFromLabel_->setObjectName( "batteryNormalFromLabel" );

    batteryNormalFrom_ = new QLineEdit( this );
    batteryNormalFrom_->setObjectName( "batteryNormalFrom" );
    batteryNormalFrom_->setValidator( new QIntValidator( 1, 100, this ) );

    batteryWarningFromLabel_ = new QLabel( this );
    batteryWarningFromLabel_->setObjectName( "batteryWarningFromLabel" );

    batteryWarningFrom_ = new QLineEdit( this );
    batteryWarningFrom_->setObjectName( "batteryWarningFrom" );
    batteryWarningFrom_->setValidator( new QIntValidator( 1, 100, this ) );

    commNormalFromLabel_ = new QLabel( this );
    commNormalFromLabel_->setObjectName( "commNormalFromLabel" );

    commNormalFrom_ = new QLineEdit( this );
    commNormalFrom_->setObjectName( "commNormalFrom" );
    commNormalFrom_->setValidator( new QIntValidator( 20, 120, this ) );

    commWarningFromLabel_ = new QLabel( this );
    commWarningFromLabel_->setObjectName( "commWarningFromLabel" );

    commWarningFrom_ = new QLineEdit( this );
    commWarningFrom_->setObjectName( "commWarningFrom" );
    commWarningFrom_->setValidator( new QIntValidator( 20, 240, this ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void configurationWidget::createLayout()
{
    QFormLayout *configLayoutInner( new QFormLayout() );
    configLayoutInner->addRow( tempUnitOfMeasureLabel_, tempUnitOfMeasure_ );
    configLayoutInner->addItem( new QSpacerItem( 0, 16 ) );
    configLayoutInner->addRow( maxTempAgeLabel_, maxTempAge_ );
    configLayoutInner->addItem( new QSpacerItem( 0, 16 ) );
    configLayoutInner->addRow( batteryNormalFromLabel_, batteryNormalFrom_ );
    configLayoutInner->addRow( batteryWarningFromLabel_, batteryWarningFrom_ );
    configLayoutInner->addItem( new QSpacerItem( 0, 16 ) );
    configLayoutInner->addRow( commNormalFromLabel_, commNormalFrom_ );
    configLayoutInner->addRow( commWarningFromLabel_, commWarningFrom_ );

    QHBoxLayout *configLayout( new QHBoxLayout() );
    configLayout->addStretch();
    configLayout->addLayout( configLayoutInner );
    configLayout->addStretch();

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addStretch();
    layout->addLayout( configLayout );
    layout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool configurationWidget::validateInput( const QLineEdit *w )
{
    const QValidator *v( w->validator() );

    if ( !v )
        return true;

    QString input( w->text() );
    int pos( 0 );

    return (QValidator::Acceptable == v->validate( input, pos ));
}
