/* @file serviceNewWrapperOneHeightBoxParam.cpp
 * Allows Service technicians to modify wrap box timings.
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 */
#if 0
#define USE_QDEBUG
#endif
#if 0
#define TEST_WRAPPER_OPERATION
#endif

#include <logmanager.h>

#include "serviceNewWrapperOneHeightBoxParam.h"
#include "ui_serviceNewWrapperOneHeightBoxParam.h"
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

serviceNewWrapperOneHeightBoxParam::serviceNewWrapperOneHeightBoxParam(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperOneHeightBoxParam),
    wrTimingsData_(NULL)
{
    setObjectName( "serviceNewWrapperOneHeightBoxParam" );

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

serviceNewWrapperOneHeightBoxParam::~serviceNewWrapperOneHeightBoxParam()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::~serviceNewWrapperOneHeightBoxParam()";
#endif
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperOneHeightBoxParam::initialize()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::initialize()";
#endif
    firstPass = true;
    activeRow = wrTimingsData_->precedence;
    numOfItems = 0;
    boxCount = 0;
    wrTimingsData_->exitByMenu = true;      //  We'll set this to false when we change to another Timings page.

    currentParam = 0;

    viewCurrentTimings_ = wrTimingsData_->viewCurrentTimings;

    if (viewCurrentTimings_)
        saveAnyChanges_ = true;
    else
        saveAnyChanges_ = false;

    cycleWrapperInProgress_ = false;
    cycleButtonStyle = "";

#ifdef TEST_WRAPPER_OPERATION
        connect(ui->butCycleWrapper,		SIGNAL(clicked()),                  this,       SLOT(cycleWrapper()));
#else
    if ( !mainWindow_->isPrimarySessionType() )
        ui->butCycleWrapper->setEnabled(false);
    else
        connect(ui->butCycleWrapper,		SIGNAL(clicked()),              	this,       SLOT(cycleWrapper()));
#endif

    if (viewCurrentTimings_)
        connect(ui->butSaveChanges,    		SIGNAL(clicked()),      			this,       SLOT(saveChanges()));

    connect(ui->butCancel,            		SIGNAL(clicked()),              	this,       SLOT(cancelChanges()));

    connect(ui->butDone,              		SIGNAL(clicked()),              	this,       SLOT(doneWithChanges()));

    connect(ui->cbo_TrayList,               SIGNAL(currentIndexChanged(int)),   this,       SLOT(trayChanged()));

    connect(ui->cbo_HeightBoxParameter,     SIGNAL(currentIndexChanged(int)),   this,       SLOT(parameterChanged()));

    ui->lineEdit_PLUNumber->setEnabled(false);

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

    if ( wrTimingsData_->updateActualPkg )
    {
        ui->label_trayFound->setText(wrTimingsData_->cycledTrayName);
    }

    buildParameterList();

    update();

    buildTrayList();		//	We already have the tray list. No need to request it again.

    requestParameterInfo();

    //  TODO:   checkViewDefaults();

    //	Set this up once, as the table may constantly change.
    ui->tblBasicTimingsList->setColumnCount(2);
    ui->tblBasicTimingsList->setColumnWidth(0,460);
    ui->tblBasicTimingsList->setColumnWidth(1,460);

    widgetUpdatedSignalMapper_ = new QSignalMapper( this );
    connect( widgetUpdatedSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onWidgetUpdated(QWidget*) ) );

    if (!viewCurrentTimings_)
    {
        ui->butSaveChanges->hide();
    }
    heightBox_ = NULL;
    cycleButtonStyle = ui->butCycleWrapper->styleSheet();	//	Get the original style of this button.
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperOneHeightBoxParam::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperOneHeightBoxParam::translate()
{
    ui->retranslateUi( this );

    QComboBox *comboBoxPtr;
    QMap<QWidget*, int>::const_iterator myIter = offsetMap_.constBegin();
    WrapParameter::wrTimingsFieldType fieldType = wrTimingsData_->wrapParameterList.at(currentParam)->getType();

    if (fieldType == WrapParameter::typeBoolean)
    {
        while ( myIter != offsetMap_.constEnd() )
        {
            comboBoxPtr = (QComboBox *) myIter.key();
            if (comboBoxPtr)
            {
                comboBoxPtr->setItemText(0, tr("False"));
                comboBoxPtr->setItemText(1, tr("True"));
            }
            ++myIter;
        }
    }
    else if (fieldType == WrapParameter::typeFilmRollToUse)
    {
        while ( myIter != offsetMap_.constEnd() )
        {
            comboBoxPtr = (QComboBox *) myIter.key();
            if (comboBoxPtr)
            {
                comboBoxPtr->setItemText(0, tr("Narrow roll"));
                comboBoxPtr->setItemText(1, tr("Wide roll"));
            }
            ++myIter;
        }
    }
    else if (fieldType == WrapParameter::typeForceOutsideElevatorSelect)
    {
        while ( myIter != offsetMap_.constEnd() )
        {
            comboBoxPtr = (QComboBox *) myIter.key();
            if (comboBoxPtr)
            {
                comboBoxPtr->setItemText(0, tr("Automatic"));
                comboBoxPtr->setItemText(1, tr("True"));
            }
            ++myIter;
        }
    }
    else if (fieldType == WrapParameter::typeOutsideGripperReleaseStates)
    {
        while ( myIter != offsetMap_.constEnd() )
        {
            comboBoxPtr = (QComboBox *) myIter.key();
            if (comboBoxPtr)
            {
                comboBoxPtr->setItemText(0, tr("Pulling"));
                comboBoxPtr->setItemText(1, tr("Pulling and braked"));
                comboBoxPtr->setItemText(2, tr("Pull complete"));
                comboBoxPtr->setItemText(3, tr("Underfolder underfolding"));
            }
            ++myIter;
        }
    }
    else if (fieldType == WrapParameter::typeSideUnderfolderReturnHomeStates)
    {
        while ( myIter != offsetMap_.constEnd() )
        {
            comboBoxPtr = (QComboBox *) myIter.key();
            if (comboBoxPtr)
            {
                comboBoxPtr->setItemText(0, tr("Early"));
                comboBoxPtr->setItemText(1, tr("Normal"));
                comboBoxPtr->setItemText(2, tr("Late"));
            }
            ++myIter;
        }
    }

    update();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////


int serviceNewWrapperOneHeightBoxParam::makeValidatorTop( int digits )
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::makeValidatorTop()";
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
void serviceNewWrapperOneHeightBoxParam::saveDataOnExit()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::saveDataOnExit()";
#endif
    saveChanges();
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::saveDataOnExit():  Done.";
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


void serviceNewWrapperOneHeightBoxParam::cycleWrapper()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::cycleWrapper()";
#endif
    if ( cycleWrapperInProgress_ )
        return;

    ui->butCycleWrapper->setStyleSheet("background-color: red;");

    if (saveAnyChanges_)
    {
        //  Only save if we are viewing current timings - not default timings.
//		writeWrapBox();
//  	writeHeightBox();

        //	HA-81
//    	session_->send(Wrapper, WrEvolDoneWrapBox, QString::number( activeRow ));			// 9,244, precedence
        session_->send(Wrapper, WrEvolDoneAttributeChanges);          						// 9,196
    }

#ifndef TEST_WRAPPER_OPERATION
    if ( heightBox_ )
    {
        QString verifyMsg = QString::number( activeRow ) + "," + heightBox_->maxHeight();   //	HA-135
        session_->send(Wrapper, WrEvolVerifyInHeightBox, verifyMsg);
    }
    session_->send(Wrapper, WrEvolTest, "24," + ui->lineEdit_PLUNumber->text());			// 9,108, WrapperCycle, PLU Exception
    cycleWrapperInProgress_ = true;
#else
    ui->butCycleWrapper->setStyleSheet("background-color: white;");
#endif
}

void serviceNewWrapperOneHeightBoxParam::cancelChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::cancelChanges()";
#endif
    saveAnyChanges_ = false;
    wrTimingsData_->exitByMenu = false;
    session_->send(Wrapper, WrEvolCancelAttributeChanges);          // 9,195
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
}

void serviceNewWrapperOneHeightBoxParam::saveChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::saveChanges():  saveAnyChanges_ = " + QString::number(saveAnyChanges_);
#endif
    if (saveAnyChanges_)
    {
        session_->send(Wrapper, WrEvolDoneAttributeChanges);          // 9,196
    }
}

void serviceNewWrapperOneHeightBoxParam::doneWithChanges()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::doneWithChanges()";
#endif
//  Don't call this here as it will be called when switching pages.    saveDataOnExit();
    wrTimingsData_->exitByMenu = false;
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
}

void serviceNewWrapperOneHeightBoxParam::requestParameterInfo()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::requestParameterInfo()";
#endif
    session_->send(Wrapper, WrEvolReadAllForAttribute, QString::number(activeRow) + ",," + QString::number(currentParam));
}

void serviceNewWrapperOneHeightBoxParam::trayChanged()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::trayChanged()";
#endif
    saveChanges();

    activeRow = ui->cbo_TrayList->currentIndex();
    wrTimingsData_->precedence = activeRow;
    requestParameterInfo();
}

void serviceNewWrapperOneHeightBoxParam::parameterChanged()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::parameterChanged()";
#endif
    saveChanges();

    currentParam = ui->cbo_HeightBoxParameter->itemData(ui->cbo_HeightBoxParameter->currentIndex()).toInt();
    requestParameterInfo();
}

void serviceNewWrapperOneHeightBoxParam::onWidgetUpdated( QWidget *widget )
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::onWidgetUpdated()";
#endif

    QComboBox *comboBox;
    hobart::numericLineEdit *numericLineEdit;
    QSpinBox *spinBox;

    //
    // combo boxes
    //

    if ( (comboBox = dynamic_cast<QComboBox*>( widget )) )
    {
        comboBoxChanged( offsetMap_[comboBox], comboBox );
    }

    //
    // numeric line edits
    //

    else if ( (numericLineEdit = dynamic_cast<hobart::numericLineEdit*>( widget )) )
    {
        validateEntry( offsetMap_[widget], dynamic_cast<hobart::numericLineEdit*>(widget));
    }

    //
    // spin box edits
    //
    else if ( (spinBox = dynamic_cast<QSpinBox*>( widget )) )
    {
        validateSpinBox( offsetMap_[widget], dynamic_cast<QSpinBox*>(widget) );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperOneHeightBoxParam::onScreenChanging()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::onScreenChanging()";
#endif

    if (wrTimingsData_->exitByMenu)
        return false;
    else
    {
        saveDataOnExit();
    }

    return _Mybase::onScreenChanging();
}

void serviceNewWrapperOneHeightBoxParam::buildTrayList()
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::buildTrayList()";
#endif
    ui->cbo_TrayList->blockSignals(true);

    ui->cbo_TrayList->clear();
    DomNewWrapperAllTrays *domList = wrTimingsData_->trayList;		//	We already have this list.

    int myItems = domList->numOfTrays().toInt();

    if (myItems > 0)
    {
        QVector<QString> trayTypeArray = TokenParser::splitStringTokens( domList->trayTypes() );
        for (int i = 0; i < myItems; i++)
        {
            ui->cbo_TrayList->addItem(trayTypeArray.at(i), i);
        }
    }
    ui->cbo_TrayList->setCurrentIndex(activeRow);

    ui->cbo_TrayList->blockSignals(false);
}

void serviceNewWrapperOneHeightBoxParam::buildParameterList()
{
    LOG_CLASS_INFO( "serviceNewWrapperOneHeightBoxParam", "buildParameterList" );

    LOG_DEBUG( "entry", NULL );

    // setup dropdown ordering for simplified timings
    QMap<int, int> simplifiedTimingsOrder;
    unsigned int order( 0 );

    simplifiedTimingsOrder[WrapParameter::DISTANCE] = order++;

/* Should be same order as the edit timings page
    if( isNGWWrapper() )
    {
        simplifiedTimingsOrder[WrapParameter::FORCE_OUTSIDE_ELEVATOR] = order++;
    }
*/
    simplifiedTimingsOrder[WrapParameter::GRIPPER_ADJUST] = order++;
    simplifiedTimingsOrder[WrapParameter::REAR_UF_TRIP_DELAY] = order++;
    simplifiedTimingsOrder[WrapParameter::HEIGHT_RESTRICT] = order++;
    simplifiedTimingsOrder[WrapParameter::BRAKE_TIMING] = order++;
    simplifiedTimingsOrder[WrapParameter::PERCENT_STRETCH_CLAMP_ON] = order++;
    simplifiedTimingsOrder[WrapParameter::CLAMP_HEIGHT] = order++;
    simplifiedTimingsOrder[WrapParameter::SIDE_UF_ADJUST] = order++;

    if ( isDualRollWrapper() )
        simplifiedTimingsOrder[WrapParameter::FILM_ROLL_TO_USE] = order++;

    if ( isNGWWrapper() )
    {
        simplifiedTimingsOrder[WrapParameter::FORCE_OUTSIDE_ELEVATOR] = order++;
        simplifiedTimingsOrder[WrapParameter::FRONT_UF_ADJUST] = order++;
        simplifiedTimingsOrder[WrapParameter::PRIMARY_LABEL_VERT_OFFSET] = order++;
        simplifiedTimingsOrder[WrapParameter::SECONDARY_LABEL_VERT_OFFSET] = order++;
    }

    simplifiedTimingsOrder[WrapParameter::WRAP_SPEED] = order++;
    simplifiedTimingsOrder[WrapParameter::DISCHARGE_SPEED] = order++;

    // generate parameter list
    typedef QPair<QString, int> parameter;
    QMap<int, parameter> parameters;

    for ( int i( wrTimingsData_->wrapParameterList.count() ); i--; )
    {
        bool skipMe( false );
        int index( i );

        // Film roll to use
        if ( WrapParameter::FILM_ROLL_TO_USE == i )
            skipMe = !isDualRollWrapper();

        //  This is an unused parameter that we don't want in the list.
        //	Not used. Leave out.
        else if (( WrapParameter::UNUSED_12 == i ) || ( WrapParameter::SIDE_CLAMP_GAP == i ))
            skipMe = true;
        else if( ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == i ) && !session_->scaleConfig->isWrapperTwoPrinter() )
            skipMe = true;
        else if ( isUsingSimplifiedTimings() )
        {
            skipMe = !isAWSEnhancedWrapParameter( i );

            if ( simplifiedTimingsOrder.contains( i ) )
                index = simplifiedTimingsOrder[i];
            else
                index += 10;
        }

        if ( skipMe )
            continue;

        parameters[index] = parameter( wrTimingsData_->wrapParameterList[i]->getName(), i );
    }

    // create parameter list
    ui->cbo_HeightBoxParameter->blockSignals( true );
    ui->cbo_HeightBoxParameter->clear();

    foreach ( const parameter &p, parameters )
        ui->cbo_HeightBoxParameter->addItem( p.first, p.second );

    ui->cbo_HeightBoxParameter->blockSignals( false );

    LOG_DEBUG( "exit", NULL );
}

bool serviceNewWrapperOneHeightBoxParam::isUsingSimplifiedTimings() const
{
    //int model = wrTimingsData_->wrapperModelNum;
    // is model <= reducedTimingModels(10)

    if( DomEvolWrapperSystemInformation::reducedTimingModels <= wrTimingsData_->wrapperModelNum )
        return true;
    else
        return false;
}

//Pass in a height box parameter and it will tell you whether the parameter is part of the AWSEnhanced timings
bool serviceNewWrapperOneHeightBoxParam::isAWSEnhancedWrapParameter( int wrapParam ) const
{
    if( ( wrapParam == WrapParameter::DISTANCE ) ||
            ( wrapParam == WrapParameter::GRIPPER_ADJUST ) ||
            ( wrapParam == WrapParameter::PERCENT_STRETCH_CLAMP_ON ) ||
            ( wrapParam == WrapParameter::CLAMP_HEIGHT ) ||
            ( wrapParam == WrapParameter::SIDE_UF_ADJUST ) ||
            ( wrapParam == WrapParameter::BRAKE_TIMING ) ||
            ( wrapParam == WrapParameter::HEIGHT_RESTRICT ) ||
            ( wrapParam == WrapParameter::REAR_UF_TRIP_DELAY ) ||
            ( wrapParam == WrapParameter::FRONT_UF_ADJUST && isNGWWrapper() ) ||                    // Trailing Edge on NGW
            ( wrapParam == WrapParameter::WRAP_SPEED ) ||
            ( wrapParam == WrapParameter::DISCHARGE_SPEED ) ||
            ( wrapParam == WrapParameter::FORCE_OUTSIDE_ELEVATOR && isNGWWrapper() ) ||             // Leading Edge on NGW
            ( wrapParam == WrapParameter::PRIMARY_LABEL_VERT_OFFSET && isNGWWrapper() ) ||
            ( wrapParam == WrapParameter::SECONDARY_LABEL_VERT_OFFSET && isNGWWrapper() )
      )
        return true;
    else
        return false;
}

bool serviceNewWrapperOneHeightBoxParam::isDualRollWrapper() const
{
    if (( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS2RL ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS2LR ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS3RL ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS3LR ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS4RL ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::AWS4LR ) ||
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::NGW2_C_RL ) || // SOF-2926
        ( wrTimingsData_->wrapperModelNum == DomEvolWrapperSystemInformation::NGW2_C_LR ))   // SOF-2926
        return true;

    return false;
}

bool serviceNewWrapperOneHeightBoxParam::isNGWWrapper() const
{
    return (DomEvolWrapperSystemInformation::nextGenModels <= wrTimingsData_->wrapperModelNum);
}

void serviceNewWrapperOneHeightBoxParam::buildTable(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::buildTable()";
#endif
    DomOneParamAllHeightBoxes allHeightBoxesMsg( msg );

    QFont textFont = font();
    textFont.setPointSize( 20 );

    ui->lineEdit_PLUNumber->setText(allHeightBoxesMsg.pluException());

    int totalRowCount = ui->tblBasicTimingsList->rowCount();
    boxCount = allHeightBoxesMsg.numOfItems().toInt();
    QVector<QString> heightArray = TokenParser::splitStringTokens( allHeightBoxesMsg.heightList() );
    QVector<QString> attribValArray = TokenParser::splitStringTokens( allHeightBoxesMsg.attributeValueList() );

    int j = 0;

    ui->tblBasicTimingsList->clear();

    allHeightBoxes.clear();

    if (offsetMap_.count() > 0)
        offsetMap_.clear();

    int existingCount = (boxCount < totalRowCount) ? boxCount : totalRowCount;
    QTableWidgetItem *heightTxt = NULL;
    for (j = 0; j < existingCount; j++)
    {
        heightTxt = ui->tblBasicTimingsList->item(j,0);
        if (heightTxt != 0)
            heightTxt->setText(heightArray.at(j));  //  Reuse
        else
        {
            heightTxt = new QTableWidgetItem;
            heightTxt->setText(heightArray.at(j));
            heightTxt->setFont(textFont);
            ui->tblBasicTimingsList->setItem(j, 0, heightTxt);
        }

        AllHeightBoxes *newBox = new AllHeightBoxes;
        newBox->boxName = heightArray.at(j);
        newBox->value = attribValArray.at(j).toInt();
        allHeightBoxes.append(newBox);

        //  Remove the old widget that may reside in this right side cell.
        QWidget *oldWidget = ui->tblBasicTimingsList->cellWidget(j, 1);
        if (oldWidget)
        {
            oldWidget = NULL;
        }
    }

    DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );
    const bool isAWS = (evolWrapperSystemInformation->awsModelNumber() < DomEvolWrapperSystemInformation::reducedTimingModels);

    WrapParameter::wrTimingsFieldType fieldType = wrTimingsData_->wrapParameterList.at(currentParam)->getType();
    int minVal = 0;
    int maxVal = 0;

    for (j = 0; j < existingCount; j++)
    {
        if ((fieldType == WrapParameter::typeUnsignedInt) ||
            (fieldType == WrapParameter::typeSignedInt))
        {
            //	Basic timings
            if (currentParam == WrapParameter::PERCENT_STRETCH_CLAMP_ON)
            {
                minVal = HB_MIN_PERCENT;
                maxVal = HB_MAX_PERCENT;

                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                    maxVal = HB_MAX_PERCENT_AWS1_AND_2;
            }
            else if (currentParam == WrapParameter::KNIFE_HEIGHT)
            {
                minVal = HB_MIN_KNIFE_UP;
                maxVal = HB_MAX_KNIFE_UP;
            }
            else if (currentParam == WrapParameter::CLAMP_HEIGHT)
            {
                minVal = HB_MIN_CLAMP_OPEN;
                maxVal = HB_MAX_CLAMP_OPEN;

                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                    maxVal = HB_MAX_CLAMP_OPEN_AWS1_AND_2;
            }
            else if (currentParam == WrapParameter::DISTANCE)
			{
                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                {
                    minVal = HB_MIN_DISTANCE_AWS1_AND_2;
                    maxVal = HB_MAX_DISTANCE_AWS1_AND_2;
                }
                else
                {
                    minVal = HB_MIN_DISTANCE;
                    maxVal = HB_MAX_DISTANCE;
                }
            }
            else if (currentParam == WrapParameter::GRIPPER_ADJUST)
			{
                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                {
                    minVal = HB_MIN_GRIPPER_ADJ_AWS1_AND_2;
                    maxVal = HB_MAX_GRIPPER_ADJ_AWS1_AND_2;
                }
                else
                {
                    minVal = HB_MIN_GRIPPER_ADJ;
                    maxVal = HB_MAX_GRIPPER_ADJ;
                }
            }
            else if (currentParam == WrapParameter::SIDE_UF_ADJUST)
            {
                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                {
                    minVal = HB_MIN_SIDE_FOLD_ADJ_AWS1_AND_2;
                    maxVal = HB_MAX_SIDE_FOLD_ADJ_AWS1_AND_2;
                }
                else
                {
                    minVal = HB_MIN_SIDE_FOLD_ADJ;
                    maxVal = HB_MAX_SIDE_FOLD_ADJ;
                }
            }
            else if (currentParam == WrapParameter::INSIDE_GRIPPER_RELEASE)
            {
                minVal = HB_MIN_INSIDE_GRIPPER_RELEASE;
                maxVal = HB_MAX_INSIDE_GRIPPER_RELEASE;
            }
            //	Advanced timings
            else if (currentParam == WrapParameter::FRONT_UF_ADJUST)
            {
                if ( isNGWWrapper() )
                {
                    minVal = HB_MIN_TRAILEDGE_LABEL_OFFSET;
                    maxVal = HB_MAX_TRAILEDGE_LABEL_OFFSET;
                }
                else
                {
                    minVal = HB_MIN_FRONT_ADJ;
                    maxVal = HB_MAX_FRONT_ADJ;
                }
            }
            else if (currentParam == WrapParameter::WRAP_SPEED)
			{
                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                {
                    minVal = HB_MIN_WRAP_SPEED_AWS1_AND_2;
                    maxVal = HB_MAX_WRAP_SPEED_AWS1_AND_2;
                }
                else
                {
                    minVal = HB_MIN_WRAP_SPEED;
                    maxVal = HB_MAX_WRAP_SPEED;
                }
			}
            else if (currentParam == WrapParameter::DISCHARGE_SPEED)
			{
            	minVal = HB_MIN_DISCHARGE_SPEED;
				maxVal = HB_MAX_DISCHARGE_SPEED;

                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                    maxVal = HB_MAX_DISCHARGE_SPEED_AWS1_AND_2;
            }
            else if (currentParam == WrapParameter::REAR_UF_TRIP_DELAY)
			{
            	minVal = HB_MIN_REAR_TRIP_DELAY;
				maxVal = HB_MAX_REAR_TRIP_DELAY;

                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                    maxVal = HB_MAX_REAR_TRIP_DELAY_AWS1_AND_2;
            }
            else if (currentParam == WrapParameter::BRAKE_TIMING)
			{
            	minVal = HB_MIN_BRAKE_TIME;
				maxVal = HB_MAX_BRAKE_TIME;

                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                    maxVal = HB_MAX_BRAKE_TIME_AWS1_AND_2;
            }
			//	Other Timings
            else if (currentParam == WrapParameter::HEIGHT_RESTRICT)
			{
            	minVal = HB_MIN_HEIGHT_RESTRICT;
				maxVal = HB_MAX_HEIGHT_RESTRICT;

                // SOF-4434 Need to allow larger range for AWS1 and AWS2
                if ( isAWS )
                    maxVal = HB_MAX_HEIGHT_RESTRICT_AWS1_AND_2;
            }
            else if (currentParam == WrapParameter::SIDE_CLAMP_GAP)
            {
                minVal = HB_MIN_SIDE_CLAMP_GAP;
                maxVal = HB_MAX_SIDE_CLAMP_GAP;
            }
            else if (currentParam == WrapParameter::FORCE_OUTSIDE_ELEVATOR)
            {
                if( isNGWWrapper() )
                {
                    minVal = HB_MIN_TRAILEDGE_LABEL_OFFSET;
                    maxVal = HB_MAX_TRAILEDGE_LABEL_OFFSET;
                }
            }
            else if (currentParam == WrapParameter::PRIMARY_LABEL_VERT_OFFSET ||
                     currentParam == WrapParameter::SECONDARY_LABEL_VERT_OFFSET
            )
            {
                if( isNGWWrapper() )
                {
                    minVal = HB_MIN_VERTICAL_LABEL_OFFSET;
                    maxVal = HB_MAX_VERTICAL_LABEL_OFFSET;
                }
            }
            else
            {
#ifdef USE_QDEBUG
                qDebug() << "serviceNewWrapperOneHeightBoxParam::buildTable():	Missing a signed/unsigned Integer parameter!!!";
#endif
                minVal = 0;
                maxVal = 0;
            }

            // SOF-1714
            int attribVal( attribValArray.at(j).toInt() );

            if ( ( ( WrapParameter::FRONT_UF_ADJUST == currentParam ) && ( isNGWWrapper() ) && ( 100 <= attribVal ) ) ||
                 ( ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == currentParam ) && ( isNGWWrapper() ) && ( 100 <= attribVal ) )
            )
            {
                attribVal -= 100;
            }

            QSpinBox *value = createQSpinBox(j, minVal, maxVal, QString::number( attribVal ) );
            ui->tblBasicTimingsList->setCellWidget(j, 1, value);
        }
        else if (fieldType == WrapParameter::typeBoolean)
        {
            QComboBox *box = createBooleanCombo(j, attribValArray.at(j));
            ui->tblBasicTimingsList->setCellWidget(j, 1, box);
        }
        else if (fieldType == WrapParameter::typeFilmRollToUse)
        {
            QComboBox *box = createFilmRollToUseCombo(j, attribValArray.at(j));
            ui->tblBasicTimingsList->setCellWidget(j, 1, box);
        }
        else if (fieldType == WrapParameter::typeForceOutsideElevatorSelect)
        {
            QComboBox *box = createForceOutsideElevatorCombo(j, attribValArray.at(j));
            ui->tblBasicTimingsList->setCellWidget(j, 1, box);
        }
        else if (fieldType == WrapParameter::typeOutsideGripperReleaseStates)
        {
            QComboBox *box = createOutsideGripperReleaseCombo(j, attribValArray.at(j));
            ui->tblBasicTimingsList->setCellWidget(j, 1, box);
        }
        else if (fieldType == WrapParameter::typeSideUnderfolderReturnHomeStates)
        {
            QComboBox *box = createSideUnderfolderReturnHomeCombo(j, attribValArray.at(j));
            ui->tblBasicTimingsList->setCellWidget(j, 1, box);
        }
    }
}

QComboBox *serviceNewWrapperOneHeightBoxParam::createBooleanCombo(int offset, QString value)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::createBooleanCombo()";
#endif
    QFont cboFont = font();
    cboFont.setPointSize( 20 );		//	HA-87

    QComboBox *cbo_Boolean = new QComboBox(this);
    if (cbo_Boolean)
    {
        cbo_Boolean->setFont(cboFont);

        cbo_Boolean->setObjectName( "bool_" + offset );
        cbo_Boolean->addItem( tr("False"), QString("0"));
        cbo_Boolean->addItem( tr("True"),  QString("1"));\
        int index = cbo_Boolean->findData( value );
        if (index >= 0)
            cbo_Boolean->setCurrentIndex(index);

        if (!viewCurrentTimings_)
            cbo_Boolean->setEnabled(false);
        else
        {
            connect( cbo_Boolean, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
            widgetUpdatedSignalMapper_->setMapping( cbo_Boolean, cbo_Boolean );
        }
        offsetMap_[cbo_Boolean] = offset;
    }
    return cbo_Boolean;
}

QComboBox *serviceNewWrapperOneHeightBoxParam::createFilmRollToUseCombo(int offset, QString value)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::createFilmRollToUseCombo()";
#endif
    QFont cboFont = font();
    cboFont.setPointSize( 20 );		//	HA-87

    QComboBox *cbo_FilmRoll = new QComboBox(this);
    if (cbo_FilmRoll)
    {
        cbo_FilmRoll->setFont(cboFont);

        cbo_FilmRoll->setObjectName( "frtu_" + offset );
        cbo_FilmRoll->addItem( tr("Narrow roll"), QString("100"));
        cbo_FilmRoll->addItem( tr("Wide roll"),   QString("200"));
        int index = cbo_FilmRoll->findData( value );
        if (index >= 0)
            cbo_FilmRoll->setCurrentIndex(index);

        if (!viewCurrentTimings_)
            cbo_FilmRoll->setEnabled(false);
        else
        {
            connect( cbo_FilmRoll, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
            widgetUpdatedSignalMapper_->setMapping( cbo_FilmRoll, cbo_FilmRoll );
        }

        offsetMap_[cbo_FilmRoll] = offset;
    }
    return cbo_FilmRoll;
}

QComboBox *serviceNewWrapperOneHeightBoxParam::createForceOutsideElevatorCombo(int offset, QString value)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::createForceOutsideElevatorCombo()";
#endif
    QFont cboFont = font();
    cboFont.setPointSize( 20 );		//	HA-87

    QComboBox *cbo_FOEState = new QComboBox(this);
    if (cbo_FOEState)
    {
        cbo_FOEState->setFont(cboFont);

        cbo_FOEState->setObjectName( "foe_" + offset );
        cbo_FOEState->addItem( tr("Automatic"), QString("0"));
        cbo_FOEState->addItem( tr("True"),      QString("1"));
        int index = cbo_FOEState->findData( value );
        if (index >= 0)
            cbo_FOEState->setCurrentIndex(index);

        if (!viewCurrentTimings_)
            cbo_FOEState->setEnabled(false);
        else
        {
            connect( cbo_FOEState, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
            widgetUpdatedSignalMapper_->setMapping( cbo_FOEState, cbo_FOEState );
        }

        offsetMap_[cbo_FOEState] = offset;
    }
    return cbo_FOEState;
}

QComboBox *serviceNewWrapperOneHeightBoxParam::createOutsideGripperReleaseCombo(int offset, QString value)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::createOutsideGripperReleaseCombo()";
#endif
    QFont cboFont = font();
    cboFont.setPointSize( 20 );		//	HA-87

    QComboBox *cbo_OGReleaseState = new QComboBox(this);
    if (cbo_OGReleaseState)
    {
        cbo_OGReleaseState->setFont(cboFont);

        cbo_OGReleaseState->setObjectName( "ogr_" + offset );
        cbo_OGReleaseState->addItem( tr("Pulling"),                  QString("4"));
        cbo_OGReleaseState->addItem( tr("Pulling and braked"),       QString("5"));
        cbo_OGReleaseState->addItem( tr("Pull complete"),            QString("6"));
        cbo_OGReleaseState->addItem( tr("Underfolder underfolding"), QString("7"));
        int index = cbo_OGReleaseState->findData( value );
        if (index >= 0)
            cbo_OGReleaseState->setCurrentIndex(index);

        if (!viewCurrentTimings_)
            cbo_OGReleaseState->setEnabled(false);
        else
        {
            connect( cbo_OGReleaseState, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
            widgetUpdatedSignalMapper_->setMapping( cbo_OGReleaseState, cbo_OGReleaseState );
        }

        offsetMap_[cbo_OGReleaseState] = offset;
    }
    return cbo_OGReleaseState;
}

QComboBox *serviceNewWrapperOneHeightBoxParam::createSideUnderfolderReturnHomeCombo(int offset, QString value)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::createSideUnderfolderReturnHomeCombo()";
#endif
    QFont cboFont = font();
    cboFont.setPointSize( 20 );		//	HA-87

    QComboBox *cbo_SUFReturnState = new QComboBox(this);
    if (cbo_SUFReturnState)
    {
        cbo_SUFReturnState->setFont(cboFont);

        cbo_SUFReturnState->setObjectName( "surh_" + offset );
        cbo_SUFReturnState->addItem( tr("Early"),  QString("0"));
        cbo_SUFReturnState->addItem( tr("Normal"), QString("1"));
        cbo_SUFReturnState->addItem( tr("Late"),   QString("2"));
        int index = cbo_SUFReturnState->findData( value );
        if (index >= 0)
            cbo_SUFReturnState->setCurrentIndex(index);

        if (!viewCurrentTimings_)
            cbo_SUFReturnState->setEnabled(false);
        else
        {
            connect( cbo_SUFReturnState, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
            widgetUpdatedSignalMapper_->setMapping( cbo_SUFReturnState, cbo_SUFReturnState );
        }

        offsetMap_[cbo_SUFReturnState] = offset;
    }
    return cbo_SUFReturnState;
}

QSpinBox* serviceNewWrapperOneHeightBoxParam::createQSpinBox(int offset, int minVal, int maxVal, QString currVal)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::createQSpinBox()";
#endif

    QSpinBox *integerField = new QSpinBox( this );
    if (integerField)
    {
        integerField->setObjectName( "param" + offset );
        integerField->setStyleSheet("background-color: white; font: 18pt \"MS Shell Dlg 2\";");

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

void serviceNewWrapperOneHeightBoxParam::validateSpinBox(int offset, QSpinBox* intField)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::validateSpinBox()";
#endif
    int valWas = allHeightBoxes.at(offset)->value;
    int valIs = intField->value();

    if (valWas != valIs)
    {
        saveAttributeChange(offset, valIs);
    }
}

void serviceNewWrapperOneHeightBoxParam::validateEntry(int offset, hobart::numericLineEdit *intField)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::validateEntry()";
#endif
    int valWas = allHeightBoxes.at(offset)->value;
    int valIs = intField->text().toInt();

    if (valWas != valIs)
    {
        saveAttributeChange(offset, valIs);
    }
}

void serviceNewWrapperOneHeightBoxParam::comboBoxChanged(int offset, QComboBox *box)
{
    LOG_CLASS_INFO( "snwOneHeightBox", "comboBoxChanged" );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::comboBoxChanged()";
#endif
    if (box)
    {
        int index = box->currentIndex();
        int valWas = allHeightBoxes.at(offset)->value;
        QString itemText = box->itemText(index);
        int valIs = box->itemData(index).toInt();		//	HA-131
        LOG_TRACE( "offset = %d,  index = %d,  valWas = %d,  valIs = %d,  itemText = %s", offset, index, valWas, valIs, qPrintable(itemText) );
        if (valWas != valIs)
        {
            saveAttributeChange(offset, valIs);
        }
    }
}

void serviceNewWrapperOneHeightBoxParam::updateWrapperPkgDescription(const SessionMessage& msg)
{
    Q_UNUSED( msg )

    // SOF-240: disable yellow highlight
    return;
/*
    LOG_CLASS_INFO( "snwOneHeightBox", "updateWrapperPkgDescription" );

#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::updateWrapperPkgDescription()";
#endif

    int wrModel = wrTimingsData_->wrapperModelNum;
    LOG_TRACE( "wrModel = %d", wrModel );

    if ( (wrModel > DomEvolWrapperSystemInformation::None) && (wrModel < DomEvolWrapperSystemInformation::endWrapperModels))
    {
        DomEvolWrapperPkgDesc pkgDescription( msg );

        int pnsCell = pkgDescription.heightBoxIndex().toInt();
        LOG_TRACE( "currentParam = %d,  pnsCell = %d", currentParam, pnsCell );

        if ( pkgDescription.modifiedNetStretch() == true )
        {
            QString highlightBgColor("yellow");
            QString spinBoxStyleHighlight("background-color: yellow; font: 20pt \"MS Shell Dlg 2\";");

            ui->tblBasicTimingsList->item(pnsCell,0)->setBackgroundColor( highlightBgColor );
            ui->tblBasicTimingsList->cellWidget(pnsCell,1)->setStyleSheet( spinBoxStyleHighlight );		//	HA-132
        }
        else
        {
            QString normalBgColor("white");
            QString spinBoxStyleNormal("background-color: white; font: 20pt \"MS Shell Dlg 2\";");

            ui->tblBasicTimingsList->item(pnsCell,0)->setBackgroundColor( normalBgColor );
            ui->tblBasicTimingsList->cellWidget(pnsCell,1)->setStyleSheet( spinBoxStyleNormal );		//	HA-132
        }
    }
*/
}

void serviceNewWrapperOneHeightBoxParam::wrapperCycleComplete(const SessionMessage& msg)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::wrapperCycleComplete()";
#endif
    DomEvolWrapperTestComplete m( msg );
    if (m.testNum().toInt() == 24)
    {
        ui->butCycleWrapper->setStyleSheet(cycleButtonStyle);
        cycleWrapperInProgress_ = false;
    }
}

void serviceNewWrapperOneHeightBoxParam::saveAttributeChange(int offset, int newVal)
{
#ifdef USE_QDEBUG
    qDebug() << "serviceNewWrapperOneHeightBoxParam::saveAttributeChange(" << QString::number(offset) << ", " << QString::number(newVal) << ")";
#endif

    // SOF-1714
    if ( ( ( WrapParameter::FRONT_UF_ADJUST == currentParam ) && ( isNGWWrapper() ) ) ||
         ( ( WrapParameter::FORCE_OUTSIDE_ELEVATOR == currentParam ) && ( isNGWWrapper() ) )
    )
    {
        newVal += 100;
    }

    allHeightBoxes.at(offset)->value = newVal;

    QString msgBody = QString::number(activeRow);
    msgBody += "," + allHeightBoxes.at(offset)->boxName;
    msgBody += "," + QString::number(currentParam);
    msgBody += "," + QString::number(newVal);

    session_->send(Wrapper, WrEvolSaveAttributeChanges, msgBody);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperOneHeightBoxParam::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

#ifdef USE_QDEBUG
    if ((device != 145) && (device != 170) && !((device == 9) && (event == 51)))
        qDebug() << "serviceNewWrapperOneHeightBoxParam::onSessionMessage():  device = " << msg[0] << ",  event = " << msg[1];
#endif
    if (( Wrapper == device ) && ( WrEvolWriteAllForAttribute == event ))
    {
        // Received 9,198 - Update the Height box list for this parameter
        buildTable(msg);
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
    else if (( Wrapper == device ) && ( WrEvolWriteHeightBox == event ))
    {
        // Received 9,247 - Update the Basic timings info / Height box info for this tray
        DomHeightBox heightBox( msg );
        heightBox_ = new DomHeightBox(heightBox);
    }
}

