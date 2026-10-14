/**
 * @file settingsviewerwidget.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "settingsviewerwidget.h"

#include <QFile>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QStringList>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
settingsViewerWidget::settingsViewerWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString settingsViewerWidget::filename()
{
#if defined( Q_OS_LINUX )
    return "/usr/share/walmart-apphook/web_headers.config";
#else
    return "web_headers.config";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void settingsViewerWidget::onButtonClicked()
{
    if ( exit_ == sender() )
    {
        emit exitScreen();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void settingsViewerWidget::initialize()
{
    QFont f( font() );
    f.setPointSize( (f.pointSize() * 66) / 100 );

    QStringList keys;
    keys.append( "Scale_Type" );
    keys.append( "WM_CONSUMER.ID" );
    keys.append( "WM_SVC.NAME" );
    keys.append( "WM_SVC.VERSION" );
    keys.append( "WM_SVC.ENV" );
    keys.append( "" );
    keys.append( "WebServiceAddress" );
    keys.append( "GetOneItem" );
    keys.append( "GetAllItems" );
    keys.append( "HealthCheck" );
    keys.append( "HttpsActive" );
    keys.append( "" );
    keys.append( "CacheExpirationTime" );
    keys.append( "CacheExpirationUnits" );
    keys.append( "" );
    keys.append( "TimeoutLength" );
    keys.append( "AllItemsTimeoutLength" );
    keys.append( "TimeoutRetryInterval" );

    // settings
    QSettings s( filename(), QSettings::IniFormat );

    foreach ( const QString& key, keys )
    {
        QLabel *first( new QLabel( this ) );
        first->setFont( f );

        QLabel *second( new QLabel( this ) );
        second->setFont( f );

        if ( key.length() )
        {
            first->setText( key );
            second->setText( s.value( key ).toString() );
        }

        settings_.append( widgetPair( first, second ) );
    }

    // exit
    exit_ = new QPushButton( this );
    exit_->setMinimumHeight( 64 );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setObjectName( "exit" );
    exit_->setText( tr( "Back" ) );
    connect( exit_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void settingsViewerWidget::createLayout()
{
    QFormLayout *form( new QFormLayout );

    foreach ( const widgetPair& setting, settings_ )
        form->addRow( setting.first, setting.second );

    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addStretch();
    buttons->addWidget( exit_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );

    layout->addLayout( form, 1 );
    layout->addLayout( buttons );
}
