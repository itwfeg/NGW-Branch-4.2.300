/**
 * @file messageboxdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "dcadialog.h"
#include "messageboxdialog.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include "../modes/modewidget.h"

#include "../screens/plumainscreen.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

const bool MessageBoxDialog::convertUpperCase_ = true;

///////////////////////////////////////////////////////////////////////////////////////////////////
MessageBoxDialog::MessageBoxDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    allowOverlayClickDismiss_( true ),
    blockDialogs_( false ),
    priority_( Normal ),
    sizeToFit_( false )
{
    setObjectName( "MessageBoxDialog" );

    // initialize interface
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MessageBoxDialog::MessageBoxDialog( real_time_priority_t, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    allowOverlayClickDismiss_( true ),
    blockDialogs_( false ),
    priority_( RealTime ),
    sizeToFit_( false )
{
    setObjectName( "MessageBoxDialog" );

    // initialize interface
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MessageBoxDialog::~MessageBoxDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QAbstractButton *MessageBoxDialog::button( QDialogButtonBox::StandardButton role ) const
{
    return buttons_->button( role );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap MessageBoxDialog::icon() const
{
    return iconPixmap_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString MessageBoxDialog::text() const
{
    return text_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int MessageBoxDialog::textFontPointSize() const
{
    QFont f( text_->font() );

    return f.pointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString MessageBoxDialog::title() const
{
    return title_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int MessageBoxDialog::titleFontPointSize() const
{
    QFont f( title_->font() );

    return f.pointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setBlockDialogs( bool value )
{
    // already blocking
    if ( blockDialogs() )
        return;

    blockDialogs_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setButtonRole( QAbstractButton *button, QDialogButtonBox::ButtonRole role )
{
    buttons_->removeButton( button );
    buttons_->addButton( button, role );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setIcon( const QPixmap& value )
{
    modifyPaletteBackground setBackground( iconPixmap_ = value );
    setBackground( icon_ );

    icon_->setAutoFillBackground( true );
    icon_->setFixedSize( iconPixmap_.size() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setText( const QString& value )
{
    text_->setText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setTextFontPointSize( int value )
{
    QFont f( text_->font() );
    f.setPointSize( value );

    text_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setTitle( const QString& value )
{
    title_->setText( convertUpperCase_ ? value.toUpper() : value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setTitleFontPointSize( int value )
{
    QFont f( title_->font() );
    f.setPointSize( value );

    title_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDialogButtonBox::StandardButtons MessageBoxDialog::standardButtons() const
{
    return buttons_->standardButtons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setStandardButtons( QDialogButtonBox::StandardButtons value )
{
    LOG_CLASS_INFO( "MessageBoxDialog", "setStandardButtons" );

    buttons_->setStandardButtons( value );

    // log out buttons and roles
    QList<QAbstractButton*> buttons( buttons_->buttons() );

    foreach ( QAbstractButton *b, buttons )
    {
        QDialogButtonBox::ButtonRole role( buttonRole( b ) );

        LOG_INFO( "button 0x%08x with role %d and text '%s'",
            b, role, qPrintable( b->text() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize MessageBoxDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 640) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 300) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::hideEvent( QHideEvent *e )
{
    // restart dialogs
    if ( blockDialogs() )
        session_->send( SessionDev, SnRestartDialogs );

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::showEvent( QShowEvent *e )
{
    // stop dialogs
    if ( blockDialogs() )
        session_->send( SessionDev, SnStopDialogs );

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::onButtonClicked( QAbstractButton *button )
{
    LOG_CLASS_INFO( "MessageBoxDialog", "onButtonClicked" );

    if ( !button )
        return;

    QDialogButtonBox::ButtonRole role( buttonRole( button ) );

    LOG_INFO( "button 0x%08x pressed with role %d and text '%s'",
        button, role, qPrintable( button->text() ) );

    if (( QDialogButtonBox::RejectRole == role ) || ( QDialogButtonBox::NoRole == role ))
        reject();
    else if (( QDialogButtonBox::AcceptRole == role ) || ( QDialogButtonBox::YesRole == role ))
        accept();
    else
    {
        done( role );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // when size to fit enabled, scale down text font size to fit window
    if ( sizeToFit() )
    {
        QFont f( text_->font() );

        // go from large font to smaller font
        for ( int ps( sizeToFitFontPointSize_ ); ps >= 9; --ps )
        {
            // set font point size
            f.setPointSize( ps );
            text_->setFont( f );

            // compute bounding rect
            QRect rect( text_->rect() );
            rect.setHeight( INT_MAX );

            QFontMetrics metrics( text_->font() );
            QRect boundingRect( metrics.boundingRect( rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, text_->text() ) );

            // check for fit, break when okay
            if ( boundingRect.height() <= text_->height() )
                break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDialogButtonBox::ButtonRole MessageBoxDialog::buttonRole( QAbstractButton *button ) const
{
    return buttons_->buttonRole( button );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<QAbstractButton*>	MessageBoxDialog::buttons() const
{
    return buttons_->buttons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::setForegroundPalette( const QColor& color )
{
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( color ) );

    setForeground( title_ );
    setForeground( text_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *MessageBoxDialog::addButton( QDialogButtonBox::StandardButton button )
{
    QPushButton *b( buttons_->addButton( button ) );
    groomPushButton( b ); // FT-595

    return b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *MessageBoxDialog::addButton( const QString& text, QDialogButtonBox::ButtonRole role )
{
    QPushButton *b( buttons_->addButton( text, role ) );
    groomPushButton( b ); // FT-595

    return b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::onButtonClickedInternal( QAbstractButton *button )
{
    // call handler
    onButtonClicked( button );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::initialize()
{
    modifyFontPointSize setFontPointSize18( (mainWindow_->height() * 18) / SessionMainWindow::scalingDividerY_ );

    // icon
    icon_ = new QWidget( this );
    icon_->setObjectName( "icon" );

    // title
    title_ = new QLabel( this );
    title_->setObjectName( "title" );
    setFontPointSize18( title_ );

    // text
    text_ = new QLabel( this );
    text_->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    text_->setObjectName( "text" );
    text_->setWordWrap( true );

    const QFont textFont( text_->font() );
    sizeToFitFontPointSize_ = textFont.pointSize();

    // buttons
    buttons_ = new QDialogButtonBox( this );
    buttons_->setObjectName( "buttons" );

    connect( buttons_, SIGNAL( clicked(QAbstractButton*) ), SLOT( onButtonClickedInternal(QAbstractButton*) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::createLayout()
{
    messageAreaInner_ = new QVBoxLayout();
    messageAreaInner_->setContentsMargins( QMargins() );
    messageAreaInner_->addWidget( text_ );

    messageArea_ = new QHBoxLayout();
    messageArea_->setContentsMargins( QMargins() );
    messageArea_->addLayout( messageAreaInner_ );

    QHBoxLayout *hlayout( new QHBoxLayout() );
    hlayout->addStretch();
    hlayout->addWidget( icon_ );
    hlayout->addWidget( title_ );
    hlayout->addStretch();

    // set layout
    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->addLayout( hlayout );
    screenLayout->addSpacing( 16 );
    screenLayout->addLayout( messageArea_, 1 );

    buttonsArea_ = new QHBoxLayout();
    buttonsArea_->setContentsMargins( QMargins() );

    buttonsArea_->addWidget( buttons_ );
    screenLayout->addLayout( buttonsArea_ );

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );
    setForeground( buttons_ );

    connect( buttons_, SIGNAL( clicked(QAbstractButton*) ), SLOT( onButtonClickedInternal(QAbstractButton*) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MessageBoxDialog::groomPushButton( QPushButton *button )
{
    if ( !button )
        return;

    // FT-595 set button properties
    button->setAutoDefault( false );
    button->setDefault( false );
    button->setFocusPolicy( Qt::NoFocus );
}
