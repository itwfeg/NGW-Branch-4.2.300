/* @file serviceNewWrapperAdvancedTimings.cpp
 * Allows Service technicians to modify wrap box timings.
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 */
#if 0
#define USE_QDEBUG
#endif
#if 0
#define USE_MIN_MAX
#endif

#include "serviceNewWrapperAdvancedTimings.h"
#include "ui_serviceNewWrapperAdvancedTimings.h"
#include "serviceNewWrapperSetTimings.h"
#include "serviceNewWrapperEditBox.h"
#include "serviceNewWrapperOtherTimings.h"
#include "servicePrinterConfig.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/servicemode.h"

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>
#include <QSignalMapper>
#include <palette.h>
#include <backgroundwidgetstyles.h>
#include <numericlineedit.h>


namespace hobart{
    class numericLineEdit;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

serviceNewWrapperAdvancedTimings::serviceNewWrapperAdvancedTimings(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperAdvancedTimings),
    wrTimingsData_(NULL)
{
    setObjectName( "serviceNewWrapperAdvancedTimings" );

//	Retrieve config object
    if ( mainWindow_->isServiceMode() )
		wrTimingsData_ = &dynamic_cast<ServiceMode*>( mode_ )->wrTimingsData_;

    ui->setupUi(this);
    scaleScreen();

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    setHighlight( ui->tblBasicTimingsList );

    //--------------------------
    // Request a Printer Inquiry
    //--------------------------
    session_->send( Printer, PrReqInquire, "0" );

    initialize();
    translate();
}

serviceNewWrapperAdvancedTimings::~serviceNewWrapperAdvancedTimings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::~serviceNewWrapperAdvancedTimings()";
#endif
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperAdvancedTimings::initialize()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::initialize()";
#endif
	firstPass = true;
    activeRow = wrTimingsData_->precedence;
    numOfItems = 0;
    boxCount = 0;
    wrTimingsData_->exitByMenu = true;      //  We'll set this to false when we change to another Timings page.

    viewCurrentTimings_ = wrTimingsData_->viewCurrentTimings;
    heightBoxSelected_ = wrTimingsData_->heightBoxSelected;

    cbo_SUFReturnState_ = NULL;
    cbo_OGReleaseState_ = NULL;
    le_FrontUFAdjust_ = NULL;
    le_WrapSpeed_ = NULL;
    le_DischargeSpeed_ = NULL;
    le_SideUFAdjust_ = NULL;
    le_RearUFTrip_ = NULL;
    le_BrakeTiming_ = NULL;
    heightBox_ = NULL;

	if (viewCurrentTimings_)
        saveAnyChanges_ = true;
    else
        saveAnyChanges_ = false;

    cycleButtonStyle = "";

    connect(ui->cbo_HeightBox,              SIGNAL(currentIndexChanged(int)),   this,       SLOT(requestHeightBox()));

    if ( !mainWindow_->isPrimarySessionType() )
        ui->butCycleWrapper->setEnabled(false);
    else
        connect(ui->butCycleWrapper,		SIGNAL(clicked()),              	this,       SLOT(cycleWrapper()));

    if (viewCurrentTimings_)
	    connect(ui->butSaveChanges,    		SIGNAL(clicked()),      			this,       SLOT(saveChanges()));

    connect(ui->butCancel,                 	SIGNAL(clicked()),                  this,       SLOT(cancelChanges()));

    connect(ui->butDone,                   	SIGNAL(clicked()),                  this,       SLOT(doneWithChanges()));

    connect(ui->butOtherTimings,           	SIGNAL(clicked()),                  this,       SLOT(goToOtherTimings()));

    connect(ui->butBasicTimings, 			SIGNAL(clicked()),      			this,       SLOT(goToBasicTimings()));

    ui->le_TrayName->setEnabled(false);
    ui->le_PLUNumber->setEnabled(false);

    ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
    if (viewCurrentTimings_)
    {
        modifyPaletteBackground setEditBoxBackground( ( QApplication::palette() ).background());
        setEditBoxBackground( serviceMode );
    }
    else
    {
        modifyPaletteBackground setDefaultTimingsBackground( hobart::colorPalette::red );
        setDefaultTimingsBackground( serviceMode );
    }

    buildParamTable();

    update();

    session_->send(Wrapper, WrEvolReadWrapBox, QString::number( activeRow ));              // 9,240,precedence

    if ( wrTimingsData_->heightBoxSelected != "" )
    {
        session_->send(Wrapper, WrEvolReadHeightBox, QString::number( activeRow ) + "," + wrTimingsData_->heightBoxSelected);              // 9,246,precedence
    }
    if ( wrTimingsData_->updateActualPkg )
    {
        ui->label_trayFound->setText(wrTimingsData_->cycledTrayName);
    }

    if (!viewCurrentTimings_)
    {
        ui->butSaveChanges->hide();
    }
    cycleButtonStyle = ui->butCycleWrapper->styleSheet();	//	Get the original style of this button.

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::initialize():  Done.";
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperAdvancedTimings::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperAdvancedTimings::translate()
{
    ui->retranslateUi( this );

    if (cbo_SUFReturnState_)
    {
        cbo_SUFReturnState_->setItemText(0, tr("Early"));
        cbo_SUFReturnState_->setItemText(1, tr("Normal"));
        cbo_SUFReturnState_->setItemText(2, tr("Late"));
    }

    if (cbo_OGReleaseState_)
    {
        cbo_OGReleaseState_->setItemText(0, tr("Pulling"));
        cbo_OGReleaseState_->setItemText(1, tr("Pulling and braked"));
        cbo_OGReleaseState_->setItemText(2, tr("Pull complete"));
        cbo_OGReleaseState_->setItemText(3, tr("Underfolder underfolding"));
    }

    update();
}

bool serviceNewWrapperAdvancedTimings::isNGWWrapper() const
{
    return (DomEvolWrapperSystemInformation::nextGenModels <= wrTimingsData_->wrapperModelNum);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void serviceNewWrapperAdvancedTimings::writeHeightBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::writeHeightBox()";
#endif
    if (wrTimingsData_->pageMode != WrapperTimingsData::pmAutoConfigure)
	{
        if( heightBox_ )//HA-127 this variable wasn't initialized when attempting to send the message
        {
            QString writeMsg = QString::number(activeRow);
            writeMsg += "," + heightBox_->maxHeight();
            writeMsg += "," + QString::number(wrTimingsData_->wrapParameterList.count());
            writeMsg += ",\"";
            for (int j = 0; j < wrTimingsData_->wrapParameterList.count(); j++)
            {
                if (j > 0)
                    writeMsg += ",";
                int value( wrTimingsData_->wrapParameterList[j]->getData() );

                if ( ( ( WrapParameter::FRONT_UF_ADJUST == j ) && ( isNGWWrapper() ) ) ||
                     ( ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == j ) && ( isNGWWrapper() ) )
                )
                {
                    value += 100;
                }

                writeMsg += QString::number( value );
            }
            writeMsg += "\"";

            session_->send(Wrapper, WrEvolWriteHeightBox, writeMsg);              // 9,247,precedence,maxHeight,heightBoxes list
        }
	}
}

void serviceNewWrapperAdvancedTimings::writeWrapBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::writeWrapBox()";
#endif
    QString writeMsg = QString::number(activeRow);
    writeMsg += ",\"" + ui->le_TrayName->text() + "\"";
    writeMsg += "," + ui->le_PLUNumber->text();
    writeMsg += ",";	// + ui->le_MinLength->text();
    writeMsg += ",";	// + ui->le_MaxLength->text();
    writeMsg += ",";	// + ui->le_MinWidth->text();
    writeMsg += ",";	// + ui->le_MaxWidth->text();
    writeMsg += ",";	// + ui->le_MaxLip->text();
    writeMsg += "," + QString::number(ui->cbo_HeightBox->count());
    writeMsg += ",\"";
    for (int j = 0; j < ui->cbo_HeightBox->count(); j++)
    {
        if (j > 0)
            writeMsg += ",";
        writeMsg += ui->cbo_HeightBox->itemData(j).toString();
    }
    writeMsg += "\"";

    session_->send(Wrapper, WrEvolWriteWrapBox, writeMsg);              // 9,241,precedence,trayName,...
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int serviceNewWrapperAdvancedTimings::makeValidatorTop( int digits )
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::makeValidatorTop()";
#endif
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperAdvancedTimings::saveDataOnExit()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::saveDataOnExit():  wrTimingsData_->pageMode = " << QString::number(wrTimingsData_->pageMode);
#endif
    saveChanges();
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::saveDataOnExit():  Done.";
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void serviceNewWrapperAdvancedTimings::cycleWrapper()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::cycleWrapper()";
#endif

    ui->butCycleWrapper->setStyleSheet("background-color: red;");

    if (saveAnyChanges_)
    {
        //  Only save if we are viewing current timings - not default timings.
        writeWrapBox();
        writeHeightBox();

    	//	HA-81
    	session_->send(Wrapper, WrEvolDoneWrapBox, QString::number( activeRow ));			// 9,244, precedence
    }


    //send test label to apply
    QString printer = "0,";
    QString testLabel = "2,";
    QString qauntity = "0";
    QString label = printer + testLabel + qauntity;
    session_->send( Printer, PrTestLabel, label );

    QString verifyMsg = QString::number( activeRow ) + "," + heightBoxSelected_;        	//	HA-135
    session_->send(Wrapper, WrEvolVerifyInHeightBox, verifyMsg);
    session_->send(Wrapper, WrEvolTest, "24," + ui->le_PLUNumber->text());					// 9,108, WrapperCycle, PLU Exception
}

void serviceNewWrapperAdvancedTimings::cancelChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::cancelChanges()";
#endif
    saveAnyChanges_ = false;
    wrTimingsData_->exitByMenu = false;
    session_->send(Wrapper, WrEvolCancelWrapBoxChanges, QString::number( activeRow ));		// 9,243,precedence
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
}

void serviceNewWrapperAdvancedTimings::saveChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::saveChanges():  saveAnyChanges_ = " + QString::number(saveAnyChanges_);
#endif
    if (saveAnyChanges_)
    {
        writeWrapBox();
        writeHeightBox();
        session_->send(Wrapper, WrEvolDoneWrapBox, QString::number( activeRow ));		// 9,244,precedence
    }
}

void serviceNewWrapperAdvancedTimings::doneWithChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::doneWithChanges()";
#endif
//  Don't call this here as it will be called when switching pages.    saveDataOnExit();
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
}

void serviceNewWrapperAdvancedTimings::goToOtherTimings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::goToOtherTimings()";
#endif
	wrTimingsData_->pageMode = WrapperTimingsData::pmViewEdit;
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperOtherTimings>();
}

void serviceNewWrapperAdvancedTimings::goToBasicTimings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::goToBasicTimings()";
#endif
	wrTimingsData_->pageMode = WrapperTimingsData::pmViewEdit;
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperEditBox>();
}

void serviceNewWrapperAdvancedTimings::requestHeightBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::requestHeightBox()";
#endif
	writeHeightBox();
    heightBoxSelected_ = ui->cbo_HeightBox->itemText(ui->cbo_HeightBox->currentIndex());
    session_->send(Wrapper, WrEvolReadHeightBox, QString::number(activeRow) + "," + heightBoxSelected_);
}

void serviceNewWrapperAdvancedTimings::onWidgetUpdated( QWidget *widget )
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::onWidgetUpdated()";
#endif

    QComboBox *comboBox;
    QSpinBox *spinBox;

    //
    // combo boxes
    //

    if ( (comboBox = dynamic_cast<QComboBox*>( widget )) )
    {
        int offset = -1;
        int itemData( comboBox->itemData( comboBox->currentIndex() ).toInt() );

        if ( cbo_SUFReturnState_ == widget )
            offset = WrapParameter::SIDE_UF_RETURN;   				//  11
        else if ( cbo_OGReleaseState_ == widget )
            offset = WrapParameter::OUTSIDE_GRIPPER_RELEASE;   	//  17

        if (offset > -1)
        {
//            int isNow = wrTimingsData_->wrapParameterList[offset]->getData();
            wrTimingsData_->wrapParameterList[offset]->setData(itemData);
        }
    }

    //
    // spin box edits
    //
    else if ( (spinBox = dynamic_cast<QSpinBox*>( widget )) )
    {
        validateEntry( offsetMap_[widget], dynamic_cast<QSpinBox*>(widget) );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperAdvancedTimings::onScreenChanging()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::onScreenChanging()";
#endif

    if (wrTimingsData_->exitByMenu)
        return false;
    else
    {
        wrTimingsData_->heightBoxSelected = heightBoxSelected_;     //  Save the current height box.
        saveDataOnExit();
    }

    return _Mybase::onScreenChanging();
}

void serviceNewWrapperAdvancedTimings::buildWrapDimensions(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::buildWrapDimensions()";
#endif
    DomWrapBoxParams wrapBoxParams( msg );

    ui->le_TrayName->setText(wrTimingsData_->trayName);
    ui->le_PLUNumber->setText(wrTimingsData_->pluException);

    wrapBoxParams_ = new DomWrapBoxParams(wrapBoxParams);
	buildHeightCombo();
}

void serviceNewWrapperAdvancedTimings::buildHeightCombo()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::buildHeightCombo()";
#endif
    if (wrapBoxParams_)
    {
        boxCount = wrapBoxParams_->numHeightBoxes().toInt();
        if (boxCount > 0)
        {
            int index = 0;

            //  Block the signals on these combo boxes until they are set up.
            ui->cbo_HeightBox->blockSignals(true);

            ui->cbo_HeightBox->clear();

            //  Add each Height to the combo box
            QVector<QString> heightArray = TokenParser::splitStringTokens( wrapBoxParams_->heightBoxes() );
            for (index = 0; index < boxCount; index++)
            {
                ui->cbo_HeightBox->addItem(heightArray.at(index), heightArray.at(index));
            }

            //	Select the current height box
            if (heightBoxSelected_.isEmpty())
                index = 0;
            else
                index = ui->cbo_HeightBox->findText(heightBoxSelected_);
            ui->cbo_HeightBox->setCurrentIndex(index);

            ui->cbo_HeightBox->setEnabled(true);

            ui->cbo_HeightBox->blockSignals(false);
        }
    }
}

void serviceNewWrapperAdvancedTimings::setupHeightBox(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::setupHeightBox()";
#endif
    DomHeightBox heightBox( msg );
    heightBox_ = new DomHeightBox(heightBox);

	selectHeight();
    setupHeightBoxParams();
}

void serviceNewWrapperAdvancedTimings::selectHeight()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::selectHeight()";
#endif
    int index = -1;
    if (boxCount > 0)
    {
        ui->cbo_HeightBox->blockSignals(true);

        index = ui->cbo_HeightBox->findData( heightBox_->maxHeight() );
        if (index >= 0)
        {
            ui->cbo_HeightBox->setCurrentIndex(index);
        }

        ui->cbo_HeightBox->blockSignals(false);
    }
}

void serviceNewWrapperAdvancedTimings::buildParamTable()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::buildParamTable()";
#endif
    QFont cboFont = font();
    cboFont.setPointSize( 20 );		//	HA-87
    QFont textFont = font();
    textFont.setPointSize( 20 );

    ui->tblBasicTimingsList->setColumnCount(2);
    ui->tblBasicTimingsList->setColumnWidth(0,510);
    ui->tblBasicTimingsList->setColumnWidth(1,410);

    widgetUpdatedSignalMapper_ = new QSignalMapper( this );
    connect( widgetUpdatedSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onWidgetUpdated(QWidget*) ) );

    int j = 0;

    ui->tblBasicTimingsList->clear();

    //  Front underfolder adjust
    int paramOffset = WrapParameter::FRONT_UF_ADJUST;
    QTableWidgetItem *paramNameItem6 = new QTableWidgetItem;
    paramNameItem6->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem6->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem6);
    le_FrontUFAdjust_ = createQSpinBox(paramOffset, HB_MIN_FRONT_ADJ, HB_MAX_FRONT_ADJ, 0);
    if (le_FrontUFAdjust_)
		ui->tblBasicTimingsList->setCellWidget(j++, 1, le_FrontUFAdjust_);

    //  Wrap speed
    paramOffset = WrapParameter::WRAP_SPEED;
    QTableWidgetItem *paramNameItem7 = new QTableWidgetItem;
    paramNameItem7->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem7->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem7);
    le_WrapSpeed_ = createQSpinBox(paramOffset, HB_MIN_WRAP_SPEED_AWS1_AND_2, HB_MAX_WRAP_SPEED_AWS1_AND_2, 0);
    if (le_WrapSpeed_)
		ui->tblBasicTimingsList->setCellWidget(j++, 1, le_WrapSpeed_);

    //  Discharge speed
    paramOffset = WrapParameter::DISCHARGE_SPEED;
    QTableWidgetItem *paramNameItem8 = new QTableWidgetItem;
    paramNameItem8->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem8->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem8);
    le_DischargeSpeed_ = createQSpinBox(paramOffset, HB_MIN_DISCHARGE_SPEED, HB_MAX_DISCHARGE_SPEED_AWS1_AND_2, 0);
    if (le_DischargeSpeed_)
    	ui->tblBasicTimingsList->setCellWidget(j++, 1, le_DischargeSpeed_);

    //  Side underfolder adjust
    paramOffset = WrapParameter::SIDE_UF_ADJUST;
    QTableWidgetItem *paramNameItem9 = new QTableWidgetItem;
    paramNameItem9->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem9->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem9);
    le_SideUFAdjust_ = createQSpinBox(paramOffset, HB_MIN_SIDE_FOLD_ADJ_AWS1_AND_2, HB_MAX_SIDE_FOLD_ADJ_AWS1_AND_2, 0);
    if (le_SideUFAdjust_)
        ui->tblBasicTimingsList->setCellWidget(j++, 1, le_SideUFAdjust_);

    //  Side underfolder return home - Combo box
    paramOffset = WrapParameter::SIDE_UF_RETURN;
    QTableWidgetItem *paramNameItem11 = new QTableWidgetItem;
    paramNameItem11->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem11->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem11);

    cbo_SUFReturnState_ = new QComboBox;
    if (cbo_SUFReturnState_)
	{
        cbo_SUFReturnState_->setFont(cboFont);

	    cbo_SUFReturnState_->addItem( tr("Early"), QString("0"));
	    cbo_SUFReturnState_->addItem( tr("Normal"), QString("1"));
	    cbo_SUFReturnState_->addItem( tr("Late"), QString("2"));

	    if (!viewCurrentTimings_)
	        cbo_SUFReturnState_->setEnabled(false);
	    else
		{
	        connect(cbo_SUFReturnState_,	SIGNAL(currentIndexChanged(int)),   widgetUpdatedSignalMapper_, SLOT( map() ));
	        widgetUpdatedSignalMapper_->setMapping( cbo_SUFReturnState_, cbo_SUFReturnState_ );
	    }
	    ui->tblBasicTimingsList->setCellWidget(j++, 1, cbo_SUFReturnState_);
	}

    //  Rear underfolder trip delay
    paramOffset = WrapParameter::REAR_UF_TRIP_DELAY;
    QTableWidgetItem *paramNameItem15 = new QTableWidgetItem;
    paramNameItem15->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem15->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem15);
    le_RearUFTrip_ = createQSpinBox(paramOffset, HB_MIN_REAR_TRIP_DELAY, HB_MAX_REAR_TRIP_DELAY_AWS1_AND_2, 0);
    if (le_RearUFTrip_)
	    ui->tblBasicTimingsList->setCellWidget(j++, 1, le_RearUFTrip_);

    //  Brake timing
    paramOffset = WrapParameter::BRAKE_TIMING;
    QTableWidgetItem *paramNameItem16 = new QTableWidgetItem;
    paramNameItem16->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem16->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem16);
    le_BrakeTiming_ = createQSpinBox(paramOffset, HB_MIN_BRAKE_TIME, HB_MAX_BRAKE_TIME_AWS1_AND_2, 0);
    if (le_BrakeTiming_)
	    ui->tblBasicTimingsList->setCellWidget(j++, 1, le_BrakeTiming_);

    //  Outside gripper release - Combo box
    paramOffset = WrapParameter::OUTSIDE_GRIPPER_RELEASE;
    QTableWidgetItem *paramNameItem17 = new QTableWidgetItem;
    paramNameItem17->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem17->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem17);

    cbo_OGReleaseState_ = new QComboBox;
    if (cbo_OGReleaseState_)
	{
        cbo_OGReleaseState_->setFont(cboFont);

	    cbo_OGReleaseState_->addItem( tr("Pulling"), QString("4"));
	    cbo_OGReleaseState_->addItem( tr("Pulling and braked"), QString("5"));
	    cbo_OGReleaseState_->addItem( tr("Pull complete"), QString("6"));
	    cbo_OGReleaseState_->addItem( tr("Underfolder underfolding"), QString("7"));

	    if (!viewCurrentTimings_)
	        cbo_OGReleaseState_->setEnabled(false);
	    else
		{
	        connect(cbo_OGReleaseState_,	SIGNAL(currentIndexChanged(int)),   widgetUpdatedSignalMapper_, SLOT( map() ));
	        widgetUpdatedSignalMapper_->setMapping( cbo_OGReleaseState_, cbo_OGReleaseState_ );
		}
	    ui->tblBasicTimingsList->setCellWidget(j++, 1, cbo_OGReleaseState_);
	}
}

void serviceNewWrapperAdvancedTimings::setupHeightBoxParams()
{
    int j = 0;

    QVector<QString> paramArray = TokenParser::splitStringTokens( heightBox_->wrapAttributes());

    //  First, update our wrapParameterList with the height box params
    for (j = 0; j < wrTimingsData_->wrapParameterList.count(); j++)
    {
        WrapParameter *tmpParam = wrTimingsData_->wrapParameterList[j];
        if (tmpParam)
        {
            int value( paramArray[j].toInt() );
            if ( ( ( WrapParameter::FRONT_UF_ADJUST == j ) && ( isNGWWrapper() ) && ( 100 <= value ) ) ||
                 ( ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == j ) && ( isNGWWrapper() ) && ( 100 <= value ) )
            )
            {
                paramArray[j] = QString::number( value -= 100 );
            }

            tmpParam->setData(value);
        }
    }
    //  Front underfolder adjust
    if (le_FrontUFAdjust_)
        le_FrontUFAdjust_->setValue((paramArray.at(WrapParameter::FRONT_UF_ADJUST)).toInt());

    //  Wrap speed
    if (le_WrapSpeed_)
        le_WrapSpeed_->setValue((paramArray.at(WrapParameter::WRAP_SPEED)).toInt());

    //  Discharge speed
    if (le_DischargeSpeed_)
        le_DischargeSpeed_->setValue(paramArray.at((WrapParameter::DISCHARGE_SPEED)).toInt());

    //  Side underfolder adjust
    if (le_SideUFAdjust_)
        le_SideUFAdjust_->setValue(paramArray.at((WrapParameter::SIDE_UF_ADJUST)).toInt());

    //  Side underfolder return home - Combo box
    if (cbo_SUFReturnState_)
	{
	    cbo_SUFReturnState_->blockSignals(true);
	    cbo_SUFReturnState_->setCurrentIndex( cbo_SUFReturnState_->findData( paramArray.at(WrapParameter::SIDE_UF_RETURN) ));
	    cbo_SUFReturnState_->blockSignals(false);
	}

    //  Rear underfolder trip delay
    if (le_RearUFTrip_)
        le_RearUFTrip_->setValue((paramArray.at(WrapParameter::REAR_UF_TRIP_DELAY)).toInt());

    //  Brake timing
    if (le_BrakeTiming_)
        le_BrakeTiming_->setValue((paramArray.at(WrapParameter::BRAKE_TIMING)).toInt());

    //  Outside gripper release - Combo box
    if (cbo_OGReleaseState_)
	{
	    cbo_OGReleaseState_->blockSignals(true);
	    cbo_OGReleaseState_->setCurrentIndex( cbo_OGReleaseState_->findData(paramArray.at(WrapParameter::OUTSIDE_GRIPPER_RELEASE)) );
	    cbo_OGReleaseState_->blockSignals(false);
	}
}

QSpinBox* serviceNewWrapperAdvancedTimings::createQSpinBox(int offset, int minVal, int maxVal, QString currVal)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::createQSpinBox(" << QString::number(offset) << ", " << QString::number(minVal) << ", " << QString::number(maxVal) << ", " << currVal << ")";
#endif
    QSpinBox *integerField = new QSpinBox( this );
    if (integerField)
    {
        integerField->setObjectName( "param" + offset );
        integerField->setStyleSheet("background-color: white; font: 20pt \"MS Shell Dlg 2\";");

        integerField->setRange(minVal, maxVal);		//	HA-129: Set up the integerField regardless of Current or Default settings.
        integerField->setValue(currVal.toInt());
        integerField->setButtonSymbols(QAbstractSpinBox::NoButtons);

        if (viewCurrentTimings_)
        {
            connect(integerField, SIGNAL( editingFinished() ), widgetUpdatedSignalMapper_, SLOT( map() ) );
            widgetUpdatedSignalMapper_->setMapping( integerField, integerField );
        }
        else
        {
            integerField->setEnabled(false);
        }

        offsetMap_[integerField] = offset;
    }
    return integerField;
}

int serviceNewWrapperAdvancedTimings::validateField(int oldValue, int newValue, int MinValue, int MaxValue)
{
    if (oldValue != newValue)
    {
#ifdef USE_MIN_MAX
        if (newValue < MinValue)
            newValue = MinValue;
        else if (newValue > MaxValue)
            newValue = MaxValue;
#else
        if ((newValue < MinValue) || (newValue > MaxValue))
            newValue = oldValue;
#endif
	}
	return newValue;
}

void serviceNewWrapperAdvancedTimings::validateEntry(int offset, QSpinBox *intField)
{
    int valWas = wrTimingsData_->wrapParameterList[offset]->getData();
    int valIs = intField->value();

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::validateEntry():  offset = " << QString::number(offset) << ".  valWas: " << QString::number(valWas) << ",  valIs: " << QString::number(valIs);
#endif

    if (valWas != valIs)
    {
        if (offset == WrapParameter::FRONT_UF_ADJUST)
        {
            if (le_FrontUFAdjust_)
			{
				valIs = validateField(valWas, valIs, HB_MIN_FRONT_ADJ, HB_MAX_FRONT_ADJ);
                le_FrontUFAdjust_->setValue(valIs);
			}
        }
        else if (offset == WrapParameter::WRAP_SPEED)
        {
		    if (le_WrapSpeed_)
			{
                valIs = validateField(valWas, valIs, HB_MIN_WRAP_SPEED_AWS1_AND_2, HB_MAX_WRAP_SPEED_AWS1_AND_2);
                le_WrapSpeed_->setValue(valIs);
			}
        }
        else if (offset == WrapParameter::DISCHARGE_SPEED)
        {
		    if (le_DischargeSpeed_)
			{
                valIs = validateField(valWas, valIs, HB_MIN_DISCHARGE_SPEED, HB_MAX_DISCHARGE_SPEED_AWS1_AND_2);
                le_DischargeSpeed_->setValue(valIs);
			}
        }
        else if (offset == WrapParameter::SIDE_UF_ADJUST)
        {
		    if (le_SideUFAdjust_)
			{
                valIs = validateField(valWas, valIs, HB_MIN_SIDE_FOLD_ADJ_AWS1_AND_2, HB_MAX_SIDE_FOLD_ADJ_AWS1_AND_2);
                le_SideUFAdjust_->setValue(valIs);
			}
        }
        else if (offset == WrapParameter::REAR_UF_TRIP_DELAY)
        {
		    if (le_RearUFTrip_)
			{
                valIs = validateField(valWas, valIs, HB_MIN_REAR_TRIP_DELAY, HB_MAX_REAR_TRIP_DELAY_AWS1_AND_2);
                le_RearUFTrip_->setValue(valIs);
			}
        }
        else if (offset == WrapParameter::BRAKE_TIMING)
        {
		    if (le_BrakeTiming_)
			{
                valIs = validateField(valWas, valIs, HB_MIN_BRAKE_TIME, HB_MAX_BRAKE_TIME_AWS1_AND_2);
                le_BrakeTiming_->setValue(valIs);
			}
        }
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::validateEntry():  wrTimingsData_->wrapParameterList[" << QString::number(offset) << "]->setData(" << QString::number(valIs) << ")";
#endif
        wrTimingsData_->wrapParameterList[offset]->setData(valIs);
    }
}

void serviceNewWrapperAdvancedTimings::updateWrapperPkgDescription(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::updateWrapperPkgDescription()";
#endif
    DomEvolWrapperPkgDesc pkgDescription( msg );

    if (wrTimingsData_->pageMode != WrapperTimingsData::pmAutoConfigure)
    {
        ui->label_trayFound->setText( pkgDescription.trayName() );
    }

    if ((wrTimingsData_->pageMode == WrapperTimingsData::pmAutoConfigure) || (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect))
    {
        if (firstPass == true)
        {
            if (wrTimingsData_->pageMode == WrapperTimingsData::pmAutoDetect)
            {
                activeRow = pkgDescription.wrapBoxIndex().toInt();
                ui->butCycleWrapper->setEnabled(true);      //  TODO: What about a remote connection?
                ui->le_TrayName->setEnabled(true);
                ui->le_PLUNumber->setEnabled(true);
            }
            firstPass = false;
        }
    }
}

void serviceNewWrapperAdvancedTimings::wrapperCycleComplete(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperAdvancedTimings::wrapperCycleComplete()";
#endif
    DomEvolWrapperTestComplete m( msg );
    if (m.testNum().toInt() == 24)
        ui->butCycleWrapper->setStyleSheet(cycleButtonStyle);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperAdvancedTimings::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

#ifdef USE_QDEBUG
    if ((device != 145) && (device != 170) && !((device == 9) && (event == 51)))
        qDebug() << "serviceNewWrapperAdvancedTimings::onSessionMessage():  device = " << msg[0] << ",  event = " << msg[1];
#endif
    if (( Wrapper == device ) && ( WrEvolWriteWrapBox == event ))
    {
        // Received 9,241 - Update the Basic timings info / Height box info for this tray
        buildWrapDimensions(msg);
    }
    else if (( Wrapper == device ) && ( WrEvolWriteHeightBox == event ))
    {
        // Received 9,247 - Update the Basic timings info / Height box info for this tray
        setupHeightBox(msg);
    }
    else if (( Wrapper == device ) && ( WrEvolPackageDesc == event ))
    {
        // Received 9,125 - Update the Basic timings info / Height box info for this tray
        updateWrapperPkgDescription(msg);
    }
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ))
    {
        // Received 9,165 - Test complete, or for these pages, the Wrapper cycle is complete
        wrapperCycleComplete(msg);
    }
    else if (( Printer == device ) && ( PrInquire == event ))
    {
    	DomPrinterConfigRead m( msg );
    }


}
