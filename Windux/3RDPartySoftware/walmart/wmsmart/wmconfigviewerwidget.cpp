/**
 * @file wmconfigviewerwidget.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "wmconfigviewerwidget.h"

#include <QFile>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QStringList>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartConfigViewerWidget::walmartConfigViewerWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString walmartConfigViewerWidget::hobartConfFilename()
{
#if defined( Q_OS_LINUX )
    return "/usr/share/walmart-apphook/hobart_internal.config";
#else
    return "hobart.config";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString walmartConfigViewerWidget::walmartConfFilename()
{
#if defined( Q_OS_LINUX )
    return "/usr/share/walmart-apphook/walmart.config";
#else
    return "walmart.config";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartConfigViewerWidget::onButtonClicked()
{
    if ( exit_ == sender() )
    {
        emit exitScreen();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartConfigViewerWidget::initialize()
{
    QFont f( font() );
    f.setPointSize( (f.pointSize() * 66) / 100 );

    QStringList keys;
    keys.append( "InitUrl" );

    // settings
    QSettings s( hobartConfFilename(), QSettings::IniFormat );

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

    // walmart config
    if ( !QFile::exists( walmartConfFilename() ) )
        wmconfig_ = NULL;
    else
    {
        QFile conf( walmartConfFilename() );

        if ( conf.open( QFile::ReadOnly ) )
        {
            QJsonParseError e;

            QJsonDocument doc( QJsonDocument::fromJson( conf.readAll(), &e ) );

            if ( QJsonParseError::NoError == e.error )
            {
                wmconfig_ = new QPlainTextEdit( this );
                wmconfig_->setFont( f );
                wmconfig_->setPlainText( doc.toJson( QJsonDocument::Indented ) );
                wmconfig_->setReadOnly( true );

                QList<QPalette::ColorGroup> groups;
                groups.append( QPalette::Active );
                groups.append( QPalette::Disabled );
                groups.append( QPalette::Inactive );

                QList<QPalette::ColorRole> roles;
                roles.append( QPalette::WindowText );
                roles.append( QPalette::Text );
                roles.append( QPalette::ButtonText );

                QPalette p( wmconfig_->palette() );

                foreach ( QPalette::ColorGroup group, groups )
                    foreach ( QPalette::ColorRole role, roles )
                        p.setColor( group, role, Qt::black );

                wmconfig_->setPalette( p );
            }

            conf.close();
        }
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
void walmartConfigViewerWidget::createLayout()
{
    QFormLayout *form( new QFormLayout );

    foreach ( const widgetPair& setting, settings_ )
        form->addRow( setting.first, setting.second );

    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addStretch();
    buttons->addWidget( exit_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );

    if ( wmconfig_ )
    {
        layout->addLayout( form );
        layout->addWidget( wmconfig_, 1 );
    }
    else
    {
        layout->addLayout( form, 1 );
    }

    layout->addLayout( buttons );
}
