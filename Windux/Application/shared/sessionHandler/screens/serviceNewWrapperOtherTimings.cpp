/* @file serviceNewWrapperOtherTimings.cpp
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

#include <logmanager.h>

#include "serviceNewWrapperOtherTimings.h"			//	Formerly serviceNewWrapperHiddenTimings
#include "ui_serviceNewWrapperOtherTimings.h"
#include "serviceNewWrapperSetTimings.h"
#include "serviceNewWrapperEditBox.h"
#include "serviceNewWrapperAdvancedTimings.h"

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

serviceNewWrapperOtherTimings::serviceNewWrapperOtherTimings(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperOtherTimings),
    wrTimingsData_(NULL)
{
    setObjectName( "serviceNewWrapperOtherTimings" );

//	Retrieve config object
    if ( mainWindow_->isServiceMode() )
		wrTimingsData_ = &dynamic_cast<ServiceMode*>( mode_ )->wrTimingsData_;

    ui->setupUi(this);
    scaleScreen();

    modifyPaletteHighlight setHighlight( hobart::colorPalette::blue );
    setHighlight( ui->tblBasicTimingsList );

    initialize();
    translate();
}

serviceNewWrapperOtherTimings::~serviceNewWrapperOtherTimings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::~serviceNewWrapperOtherTimings()";
#endif
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperOtherTimings::initialize()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::initialize()";
#endif
	firstPass = true;
    activeRow = wrTimingsData_->precedence;
    numOfItems = 0;
    boxCount = 0;
    wrTimingsData_->exitByMenu = true;      //  We'll set this to false when we change to another Timings page.
	filmRollRow = 0;

    viewCurrentTimings_ = wrTimingsData_->viewCurrentTimings;
    heightBoxSelected_ = wrTimingsData_->heightBoxSelected;

    cbo_FOEState_ = NULL;
    cbo_FilmRoll_ = NULL;
    le_HeightRestrict_ = NULL;
    le_SideClampGap_ = NULL;
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

    connect(ui->butCancel,            		SIGNAL(clicked()),              	this,       SLOT(cancelChanges()));

    connect(ui->butDone,              		SIGNAL(clicked()),              	this,       SLOT(doneWithChanges()));

    connect(ui->butAdvancedTimings,    		SIGNAL(clicked()),      			this,       SLOT(goToAdvancedTimings()));

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
    qDebug() << "serviceNewWrapperOtherTimings::initialize():  Done.";
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperOtherTimings::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperOtherTimings::translate()
{
    ui->retranslateUi( this );

    if (cbo_FOEState_)
    {
        cbo_FOEState_->setItemText(0, tr("Automatic"));
        cbo_FOEState_->setItemText(1, tr("True"));
    }

    if (cbo_FilmRoll_)
    {
        cbo_FilmRoll_->setItemText(0, tr("Narrow roll"));
        cbo_FilmRoll_->setItemText(1, tr("Wide roll"));
    }

    update();
}

bool serviceNewWrapperOtherTimings::isNGWWrapper() const
{
    return (DomEvolWrapperSystemInformation::nextGenModels <= wrTimingsData_->wrapperModelNum);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////



void serviceNewWrapperOtherTimings::writeHeightBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::writeHeightBox()";
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

                writeMsg += QString::number(value);
            }
            writeMsg += "\"";

            session_->send(Wrapper, WrEvolWriteHeightBox, writeMsg);              // 9,247,precedence,maxHeight,heightBoxes list
        }
	}
}

void serviceNewWrapperOtherTimings::writeWrapBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::writeWrapBox()";
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
int serviceNewWrapperOtherTimings::makeValidatorTop( int digits )
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::makeValidatorTop()";
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
void serviceNewWrapperOtherTimings::saveDataOnExit()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::saveDataOnExit():  wrTimingsData_->pageMode = " << QString::number(wrTimingsData_->pageMode);
#endif
    saveChanges();
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::saveDataOnExit():  Done.";
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void serviceNewWrapperOtherTimings::cycleWrapper()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::cycleWrapper()";
#endif

    cycleButtonStyle = ui->butCycleWrapper->styleSheet();
    ui->butCycleWrapper->setStyleSheet("background-color: red;");

    if (saveAnyChanges_)
    {
        //  Only save if we are viewing current timings - not default timings.
        writeWrapBox();
        writeHeightBox();

    	//	HA-81
		session_->send(Wrapper, WrEvolDoneWrapBox, QString::number( activeRow ));			// 9,244, precedence
    }

    session_->send(Wrapper, WrEvolTest, "24," + ui->le_PLUNumber->text());			// 9,108, WrapperCycle, PLU Exception
}

void serviceNewWrapperOtherTimings::cancelChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::cancelChanges()";
#endif
    saveAnyChanges_ = false;
    wrTimingsData_->exitByMenu = false;
    session_->send(Wrapper, WrEvolCancelWrapBoxChanges, QString::number( activeRow ));		// 9,243,precedence
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
}

void serviceNewWrapperOtherTimings::saveChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::saveChanges():  saveAnyChanges_ = " + QString::number(saveAnyChanges_);
#endif
    if (saveAnyChanges_)
    {
        writeWrapBox();
        writeHeightBox();
        session_->send(Wrapper, WrEvolDoneWrapBox, QString::number( activeRow ));		// 9,244,precedence
    }
}

void serviceNewWrapperOtherTimings::doneWithChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::doneWithChanges()";
#endif
//  Don't call this here as it will be called when switching pages.    saveDataOnExit();
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
}

void serviceNewWrapperOtherTimings::goToAdvancedTimings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::goToAdvancedTimings()";
#endif
	wrTimingsData_->pageMode = WrapperTimingsData::pmViewEdit;
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperAdvancedTimings>();
}

void serviceNewWrapperOtherTimings::goToBasicTimings()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::goToBasicTimings()";
#endif
	wrTimingsData_->pageMode = WrapperTimingsData::pmViewEdit;
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperEditBox>();
}

void serviceNewWrapperOtherTimings::requestHeightBox()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::requestHeightBox()";
#endif
	writeHeightBox();
    heightBoxSelected_ = ui->cbo_HeightBox->itemText(ui->cbo_HeightBox->currentIndex());
    session_->send(Wrapper, WrEvolReadHeightBox, QString::number(activeRow) + "," + heightBoxSelected_);
}

void serviceNewWrapperOtherTimings::onWidgetUpdated( QWidget *widget )
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::onWidgetUpdated()";
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

        if ( cbo_FOEState_ == widget )
            offset = WrapParameter::FORCE_OUTSIDE_ELEVATOR; //  4
        else if ( cbo_FilmRoll_ == widget )
            offset = WrapParameter::FILM_ROLL_TO_USE;   	//  14

        if (offset > -1)
        {
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
bool serviceNewWrapperOtherTimings::onScreenChanging()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::onScreenChanging()";
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

void serviceNewWrapperOtherTimings::buildWrapDimensions(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::buildWrapDimensions()";
#endif
    DomWrapBoxParams wrapBoxParams( msg );

    ui->le_TrayName->setText(wrTimingsData_->trayName);
    ui->le_PLUNumber->setText(wrTimingsData_->pluException);

    wrapBoxParams_ = new DomWrapBoxParams(wrapBoxParams);
	buildHeightCombo();
}

void serviceNewWrapperOtherTimings::buildHeightCombo()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::buildHeightCombo()";
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
            for (int index = 0; index < boxCount; index++)
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

void serviceNewWrapperOtherTimings::setupHeightBox(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::setupHeightBox()";
#endif
    DomHeightBox heightBox( msg );
    heightBox_ = new DomHeightBox(heightBox);

	selectHeight();
    setupHeightBoxParams();
}

void serviceNewWrapperOtherTimings::selectHeight()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::selectHeight()";
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

void serviceNewWrapperOtherTimings::buildParamTable()
{
    LOG_CLASS_INFO( "snwOtherTimings", "buildParamTable" );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::buildParamTable()";
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

    //  Force outside elevator up - Combo box
    int paramOffset = WrapParameter::FORCE_OUTSIDE_ELEVATOR;
    QTableWidgetItem *paramNameItem4 = new QTableWidgetItem;
    paramNameItem4->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem4->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem4);
    cbo_FOEState_ = new QComboBox;
    if (cbo_FOEState_)
	{
        cbo_FOEState_->setFont(cboFont);

	    cbo_FOEState_->addItem( tr("Automatic"), QString("0"));
	    cbo_FOEState_->addItem( tr("True"), QString("1"));

	    if (!viewCurrentTimings_)
	        cbo_FOEState_->setEnabled(false);
	    else
		{
		    connect( cbo_FOEState_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
	        widgetUpdatedSignalMapper_->setMapping( cbo_FOEState_, cbo_FOEState_ );
		}
	    ui->tblBasicTimingsList->setCellWidget(j++, 1, cbo_FOEState_);
	}

    //  Height restrict
    paramOffset = WrapParameter::HEIGHT_RESTRICT;
    QTableWidgetItem *paramNameItem10 = new QTableWidgetItem;
    paramNameItem10->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem10->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem10);
    le_HeightRestrict_ = createQSpinBox(paramOffset, HB_MIN_HEIGHT_RESTRICT, HB_MAX_HEIGHT_RESTRICT_AWS1_AND_2, 0);
	if (le_HeightRestrict_)
	    ui->tblBasicTimingsList->setCellWidget(j++, 1, le_HeightRestrict_);
/*
    //  Side clamp gap - HA-130:  Removed Feb. 2015. Gary says this parameter is not used. [CCD]
    paramOffset = WrapParameter::SIDE_CLAMP_GAP;
    QTableWidgetItem *paramNameItem13 = new QTableWidgetItem;
    paramNameItem13->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
    paramNameItem13->setFont(textFont);
    ui->tblBasicTimingsList->setItem(j, 0, paramNameItem13);
    le_SideClampGap_ = createQSpinBox(paramOffset, HB_MIN_SIDE_CLAMP_GAP, HB_MAX_SIDE_CLAMP_GAP, 0);
	if (le_SideClampGap_)
	    ui->tblBasicTimingsList->setCellWidget(j++, 1, le_SideClampGap_);
*/
    if (( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS2RL ) || ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS2LR ))
    {
        //  Film roll to use - Combo box
        paramOffset = WrapParameter::FILM_ROLL_TO_USE;
        QTableWidgetItem *paramNameItem14 = new QTableWidgetItem;
        paramNameItem14->setText( wrTimingsData_->wrapParameterList[paramOffset]->getName());
	    paramNameItem14->setFont(textFont);

        ui->tblBasicTimingsList->insertRow(j);  //  Insert this row since it's based on the wrapper model.
        ui->tblBasicTimingsList->setFixedHeight(ui->tblBasicTimingsList->maximumHeight());

        ui->tblBasicTimingsList->setItem(j, 0, paramNameItem14);
        cbo_FilmRoll_ = new QComboBox;
	    if (cbo_FilmRoll_)
		{
        	cbo_FilmRoll_->setFont(cboFont);

	        cbo_FilmRoll_->addItem( tr("Narrow roll"), QString("100"));
	        cbo_FilmRoll_->addItem( tr("Wide roll"), QString("200"));

		    if (!viewCurrentTimings_)
		        cbo_FilmRoll_->setEnabled(false);
		    else
			{
			    connect( cbo_FilmRoll_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
		        widgetUpdatedSignalMapper_->setMapping( cbo_FilmRoll_, cbo_FilmRoll_ );
			}
			filmRollRow = j;
			LOG_TRACE("filmRollRow = %d", filmRollRow);
	        ui->tblBasicTimingsList->setCellWidget(j++, 1, cbo_FilmRoll_);
	    }
    }
}

void serviceNewWrapperOtherTimings::setupHeightBoxParams()
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

            tmpParam->setData( value );
        }
    }
    //  Force outside elevator up - Combo box
    if (cbo_FOEState_)
	{
	    cbo_FOEState_->blockSignals(true);
	    cbo_FOEState_->setCurrentIndex( cbo_FOEState_->findData( paramArray.at(WrapParameter::FORCE_OUTSIDE_ELEVATOR) ));
	    cbo_FOEState_->blockSignals(false);
	}
  
    //  Height restrict
	if (le_HeightRestrict_)
        le_HeightRestrict_->setValue((paramArray.at(WrapParameter::HEIGHT_RESTRICT)).toInt());

/*	HA-130:  Removed Feb. 2015. Gary says this parameter is not used. [CCD]
    //  Side clamp gap
	if (le_SideClampGap_)
        le_SideClampGap_->setValue((paramArray.at(WrapParameter::SIDE_CLAMP_GAP)).toInt());
*/
    if (( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS2RL ) || ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS2LR ))
    {
        //  Film roll to use - Combo box
		if (cbo_FilmRoll_)
		{
		    cbo_FilmRoll_->blockSignals(true);
		    cbo_FilmRoll_->setCurrentIndex( cbo_FilmRoll_->findData( paramArray.at(WrapParameter::FILM_ROLL_TO_USE) ));
		    cbo_FilmRoll_->blockSignals(false);
		}
	}
}

QSpinBox* serviceNewWrapperOtherTimings::createQSpinBox(int offset, int minVal, int maxVal, QString currVal)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::createQSpinBox(" << QString::number(offset) << ", " << QString::number(minVal) << ", " << QString::number(maxVal) << ", " << currVal << ")";
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

int serviceNewWrapperOtherTimings::validateField(int oldValue, int newValue, int MinValue, int MaxValue)
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

void serviceNewWrapperOtherTimings::validateEntry(int offset, QSpinBox *intField)
{
    int valWas = wrTimingsData_->wrapParameterList[offset]->getData();
    int valIs = intField->value();

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::validateEntry():  offset = " << QString::number(offset) << ".  valWas: " << QString::number(valWas) << ",  valIs: " << QString::number(valIs);
#endif

    if (valWas != valIs)
    {
        if (offset == WrapParameter::HEIGHT_RESTRICT)
        {
			if (le_HeightRestrict_)
			{
                valIs = validateField(valWas, valIs, HB_MIN_HEIGHT_RESTRICT, HB_MAX_HEIGHT_RESTRICT_AWS1_AND_2);
                le_HeightRestrict_->setValue(valIs);
	        }
        }
/*	HA-130:  Removed Feb. 2015. Gary says this parameter is not used. [CCD]
        else if (offset == WrapParameter::SIDE_CLAMP_GAP)
        {
			if (le_SideClampGap_)
			{
				valIs = validateField(valWas, valIs, HB_MIN_SIDE_CLAMP_GAP, HB_MAX_SIDE_CLAMP_GAP);
                le_SideClampGap_->setValue(valIs);
			}
        }
*/
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::validateEntry():  wrTimingsData_->wrapParameterList[" << QString::number(offset) << "]->setData(" << QString::number(valIs) << ")";
#endif
        wrTimingsData_->wrapParameterList[offset]->setData(valIs);
    }
}

void serviceNewWrapperOtherTimings::updateWrapperPkgDescription(const SessionMessage& msg)
{
    LOG_CLASS_INFO( "snwOtherTimings", "updateWrapperPkgDescription" );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::updateWrapperPkgDescription()";
#endif
    DomEvolWrapperPkgDesc pkgDescription( msg );

	LOG_TRACE("filmRollRow = %d", filmRollRow);

    // SOF-240: disable yellow highlight
    //int wrModel = wrTimingsData_->wrapperModelNum;
    //if ( (wrModel > DomEvolWrapperSystemInformation::None) && (wrModel < DomEvolWrapperSystemInformation::endWrapperModels))
    //{
    //    if ( pkgDescription.modifiedNetStretch() == true )
    //    {
    //        ui->tblBasicTimingsList->item(filmRollRow, 0)->setBackgroundColor("yellow");
    //        cbo_FilmRoll_->setStyleSheet("background-color: yellow;");
    //    }
    //    else
    //    {
    //        ui->tblBasicTimingsList->item(filmRollRow, 0)->setBackgroundColor("white");
    //        cbo_FilmRoll_->setStyleSheet("background-color: white;");
    //    }
    //}

    ui->label_trayFound->setText( pkgDescription.trayName() );
}

void serviceNewWrapperOtherTimings::wrapperCycleComplete(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOtherTimings::wrapperCycleComplete()";
#endif
    DomEvolWrapperTestComplete m( msg );
    if (m.testNum().toInt() == 24)
        ui->butCycleWrapper->setStyleSheet(cycleButtonStyle);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperOtherTimings::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

#ifdef USE_QDEBUG
    if ((device != 145) && (device != 170) && !((device == 9) && (event == 51)))
        qDebug() << "serviceNewWrapperOtherTimings::onSessionMessage():  device = " << msg[0] << ",  event = " << msg[1];
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
}
