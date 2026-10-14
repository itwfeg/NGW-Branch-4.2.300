#include "timeedit.h"
#include "ui_timeedit.h"

#include <QTimer>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QDate>
#include <QTime>

TimeEdit::TimeEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeEdit)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    initialize();
}

void TimeEdit::initialize(){
    /// Set UI elements to current time/date
    QDate dateToday = QDate::currentDate();
    this->ui->calendarWidget->setSelectedDate(dateToday);
    this->ui->calendarWidget->setFirstDayOfWeek(Qt::Sunday);
    QTime timeNow = QTime::currentTime();
    this->ui->timeEdit->setTime(timeNow);
    this->ui->hourBut->setText(QString("%1").arg(timeNow.hour(), 2, 10, QChar('0')));
    this->ui->minuteBut->setText(QString("%1").arg(timeNow.minute(), 2, 10, QChar('0')));
    this->ui->secondBut->setText(QString("%1").arg(timeNow.second(), 2, 10, QChar('0')));

    /// Setup the month combo box
    this->ui->monthComboBox->addItem(tr("January"), January);
    this->ui->monthComboBox->addItem(tr("February"), February);
    this->ui->monthComboBox->addItem(tr("March"), March);
    this->ui->monthComboBox->addItem(tr("April"), April);
    this->ui->monthComboBox->addItem(tr("May"), May);
    this->ui->monthComboBox->addItem(tr("June"), June);
    this->ui->monthComboBox->addItem(tr("July"), July);
    this->ui->monthComboBox->addItem(tr("August"), August);
    this->ui->monthComboBox->addItem(tr("September"), September);
    this->ui->monthComboBox->addItem(tr("October"), October);
    this->ui->monthComboBox->addItem(tr("November"), November);
    this->ui->monthComboBox->addItem(tr("December"), December);
    this->ui->monthComboBox->setCurrentIndex(this->ui->monthComboBox->findData(dateToday.month()));

    this->ui->yearSpinBox->setValue(dateToday.year());

    ///connect changes to spinbox/combobox to update the calendar
    connect(this->ui->monthComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slt_ChangeCalendarMonth(int)));
    connect(this->ui->yearSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slt_ChangeCalendarYear(int)));
    connect(this->ui->calendarWidget, SIGNAL(currentPageChanged(int,int)), this, SLOT(slt_CalendarChanged(int,int)));

    /// Setup current time label
    QTimer* qtimer = new QTimer(this);
    QTimer::singleShot(0, this, SLOT(slt_UpdateTime()));
    connect(qtimer, SIGNAL(timeout()), this, SLOT(slt_UpdateTime()));
    connect(this->ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(this->ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(slt_SetTime()));
    qtimer->start(1000);

    connect(this->ui->hourBut, SIGNAL(clicked(bool)), this, SLOT(slt_HourSelect()));
    connect(this->ui->minuteBut, SIGNAL(clicked(bool)), this, SLOT(slt_MinuteSelect()));
    connect(this->ui->secondBut, SIGNAL(clicked(bool)), this, SLOT(slt_SecondSelect()));
    this->connect(this->ui->timeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(slt_TimeEditTimeChanged(QTime)));

    //event filter to clear time button highlighting.
    this->ui->timeEdit->installEventFilter(this);
    this->ui->calendarWidget->installEventFilter(this);
    this->ui->monthComboBox->installEventFilter(this);
    this->ui->yearSpinBox->installEventFilter(this);

}

void TimeEdit::slt_TimeEditTimeChanged(QTime)
{
    if(ui->timeEdit->currentSection() == ui->timeEdit->HourSection)
    {
        // hour changed
        ui->hourBut->setText(ui->timeEdit->sectionText(ui->timeEdit->HourSection));
        slt_HourSelect();
    }
    else if(ui->timeEdit->currentSection() == ui->timeEdit->MinuteSection)
    {
        // minute changed
        ui->minuteBut->setText(ui->timeEdit->sectionText(ui->timeEdit->MinuteSection));
        slt_MinuteSelect();
    }
    else if(ui->timeEdit->currentSection() == ui->timeEdit->SecondSection)
    {
        // second changed
        ui->secondBut->setText(ui->timeEdit->sectionText(ui->timeEdit->SecondSection));
        slt_SecondSelect();
    }
}

void TimeEdit::slt_HourSelect(){
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: white; border-radius: 5px; border: 1px white; background: rgb(0, 0, 255);}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px white; background: white;}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px whie; background: white;}");
    this->ui->timeEdit->setSelectedSection(QTimeEdit::HourSection);
}

void TimeEdit::slt_MinuteSelect(){
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px white; background: white;}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: white; border-radius: 5px; border: 1px white; background: rgb(0, 0, 255);}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px whie; background: white;}");
    this->ui->timeEdit->setSelectedSection(QTimeEdit::MinuteSection);
}

void TimeEdit::slt_SecondSelect(){
    ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px white; background: white;}");
    ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px white; background: white;}");
    ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: white; border-radius: 5px; border: 1px white; background: rgb(0, 0, 255); }");
    this->ui->timeEdit->setSelectedSection(QTimeEdit::SecondSection);
}

void TimeEdit::slt_UpdateTime(){
    QDateTime dtime = QDateTime::currentDateTime();
    this->ui->timeNow->setText(dtime.toString("dddd MMMM dd, yyyy -- hh:mm:ss"));
}

void TimeEdit::slt_SetTime(){
    static const QString script("/usr/local/hobart/python/setTime.py");
    static const QString python_exec("/usr/bin/python");
    static const QString isSlicer("/etc/hobart/slicer");

    if(!QFile::exists(script) || !QFile::exists(python_exec) || !QFile::exists(isSlicer))
        return;

    QStringList options;
    options.append(script);

    ///Taken from -- /Windux/Application/shared/sessionHandler/screens/setupUtility.cpp
    options.append("-Y"); //year
    options.append(QString::number(this->ui->calendarWidget->selectedDate().year()));
    options.append("-M"); // month
    options.append(QString("%1").arg(this->ui->calendarWidget->selectedDate().month() ,2,10, QChar('0')).toUpper());
    options.append("-D"); //day
    options.append(QString("%1").arg(this->ui->calendarWidget->selectedDate().day() ,2,10, QChar('0')).toUpper());
    options.append("-H"); // hour
    options.append(QString("%1").arg(this->ui->timeEdit->time().hour(),2,10, QChar('0')).toUpper());
    options.append("-m"); // minute
    options.append(QString("%1").arg(this->ui->timeEdit->time().minute(),2,10, QChar('0')).toUpper());
    options.append("-S"); // second
    options.append(QString("%1").arg(this->ui->timeEdit->time().second(),2,10, QChar('0')).toUpper());
    options.append("-A");
    options.append("0");


    QProcess p(this);
    p.start(python_exec, options);

    p.waitForFinished(300000);

    accept();
}

void TimeEdit::slt_ChangeCalendarMonth(int month){
    this->ui->calendarWidget->setCurrentPage(this->ui->calendarWidget->yearShown(), month+1);
}
void TimeEdit::slt_ChangeCalendarYear(int year){
    this->ui->calendarWidget->setCurrentPage(year, this->ui->calendarWidget->monthShown());
}
void TimeEdit::slt_CalendarChanged(int year, int month){
    this->ui->yearSpinBox->setValue(year);
    this->ui->monthComboBox->setCurrentIndex(this->ui->monthComboBox->findData(month));
}


TimeEdit::~TimeEdit()
{
    delete ui;
}

void TimeEdit::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}


bool TimeEdit::eventFilter(QObject *obj, QEvent *e){
    if(obj == this->ui->calendarWidget || obj == this->ui->monthComboBox || obj == this->ui->yearSpinBox){
        if(QEvent::FocusIn == e->type()){
            ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px white; background: white;}");
            ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px white; background: white;}");
            ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px white; background: white; }");
        }
    }
    if(obj == this->ui->timeEdit){
        if(QEvent::FocusOut == e->type()){
            ui->hourBut->setStyleSheet("QPushButton#hourBut{ color: black; border-radius: 5px; border: 1px white; background: white;}");
            ui->minuteBut->setStyleSheet("QPushButton#minuteBut{ color: black; border-radius: 5px; border: 1px white; background: white;}");
            ui->secondBut->setStyleSheet("QPushButton#secondBut{ color: black; border-radius: 5px; border: 1px white; background: white; }");
        }
    }
    return QDialog::eventFilter( obj, e );
}
