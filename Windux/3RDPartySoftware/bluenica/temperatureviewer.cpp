/**
 * @file temperatureviewer.h
 * Widget for viewing sensor temp data.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "dbtables.h"
#include "temperaturegraph.h"
#include "temperatureviewer.h"

#include <QDateTime>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
temperatureViewerWidget::temperatureViewerWidget( QWidget *parent ) :
    _Mybase( parent ),
    sid_( 0 ),
    spid_( 0 ),
    filterBottom_( 0 ),
    filterTop_( 0 ),
    useCelsius_( false )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
temperatureViewerWidget::~temperatureViewerWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureViewerWidget::setSensorFilter( unsigned int bottom, unsigned int top )
{
    if (( bottom == filterBottom_ ) && ( top == filterTop_ ))
        return;

    refresh( sid_, spid_, bottom, top );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureViewerWidget::setSensorPointID( unsigned int value )
{
    // retrieve sensor point
    sensorPointInfo sensorPoint( db_.querySensorPoint( value ) );
    sensorInfo sensor( db_.querySensor( sensorPoint.sensorAddress ) );

    // setup ui
    description_->setText( sensorPoint.description );
    sensor_->setText( sensorPoint.sensorAddress );

    descriptionLabel_->show();
    description_->show();

    // use default filter
    refresh( sensor.id, value, 0, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureViewerWidget::setSensorID( unsigned int value )
{
    // retrieve sensor
    sensorInfo sensor( db_.querySensor( value ) );

    // setup ui
    description_->clear();
    sensor_->setText( sensor.address );

    descriptionLabel_->hide();
    description_->hide();

    // use default filter
    refresh( value, 0, 0, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureViewerWidget::translate()
{
    descriptionLabel_->setText( tr( "Description:" ) );
    sensorLabel_->setText( tr( "Sensor ID:" ) );
    updatedLabel_->setText( tr( "Updated:" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureViewerWidget::refresh( unsigned int sid, unsigned int spid, unsigned int filterBottom, unsigned int filterTop )
{
    const unsigned int tempUnitOfMeasure( db_.queryConfigValue( CONFIG_TEMP_UNIT_OF_MEASURE ).toUInt() );

    const qint64 now( QDateTime::currentMSecsSinceEpoch() / 1000 );

    // set temp uom
    useCelsius_ = (TEMP_UOM_CELSIUS == tempUnitOfMeasure);

    // default to 12h ago
    if ( !filterBottom )
        filterBottom = now - (12 * 60 * 60);

    // default top to now
    if ( !filterTop )
        filterTop = now;

    // query sensor info
    if ( sid <= 0 )
        updated_->clear();
    else
    {
        sensorInfo sensor( db_.querySensor( sid ) );

        // compute sensor age
        QDateTime stamp;
        stamp.setMSecsSinceEpoch( (qint64) sensor.lastComm * 1000 );

        updated_->setText( stamp.toString() + " (" + sensorDataAge( now - sensor.lastComm ) + ")" );
    }

    // query sensor temp info
    QList<tempInfo> temps( db_.queryTemps( sid, spid, filterBottom, filterTop ) );

    if ( temps.isEmpty() )
        temp_->setText( "---" );
    else
        temp_->setText( formatDisplayTemp( temps.last().temperature ) );

    // update graph
    profileInfo profile;

    if ( spid <= 0 )
        profile.tempWarningFrom = profile.tempNormalFrom = profile.tempNormalTo = profile.tempWarningTo = profile.tempStandard = 0.0;
    else
    {
        sensorPointInfo sensorPoint( db_.querySensorPoint( spid ) );

        profile = db_.queryProfile( sensorPoint.profile );
    }

    graph_->setProfile( profile );
    graph_->setUseCelsius( useCelsius_ );
    graph_->refresh( filterBottom, filterTop, temps );

    // ---- //

    // save off graph details
    sid_ = sid;
    spid_ = spid;
    filterBottom_ = filterBottom;
    filterTop_ = filterTop;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureViewerWidget::initialize()
{
    QFont f;

    descriptionLabel_ = new  QLabel( this );
    descriptionLabel_->setObjectName( "descriptionLabel" );

    description_ = new  QLabel( this );
    description_->setObjectName( "description" );

    sensorLabel_ = new  QLabel( this );
    sensorLabel_->setObjectName( "sensorLabel" );

    sensor_ = new  QLabel( this );
    sensor_->setObjectName( "sensor" );

    updatedLabel_ = new  QLabel( this );
    updatedLabel_->setObjectName( "updatedLabel" );

    updated_ = new  QLabel( this );
    updated_->setObjectName( "updated" );

    temp_ = new  QLabel( this );
    temp_->setObjectName( "temp" );

    f = temp_->font();
    f.setPointSizeF( f.pointSizeF() * 2.0 );

    temp_->setFont( f );

    graph_ = new temperatureGraphWidget( this );
    graph_->setObjectName( "graph" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureViewerWidget::createLayout()
{
    QFormLayout *infoLayout( new QFormLayout() );
    infoLayout->addRow( descriptionLabel_, description_ );
    infoLayout->addRow( sensorLabel_, sensor_ );
    infoLayout->addRow( updatedLabel_, updated_ );

    QHBoxLayout *header( new QHBoxLayout() );
    header->addLayout( infoLayout, 1 );
    header->addWidget( temp_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addLayout( header );
    layout->addWidget( graph_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qreal temperatureViewerWidget::convertTemp( qreal temp ) const
{
    if ( useCelsius_ )
        return temp;

    temp *= 9.0;
    temp /= 5.0;
    temp += 32.0;

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString temperatureViewerWidget::formatDisplayTemp( qreal temp ) const
{
    static const QChar degree( 0260 ); // degree character in octal

    return QString( "%0 %1%2").arg( convertTemp( temp ), 0, 'f', 1 ).arg( degree ).arg( useCelsius_ ? "C" : "F" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString temperatureViewerWidget::sensorDataAge( qint64 diff )
{
    QString dataAge;

    int days( diff / (24 * 60 * 60) );
    diff %= (24 * 60 * 60);

    int hours( diff / (60 * 60) );
    diff %= (60 * 60);

    int mins( diff / 60 );
    diff %= 60;

    int secs( diff );

    if ( days )
        dataAge = QString( "%0d %1h %2m %3s" ).arg( days ).arg( hours ).arg( mins ).arg( secs );
    else if ( hours )
        dataAge = QString( "%0h %1m %2s" ).arg( hours ).arg( mins ).arg( secs );
    else if ( mins )
        dataAge = QString( "%0m %1s" ).arg( mins ).arg( secs );
    else
        dataAge = QString( "%0s" ).arg( secs );

    return dataAge;
}
