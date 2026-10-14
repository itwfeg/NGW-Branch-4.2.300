/**
 * @file serviceNewWrapperPkgHistory.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "serviceNewWrapperPkgHistory.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <backgroundwidgetstyles.h>
#include <logmanager.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperPkgHistory::serviceNewWrapperPkgHistory( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    setObjectName( "serviceNewWrapperPkgHistory" );

    // initialize
    initialize();
    createLayout();
    translate();

    // retrieve package history
    session_->send( Wrapper, WrReqPackageHistory );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperPkgHistory::~serviceNewWrapperPkgHistory()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperPkgHistory::translate()
{
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "Tray Name" ) );
    tableHeaders.push_back( tr( "Width" ) );
    tableHeaders.push_back( tr( "Length" ) );
    tableHeaders.push_back( tr( "Height" ) );
    tableHeaders.push_back( tr( "Lip" ) );
    tableHeaders.push_back( tr( "Film Selection" ) );

    table_->setHeaderLabels( tableHeaders );
    table_->setColumnCount( tableHeaders.size() );

    calcTableColumnWidths();
    calcTableFontSizes( table_->headerItem() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperPkgHistory::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // resize columns
    calcTableColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperPkgHistory::onSessionMessage( const SessionMessage& msg )
{
    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // package history
    if (( Wrapper == device ) && ( WrEvolPackageHistory == event ))
    {
        DomEvolWrapperPackageHistory m( msg );

        // process errors
        if ( m.numOfItems() )
        {
            QStringList idList( m.field<QStringList>( m.idList() ) );
            QStringList widthList( m.field<QStringList>( m.widthList() ) );
            QStringList lengthList( m.field<QStringList>( m.lengthList() ) );
            QStringList heightList( m.field<QStringList>( m.heightList() ) );
            QStringList lipList( m.field<QStringList>( m.lipList() ) );
            QStringList filmWidthList( m.field<QStringList>( m.filmWidthList() ) );
            QStringList trayNameList( m.field<QStringList>( m.trayNameList() ) );
            QStringList usedWrapBoxBestFitList( m.field<QStringList>( m.usedWrapBoxBestFitList() ) );
            QStringList modifiedNetStretchList( m.field<QStringList>( m.modifiedNetStretchList() ) );

            int count( m.numOfItems() );
            count = qMin( count, idList.size() );
            count = qMin( count, widthList.size() );
            count = qMin( count, lengthList.size() );
            count = qMin( count, heightList.size() );
            count = qMin( count, lipList.size() );
            count = qMin( count, filmWidthList.size() );
            count = qMin( count, trayNameList.size() );
            count = qMin( count, usedWrapBoxBestFitList.size() );
            count = qMin( count, modifiedNetStretchList.size() );

            // do we have errors?
            if ( 0 < count )
            {
                // clear out current list
                table_->clear();

                // process items
                for ( int i( 0 ); i < count; ++i )
                {
                    QString trayName( trayNameList[i] );
                    trayName.replace( "\"", "" );

                    QString note( ("1" == modifiedNetStretchList[i]) ? "* " : "" );

                    // item data
                    QStringList rows;
                    rows.push_back( note + trayName );
                    rows.push_back( widthList[i] );
                    rows.push_back( lengthList[i] );
                    rows.push_back( heightList[i] );
                    rows.push_back( lipList[i] );
                    rows.push_back( filmWidthList[i] );

                    // create widget item
                    QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );
                    item->setSizeHint( 0, QSize( 64, 64 ) );

                    calcTableFontSizes( item );

                    // modify background color when best fit used
                    if ( "1" == usedWrapBoxBestFitList[i] )
                    {
                        const QColor c( hobart::colorPalette::yellow );

                        item->setBackgroundColor( 0, c );
                        item->setBackgroundColor( 1, c );
                        item->setBackgroundColor( 2, c );
                        item->setBackgroundColor( 3, c );
                        item->setBackgroundColor( 4, c );
                        item->setBackgroundColor( 5, c );
                    }

                    table_->addTopLevelItem( item );
                }

            } // 0 < count of errors
        } // 0 < total errors
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperPkgHistory::initialize()
{
    tableFont_ = font();
    tableFont_.setPointSize( 12 );

    // ---- //

    // error history table
    table_ = new QTreeWidget( this );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setObjectName( "table" );
    table_->setRootIsDecorated( false );
    table_->setSelectionMode( QAbstractItemView::NoSelection );
    table_->setUniformRowHeights( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperPkgHistory::createLayout()
{
    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 48, 8, 48, 8 ) );

    screenLayout->addWidget( table_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperPkgHistory::calcTableColumnWidths()
{
    const int tableWidth( table_->contentsRect().width() );

    table_->setColumnWidth( 0, (tableWidth * 24) / 100 ); // tray name
    table_->setColumnWidth( 1, (tableWidth * 14) / 100 ); // width
    table_->setColumnWidth( 2, (tableWidth * 14) / 100 ); // length
    table_->setColumnWidth( 3, (tableWidth * 14) / 100 ); // height
    table_->setColumnWidth( 4, (tableWidth * 12) / 100 ); // lip
    table_->setColumnWidth( 5, (tableWidth * 17) / 100 ); // film selection

    // Note:
    // 5% not allocated to handle a vertical scroll bar.
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperPkgHistory::calcTableFontSizes( QTreeWidgetItem *item ) const
{
    item->setFont( 0, tableFont_ ); // tray name
    item->setFont( 1, tableFont_ ); // width
    item->setFont( 2, tableFont_ ); // length
    item->setFont( 3, tableFont_ ); // height
    item->setFont( 4, tableFont_ ); // lip
    item->setFont( 5, tableFont_ ); // film selection
}
