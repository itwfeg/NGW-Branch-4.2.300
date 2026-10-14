/* @file SuperOperatorIds.cpp
 * SuperOperatorIds 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include "superOperatorIds.h"
#include "ui_superOperatorIds.h"

#include "../sessionmainwindow.h"

#include "../dialogs/messageboxdialog.h"

#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

#include <numericlineedit.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QIntValidator>

// Hobart
#include "../../source/HFC/CfgBaseFieldOffsets.h"

namespace hobart {
    class numericLineEdit;
}

static const int OK_PRESSED = 1;

///////////////////////////////////////////////////////////////////////////////////////////////////
SuperOperatorIds::SuperOperatorIds( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    ui_( new Ui::SuperOperatorIds ),
    scaleConfigData_( &session_->scaleConfig->scaleConfig ),
    singleOperator_( NULL ),
    operatorInformation_( NULL ),
    ignoreSignals_( false ),
    hasChanged_( false ),
    createNew_( false ),
    currentSelectedRow_( -1 ),
    numOfOperators_( 0 ),
    handsFreeLoginEnabled_( false )
{
    // coming from Supervisor panel
    if ( mainWindow_->isSupervisorMode() )
    {
        singleOperator_ = &dynamic_cast<SupervisorMode*>( mode_ )->singleOperator_;
        operatorInformation_ = &dynamic_cast<SupervisorMode*>( mode_ )->operatorInformation_;
    }
    // coming from Service panel
    else if ( mainWindow_->isServiceMode() )
    {
        singleOperator_ = &dynamic_cast<ServiceMode*>( mode_ )->singleOperator_;
        operatorInformation_ = &dynamic_cast<ServiceMode*>( mode_ )->operatorInformation_;
    }

    // check hands free enabled
    handsFreeLoginEnabled_ = session_->uiUtilities->isFeatureLicensed( SCALE_HANDSFREEDEVICE );

    // ---- //

    ui_->setupUi( this );
    scaleScreen();

    initialize();
    translate();

    clearOperatorFields();

    // ---- //

    // send 102,1 request, read all operators
    session_->send( OperatorDev, OpDtReadAll );

    // start monitoring HFPD operators
    session_->send( OperatorDev, OpNotifyUIofRFIDInputStart );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
SuperOperatorIds::~SuperOperatorIds()
{
    delete ui_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::translate()
{
    QStringList headersStr;
    headersStr.append( tr("ID") );
    headersStr.append( tr("Access") );
    headersStr.append( tr("Language") );
    headersStr.append( tr("Operator Name") );
    headersStr.append( tr("TAG ID") );

    // ---- //

    ui_->retranslateUi( this );

    ui_->tblOpIdsListBody->setHorizontalHeaderLabels( headersStr );

    ui_->cboPreferredLanguage->setItemText( 0, tr("Primary Language") );
    ui_->cboPreferredLanguage->setItemText( 1, tr("Secondary Langage") );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::resizeEvent( QResizeEvent *e )
{
    const int width( ui_->tblOpIdsListBody->width() - 35 ); // allow for scrollbar

    if ( handsFreeLoginEnabled_ )
    {
        ui_->tblOpIdsListBody->setColumnWidth( COL_ID, (width * 22) / 100 );
        ui_->tblOpIdsListBody->setColumnWidth( COL_ACCESS, (width * 9) / 100 );
        ui_->tblOpIdsListBody->setColumnWidth( COL_LANG, (width * 12) / 100 );
        ui_->tblOpIdsListBody->setColumnWidth( COL_OPER_NAME, (width * 35) / 100 );
        ui_->tblOpIdsListBody->setColumnWidth( COL_TAG_ID, (width * 22) / 100 );
    }
    else
    {
        ui_->tblOpIdsListBody->setColumnWidth( COL_ID, (width * 15) / 100 );
        ui_->tblOpIdsListBody->setColumnWidth( COL_ACCESS, (width * 10) / 100 );
        ui_->tblOpIdsListBody->setColumnHidden( COL_LANG, true );
        ui_->tblOpIdsListBody->setColumnWidth( COL_OPER_NAME, (width * 75) / 100 );
        ui_->tblOpIdsListBody->setColumnHidden( COL_TAG_ID, true );
    }

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperOperatorIds::onScreenChanging()
{
    saveCurrentOperator();

    // stop monitoring HFPD operators
    session_->send( OperatorDev, OpNotifyUIofRFIDInputEnd );

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::onSessionMessage( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // 102,2 - update a single operator information
    if (( OperatorDev == device ) && ( OpDtWrite == event ))
        updateSingleOperator( DomSingleOperator( msg ) );

    // 102,3 - update all operator id information
    else if (( OperatorDev == device ) && ( OpDtWriteAll == event ))
        updateOperatorInformation( DomOperatorInfomation( msg ) );

    // 15,20 - current RFID
    else if (( RFIDReader == device ) && ( RFIDCurrentID == event ) && ( handsFreeLoginEnabled_ ) && ( !scaleConfigData_->autoCreateRFIDOp() ))
    {
        DomRfidCurrentId m( msg );

        const QString encodedTagId( m.rfid() );
        const QByteArray a( QByteArray::fromHex( encodedTagId.toLatin1() ) );

        QByteArray zeros;
        zeros.fill( '0', 16 );

        // check for valid tag
        if ( a != zeros )
        {
            const QString tagId( QString::fromLatin1( a ) );

            // check if tag exists in database
            if (( !ignored_.contains( tagId ) ) && ( currentTagPending_.isEmpty() ))
            {
                session_->send( OperatorDev, OpDoesRFIDExist, tagId );
                currentTagPending_ = tagId;
            }
        }
    }

    // 102,27 - indicates RFID exists
    else if (( OperatorDev == device ) && ( OpRFIDExists == event ))
    {
        if ( !ignored_.contains( currentTagPending_ ) )
            ignored_.append( currentTagPending_ );

        currentTagPending_.clear();
    }

    // 101,28 - indicates RFID does not exist
    else if (( OperatorDev == device ) && ( OpRFIDDoesntExist == event ))
    {
        MessageBoxDialog dialog( mainWindow_ );

        dialog.setTitle( tr( "New Hands Free Device" ) );
        dialog.setText(
            tr( "A new hands free device has been detected:" ) + "\n" +
            currentTagPending_ + "\n" +
            "\n" +
            tr( "Would you like to create an operator record for this device?" ) );

        dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

        // prompt
        if ( QDialog::Accepted == dialog.exec() )
        {
            // send 102,12,id,rfid - create new operator
            session_->send( OperatorDev, OpDtCreateNew, currentTagPending_ + "," + currentTagPending_ );

            createNew_ = true;
        }

        if ( !ignored_.contains( currentTagPending_ ) )
            ignored_.append( currentTagPending_ );

        currentTagPending_.clear();
    }

    else if (( Error == device ) && ( ErrOperatorDevice == event ))
    {
        // got a dialog indicating that the entered operator id does not exist,
        // asks if we would like to create it create it if the user pressed OK.
        if (( 102003 == msg[2].toInt() ) && ( 7 <= msg.size() ))
            requestToCreateNewOperator( msg[7].toInt() );
    }

    _Mybase::onSessionMessage( msg );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::checkAcccessLevel()
{
    if ( ignoreSignals_ )
        return;

    const int newAccessLevel( ui_->txtOperatorAccessLevel->text().toInt() );
    const int oldAccessLevel( singleOperator_->operatorAccessLevel() );

    // nothing to do
    if ( newAccessLevel == oldAccessLevel )
        return;

    // update class information
    singleOperator_->operatorAccessLevelRaw() = ui_->txtOperatorAccessLevel->text();

    // update information in cell
    QTableWidgetItem *item( ui_->tblOpIdsListBody->item( currentSelectedRow_, COL_ACCESS ) );

    if ( item )
    {
        const QString tmpText( ui_->txtOperatorAccessLevel->text() );

        item->setText( tmpText );
    }

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::checkboxChanged( int state )
{
    if ( ignoreSignals_ )
        return;

    if ( ui_->chkForcedLogin == sender() )
    {
        scaleConfigData_->forcedLoginRaw() = ( 0 == state ) ? "0" : "1";

        QString msg;
        msg += ",";
        msg += QString::number( fieldOffsetCfgOperateLoginRequired ) + ",";
        msg += scaleConfigData_->forcedLoginRaw();

        // send 112,18 - scale config single field write
        session_->send( ConfigurationDev, CfgSaveSingleField, msg );
    }
    else if ( ui_->chkAutoCreateOper == sender() )
    {
        scaleConfigData_->autoCreateRFIDOpRaw() = ( 0 == state ) ? "0" : "1";

        QString msg;
        msg += ",";
        msg += QString::number( fieldOffsetCfgAutoCreateRFIDOperator ) + ",";
        msg += scaleConfigData_->autoCreateRFIDOpRaw();

        // send 112,18 - scale config single field write
        session_->send( ConfigurationDev, CfgSaveSingleField, msg );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::deleteOperator()
{
    const QString opId( singleOperator_->operatorID() );

    // if this operator exists, then delete it
    if ( opId.isEmpty() )
        return;

    // physically remove the item from the table
    if ( !removeOperatorRow( opId ) )
         return;

    //	102,4,operatorID - tell the App to remove it from the DB
    session_->send( OperatorDev, OpDtDelete, opId );

    // ok we just removed a row, so now we need to set the current index to request the deleted items neighbor.
    // Right now, the currentSelectedRow points to the item that was just deleted so update it
    if ( 0 < numOfOperators_ )
    {
        if ( 0 < currentSelectedRow_ )
            currentSelectedRow_--;                  // move up to the previous item if we can
        else
            currentSelectedRow_ = 0;                // drop down if we have to

        // get the id at the previous row
        QTableWidgetItem *idItem( ui_->tblOpIdsListBody->item( currentSelectedRow_, COL_ID ) );

        if ( idItem )
        {
            const QString selectedId( idItem->text() );

            // now select the new row
            ui_->tblOpIdsListBody->selectRow( currentSelectedRow_ );

            // send 102,0,id - read the next operator
            session_->send( OperatorDev, DtRead, selectedId );
        }
    }
    else
    {
        clearOperatorFields();                      // must be none left so clearn up the mess
        currentSelectedRow_ = -1;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::operatorIDEntered()
{
    if ( ignoreSignals_  )
        return;

    const QString newId( ui_->txtOperatorID->text() );
    const QString oldId( singleOperator_->operatorID() );

    if (( 0 < newId.length() ) && ( newId != oldId ))
    {
        saveCurrentOperator();

        session_->send( OperatorDev, DtRead, newId );   //	102,0,newOperator
        singleOperator_->operatorID() = newId;

        hasChanged_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::selectTheRow( int row, int col )
{
    Q_UNUSED( col );

    if ( ignoreSignals_ )
        return;
    else if (( row < 0 ) || ( numOfOperators_ < row ))
        return;

    saveCurrentOperator();

    // means we have a row already selected
    if ( row == currentSelectedRow_ )
    {
        ui_->tblOpIdsListBody->clearSelection();
        currentSelectedRow_ = -1;
    }
    else
    {
        ui_->tblOpIdsListBody->selectRow( row );
        currentSelectedRow_ = row;
    }

    if (( 0 <= currentSelectedRow_ ) && ( currentSelectedRow_ < numOfOperators_ ))
    {
        ui_->txtOperatorAccessLevel->setEnabled( false );
        ui_->txtOperatorName->setEnabled( false );
        ui_->cboPreferredLanguage->setEnabled( false );
        ui_->butDeleteButton->setEnabled( false );

        QTableWidgetItem *idItem( ui_->tblOpIdsListBody->item( currentSelectedRow_, COL_ID ) );

        if ( idItem )
        {
            const QString selectedId( idItem->text() );

            // send 102,0,id - read new selected operator
            session_->send( OperatorDev, DtRead, selectedId );
        }
    }
    else
    {
        clearOperatorFields();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::textboxChanged()
{
    if ( ignoreSignals_  )
        return;

    const QString newName( ui_->txtOperatorName->text() );
    const QString oldName( singleOperator_->operatorName() );

    // can't request the same thing again
    if ( nameEmitted_ == newName )
        return;

    nameEmitted_ = newName;

    if ( newName == oldName )
        return;

    // update class information
    singleOperator_->operatorNameRaw() = newName;

    // update information in cell
    QTableWidgetItem *nameItem( new QTableWidgetItem );
    nameItem->setText( newName );

    ui_->tblOpIdsListBody->setItem( currentSelectedRow_, COL_OPER_NAME, nameItem );

    hasChanged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::updatePreferredLangauge( int newIndex )
{
    if ( ignoreSignals_ )
        return;

    const QString tmpLangNum( ui_->cboPreferredLanguage->itemData( newIndex ).toString() );

    singleOperator_->preferredLanguageRaw() = tmpLangNum;

    // update information in cell
    QTableWidgetItem *item( ui_->tblOpIdsListBody->item( currentSelectedRow_, COL_LANG ) );

    if ( item )
    {
        const QString tmpLangName( getLanguageText( singleOperator_->preferredLanguage() ) );

        item->setText( tmpLangName );
    }

    hasChanged_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::initialize()
{    
    ui_->tblOpIdsListBody->clear();                                  // in case we are coming back in again - force a clear
    ui_->tblOpIdsListBody->setColumnCount( NUM_COLS );

    QSizePolicy sizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );
    sizePolicy.setHeightForWidth( ui_->tblOpIdsListBody->sizePolicy().hasHeightForWidth() );
    ui_->tblOpIdsListBody->setSizePolicy( sizePolicy );

    ui_->tblOpIdsListBody->setStyleSheet(
        QLatin1String(
            "QTableWidget{\n"
                "color: black;\n"
            "}\n"
            "\n"
            "QTableWidget::item{ \n"
                "selection-background-color: #00b0f0;\n"
            "}" ) );

    ui_->tblOpIdsListBody->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    ui_->tblOpIdsListBody->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    ui_->tblOpIdsListBody->setEditTriggers( QAbstractItemView::NoEditTriggers );
    ui_->tblOpIdsListBody->setSelectionMode( QAbstractItemView::SingleSelection );
    ui_->tblOpIdsListBody->setSelectionBehavior( QAbstractItemView::SelectRows );

    ui_->tblOpIdsListBody->horizontalHeader()->setHighlightSections( false );
    ui_->tblOpIdsListBody->horizontalHeader()->setStretchLastSection( true );

    ui_->tblOpIdsListBody->verticalHeader()->setVisible( false );
    ui_->tblOpIdsListBody->verticalHeader()->setCascadingSectionResizes( true );
    ui_->tblOpIdsListBody->verticalHeader()->setDefaultSectionSize( 50 );
    ui_->tblOpIdsListBody->verticalHeader()->setHighlightSections( false );
    ui_->tblOpIdsListBody->verticalHeader()->setMinimumSectionSize( 50 );
    ui_->tblOpIdsListBody->verticalHeader()->setStretchLastSection( false );

    ui_->cboPreferredLanguage->addItem( "", QString( "1" ) );
    ui_->cboPreferredLanguage->addItem( "", QString( "2" ) );

    // set the validator for access levels
    ui_->txtOperatorAccessLevel->setValidator( accessLevelValidator_ = new QIntValidator( 1, 9, this ) );
    ui_->txtOperatorAccessLevel->setDecimalPlaces( 0 );
    ui_->txtOperatorAccessLevel->setInputWrapping( true );

    ui_->lblPreferredLanguage->setVisible( handsFreeLoginEnabled_ );
    ui_->cboPreferredLanguage->setVisible( handsFreeLoginEnabled_ );

    ui_->chkForcedLogin->setChecked( scaleConfigData_->forcedLogin() );

    ui_->chkAutoCreateOper->setVisible( handsFreeLoginEnabled_ );
    ui_->chkAutoCreateOper->setChecked( scaleConfigData_->autoCreateRFIDOp() );

    connect(ui_->tblOpIdsListBody,       SIGNAL(cellClicked(int,int)),       SLOT(selectTheRow(int,int)));             // click cell
    connect(ui_->tblOpIdsListBody,       SIGNAL(cellDoubleClicked(int,int)), SLOT(selectTheRow(int,int)));             // click cell
    connect(ui_->cboPreferredLanguage,   SIGNAL(currentIndexChanged(int)),   SLOT(updatePreferredLangauge(int)));
    connect(ui_->butDeleteButton,        SIGNAL(clicked()),                  SLOT(deleteOperator()));
    connect(ui_->chkForcedLogin,         SIGNAL(stateChanged(int)),          SLOT(checkboxChanged(int)));
    connect(ui_->chkAutoCreateOper,      SIGNAL(stateChanged(int)),          SLOT(checkboxChanged(int)));
    connect(ui_->txtOperatorID,          SIGNAL(editingFinished()),          SLOT(operatorIDEntered()));
    connect(ui_->txtOperatorName,        SIGNAL(editingFinished()),          SLOT(textboxChanged()));
    connect(ui_->txtOperatorAccessLevel, SIGNAL(editingFinished()),          SLOT(checkAcccessLevel()));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::clearOperatorFields()
{
    singleOperator_->operatorID() = "";
    ui_->txtOperatorID->setText("");

    ui_->txtOperatorAccessLevel->setText("");
    ui_->txtOperatorAccessLevel->setEnabled( false );

    ui_->txtOperatorName->setText("");
    ui_->txtOperatorName->setEnabled( false );

    ui_->cboPreferredLanguage->blockSignals( true );
    ui_->cboPreferredLanguage->setCurrentIndex( 0 );
    ui_->cboPreferredLanguage->setEnabled( false );
    ui_->cboPreferredLanguage->blockSignals( false );

    ui_->butDeleteButton->setEnabled( false );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::requestToCreateNewOperator( int buttonAction )
{
    if ( buttonAction == OK_PRESSED )
    {
        const QString opId( ui_->txtOperatorID->text() );

        // send 102,12,id - create new operator
        session_->send( OperatorDev, OpDtCreateNew, opId );

        createNew_ = true;
    }
    else
    {
        ui_->tblOpIdsListBody->clearSelection();
        currentSelectedRow_ = -1;

        clearOperatorFields();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::insertOperatorRow( const QString& opId, const QString& opRFID )
{
    numOfOperators_++;

    // This is a new Operator
    ui_->tblOpIdsListBody->insertRow( 0 );                          // insert (create) a new row at the top
    ui_->tblOpIdsListBody->selectRow( 0 );                          // select the row
    currentSelectedRow_ = 0;

    QTableWidgetItem *idItem( new QTableWidgetItem );               // create a column an ID
    idItem->setText( opId );

    ui_->tblOpIdsListBody->setItem( 0, COL_ID, idItem );            // add the operator ID

    QTableWidgetItem *accessItem( new QTableWidgetItem );           // create a column an access level
    accessItem->setText( ui_->txtOperatorAccessLevel->text() );

    ui_->tblOpIdsListBody->setItem( 0, COL_ACCESS, accessItem );    // add the default access level

    QTableWidgetItem *langItem( new QTableWidgetItem );             // create a column the default language
    langItem->setText( getLanguageText( singleOperator_->preferredLanguage() ) );

    ui_->tblOpIdsListBody->setItem( 0, COL_LANG, langItem );        // add the default language

    if ( opRFID.length() )
    {
        QTableWidgetItem *tagItem( new QTableWidgetItem );              // create a column for tag
        tagItem->setText( opRFID );

        ui_->tblOpIdsListBody->setItem( 0, COL_TAG_ID, tagItem );       // add the tag
    }

    // we don't add a default name, they fill it out themselves
    hasChanged_ = true;
    createNew_ = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperOperatorIds::removeOperatorRow( const QString& opId )
{
    bool retVal( false );

    // only -1 if no row has been selected, which we cant delete if we dont select one
    if ( 0 <= currentSelectedRow_ )
    {
        QTableWidgetItem *nameItem( ui_->tblOpIdsListBody->item( currentSelectedRow_, COL_ID ) );

        if ( nameItem )
        {
            const QString selectedId( nameItem->text() );

            // sanity check, this better be true
            if ( selectedId == opId )
            {
                QTableWidgetItem *tagItem( ui_->tblOpIdsListBody->item( currentSelectedRow_, COL_TAG_ID ) );

                // if we are ignoring this tag lets start paying attention to it...
                if (( tagItem ) && ( ignored_.contains( tagItem->text() ) ))
                    ignored_.removeAll( tagItem->text() );

                // ---- //

                ui_->tblOpIdsListBody->removeRow( currentSelectedRow_ );

                numOfOperators_--;
                retVal = true;
            }
        }
    }

    return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::saveCurrentOperator()
{
    if ( !hasChanged_ )
        return;

    const QString opId( singleOperator_->operatorID() );

    if ( opId.length() > 0 )
    {
        QString msg;
        msg += opId + ",";
        msg += singleOperator_->operatorRFID() + ",";
        msg += "\"" + singleOperator_->operatorName() + "\",";
        msg += QString::number( singleOperator_->operatorAccessLevel() ) + ",";
        msg += QString::number( singleOperator_->preferredLanguage() );

        // send 102,6,msg - save current operator
        session_->send( OperatorDev, OpDtSave, msg );
    }

    hasChanged_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::updateSingleOperator( const DomSingleOperator& m )
{
    if ( singleOperator_ )
        *singleOperator_ = m;

    const QString opId( m.operatorID() );

    if ( opId.isEmpty() )
        clearOperatorFields();
    else
    {
        int lang( ui_->cboPreferredLanguage->findData( QString::number( m.preferredLanguage() ) ) );

        if ( lang < 0 )
            lang = 0;

        // update all of the text input fields
        ui_->txtOperatorAccessLevel->setEnabled( true );
        ui_->txtOperatorName->setEnabled( true );
        ui_->cboPreferredLanguage->setEnabled( true );
        ui_->butDeleteButton->setEnabled( true );

        ui_->txtOperatorAccessLevel->setText( QString::number( m.operatorAccessLevel() ) );
        ui_->txtOperatorName->setText( m.operatorName() );

        ui_->cboPreferredLanguage->blockSignals( true );
        ui_->cboPreferredLanguage->setCurrentIndex( lang );
        ui_->cboPreferredLanguage->blockSignals( false );

        ui_->txtOperatorID->setText( m.operatorID() );

        if ( createNew_ )
            insertOperatorRow( opId, m.operatorRFID() );

        // locate and select operator
        else
        {
            for ( int row( 0 ); row < ui_->tblOpIdsListBody->rowCount(); ++row )
            {
                QTableWidgetItem *item( ui_->tblOpIdsListBody->item( row, COL_ID ) );

                if (( item ) && ( item->text() == m.operatorID() ))
                {
                    ui_->tblOpIdsListBody->selectRow( row );
                    currentSelectedRow_ = row;

                    break;
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorIds::updateOperatorInformation( const DomOperatorInfomation& m )
{
    if ( operatorInformation_ )
        *operatorInformation_ = m;

    ignoreSignals_ = true;

    // remove all rows
    while ( ui_->tblOpIdsListBody->rowCount() )
        ui_->tblOpIdsListBody->removeRow( 0 );

    if ( (numOfOperators_ = operatorInformation_->numOfIDs()) )
    {
        const QStringList idArray( m.allOperatorIDs().split(",") );
        const QStringList accessArray( m.allAccess().split(",") );
        const QStringList langArray( m.allLanguages().split(",") );
        const QVector<QString> nameArray( TokenParser::splitStringTokens( m.allOperatorNames() ) );
        const QStringList tagArray( m.allOperatorRFIDs().split(",") );

        ui_->tblOpIdsListBody->setRowCount( numOfOperators_ );

        for ( int row( 0 ); row < numOfOperators_; row++ )
        {
            QTableWidgetItem *idItem( new QTableWidgetItem );
            idItem->setText( idArray.at( row ) );

            ui_->tblOpIdsListBody->setItem( row, COL_ID, idItem );

            QTableWidgetItem *accessItem( new QTableWidgetItem );
            accessItem->setText( accessArray.at( row ) );

            ui_->tblOpIdsListBody->setItem( row, COL_ACCESS, accessItem );

            QTableWidgetItem *langItem( new QTableWidgetItem );
            langItem->setText( getLanguageText( langArray.at( row ).toInt() ) );

            ui_->tblOpIdsListBody->setItem( row, COL_LANG, langItem );

            QTableWidgetItem *nameItem( new QTableWidgetItem );
            nameItem->setText( nameArray.at( row ) );

            ui_->tblOpIdsListBody->setItem( row, COL_OPER_NAME, nameItem );

            QTableWidgetItem *tagItem( new QTableWidgetItem );
            tagItem->setText( tagArray.at( row ) );

            ui_->tblOpIdsListBody->setItem( row, COL_TAG_ID, tagItem );
        }
    }

    ignoreSignals_ = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperOperatorIds::getLanguageText( int langNum ) const
{
    QString retLangText;

    QStringList countryLangArray;
    countryLangArray.append( "" );
    countryLangArray.append( scaleConfigData_->languageCode() );
    countryLangArray.append( scaleConfigData_->secLanguageCode() );

    if (( 0 <= langNum ) && ( langNum < countryLangArray.size() ))
    {
        const QString langAbbr( countryLangArray.at( langNum ) );

        if ( langAbbr == "en" )
            retLangText = tr("English");
        else if ( langAbbr == "es" )
            retLangText = tr("Spanish");
        else if ( langAbbr == "fr" )
            retLangText = tr("French");
    }

    return retLangText;
}
