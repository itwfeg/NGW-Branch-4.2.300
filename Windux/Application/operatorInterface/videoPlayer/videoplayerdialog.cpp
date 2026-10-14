/**
 * @file videoplayerdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "videoplayerdialog.h"
#include "ui_videoplayerdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayerDialog::videoPlayerDialog( const QPoint& pos, QWidget *parent ) :
    QDialog( parent ),
    ui_( new Ui::videoPlayerDialog )
{
    // set player position
    if ( !pos.isNull() )
        move( pos );

    // setup designer ui
    ui_->setupUi( this );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayerDialog::~videoPlayerDialog()
{
    delete ui_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool videoPlayerDialog::active() const
{
    if ( videoWidget_ )
        return videoWidget_->active();

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerDialog::disableUserInterface()
{
    LOG_CLASS_INFO( "videoPlayerDialog", "disableUserInterface" );

    if ( videoWidget_ )
    {
        LOG_INFO( "disabling user interface", NULL );
        videoWidget_->disableUserInterface();

        // blacken the background since there is no user interface
        QPalette p( palette() );

        for ( int i = 0; i < QPalette::NColorRoles; ++i )
            p.setBrush( (QPalette::ColorRole) i, Qt::black );

        setPalette( p );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool videoPlayerDialog::play( const QString& filename )
{
    LOG_CLASS_INFO( "videoPlayerDialog", "play" );

    LOG_INFO( "playing file '%s'...", qPrintable( filename ) );

    // check for video widget
    if ( !videoWidget_ )
    {
        LOG_WARN( "no widget!", NULL );
        return false;
    }

    // set the window title
    setWindowTitle( "Video Player [" + filename + "]" );

    // play!
    return videoWidget_->play( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerDialog::onFinished()
{
    LOG_CLASS_INFO( "videoPlayerDialog", "onFinished" );

    LOG_INFO( "media has completed", NULL );
    close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerDialog::onHasVideoChanged( bool hasVideo )
{
    LOG_CLASS_INFO( "videoPlayerDialog", "onHasVideoChanged" );

    // show!
    if ( hasVideo )
    {
        LOG_INFO( "showing video", NULL );

        if (( videoWidget_ ) && ( videoWidget_->isUserInterfaceDisabled() ))
            showFullScreen();
        else
            show();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void videoPlayerDialog::initialize()
{
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 0 );

    // create video player widget
    videoWidget_ = new videoPlayerWidget( device(), this );
    layout->addWidget( videoWidget_ );

    connect( videoWidget_, SIGNAL( finished(bool) ), SLOT( onFinished() ) );
    connect( videoWidget_, SIGNAL( hasVideoChanged(bool) ), SLOT( onHasVideoChanged(bool) ) );

    // set the window title
    setWindowTitle( "Video Player" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
videoPlayerDevice::type videoPlayerDialog::device() const
{
    LOG_CLASS_INFO( "videoPlayerDialog", "device" );

    videoPlayerDevice::type result = videoPlayerDevice::Operator;

    QDesktopWidget *desktop = QApplication::desktop();

    // check for dual display
    if (( desktop ) &&( 1 < desktop->screenCount() ))
    {
        int screenCount = desktop->screenCount();
        int screenNumber = desktop->screenNumber( this );

        LOG_INFO( "detected multiple displays (%d):", screenCount );

        for ( int i = 0; i < screenCount; ++i )
        {
            QRect geometry( desktop->screenGeometry( i ) );

            LOG_INFO( "[screen %d left %d top %d width %d height %d]",
                i,
                geometry.left(),
                geometry.top(),
                geometry.width(),
                geometry.height() );
        }

        // multiple screens, on second screen
        if ( 1 == screenNumber )
        {
            LOG_INFO( "we are on customer screen", NULL );
            result = videoPlayerDevice::Customer;
        }

        // single screen, but is virtual
        else if ( desktop->isVirtualDesktop() )
        {
            QRect rect = desktop->screenGeometry( 0 );

            // check geometry is on second screen.
            if ( rect.width() <= geometry().left() )
            {
                LOG_INFO( "we are on customer screen coordinate system", NULL );
                result = videoPlayerDevice::Customer;
            }
        }
    }

    return result;
}
