/**
 * @file voidtransactionpanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "voidtransactionpanel.h"

#include "../sessiondialog.h"
#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <palette.h>
#include <pushbutton.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <UiUtilities.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

// number of transactions to request at a time
const int VoidTransactionPanel::requestGroupSize_ = 16;

///////////////////////////////////////////////////////////////////////////////////////////////////
VoidTransactionPanel::VoidTransactionPanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent )
{
    setObjectName( "VoidTransactionPanel" );

    // initialize
    initialize();

    // request transaction type
    session_->send( TransactionTypeDev, TransactionReadNonVoidGroup );

    // request transaction (-1 = prev, 0 = first, 1 = next)
    session_->send( TransactionDev, TransactionEventReadNonVoidGroup, ",,,,0," + QString::number( requestGroupSize_ ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
VoidTransactionPanel::~VoidTransactionPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void VoidTransactionPanel::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "VoidTransactionPanel", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // transaction types
    if (( TransactionTypeDev == device ) && ( TransactionTypeWriteAllNames == event ))
    {
        DomTotalTypesList m( msg );

        ids_ = TokenParser::splitStringTokens( m.allRecordIds() );
        namesPri_ = TokenParser::splitStringTokens( m.namesList() );
        namesSec_ = TokenParser::splitStringTokens( m.secNamesList() );

        LOG_DEBUG( "counts id %d %d primary %d %d secondary %d %d",
            ids_.count(), m.numOfRecordIds(),
            namesPri_.count(), m.priNumOfNames(),
            namesSec_.count(), m.secNumOfNames() );

        if ( ids_.count() != m.numOfRecordIds() )
            LOG_WARN( "mismatch in number of ids %d %d", ids_.count(), m.numOfRecordIds() );

        if ( namesPri_.count() != m.priNumOfNames() )
            LOG_WARN( "mismatch in number of primary names %d %d", namesPri_.count(), m.priNumOfNames() );

        if ( namesSec_.count() != m.secNumOfNames() )
            LOG_WARN( "mismatch in number of secondary names %d %d", namesSec_.count(), m.secNumOfNames() );
    }

    // transaction
    else if (( TransactionDev == device ) && ( TransactionWriteNonVoidGroup == event ))
    {
        DomVoidableTransactionGroup m( msg );

        QVector<QString> ids( TokenParser::splitStringTokens( m.listIds() ) );
        QVector<QString> dateTimes( TokenParser::splitStringTokens( m.listDateTimes() ) );
        QVector<QString> operators( TokenParser::splitStringTokens( m.listOperators() ) );
        QVector<QString> transTypes( TokenParser::splitStringTokens( m.listTransTypes() ) );
        QVector<QString> plus( TokenParser::splitStringTokens( m.listPLUs() ) );
        QVector<QString> weights( TokenParser::splitStringTokens( m.listWeights() ) );
        QVector<QString> prices( TokenParser::splitStringTokens( m.listPrices() ) );
        QVector<QString> items( TokenParser::splitStringTokens( m.listItems() ) );

        LOG_DEBUG( "counts id %d date times %d operators %d trans types %d plus %d weights %d prices %d items %d",
            ids.count(),
            dateTimes.count(),
            operators.count(),
            transTypes.count(),
            plus.count(),
            weights.count(),
            prices.count(),
            items.count() );

        if ( ids.count() != m.numberIds() )
            LOG_WARN( "mismatch in number of ids %d %d", ids.count(), m.numberIds() );

        if ( dateTimes.count() != m.numberDateTimes() )
            LOG_WARN( "mismatch in number of date times %d %d", dateTimes.count(), m.numberDateTimes() );

        if ( operators.count() != m.numberOperators() )
            LOG_WARN( "mismatch in number of operators %d %d", operators.count(), m.numberOperators() );

        if ( transTypes.count() != m.numberTransTypes() )
            LOG_WARN( "mismatch in number of trans types %d %d", transTypes.count(), m.numberTransTypes() );

        if ( plus.count() != m.numberPLUs() )
            LOG_WARN( "mismatch in number of plus %d %d", plus.count(), m.numberPLUs() );

        if ( weights.count() != m.numberWeights() )
            LOG_WARN( "mismatch in number of weights %d %d", weights.count(), m.numberWeights() );

        if ( prices.count() != m.numberPrices() )
            LOG_WARN( "mismatch in number of prices %d %d", prices.count(), m.numberPrices() );

        if ( items.count() != m.numberItems() )
            LOG_WARN( "mismatch in number of items %d %d", items.count(), m.numberItems() );

        int count = qMin( ids.count(), m.numberIds() );

        count = qMin( count, dateTimes.count() );
        count = qMin( count, operators.count() );
        count = qMin( count, transTypes.count() );
        count = qMin( count, plus.count() );
        count = qMin( count, weights.count() );
        count = qMin( count, prices.count() );
        count = qMin( count, items.count() );

        LOG_DEBUG( "have %d entries", count );

        MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
        QString tempStr;

        //----------------------------------------------------------------------------
        // SOF-43 Calculate the date/timestamp of the transactions that can be viewed.
        //
        //        The operator can view the previous X hours of transactions,
        //        starting with the current date/time and substracting the number
        //        of hours to view.
        //---------------------------------------------------------------------------
        QDateTime cutoffDateTimeForTransToView =
            QDateTime::currentDateTime().addSecs( ( ( scaleConfig->voidTransactionsViewHours() * 3600 ) * -1 ) );

        tempStr = cutoffDateTimeForTransToView.toString();
        LOG_DEBUG("VoidTransactionPanel: ===>cutoffDateTimeForTransToView=|%s|", tempStr.toLatin1().constData() );

        QDateTime currentRowDateTime;

        // add transaction to list
        for ( int i = 0; i < count; ++i )
        {
            QTreeWidgetItem *item;
            QStringList rows;

            currentRowDateTime = QDateTime::fromString( dateTimes[i], "yyyy-MM-dd hh:mm:ss" );

            tempStr = currentRowDateTime.toString();
            LOG_DEBUG("VoidTransactionPanel: dateTimes[%d], currentRowDateTime=|%s|", i, tempStr.toLatin1().constData() );

            // SOF-43: Does this record fall within the range of records allowed to be viewed?
            if( currentRowDateTime >= cutoffDateTimeForTransToView )
            {
                // transaction data
                rows.push_back( dateTimes[i] );
                rows.push_back( lookupTransTypeName( transTypes[i] ) );
                rows.push_back( plus[i] );
                rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( weights[i] ) );
                rows.push_back( session_->uiUtilities->youGottaSetTheSeparator( prices[i] ) );
                rows.push_back( items[i] );
                rows.push_back( ids[i] );

                // create widget item
                item = new QTreeWidgetItem( rows );
                item->setSizeHint( 0, QSize( 32, 32 ) );

                LOG_DEBUG( "adding item with dateTimes '%s' to table", dateTimes[i].toLatin1().constData() );
                LOG_DEBUG( "adding item with key '%s' to table", ids[i].toLatin1().constData() );
                table_->addTopLevelItem( item );
            }
            else
            {
                // Data is returned from the scale application in order from newest-to-oldest.
                // End the loop when you encounter a date that is older than the cut-off date/time.

                LOG_DEBUG( "VoidTransactionPanel: Date too old, jumped out of loop", NULL );
                i = count;
            }
        }

        // no items in table, present message box "no records"
        if ( !table_->topLevelItemCount() )
        {
            LOG_DEBUG( "no transactions!", NULL );
            session_->send( Error, ErrOperatorTotalDevice, "151002,5" );

            // reject to show dialog!
            reject();
        }
        else
        {
            // enable buttons

            if ( 1 == scaleConfig->showSelectAllWhenVoidingTrans() )
            {
                selectAll_->setEnabled( true );
                selectAll_->setVisible( true );
            }
            // request more records (-1 = prev, 0 = first, 1 = next)
            if ( m.nextAvailable() )
            {
                LOG_DEBUG( "requesting more transactions...", NULL );
                session_->send( TransactionDev, TransactionEventReadNonVoidGroup, ",,,,1," + QString::number( requestGroupSize_ ) );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void VoidTransactionPanel::onExportButtonClicked()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void VoidTransactionPanel::onSelectAllButtonClicked()
{
    LOG_CLASS_INFO( "VoidTransactionPanel", "onSelectAllButtonClicked" );

    QList<QTreeWidgetItem*> selectedItems( table_->selectedItems() );

    // select all clicked when all are already selected
    if ( selectedItems.count() == table_->topLevelItemCount() )
    {
        LOG_TRACE( "clearing selection", NULL );
        table_->clearSelection();
    }

    // select all items
    else
    {
        LOG_TRACE( "selecting all", NULL );
        table_->selectAll();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void VoidTransactionPanel::onTableItemSelectionChanged()
{
    QList<QTreeWidgetItem*> selectedItems( table_->selectedItems() );

    // enable/disable void button
    void_->setEnabled( selectedItems.count() ? true : false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void VoidTransactionPanel::onVoidButtonClicked()
{
    LOG_CLASS_INFO( "VoidTransactionPanel", "onVoidButtonClicked" );

    QList<QTreeWidgetItem*> selectedItems( table_->selectedItems() );
    int count = selectedItems.count();

    LOG_DEBUG( "voiding %d items...", count );

    if ( count )
    {
        QString list;

        // make list of item keys
        foreach( QTreeWidgetItem *item, selectedItems )
        {
            if ( !list.isEmpty() )
                list += ",";

            list += item->data( 6, 0 ).toString();
        }

        LOG_DEBUG( "list of keys to void '%s'", list.toLatin1().constData() );

        // send void message
        session_->send( TransactionDev, TransactionEventVoidTransactions, "\"" + QString::number( count ) + ",\"\"" + list + "\"\"\"" );
    }

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void VoidTransactionPanel::initialize()
{
    static const int spacing( 7 );

    const QSize buttonSize(
        (mainWindow_->width() * 100) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 45) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

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
    setTitle( tr( "Void Transactions" ) );

    // table headers
    QStringList tableHeaders;
    tableHeaders.push_back( tr( "Date / Time" ) );
    tableHeaders.push_back( tr( "Trans. Type" ) );
    tableHeaders.push_back( tr( "PLU" ) );
    tableHeaders.push_back( tr( "Weight" ) );
    tableHeaders.push_back( tr( "Price" ) );
    tableHeaders.push_back( tr( "Items" ) );
    tableHeaders.push_back( tr( "Key" ) );

    // table
    table_ = new QTreeWidget( this );
    table_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    table_->setHeaderLabels( tableHeaders );
    table_->setObjectName( "table" );
    table_->setRootIsDecorated( false );
    table_->setSelectionMode( QAbstractItemView::MultiSelection );
    table_->setUniformRowHeights( true );
    setHighlight( table_ );
    setHighlightText( table_ );

    table_->setColumnCount( tableHeaders.size() );
    table_->setColumnWidth( 0, 128 );
    table_->setColumnWidth( 1, 128 );
    table_->setColumnWidth( 2, 48 );
    table_->setColumnWidth( 3, 48 );
    table_->setColumnWidth( 4, 48 );
    table_->setColumnWidth( 5, 32 );
    table_->setColumnWidth( 6, 0 );
    table_->setColumnHidden( 6, true );

    widgetsLayout_->insertWidget( 1, table_ );

    connect( table_, SIGNAL( itemSelectionChanged() ), SLOT( onTableItemSelectionChanged() ) );

    // export
    export_ = new hobart::pushButton( tr( "Export" ), this, buttonPixmap );
    export_->setEnabled( false );
    export_->setFocusPolicy( Qt::NoFocus );
    export_->setObjectName( "exportButton" );
    setForeground( export_ );

    // Note:
    // The Beyond design documentation calls for an Export button but Jim Highley has no idea what
    // it is for. I added skeleton code for the button but am leaving it hidden.
    export_->setVisible( false );

    bottomLayout_->addWidget( export_ );

    connect( export_, SIGNAL( clicked() ), SLOT( onExportButtonClicked() ) );

    // select all
    selectAll_ = new hobart::pushButton( tr( "Select All" ), this, buttonPixmap );
    selectAll_->setEnabled( false );
    selectAll_->setVisible( false );
    selectAll_->setFocusPolicy( Qt::NoFocus );
    selectAll_->setObjectName( "selectAllButton" );
    setForeground( selectAll_ );

    bottomLayout_->addWidget( selectAll_ );

    connect( selectAll_, SIGNAL( clicked() ), SLOT( onSelectAllButtonClicked() ) );

    // void
    void_ = new hobart::pushButton( tr( "Void" ), this, buttonPixmap );
    void_->setEnabled( false );
    void_->setFocusPolicy( Qt::NoFocus );
    void_->setObjectName( "voidButton" );
    setForeground( void_ );

    bottomLayout_->addWidget( void_ );

    connect( void_, SIGNAL( clicked() ), SLOT( onVoidButtonClicked() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString VoidTransactionPanel::lookupTransTypeName( const QString& id ) const
{
    LOG_CLASS_INFO( "VoidTransactionPanel", "lookupTransTypeName" );

    QString result;

    // compute list size
    int count = ids_.count();
    count = qMin( count, namesPri_.count() );
    count = qMin( count, namesSec_.count() );

    // process list
    for ( int i = 0; i < count; ++i )
        if ( id == ids_[i] )
        {
            if ( 2 == session_->scaleConfig->currentLanguage )
                result = namesSec_[i];
            else
                result = namesPri_[i];

            break;
        }

    LOG_TRACE( "result %s", result.toLatin1().constData() );
    return result;
}
