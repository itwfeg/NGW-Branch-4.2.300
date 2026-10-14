/**
 * @file recallviewerdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */


#include "recalldialog.h"
#include "recallviewerdialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <logmanager.h>
#include <palette.h>
#include <pushbutton.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QSignalMapper>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
RecallViewerDialog::RecallViewerDialog( const QString& id, const QString& date, const QString& recallText, bool isNew, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    id_( id ),
    date_( date ),
    recallText_( recallText ),
    isNew_( isNew )
{
    // set object name
    setObjectName( "RecallViewerDialog" );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
RecallViewerDialog::~RecallViewerDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallViewerDialog::onButtonClicked( QWidget *button )
{
    LOG_CLASS_INFO( "RecallViewerDialog", "onButtonClicked" );

    // sanity check
    if ( !button )
        return;

    // dismiss button
    if ( dismiss_ == button )
    {
        LOG_DEBUG( "dismiss button was pressed", NULL );

        // TODO
        // send message to backend for recall dismiss
    }

    // show recall dialog
    RecallDialog *dialog = new RecallDialog( (QWidget*) mainWindow_ );
    dialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallViewerDialog::initialize()
{
    const QSize buttonSize(
        (mainWindow_->width() * 100) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 45) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    modifyFontPointSize setFontPointSize10( (mainWindow_->height() * 10) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    setPalette( QApplication::palette() );
    setFontPointSize12( this );

    // icon
    icon_ = new QWidget( this );
    icon_->setObjectName( "icon" );

    QPixmap warningIcon( ":/images/dialogs/warningIcon.png" );
    modifyPaletteBackground setWarningIconBackground( warningIcon );
    setWarningIconBackground( icon_ );

    icon_->setAutoFillBackground( true );
    icon_->setFixedSize( warningIcon.size() );

    // title
    title_ = new QLabel( date_, this );
    title_->setObjectName( "title" );

    // text
    text_ = new QPlainTextEdit( recallText_, this );
    text_->setFocusPolicy( Qt::NoFocus );
    text_->setObjectName( "text" );
    text_->setReadOnly( true );
    text_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // signal mapper
    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // dismiss button
    dismiss_ = new hobart::pushButton( tr( "Mark\nComplete" ), this, buttonPixmap );
    dismiss_->setFocusPolicy( Qt::NoFocus );
    dismiss_->setObjectName( "dismiss" );
    setFontPointSize10( dismiss_ );
    setForeground( dismiss_ );

    signalMapper_->setMapping( dismiss_, dismiss_ );
    connect( dismiss_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    // close button
    close_ = new hobart::pushButton( tr( "Cancel" ), this, buttonPixmap );
    close_->setFocusPolicy( Qt::NoFocus );
    close_->setObjectName( "close" );
    setFontPointSize10( close_ );
    setForeground( close_ );

    signalMapper_->setMapping( close_, close_ );
    connect( close_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    // for recalls that are *not* new they may only close the dialog
    if ( !isNew_ )
    {
        icon_->setVisible( false );
        dismiss_->setVisible( false );
        close_->setText( tr( "Close" ) );
    }

    // create layouts
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    QHBoxLayout *hlayout;

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( icon_ );
    hlayout->addWidget( title_ );
    hlayout->addStretch();
    screenLayout->addLayout( hlayout );

    screenLayout->addWidget( text_ );

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( dismiss_ );
    hlayout->addWidget( close_ );
    screenLayout->addLayout( hlayout );
}
