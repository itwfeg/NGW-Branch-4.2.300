/**
 * @file timeEntryDialog.cpp
 *
 * @copyright Copyright (c) 2014, Hobart / ITW Food Equipment Group, U.S.A.
 * @author Clare Dickey
 */


#include "timeEntryDialog.h"
#include "ui_timeEntryDialog.h"

#include "../sessionmainwindow.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

timeEntryDialog::timeEntryDialog( const QMap<QWidget*, int>& theTimesList, int offset, QWidget *parent ) :
    SessionDialog( LeftDialogLocation, parent ),
    ui( new Ui::timeEntryDialog ),
	timesList_( theTimesList ),
	offset_( offset )
{
//    blockSessionDialogs_( true );

    SessionDialog::setDialogAlignment( Qt::AlignTop );  //  I want this positioned in the Top left corner.

    // keep track of parent main window
    mainWindow_ = dynamic_cast<SessionMainWindow*>( parent );

    setObjectName( "timeEntryDialog" );

    ui->setupUi(this);

    initialize();
    translate();

//    // FT-415
//    // while this widget is open do not show system dialogs
//    if ( blockSessionDialogs_ )
//        session_->send( SessionDev, SnStopDialogs );

}

timeEntryDialog::~timeEntryDialog()
{
//    // FT-415
//    // resume dialogs
//    if ( blockSessionDialogs_ )
//        session_->send( SessionDev, SnRestartDialogs );

	if (valHours_ != NULL)
		delete valHours_;

	if (valMinutes_ != NULL)
		delete valMinutes_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void timeEntryDialog::initialize()
{
    valHours_ = NULL;
    valMinutes_ = NULL;

    // NUMERIC VALIDATORS
    ui->txtHour->setValidator(valHours_ = new QIntValidator(0, INT_MAX, this) );
    valHours_->setTop(12);
    valHours_->setBottom(1);
    ui->txtHour->setInputLeadingZeros(true);
    ui->txtHour->setInputWrapping(true);
    ui->txtHour->setInputTruncation(true);
    ui->txtHour->setDecimalPlaces(0);

    ui->txtMinute->setValidator(valMinutes_ = new QIntValidator(0, INT_MAX, this) );
    valMinutes_->setTop(59);
    valMinutes_->setBottom(0);
    ui->txtMinute->setInputLeadingZeros(true);
    ui->txtMinute->setInputWrapping(true);
    ui->txtMinute->setInputTruncation(true);
    ui->txtMinute->setDecimalPlaces(0);

    QLineEdit* timeWidget = NULL;
    QMap<QWidget*, int>::const_iterator myIter = timesList_.constBegin();
    while (myIter != timesList_.constEnd())
    {
        int theOffset = myIter.value();
        if (offset_ == theOffset)
        {
            timeWidget = (QLineEdit*) myIter.key();
            break;
        }
        ++myIter;
    }
    if (timeWidget != NULL)
    {
        QString tmpHr = "12";
        QString tmpMin = "00";
        QString tmpAmPm = tr("AM");

        QString timeStr = timeWidget->text();
        if ( timeStr == "None" )
        {
            tmpHr = "";
            tmpMin = "";
        }
        else if( timeStr != "" && timeStr != " ")
        {
            tmpHr = timeStr.mid(0,2);
            tmpMin =  timeStr.mid(3,2);
            tmpAmPm = timeStr.mid(6,2);

            if(tmpAmPm.length() == 0)
                tmpAmPm = tr("AM");                                         // default AM if blank
        }

        if (tmpAmPm == "AM")
            isAM_ = true;
        else
            isAM_ = false;

        ui->txtHour->setText(tmpHr);
        ui->txtMinute->setText(tmpMin);
        ui->but_AmPm->setText( tmpAmPm );
    }
	//	Set up the Day of Week box
    dayOfWeekIndex_ = (offset_ % 7);

	//	Set up the Time On/Off box
    if (offset_ < 7)
        onOffIndex_ = 1;
    else
	    onOffIndex_ = 0;

    connect(ui->but_AmPm,           SIGNAL(clicked()),                  this,   SLOT(toggleAmPm()));
    connect(ui->butApply,           SIGNAL(clicked()),                  this,   SLOT(applyTime()));
    connect(ui->butApplyNExit,      SIGNAL(clicked()),                  this,   SLOT(applyTimeThenExit()));
    connect(ui->butExit,            SIGNAL(clicked()),                  this,   SLOT(exitTimeEntry()));
    connect(ui->butClearTime,       SIGNAL(clicked()),                  this,   SLOT(setFieldToNone()));
    connect(ui->butClearAllTimes,   SIGNAL(clicked()),                  this,   SLOT(setAllFieldsToNone()));
}

void timeEntryDialog::translate()
{
    ui->retranslateUi(this);

    ui->cbo_DayOfWeek->clear();
    ui->cbo_DayOfWeek->addItem( tr("Sunday"), 0 );
    ui->cbo_DayOfWeek->addItem( tr("Monday"), 1 );
    ui->cbo_DayOfWeek->addItem( tr("Tuesday"), 2 );
    ui->cbo_DayOfWeek->addItem( tr("Wednesday"), 3 );
    ui->cbo_DayOfWeek->addItem( tr("Thursday"), 4 );
    ui->cbo_DayOfWeek->addItem( tr("Friday"), 5 );
    ui->cbo_DayOfWeek->addItem( tr("Saturday"), 6 );
    ui->cbo_DayOfWeek->addItem( tr("Monday - Friday"), 7 );
    ui->cbo_DayOfWeek->addItem( tr("Saturday - Sunday"), 8 );
    ui->cbo_DayOfWeek->addItem( tr("Every day"), 9 );
    ui->cbo_DayOfWeek->setCurrentIndex(dayOfWeekIndex_);

    ui->cbo_OnOrOff->clear();
    ui->cbo_OnOrOff->addItem( tr("Off"), 0 );
    ui->cbo_OnOrOff->addItem( tr("On"), 1 );
    ui->cbo_OnOrOff->setCurrentIndex(onOffIndex_);

    update();
}

void timeEntryDialog::toggleAmPm()
{
    if ( isAM_ )
    {
        ui->but_AmPm->setText("PM");
        isAM_ = false;
    }
    else
    {
        ui->but_AmPm->setText("AM");
        isAM_ = true;
    }
}

void timeEntryDialog::applyTime(bool clearTime)
{
    //  First check the day of the week
    int myIndex = ui->cbo_DayOfWeek->currentIndex();
    bool isOn = ui->cbo_OnOrOff->currentIndex();

    if (myIndex < 7)
    {
        //  We are dealing with a single day - set it up.
        setTimeToWidget(myIndex, isOn, clearTime);
    }
    else
    {
        if ((myIndex == 7) || (myIndex == 9))
        {
            //  Set up for Monday thru Friday
            for (int day = 1; day < 6; day++)
            {
                setTimeToWidget(day, isOn, clearTime);
            }
        }
        if ((myIndex == 8) || (myIndex == 9))
        {
            setTimeToWidget(0, isOn, clearTime);
            setTimeToWidget(6, isOn, clearTime);
        }
    }
}

void timeEntryDialog::setTimeToWidget(int myIndex, bool isOn, bool clearTime)
{
    //  We are dealing with a single day - set it up.
    if (!isOn)
        myIndex += 7;               //  It's off. Add 7 to the index.
    QLineEdit* timeWidget = NULL;
    QMap<QWidget*, int>::const_iterator myIter = timesList_.constBegin();
    while (myIter != timesList_.constEnd())
    {
        int theOffset = myIter.value();
        if (myIndex == theOffset)
        {
            timeWidget = (QLineEdit*) myIter.key();
            break;
        }
        ++myIter;
    }
    if (timeWidget)
    {
        if (clearTime)
		{
            ui->txtHour->setText("");
            ui->txtMinute->setText("");

	        timeWidget->setText( tr("None") );
		}
		else
		{
            QString timeStr = tr("None");

            QString hourStr = ui->txtHour->text();
            if (hourStr.length() == 1)
                hourStr = "0" + hourStr;

	        QString minStr = ui->txtMinute->text();
            if (minStr.length() == 1)
                minStr = "0" + minStr;

            if ((hourStr.length() + minStr.length()) == 4)
                timeStr = hourStr + ":" + minStr + " " + (isAM_ ? "AM" : "PM");
	        timeWidget->setText(timeStr);
		}
        timeWidget->setStyleSheet("background-color: yellow;");
    }
}

void timeEntryDialog::applyTimeThenExit()
{
    applyTime();
    exitTimeEntry();
}

void timeEntryDialog::exitTimeEntry()
{
    accept();
}

void timeEntryDialog::setFieldToNone()			//	HA-189
{
	applyTime(true);
}

void timeEntryDialog::setAllFieldsToNone()		//	HA-189
{
    QLineEdit* timeWidget = NULL;
    QMap<QWidget*, int>::const_iterator myIter = timesList_.constBegin();
    while (myIter != timesList_.constEnd())
    {
        timeWidget = (QLineEdit*) myIter.key();
	    if (timeWidget)
	    {
	        timeWidget->setText( tr("None") );
	        timeWidget->setStyleSheet("background-color: yellow;");
			timeWidget = NULL;
	    }
        ++myIter;
    }
    ui->txtHour->setText("");
    ui->txtMinute->setText("");
}

void timeEntryDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

