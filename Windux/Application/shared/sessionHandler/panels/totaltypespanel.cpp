/**
 * @file totaltypespanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "totaltypespanel.h"

#include "../sessionmainwindow.h"

#include <font.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <TokenParser.h>

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
TotalTypesPanel::TotalTypesPanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    process_( false ),
    done_( false )
{
    setObjectName( "TotalTypesPanel" );

    // initialize
    initialize();

    // request transaction type
    session_->send( TransactionTypeDev, TransactionReadNonVoidGroup );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
TotalTypesPanel::~TotalTypesPanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TotalTypesPanel::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "TotalTypesPanel", "onSessionMessage" );

    // nothing to do
    if ( done_ )
        return;

    int device = msg[0].toInt();
    int event = msg[1].toInt();

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

        int count = qMin( ids_.count(), m.numOfRecordIds() );

        count = qMin( count, namesPri_.count() );
        count = qMin( count, namesSec_.count() );

        LOG_DEBUG( "have %d entries", count );

        // disable signal processing
        process_ = false;

        // clear existing entries
        comboBox_->clear();

        // process list
        for ( int i = 0; i < count; ++i )
        {
            if ( 2 == session_->scaleConfig->currentLanguage )
                comboBox_->addItem( namesSec_[i] );
            else
                comboBox_->addItem( namesPri_[i] );
        }

        // enable signal processing
        process_ = true;

        // request current transaction type
        session_->send( TransactionTypeDev, TransactionTypeReadDates );
    }

    // transaction type
    else if (( TransactionTypeDev == device ) && ( TransactionTypeWriteCurrentType == event ))
    {
        DomTotalTypeNameSetting m( msg );

        // look for our transaction type
        for ( int i = 0; i < ids_.count(); ++i )
            if ( ids_[i] == m.transNum() )
            {
                // disable signal processing
                process_ = false;

                // set combo index and enable
                comboBox_->setCurrentIndex( i );
                comboBox_->setEnabled( comboBox_->count() ? true : false );

                // enable signal processing
                process_ = true;

                break;
            }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TotalTypesPanel::onCurrentIndexChanged( int index )
{
    LOG_CLASS_INFO( "TotalTypesPanel", "onCurrentIndexChanged" );

    // not processing signals
    if ( !process_ )
        return;

    // invalid index
    if (( index < 0 ) || ( ids_.count() <= index ))
    {
        LOG_WARN( "invalid index %d count %d", index, ids_.count() );
        return;
    }

    QString selected = ids_[index];
    LOG_INFO( "index %d id %s was chosen", index, selected.toLatin1().constData() );

    // change transaction type
    session_->send( TransactionTypeDev, TransactionTypeWriteCurrentType, selected );
    done_ = true;

    // update session
    session_->send( SessionDev, SnReadWeighModeTransactionType );

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TotalTypesPanel::initialize()
{
    // set panel title
    setTitle( tr( "Total Types" ) );


    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    setFontPointSize12( this );

    // add widgets
    widgetsLayout_->insertSpacing( 1, 32 );
    widgetsLayout_->insertWidget( 2, (groupBox_ = new QGroupBox( tr( "Select Total Type" ), this )) );

    groupBox_->setObjectName( "groupBox" );

    comboBox_ = new QComboBox( groupBox_ );
    comboBox_->setEnabled( false );
    comboBox_->setFocusPolicy( Qt::NoFocus );
    comboBox_->setObjectName( "totalTypeCombo" );
    comboBox_->setPalette( QApplication::palette() );
    comboBox_->setStyleSheet("font: 20pt;");

    connect( comboBox_, SIGNAL( currentIndexChanged(int) ), SLOT( onCurrentIndexChanged(int) ) );

    QHBoxLayout *groupBoxLayout = new QHBoxLayout( groupBox_ );
    groupBoxLayout->addWidget( comboBox_ );
}

