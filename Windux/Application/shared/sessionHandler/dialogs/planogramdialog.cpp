/**
 * @file planogramdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "graphicdialog.h"
#include "planogramdialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <filesystem.h>
#include <font.h>
#include <logmanager.h>
#include <palette.h>
#include <pushbutton.h>

#include <Messages.h>

#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSignalMapper>

const QSize PlanogramDialog::iconSize_( 128, 64 );
const QSize PlanogramDialog::iconSizeList_( 160, 64 );

///////////////////////////////////////////////////////////////////////////////////////////////////
PlanogramDialog::PlanogramDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f )
{
    setObjectName( "PlanogramDialog" );

    // initialize interface
    initialize();

    // request list of planograms
    requestList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PlanogramDialog::~PlanogramDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *PlanogramDialog::rendererAppearance() const
{
    return new hobart::lightOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int PlanogramDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlanogramDialog::addItemsToList( const QStringList& files, const QStringList& dates )
{
    LOG_CLASS_INFO( "PlanogramDialog", "addItemsToList" );

    // remove all existing items
    while ( list_->count() )
    {
        QListWidgetItem *doomed = list_->takeItem( 0 );

        if ( doomed )
            delete doomed;
    }

    if ( files.count() != dates.count() )
        LOG_WARN( "mismatch in items %d %d", files.count(), dates.count() );

    // add all images to the list
    for ( int i = 0; i < files.count(); ++i )
    {
        QString filename( files[i] );
        QString fullFilename( filesystem::absoluteFilePath( filename, folderType::PlanogramImages ) );

        // if we were given date info tack that onto the description
        if ( i < dates.count() )
        {
            QString date( dates[i] );

            if ( date.length() )
                filename += "\n" + date;
        }

        addItemToList( QPixmap( fullFilename ), filename, QVariant( fullFilename ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlanogramDialog::requestList()
{
    LOG_CLASS_INFO( "PlanogramDialog", "requestList" );

    LOG_TRACE( "requesting list of planograms", NULL );
    session_->send( GraphicDev, ReadAllPlanogramGraphicNames );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlanogramDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "PlanogramDialog", "onSessionMessageReceive" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // populate list
    if (( GraphicDev == device ) && ( ReadAllPlanogramGraphicNames == event ))
    {
        DomGraphicReadAllPlanogramGraphicNames m( msg );

        QStringList graphicList( m.field<QStringList>( m.graphicList() ) );
        QStringList dateList( m.field<QStringList>( m.dateList() ) );

        if ( graphicList.count() != m.numOfGraphics() )
            LOG_WARN( "mismatch in graphics %d %d", graphicList.count(), m.numOfGraphics() );

        if ( dateList.count() != m.numOfDates() )
            LOG_WARN( "mismatch in dates %d %d", dateList.count(), m.numOfDates() );

        addItemsToList( graphicList, dateList );
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlanogramDialog::onButtonClicked( QWidget *button )
{
    LOG_CLASS_INFO( "PlanogramDialog", "onButtonClicked" );

    // sanity check
    if ( !button )
        return;

    QList<QListWidgetItem*> selected( list_->selectedItems() );

    LOG_TRACE( "have %d items selected", selected.count() );

    // delete button
    if ( delete_ == button )
    {
        LOG_DEBUG( "delete button was pressed", NULL );

        if ( selected.count() )
        {
            // remove selected items from list
            for ( int i = 0; i < selected.count(); ++i )
            {
                QString fullFilename( selected[i]->data( Qt::UserRole ).toString() );
                QString filename( filesystem::fileName( fullFilename, folderType::PlanogramImages ) );

                LOG_INFO( "deleting planogram '%s'", qPrintable( filename ) );

                // delete the file
                session_->send( GraphicDev, DeletePlanogramGraphic, "\"" + filename + "\"" );
            }

            // update planogram list
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
            QString filename( selected[0]->data( Qt::UserRole ).toString() );

            // create dialog
            GraphicDialog *dialog = new GraphicDialog( filename, (QWidget*) mainWindow_ );

            // validate graphic
            if ( !dialog->isValid() )
            {
                LOG_WARN( "graphic '%s' was not valid", qPrintable( filename ) );
                dialog->deleteLater();
            }

            // show
            else
            {
                LOG_DEBUG( "showing dialog for graphic '%s'", qPrintable( filename ) );

                // show dialog!
                dialog->open();

                accept();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlanogramDialog::onListItemSelectionChanged()
{
    QList<QListWidgetItem*> selected( list_->selectedItems() );

    // enable delete button
    delete_->setEnabled( selected.count() );

    // enable view button
    view_->setEnabled( 1 == selected.count() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PlanogramDialog::initialize()
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
    bottomLayout_->addWidget( delete_ );
    bottomLayout_->addWidget( view_ );
}
