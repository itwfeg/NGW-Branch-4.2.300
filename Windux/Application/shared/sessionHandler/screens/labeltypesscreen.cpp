/**
 * @file labeltypesscreen.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "labeltypesscreen.h"

#include <font.h>
#include <labelviewerwidget.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>
#include <UiUtilities.h>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <logmanager.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelTypesScreen::LabelTypesScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    ScaleConfiguration *config = session_->scaleConfig;
    noMinorVersion = false;
    hasSecondPrinter_ = false;

    // initialize screen
    initialize();
    translate();

    // retrieve label units
    labelUnits_ = (session_->scaleConfig->getScaleIsMetric() ? "mm" : "\"");

    // request stock metrics
    session_->send( Printer, PrReqStockMetrics, "0" );                  // 7,40,0 for primary printer (existing)

    if ( 0 < session_->scaleConfig->peripheralList.secondPrinter() )
        session_->send( Printer, PrReqStockMetrics, "1" );              // 7,40,1 for secondary printer (existing)

    if ( 0 < session_->scaleConfig->peripheralList.thirdPrinter() )
        session_->send( Printer, PrReqStockMetrics, "2" );              // 7,40,2 for third printer (existing)

    // request system strings
    config->dataStore_.store<systemString>()->clear();
    session_->send( GenericTextDev, AppletViewerReadAll );

    // request first label
    config->dataStore_.store<label>()->clear();
    config->labelMetaDataMap_.clear();
    session_->send( LabelTypeDev, LabelTypeDtReadAll, ",1" );

    // request the pre-printed labels configuration
    session_->send( ConfigurationDev, CfgPrepackRead ); // update ui (sends back a CfgPrepackWrite)
}

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelTypesScreen::~LabelTypesScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelTypesScreen::translate()
{
    int index;

    labelType_->setText( tr( "Label type" ) );
    previousButton_->setText( tr( "Previous" ) );
    nextButton_->setText( tr( "Next" ) );
    deleteButton_->setText( tr( "Delete" ) );
    versionType_->setText(tr("Label version: "));

    selectedLabel_->setTitle( tr( "Selected label size" ) );

    printer_->setText( tr( "Printer" ) );

    printerCombo_->setItemText( 0, tr( "Primary" ) );

    if ( 0 <= (index = printerCombo_->findData( 1 )) )
        printerCombo_->setItemText( index, tr( "Secondary" ) );

    if ( 0 <= (index = printerCombo_->findData( 2 )) )
        printerCombo_->setItemText( index, tr( "Third" ) );

    loadedStock_->setTitle( tr( "Loaded stock size" ) );

    //rotationOnLabelStockLabel_->setText( tr( "Rotation on label stock" ) );

}


///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelTypesScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "LabelTypesScreen", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // label stock metrics
    if (( Printer == device ) && ( PrStockMetrics == event ))
    {
        DomPrinterStockMetrics m( msg );

        UiUtilities *util = session_->uiUtilities;

        // get stock width and height
        QString width = m.stockWidth();
        util->youGottaSetTheSeparator( width );

        QString height = m.stockLength();

        // set the label text
        QString metrics;

        if ( "continuous" == height )
            metrics = width + labelUnits_ + " W x " + tr( "Continuous" ) + " H";
        else
        {
            util->youGottaSetTheSeparator( height );
            metrics = width + labelUnits_ + " W x " + height + labelUnits_ + " H";
        }


        labelMetrics_[m.printerType()] = metrics;

        if ( m.printerType() == printerCombo_->itemData( printerCombo_->currentIndex() ).toInt() )
            loadedStockSize_->setText( metrics );

        if( 1 == m.printerType() )
            hasSecondPrinter_ = true;
    }

    // labels
    else if (( LabelTypeDev == device ) && ( LabelTypeDtWrite == event ))
    {
        DomLabelType m( msg );

        // SOF-1869: If invalid label type id (i.e. "0") because no more label
        //           type id's exist or there is no label data then just exit
        if( m.id() == "0" || m.data() == "" )
            return;

        bool firstLabel = (0 == labelTypeCombo_->count());

        // update label combo list
        if ( !updateLabelTypeCombo() )
        {
            // got a label (or so session says...) but label did not make it into data store or
            // is duplicate... either bad label or we are done...
            LOG_DEBUG( "done requesting labels count %d", labelTypeCombo_->count() );
        }
        else
        {

            // for first label we recieve make it the highlighted one
            if ( firstLabel )
            {
                labelTypeCombo_->setCurrentIndex( 0 );
            }

            // request next label so long as we are not getting duplicates
            //session_->send( LabelTypeDev, LabelTypeDtNext, m.id() + ",1" );
        }
    }
    else if(ConfigurationDev == msg[0].toInt() && CfgPrepackWrite == msg[1].toInt())
    {
        MadpOperatorDisplayFields m(msg);

        //LOG_INFO( "Received Primary Pre-Print: %d", m.priLabelStockRotated() );
        LOG_INFO( "Received Secondary Stock Rotated: %d",  m.secLabelStockRotated() );

		// set the label rotation value if needed
        if( hasSecondPrinter_ )
        {
            cbSecondLabelPreprint_->setEnabled( true );
            cbSecondLabelPreprint_->setChecked( m.secLabelStockRotated() == 0 ? false : true );
        }

        // cbPrimaryLabelPreprint_->setEnabled( true );
        // cbPrimaryLabelPreprint_->setChecked( m.priLabelStockRotated() == 0 ? false : true );

        // cbSecondLabelPreprint_->setEnabled( true );
        // cbSecondLabelPreprint_->setChecked( m.secLabelStockRotated() == 0 ? false : true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelTypesScreen::onLabelIndexChanged( const QString& value )
{
    LOG_CLASS_INFO( "LabelTypesScreen", "onLabelIndexedChanged" );
    ScaleConfiguration *config = session_->scaleConfig;
    UiUtilities *util = session_->uiUtilities;

    int id = value.toInt();

    // set the viewer label
    const label l = config->dataStore_.store<label>()->value( id );

    QString formattedVersion = "N/A";

    if(l.customerVersion() != "N/A")
    {
        formattedVersion = l.customerVersion();
    }

    versionTypeValue_->setText(formattedVersion);
    labelViewer_->setLabel( l);

    // retieve label meta data
    ScaleConfiguration::metaData metaData = config->labelMetaDataMap_[id];

    QString width = metaData.width;
    util->youGottaSetTheSeparator( width );

    QString height = metaData.height;
    util->youGottaSetTheSeparator( height );

    // set label dimensions
    selectedLabelSize_->setText( width + labelUnits_ + " W x " + height + labelUnits_ + " H" );

    //if ( metaData.custom )
    //{
    //    rotationOnLabelStockCombo_->setCurrentIndex( metaData.rotationOnStock.toInt() );
    //    rotationOnLabelStockCombo_->setDisabled(false);
    //}
    //else
    //{
    //    rotationOnLabelStockCombo_->setCurrentIndex( 0 );
    //    rotationOnLabelStockCombo_->setDisabled(true);
    //}

    // set previous and next button enabled
    previousButton_->setEnabled( 1 < labelTypeCombo_->count() );
    nextButton_->setEnabled( 1 < labelTypeCombo_->count() );

    // set delete button enabled
    deleteButton_->setEnabled( metaData.custom );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelTypesScreen::onPreviousButtonClicked()
{
    int index = labelTypeCombo_->currentIndex() - 1;

    if ( 0 <= index )
        labelTypeCombo_->setCurrentIndex( index );
    else
        labelTypeCombo_->setCurrentIndex( labelTypeCombo_->count() - 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelTypesScreen::onNextButtonClicked()
{
    int index = labelTypeCombo_->currentIndex() + 1;

    if ( index < labelTypeCombo_->count() )
        labelTypeCombo_->setCurrentIndex( index );
    else
        labelTypeCombo_->setCurrentIndex( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelTypesScreen::onDeleteButtonClicked()
{
    LOG_CLASS_INFO( "LabelTypesScreen", "onDeleteButtonClicked" );

    // validate something selected
    if ( 0 > labelTypeCombo_->currentIndex() )
        return;

    QString labelTypeId = labelTypeCombo_->currentText();

    // delete!!
    session_->send( LabelTypeDev, LabelTypeDtDelete, labelTypeCombo_->currentText() );
    LOG_DEBUG( "Deleted label type: %s", qPrintable( labelTypeId ) );

    // remove from combo
    labelTypeCombo_->removeItem( labelTypeCombo_->currentIndex() );

    // SOF-1869: Request the label just deleted, in case you are not in the
    //           default dept and there is a custom label or ROM from the default dept
    session_->send( LabelTypeDev, LabelTypeDtRead, labelTypeId + ",1" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelTypesScreen::onPrinterIndexChanged( const QString& value )
{
    Q_UNUSED( value );

    int printer( printerCombo_->itemData( printerCombo_->currentIndex() ).toInt() );

    if ( labelMetrics_.contains( printer ) )
        loadedStockSize_->setText( labelMetrics_[printer] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelTypesScreen::initialize()
{
    ScaleConfiguration *config = session_->scaleConfig;

    prePrintStateChanged = false;

    modifyFontPointSize setFontPointSize10( scaleFontPointSizeForScreen( 10 ) );

    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;

    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    screenLayout->setMargin( marginSpacing_ );

    ///////////////////////////////////////////////////////////////////////////
    // label navigation

    labelType_ = new QLabel( this );
    labelTypeCombo_ = new QComboBox( this );
    previousButton_ = new QPushButton( this );
    nextButton_ = new QPushButton( this );
    deleteButton_ = new QPushButton( this );
    versionType_ = new QLabel(this);
    versionTypeValue_ = new QLabel(this);

    // buttons disabled until we have data
    previousButton_->setEnabled( false );
    nextButton_->setEnabled( false );
    deleteButton_->setEnabled( false );

    connect( labelTypeCombo_, SIGNAL( currentIndexChanged(QString) ), this, SLOT( onLabelIndexChanged(QString) ) );
    connect( previousButton_, SIGNAL( clicked() ), this, SLOT( onPreviousButtonClicked() ) );
    connect( nextButton_, SIGNAL( clicked() ), this, SLOT( onNextButtonClicked() ) );
    connect( deleteButton_, SIGNAL( clicked() ), this, SLOT( onDeleteButtonClicked() ) );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget( labelType_ );
    horizontalLayout->addWidget( labelTypeCombo_ );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( versionType_);
    horizontalLayout->addWidget( versionTypeValue_);
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( previousButton_ );
    horizontalLayout->addWidget( nextButton_ );
    horizontalLayout->addWidget( deleteButton_ );
    screenLayout->addLayout( horizontalLayout );

    ///////////////////////////////////////////////////////////////////////////
    // label navigation

    labelViewer_ = new LabelViewerWidget( true, this );
    setFontPointSize10( labelViewer_ );

    // set the viewer data store
    labelViewer_->setStore( &config->dataStore_ );

    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->addStrut( 250 );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setContentsMargins(15,0,0,0);
    horizontalLayout->addWidget( labelViewer_ );
    horizontalLayout->addLayout( infoLayout );

    screenLayout->addLayout( horizontalLayout );

    ///////////////////////////////////////////////////////////////////////////
    // label and stock info

    // label info
    selectedLabel_ = new QGroupBox( this );
    selectedLabelSize_ = new QLabel( selectedLabel_ );

    verticalLayout = new QVBoxLayout( selectedLabel_ );
    verticalLayout->addWidget( selectedLabelSize_ );

    // stock info
    loadedStock_ = new QGroupBox( this );
    printer_ = new QLabel( loadedStock_ );
    printerCombo_ = new QComboBox( loadedStock_ );
    loadedStockSize_ = new QLabel( loadedStock_ );

    printerCombo_->addItem( tr( "Primary" ), 0 );

    if ( 0 < session_->scaleConfig->peripheralList.secondPrinter() )
        printerCombo_->addItem( tr( "Secondary" ), 1 );

    if ( 0 < session_->scaleConfig->peripheralList.thirdPrinter() )
        printerCombo_->addItem( tr( "Third" ), 2 );

    printerCombo_->setCurrentIndex( 0 );

    connect( printerCombo_, SIGNAL( currentIndexChanged(QString) ), this, SLOT( onPrinterIndexChanged(QString) ) );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget( printer_ );
    horizontalLayout->addWidget( printerCombo_ );

    //rotationOnLabelStockLabel_ = new QLabel( loadedStock_ );
    //rotationOnLabelStockCombo_ = new QComboBox( loadedStock_ );
    //rotationOnLabelStockCombo_->addItem( tr( "Right-side up" ), 0 );
    //rotationOnLabelStockCombo_->addItem( tr( "Up-side down" ), 1 );
    //rotationOnLabelStockCombo_->setCurrentIndex( 0 );

    //cbPrimaryLabelPreprint_ = new QCheckBox( loadedStock_ );
    //cbPrimaryLabelPreprint_->setText(tr( " Primary label pre-printed" ) );
    //cbPrimaryLabelPreprint_->setEnabled( false );
    //connect( rotationOnLabelStockCombo_, SIGNAL( currentIndexChanged(int) ), this, SLOT(onRotationChanged(int) ) );
    //connect(cbPrimaryLabelPreprint_,     SIGNAL(clicked()                 ), this, SLOT(prePrintStateChange()  ) );
    //if ( 0 < session_->scaleConfig->peripheralList.secondPrinter() )
    //    printerCombo_->addItem( tr( "Secondary" ), 1 );
    //infoLayout->addWidget( rotationOnLabelStockLabel_ );
    //infoLayout->addWidget( rotationOnLabelStockCombo_ );
    //infoLayout->addWidget( cbPrimaryLabelPreprint_ );

    cbSecondLabelPreprint_ = new QCheckBox( loadedStock_ );
    cbSecondLabelPreprint_->setText(tr( "Rotate secondary label 180°" ) );
    cbSecondLabelPreprint_->setEnabled( false );                // disable until we get msg back

    connect(cbSecondLabelPreprint_,      SIGNAL(clicked()                 ), this, SLOT(prePrintStateChange()  ) );

    verticalLayout = new QVBoxLayout( loadedStock_ );
    verticalLayout->addLayout( horizontalLayout );
    verticalLayout->addWidget( loadedStockSize_ );

    infoLayout->addWidget( selectedLabel_ );
    infoLayout->addWidget( loadedStock_ );
    
    infoLayout->addWidget( cbSecondLabelPreprint_ );

    // Only add the label rotation for the 2 Printer System
    // TODO: For now do not add these at all
    /*
    if( session_->scaleConfig->isWrapperTwoPrinter() )
    {
        infoLayout->addWidget( cboSpacer_ );
        infoLayout->addWidget( cbPrimaryLabelPreprint_ );
        infoLayout->addWidget( cbSecondLabelPreprint_ );
    }
    */
    infoLayout->addStretch();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelTypesScreen::updateLabelTypeCombo()
{
    LOG_CLASS_INFO( "LabelTypesScreen", "updateLabelTypeCombo" );
    ScaleConfiguration *config = session_->scaleConfig;

    labelContainer *labels = config->dataStore_.store<label>();
    bool result = false;
    LOG_DEBUG( "after label datastore", NULL );

    // add missing labels to combo dropdown
    for ( labelContainer::const_iterator i = labels->begin(); i != labels->end(); ++i )
    {
        QString id( QString::number( i->id() ) );

        LOG_DEBUG( "Label Type id=%s", qPrintable(id) );

        if ( 0 > labelTypeCombo_->findText( id ) )
        {
            if( labelTypeCombo_->count() == 0 )
            {
                // Just add the first one in the list
                labelTypeCombo_->addItem( id );
            }
            else
            {
                bool done = false;

                for( int loopCtr = ( labelTypeCombo_->count() - 1 ); loopCtr >= 0 && !done; --loopCtr )
                {
                    LOG_DEBUG( "LabelTypeID at this offset =|%d|", labelTypeCombo_->itemText( loopCtr ).toInt() );

                    // SOF-1869: Insert the new label type into the combo box in sorted order
                    if( id.toInt() > labelTypeCombo_->itemText( loopCtr ).toInt() )
                    {
                        labelTypeCombo_->insertItem( ( loopCtr + 1) , id );
                        done = true;

                        LOG_DEBUG( "Combobox offset to insert label type: %d", ( loopCtr + 1 ) );
                    }
                }
            }

            // set previous and next button enabled
            previousButton_->setEnabled( 1 < labelTypeCombo_->count() );
            nextButton_->setEnabled( 1 < labelTypeCombo_->count() );

            // added label!
            result = true;
        }
    }

    return result;
}


//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void LabelTypesScreen::onRotationChanged(int index)
{
    int labelId = labelTypeCombo_->currentText().toInt();

    // retrieve label meta data
    ScaleConfiguration *config = session_->scaleConfig;
    ScaleConfiguration::metaData metaData = config->labelMetaDataMap_[labelId];

    if ( metaData.custom && metaData.rotationOnStock != QString::number(index) )
    {
        metaData.rotationOnStock = QString::number(index);

        config->labelMetaDataMap_.remove(labelId);
        config->labelMetaDataMap_.insert( labelId, metaData );

        if (metaData.custom)
        {
            QString msg = QString(labelTypeCombo_->currentText()) + ",," + QString::number(index);
            session_->send( LabelTypeDev, 6, msg);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool LabelTypesScreen::onScreenChanging()
{
    // once we leave the screen see if the checkboxes changed
    savePreprintConfig();

    return _Mybase::onScreenChanging();
}

void LabelTypesScreen::prePrintStateChange()
{
    prePrintStateChanged = true;
}

void LabelTypesScreen::savePreprintConfig()
{
    LOG_CLASS_INFO("LabelTypesScreen", "savePreprintConfig()");

    if(prePrintStateChanged)
    {
        // ConfigurationDev,CfgPrepackWrite -> 112,141
        MadpOperatorDisplayFields *operatorDisplayFields = &session_->scaleConfig->operatorDisplayFields;

        operatorDisplayFields->setSecLabelStockRotated( cbSecondLabelPreprint_->isChecked() );

        LOG_INFO( "Sending Secondary Rotation: %d", cbSecondLabelPreprint_->isChecked() );

        session_->send( *operatorDisplayFields );

        session_->send(ConfigurationDev,CfgScaleSave);                  // save  (112,22)

        prePrintStateChanged = false;
    }
}
