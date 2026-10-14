/**
 * @file searchdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "searchdialog.h"

#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>

#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

const QSize SearchDialog::iconSize_( 100, 64 );

///////////////////////////////////////////////////////////////////////////////////////////////////
SearchDialog::SearchDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f )
{
    setObjectName( "SearchDialog" );

    // initialize interface
    initialize();

    setCurrentState( Initial );

    // request normal scale operation (for default search method)
    session_->send( ConfigurationDev, CfgScaleOperationRead );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SearchDialog::~SearchDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::setCurrentState( State value )
{
    if ( Done == value )
        currentState_->hide();
    else
    {
        QString text;
        QString howmany;

        switch ( value )
        {
        case Initial:
            text = tr( "Please type something to begin searching" );
            howmany = "0/0 PLUs";
            break;
        case Searching:
            text = tr( "Searching..." );
            howmany = "-/- PLUs";
            break;
        case NoResults:
            text = tr( "No results, please search again" );
            howmany = "0/0 PLUs";
            break;
        default:
            break;
        }

        howManyPLUs_->setText(  howmany.leftJustified( 13, ' ' ) );
        currentState_->setText( text );
        currentState_->raise();
        currentState_->show();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    currentState_->setGeometry( list_->geometry() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

    pluDescription_->setFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::onListItemClicked( QListWidgetItem *item, const QVariant& cookie )
{
    if ( !item )
        return;

    selected_ = cookie.toString();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::onPluDescriptionReturnPressed()
{
    LOG_CLASS_INFO( "SearchDialog", "onPluDescriptionReturnPressed" );

    LOG_DEBUG( "onPluDescriptionReturnPressed(): TOP", NULL );

    QString text( pluDescription_->text() );

    // 0 = character search
    // 1 = plu number search
    QString searchType = "0";

    if ( searchType_->isChecked() )
        searchType = "1";

    // disable search field while lookup pending
    //pluDescription_->setReadOnly( true );

    // let user know we are looking (only if nothing in list already)
    if ( !list_->count() )
        setCurrentState( Searching );

    // request items
    LOG_INFO( "searching for '%s' type %s", text.toLatin1().constData(), searchType.toLatin1().constData() );
    session_->send( ProductDev, ProductFindByDescription, text.toUpper() + "," + searchType );

    LOG_DEBUG( "onPluDescriptionReturnPressed(): BOTTOM", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::onPluDescriptionTextChanged( const QString& text )
{
    LOG_CLASS_INFO( "SearchDialog", "onPluDescriptionTextChanged" );

    // for plu number search they must hit enter (this behavior as per Larry)
    if ( searchType_->isChecked() )
        return;

    LOG_DEBUG( "have text '%s'", text.toLatin1().constData() );

    // do search
    if ( 2 <= text.length() )
        onPluDescriptionReturnPressed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::onSearchTypeToggled( bool value )
{
    // clear field
    pluDescription_->clear();
    pluDescription_->setValidator( value ? validator_ : NULL );

    // let user know to type (only if nothing in list already)
    if ( !list_->count() )
        setCurrentState( Initial );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "SearchDialog", "onSessionMessageReceive" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // product search results
    if (( ProductDev == device ) && ( ProductFindByDescription == event ))
    {
        DomOperateSearch search( msg );

        // FT-709
        bool isPluNumberSearch( searchType_->isChecked() );
        int pluNumber = isPluNumberSearch ? pluDescription_->text().toInt() : 0;

        //LOG_TRACE( "clearing search results...", NULL );
        list_->clear();
        QString myPLUs( "             " );

        //LOG_DEBUG( "have %d items and %d descriptions", search.numOfPlues(), search.numOfPlueDescs() );
        int count = qMin( search.numOfPlues(), search.numOfPlueDescs() );

        if ( count )
        {
            QStringList numbers( search.field<QStringList>( search.pluList() ) );
            QStringList descriptions( search.field<QStringList>( search.plueDescList() ) );

            //LOG_DEBUG( "list sizes %d items and %d descriptions", numbers.size(), descriptions.size() );

            if ( search.numOfPlues() != numbers.size() )
            {
                LOG_WARN( "mismatch in numbers list size %d %d", search.numOfPlues(), numbers.size() );

                for ( int i = 0; i < numbers.size(); ++i )
                    LOG_DEBUG( "[%s]", qPrintable( numbers[i] ) );
            }
            else if ( search.numOfPlueDescs() != descriptions.size() )
            {
                LOG_WARN( "mismatch in descriptions list size %d %d", search.numOfPlueDescs(), descriptions.size() );

                for ( int i = 0; i < descriptions.size(); ++i )
                    LOG_DEBUG( "[%s]", qPrintable( descriptions[i] ) );
            }
            else if ( search.numOfPlues() < count )
                LOG_WARN( "bad numbers list size %d %d", search.numOfPlues(), count );
            else if ( search.numOfPlueDescs() < count )
                LOG_WARN( "bad descriptions list size %d %d", search.numOfPlueDescs(), count );

            // process lists
            else
            {
                //LOG_INFO( "processing %d results...", count );
                // ok so everything is good, and counts match, but if we have too many counts and items, it's going to update slooooooooooow
                // who needs over 1300 items, not gonna scroll the list?
                myPLUs = "";
                if( count > 300 )
                {
                    myPLUs = "300/" + QString::number( count ) + " PLUs";
                    howManyPLUs_->setText( myPLUs.leftJustified( 13, ' ' ) );
                    count = 300;
                }
                else
                {
                    myPLUs = QString::number( count ) + "/" + QString::number( count ) + " PLUs";
                    howManyPLUs_->setText(  myPLUs.leftJustified( 13, ' ' ) );
                }

                for ( int i = 0; i < count; ++i )
                {
                    addItemToList( numbers[i], descriptions[i], numbers[i] );

                    // FT-709: highlight closet plu to our search (or the last one)
                    if (( isPluNumberSearch ) && (( pluNumber <= numbers[i].toInt() ) || ( i == count-1 )))
                    {
                        QListWidgetItem *item( NULL );
                        int row( list_->count() - 1 );

                        if ( 0 <= row )
                            item = list_->item( row );

                        if ( item )
                        {
                            list_->setItemSelected( item, true );
                            list_->scrollToItem( item );

                            isPluNumberSearch = false;
                        }
                    }
                }
            }
        }

        // enable search field
        //pluDescription_->setReadOnly( false );

        setCurrentState( (0 < count) ? Done : NoResults );
    }

    // normal scale operation
    else if (( ConfigurationDev == device ) && ( CfgScaleOperationWrite == event ))
    {
        DomNormalScaleOperation m( msg );

        // search by plu is default search method
        if ( 1 == m.defaultSearchMethod().toInt() )
        {
            searchType_->setChecked( true );
            onSearchTypeToggled( true );
        }
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SearchDialog::initialize()
{
    modifyPaletteWindow setBackgroundTransparent( Qt::transparent );

    modifyPaletteForeground setForegroundGray( modifyPaletteForeground::createPalette( hobart::colorPalette::gray ) );

    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize7( (mainWindow_->height() * 7) / SessionMainWindow::scalingDividerY_ );
    modifyFontPointSize setFontPointSize8( (mainWindow_->height() * 8) / SessionMainWindow::scalingDividerY_ );

    list_->setIconSize( iconSize_ );
    setFontPointSize12( list_ );
    setForegroundGray( list_ );

    // plu description
    pluDescription_ = new QLineEdit( this );
    pluDescription_->setFixedSize( 300, 32 );
    pluDescription_->setObjectName( "pluDescription" );
    pluDescription_->setPalette( QApplication::palette() );

    setFocusProxy( pluDescription_ );

    connect( pluDescription_, SIGNAL( returnPressed() ), SLOT( onPluDescriptionReturnPressed() ) );
    connect( pluDescription_, SIGNAL( textChanged(QString) ), SLOT( onPluDescriptionTextChanged(QString) ) );

    // how many PLUs are displayed?
    howManyPLUs_ = new QLabel( "0/0 PLUs     " );
    howManyPLUs_->setObjectName( "howManyPLUs" );
    setBackgroundTransparent( howManyPLUs_ );
    setFontPointSize7( howManyPLUs_ );

    // tell operator to add more characters for more complete results
    addMoreChars_ = new QLabel( "Please enter more characters to get more accurate and complete results.  ONLY first 200 results displayed." );
    addMoreChars_->setObjectName( "addMoreChars" );
    setBackgroundTransparent( addMoreChars_ );
    setFontPointSize8( addMoreChars_ );

    // search type
    searchType_ = new QCheckBox( tr( "PLU Search" ), this );
    searchType_->setObjectName( "searchType" );
    setBackgroundTransparent( searchType_ );
    setFontPointSize12( searchType_ );

    searchType_->setFocusPolicy( Qt::NoFocus );
    howManyPLUs_->setFocusPolicy( Qt::NoFocus);
    addMoreChars_->setFocusPolicy( Qt::NoFocus );

    validator_ = new QIntValidator( 0, 999999, this );

    connect( searchType_, SIGNAL( toggled(bool) ), SLOT( onSearchTypeToggled(bool) ) );

    // current state
    currentState_ = new QLabel( this );
    currentState_->setAlignment( Qt::AlignCenter );
    currentState_->setObjectName( "currentState" );
    setFontPointSize12( currentState_ );

    // update layout
    topLayout_->addWidget( pluDescription_ );
    topLayout_->addStretch();
    topLayout_->addWidget( searchType_ );
    //topLayout_->addStretch();
    topLayout_->addWidget( howManyPLUs_ );

    bottomLayout_->addWidget( addMoreChars_ );
}
