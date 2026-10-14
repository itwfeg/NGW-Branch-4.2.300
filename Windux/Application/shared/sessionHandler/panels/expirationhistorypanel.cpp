/**
 * @file expirationhistorypanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "expirationhistorypanel.h"

#include "../sessiondialog.h"
#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <UiUtilities.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
ExpirationHistoryPanel::ExpirationHistoryPanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent )
{
    setObjectName( "ExpirationHistoryPanel" );

    // initialize
    initialize();

    // read expiration note
    session_->send( ConfigurationDev, CfgExpirationLabelNoteRead );

    // read expiration labels
    session_->send( ProductDev, ProductReadAllExpirationLabels );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ExpirationHistoryPanel::~ExpirationHistoryPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExpirationHistoryPanel::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "ExpirationHistoryPanel", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // expiration note
    if (( ConfigurationDev == device ) && ( CfgExpirationLabelNoteWrite == event ))
    {
        DomExpiringNote m( msg );

        QString expiringNoteText;

        // get text
        if ( 2 == session_->scaleConfig->currentLanguage )
            expiringNoteText = m.secondExpiringNoteText();
        else
            expiringNoteText = m.primaryExpiringNoteText();

        // set text
        notes_->setText( expiringNoteText );
    }

    // expiration labels
    else if (( ProductDev == device ) && ( ProductWriteAllExpirationLabels == event ))
    {
        DomShelfLifeExpiration m( msg );

        QVector<QString> pluNum( TokenParser::splitStringTokens( m.pluNumList() ) );
        QVector<QString> daysExpired( TokenParser::splitStringTokens( m.daysExpiredList() ) );
        QVector<QString> expiredTime( TokenParser::splitStringTokens( m.expiredTimeList() ) );
        QVector<QString> desc( TokenParser::splitStringTokens( m.descList() ) );

        LOG_DEBUG( "counts plu num %d days expired %d expired time %d desc %d",
            pluNum.count(),
            daysExpired.count(),
            expiredTime.count(),
            desc.count() );

        if ( pluNum.count() != m.numOfPLUs() )
            LOG_WARN( "mismatch in number of plu %d %d", pluNum.count(), m.numOfPLUs() );

        if ( daysExpired.count() != m.numOfDaysExpired() )
            LOG_WARN( "mismatch in number of days expired %d %d", daysExpired.count(), m.numOfDaysExpired() );

        if ( expiredTime.count() != m.numOfExpiredTimes() )
            LOG_WARN( "mismatch in number of expired times %d %d", expiredTime.count(), m.numOfExpiredTimes() );

        if ( desc.count() != m.numOfDescs() )
            LOG_WARN( "mismatch in number of desc %d %d", desc.count(), m.numOfDescs() );

        int count = qMin( pluNum.count(), m.numOfPLUs() );

        count = qMin( count, daysExpired.count() );
        count = qMin( count, expiredTime.count() );
        count = qMin( count, desc.count() );

        LOG_DEBUG( "have %d entries", count );

        // add transaction to list
        for ( int i = 0; i < count; ++i )
        {
            QTreeWidgetItem *item;
            QColor c;

            QStringList rows;

            // label data
            rows.push_back( pluNum[i] );
            rows.push_back( convertDaysText( daysExpired[i].toInt(), c ) );
            rows.push_back( expiredTime[i] );
            rows.push_back( session_->uiUtilities->stripFontChanges( desc[i] ) );

            // create widget item
            item = new QTreeWidgetItem( rows );
            item->setSizeHint( 0, QSize( 32, 32 ) );

            item->setTextColor( 0, c );
            item->setTextColor( 1, c );
            item->setTextColor( 2, c );
            item->setTextColor( 3, c );

            LOG_DEBUG( "adding plu num '%s' to table", qPrintable( pluNum[i] ) );
            table_->addTopLevelItem( item );

            // resize plu description column to fit contents
            table_->resizeColumnToContents( 3 );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ExpirationHistoryPanel::initialize()
{
    static const int spacing( 7 );

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    modifyPaletteHighlightText setHighlightText( hobart::colorPalette::white );

    // setup layouts
    widgetsLayout_->setSpacing( spacing );
    QLayoutItem *doomed = widgetsLayout_->takeAt( 1 );

    bottomLayout_->addStretch();
    bottomLayout_->setSpacing( spacing );

    if (( doomed ) && ( doomed->spacerItem() ) )
        delete doomed->spacerItem();

    // set panel title
    setTitle( tr( "Expiration Label History" ) );

    // notes
    notes_ = new QLabel( this );
    notes_->setObjectName( "notes" );

    widgetsLayout_->insertWidget( 1, notes_ );

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "PLU" ) );
    tableHeaders.push_back( tr( "Expiration Day" ) );
    tableHeaders.push_back( tr( "Time" ) );
    tableHeaders.push_back( tr( "Description" ) );

    // table
    table_ = new QTreeWidget( this );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setHeaderLabels( tableHeaders );
    table_->setObjectName( "table" );
    table_->setRootIsDecorated( false );
    table_->setSelectionMode( QAbstractItemView::NoSelection );
    table_->setUniformRowHeights( true );
    setHighlight( table_ );
    setHighlightText( table_ );

    table_->setColumnCount( tableHeaders.size() );
    table_->setColumnWidth( 0, 48 );
    table_->setColumnWidth( 1, 96 );
    table_->setColumnWidth( 2, 64 );

    widgetsLayout_->insertWidget( 2, table_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ExpirationHistoryPanel::convertDaysText( int days, QColor& c )
{
    QString result;

    if ( days < 0 )
    {
        result = tr( "Expired" );
        c = hobart::colorPalette::red;
    }
    else if ( 0 == days )
    {
        result = tr( "Today" );
        c = hobart::colorPalette::blue;
    }
    else if ( 1 == days )
    {
        result = tr( "Tomorrow" );
        c = hobart::colorPalette::blue;
    }
    else
    {
        result = QString( "%1 %2" ).arg( days ).arg( tr( "Days" ) );
        c = hobart::colorPalette::gray;
    }

    return result;
}
