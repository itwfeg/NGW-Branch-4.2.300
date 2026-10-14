/**
 * @file recalldialog.cpp
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

#include <Session.h>
#include <Messages.h>

#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSignalMapper>

const QSize RecallDialog::iconSize_( 168, 64 );
const QSize RecallDialog::iconSizeList_( 175, 64 );

///////////////////////////////////////////////////////////////////////////////////////////////////
RecallDialog::RecallDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f )
{
    setObjectName( "RecallDialog" );

    // initialize interface
    initialize();

    // request list of recalls
    requestList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
RecallDialog::~RecallDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallDialog::addItemsToList( const QStringList& dates, const QStringList& textList, const QStringList& wasReadFlags, const QStringList& cookies )
{
    LOG_CLASS_INFO( "RecallDialog", "addItemsToList" );

    // remove all existing items
    while ( list_->count() )
    {
        QListWidgetItem *doomed = list_->takeItem( 0 );

        if ( doomed )
            delete doomed;
    }

    recalls_.clear();

    // validate list sizes
    if ( dates.count() != textList.count() )
        LOG_WARN( "mismatch in list sizes %d %d", dates.count(), textList.count() );

    if ( dates.count() != wasReadFlags.count() )
        LOG_WARN( "mismatch in list sizes %d %d", dates.count(), wasReadFlags.count() );

    if ( dates.count() != cookies.count() )
        LOG_WARN( "mismatch in list sizes %d %d", dates.count(), cookies.count() );

    // compute list size
    int count = dates.count();
    count = qMin( count, textList.count() );
    count = qMin( count, wasReadFlags.count() );
    count = qMin( count, cookies.count() );

    LOG_DEBUG( "processing %d items", count );

    for ( int i = 0; i < count; ++i )
    {
        bool isNew = ("0" == wasReadFlags[i]);

        addItemToList( dates[i], textList[i], isNew, cookies[i] );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallDialog::addItemToList( const QString& date, const QString& text, bool isNew, const QVariant& cookie )
{
    QPalette p( list_->palette() );

    // draw the thumbnail
    QPixmap thumbnail( iconSize_ );
    thumbnail.fill( Qt::white );

    QPainter painter( &thumbnail );
    painter.setFont( list_->font() );
    painter.setPen( p.color( list_->foregroundRole() ) );
    painter.setRenderHint( QPainter::Antialiasing, true );

    QRect dateRect( QPoint( 0, 0 ), iconSize_ );

    // for new recalls we must display the alert icon
    if ( isNew )
    {
        QPixmap warningIcon( ":/images/dialogs/warningIcon.png" );
        QRect warningIconRect( dateRect );

        // warning icon on top
        warningIconRect.setBottom( iconSize_.height() / 2 );

        // date text on bottom
        dateRect.setTop( iconSize_.height() / 2 );

        const QSize sourceSize( warningIcon.size() );

        // scale source
        QSize scaledSourceSize( sourceSize );
        scaledSourceSize.scale( warningIconRect.size(), Qt::KeepAspectRatio );

        QRect rect(
            (warningIconRect.width() - scaledSourceSize.width()) / 2,
            (warningIconRect.height() - scaledSourceSize.height()) / 2,
            scaledSourceSize.width(),
            scaledSourceSize.height() );

        // draw pixmap
        painter.drawPixmap( rect, warningIcon );
    }

    painter.drawText( dateRect, Qt::AlignCenter, date );

    _Mybase::addItemToList( thumbnail, text, cookie );

    // add item to internal list
    recallItem r;
    r.date_ = date;
    r.text_ = text;
    r.isNew_ = isNew;

    recalls_.insert( cookie.toString(), r );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallDialog::requestList()
{
    // request list of recalls
    session_->send( RecallsDev, RecallsDevReadAll );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "RecallDialog", "onSessionMessageReceive" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // populate list
    if (( RecallsDev == device ) && ( RecallsDevWriteAll == event ))
    {
        DomRecalls m( msg );

        QStringList ids( m.field<QStringList>( m.ids() ) );
        QStringList dates( m.field<QStringList>( m.dates() ) );
        QStringList texts( m.field<QStringList>( m.texts() ) );
        QStringList wasReadFlags( m.field<QStringList>( m.wasReadFlags() ) );

        if ( ids.count() != m.numIds() )
            LOG_WARN( "mismatch in ids %d %d", ids.count(), m.numIds() );

        if ( dates.count() != m.numDates() )
            LOG_WARN( "mismatch in dates %d %d", dates.count(), m.numDates() );

        if ( texts.count() != m.numTexts() )
            LOG_WARN( "mismatch in texts %d %d", texts.count(), m.numTexts() );

        if ( wasReadFlags.count() != m.numWasReadFlags() )
            LOG_WARN( "mismatch in read flags %d %d", wasReadFlags.count(), m.numWasReadFlags() );

        addItemsToList( dates, texts, wasReadFlags, ids );
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallDialog::onButtonClicked( QWidget *button )
{
    LOG_CLASS_INFO( "RecallDialog", "onButtonClicked" );

    // sanity check
    if ( !button )
        return;

    QList<QListWidgetItem*> selected( list_->selectedItems() );

    LOG_TRACE( "have %d items selected", selected.count() );

    // dismiss button
    if ( dismiss_ == button )
    {
        LOG_DEBUG( "dismiss button was pressed", NULL );

        // dismiss items
        if ( selected.count() )
        {
            // mark selected items as read
            for ( int i = 0; i < selected.count(); ++i )
            {
                QString id( selected[i]->data( Qt::UserRole ).toString() );

                LOG_INFO( "marking read recall '%s'", qPrintable( id ) );

                // mark read
                session_->send( RecallsDev, RecallsDevMarkRead, id );

                // mark internal recall item read also
                recallItem r( recalls_[id] );
                r.isNew_ = false;

                recalls_[id] = r;
            }
        }
    }

    // delete button
    else if ( delete_ == button )
    {
        LOG_DEBUG( "delete button was pressed", NULL );

        // remove selected items from list
        if ( selected.count() )
        {
            // remove selected items from list
            for ( int i = 0; i < selected.count(); ++i )
            {
                QString id( selected[i]->data( Qt::UserRole ).toString() );

                LOG_INFO( "deleting recall '%s'", qPrintable( id ) );

                // delete the recall
                session_->send( RecallsDev, RecallsDevDelete, id );
            }

            // update recall list
            requestList();
        }
    }

    // view button
    else if ( view_ == button )
    {
        LOG_DEBUG( "view button was pressed", NULL );

        // view the selected item
        if ( 1 == selected.count() )
        {
            QString id( selected[0]->data( Qt::UserRole ).toString() );

            LOG_INFO( "viewing recall '%s'", qPrintable( id ) );

            // lookup recall
            recallItem r( recalls_[id] );

            // open
            RecallViewerDialog *dialog = new RecallViewerDialog(
                id,
                r.date_,
                r.text_,
                r.isNew_,
                (QWidget*) mainWindow_ );

            dialog->open();
            accept();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallDialog::onListItemSelectionChanged()
{
    QList<QListWidgetItem*> selected( list_->selectedItems() );

    // enable dismiss button
    dismiss_->setEnabled( selected.count() );

    // enable delete button
    delete_->setEnabled( selected.count() );

    // enable view button
    view_->setEnabled( 1 == selected.count() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RecallDialog::initialize()
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

    setFontPointSize10( this );

    // list
    list_->setIconSize( iconSizeList_ );
    list_->setSelectionMode( QAbstractItemView::MultiSelection );
    setFontPointSize12( list_ );

    setThumbnailSize( iconSize_ );

    connect( list_, SIGNAL( itemSelectionChanged() ), SLOT( onListItemSelectionChanged() ) );

    // signal mapper
    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // dismiss button
    dismiss_ = new hobart::pushButton( tr( "Mark\nComplete" ), this, buttonPixmap );
    dismiss_->setEnabled( false );
    dismiss_->setFocusPolicy( Qt::NoFocus );
    dismiss_->setObjectName( "dismiss" );
    setForeground( dismiss_ );

    signalMapper_->setMapping( dismiss_, dismiss_ );
    connect( dismiss_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    // delete button
    delete_ = new hobart::pushButton( tr( "Delete" ), this, buttonPixmap );
    delete_->setEnabled( false );
    delete_->setFocusPolicy( Qt::NoFocus );
    delete_->setObjectName( "delete" );
    setForeground( delete_ );

    signalMapper_->setMapping( delete_, delete_ );
    connect( delete_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    // view button
    view_ = new hobart::pushButton( tr( "View" ), this, buttonPixmap );
    view_->setEnabled( false );
    view_->setFocusPolicy( Qt::NoFocus );
    view_->setObjectName( "view" );
    setForeground( view_ );

    signalMapper_->setMapping( view_, view_ );
    connect( view_, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );

    // layouts
    bottomLayout_->addStretch();
    bottomLayout_->addWidget( dismiss_ );
    bottomLayout_->addWidget( delete_ );
    bottomLayout_->addWidget( view_ );
}

