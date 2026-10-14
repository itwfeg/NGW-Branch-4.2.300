#include "message.h"
#include <math.h>

#include <QDebug>
#include <QSettings>

message::message(QObject *parent) : QObject(parent)
{
    this->overFlow = false;
    this->timer = new QTimer(this);
    this->dates = QList<int>();
    dates.clear();
    this->times = QList<QTime>();
    times.clear();

    //getNextExpire();
}

long long message::getNextExpire()
{
    long long timeUntil = LONG_LONG_MAX;
    if(!this->IsDiabled)
    {
        if(type == msgBoxConfig::Monthly)
            timeUntil = getNextExpireMonthly();
        else if(type == msgBoxConfig::Weekly)
            timeUntil = getNextExpireWeekly();
    }
    else
    {
        //qDebug() << "Alert " << this->header << " is disabled. No alert set.";
    }
    return timeUntil;
}

void message::debugNextTimeAlert(long long msecs){
    QTime timeNow = QTime::currentTime();
    QDate dayToday = QDate::currentDate();

    //Check next alert time is what the user entered:
    QDate checkNextAlertDay = dayToday.addDays(msecs/ONE_DAY_IN_MSEC);
    if((timeNow.msecsSinceStartOfDay() + msecs % ONE_DAY_IN_MSEC) > ONE_DAY_IN_MSEC){
        checkNextAlertDay = checkNextAlertDay.addDays(1);
    }
    QTime checkNextAlertTime = timeNow.addMSecs(msecs%ONE_DAY_IN_MSEC);
    qDebug() << "Alert " << this->header << " on " << checkNextAlertDay.toString(Qt::TextDate) << " at: " << checkNextAlertTime.toString() << " -- this is in: " << msecs << " msecs";
}

long long message::getNextExpireWeekly()
{
    //qDebug() << "Weekly";
    QTime timeNow = QTime::currentTime();
    QDate dayToday = QDate::currentDate();
    QTime earliestTime = QTime(23,59);
    long long minimumNextAlertTime = LONG_LONG_MAX;

    for(QTime time: times)
    {
        //Skip if the time is blank as it will set the next expiration to now
        if(time.toString() == "")
            continue;
        int msecsUntilTime = timeNow.msecsTo(time);
        if(msecsUntilTime >= 0 && msecsUntilTime < minimumNextAlertTime)
        {
            minimumNextAlertTime = msecsUntilTime;
        }
        if(time < earliestTime)
        {
            earliestTime = time;
        }
    }

    // if there is an alert today
    if(minimumNextAlertTime != LONG_LONG_MAX && isDaySet(possibleDaysOfWeek[(dayToday.dayOfWeek() % 7)]))
    {
        debugNextTimeAlert(minimumNextAlertTime);
        return minimumNextAlertTime;
    }
    // the next alert is some other day
    else
    {
        int numberOfDays = 7;
        // QDate is from ranges Monday = 1 --> Sunday = 7.
        int dayIndex = (dayToday.dayOfWeek() + 1) % 7;
                    // -- start checking at dayToday + 1 (tomorrow) because no remaining alerts today
        bool foundNextDate = false;
        int howManyDaysUntilNextAlert = 0;
        //iterate through the rest of the week (today -> saturday) to get the next day the alert should come up
        for(int c1 = dayIndex; c1 < numberOfDays; c1++)
        {
            howManyDaysUntilNextAlert++;
            if(isDaySet(possibleDaysOfWeek[c1]))
            {
                foundNextDate = true;
                break;
            }
        }
        //if a next date hasn't already been found, loop around (sunday -> today) to check up until the same day next week.
        if(!foundNextDate)
        {
            for(int c1 = 0; c1 < dayIndex; c1++)
            {
                howManyDaysUntilNextAlert++;
                if(isDaySet(possibleDaysOfWeek[c1])){
                    foundNextDate = true;
                    break;
                }
            }
        }
        //only set the next alert if there is a day checked off
        if(foundNextDate)
        {
            long long nextAlertTime = ONE_DAY_IN_MSEC * (long long)howManyDaysUntilNextAlert;

            //set next alert time to 00:00:00 of next alert day
            nextAlertTime -= timeNow.msecsSinceStartOfDay();

            //add the number of msecs until the alert on that day
            nextAlertTime += earliestTime.msecsSinceStartOfDay();

            debugNextTimeAlert(nextAlertTime);
            return nextAlertTime;
        }
    }
    //this case should only happen when no boxes are checked
    return LONG_LONG_MAX;
}

long long message::getNextExpireMonthly()
{
    //qDebug() << "Monthly";
    QTime timeNow = QTime::currentTime();
    QDate dayToday = QDate::currentDate();
    int todaysDate = dayToday.day();
    int daysInThisMonth = dayToday.daysInMonth();
    long long minimumNextAlertTime = LONG_LONG_MAX;
    QTime earliestTime = QTime(23,59);

    for(QTime time: times)
    {
        //Skip if the time is blank as it will set the next expiration to now
        if(time.toString() == "")
            continue;
        int msecsUntilTime = timeNow.msecsTo(time);
        if(msecsUntilTime >= 0 && msecsUntilTime < minimumNextAlertTime)
        {
            minimumNextAlertTime = msecsUntilTime;
        }
        if(time < earliestTime)
        {
            earliestTime = time;
        }
    }

    for(int day: dates){
        if(day == 0) break; //text field defaults to 0 if invalid text.
        if(day > daysInThisMonth) break; //don't set alert if date is out of bounds of month (e.g. June has 30 days, alert day = 31)
        if(day == todaysDate){
            debugNextTimeAlert(minimumNextAlertTime);
            return minimumNextAlertTime;
        }

        long long nextAlertTime = LONG_LONG_MAX;

        //if next time involves wraparound to the next month
        if(day <= todaysDate){
            int daysUntilNextAlert = day - todaysDate + daysInThisMonth;
            nextAlertTime = daysUntilNextAlert * (long long)ONE_DAY_IN_MSEC;
        }
        else{
            int daysUntilNextAlert = day - todaysDate;
            nextAlertTime = daysUntilNextAlert * (long long)ONE_DAY_IN_MSEC;
        }

        //set next alert time to 00:00:00 of next alert day
        nextAlertTime -= timeNow.msecsSinceStartOfDay();

        //add the number of msecs until the alert on that day
        nextAlertTime += earliestTime.msecsSinceStartOfDay();

        debugNextTimeAlert(nextAlertTime);
        return nextAlertTime;

    }

    return LONG_LONG_MAX;
}

void message::startTimer(){
    // use my set interval because it checks for timer overflow
    setInterval(getNextExpire());
    connect(timer, SIGNAL(timeout()), this, SLOT(slt_TimerExpired()));
    timer->start();
}

void message::setInterval(long long interval){
    if(interval > INT_MAX){
        timer->setInterval(INT_MAX);
        remainder = interval - INT_MAX;
        overFlow = true;
    }else{
        timer->setInterval((int)interval);
        overFlow = false;
    }
}

bool message::getIsSystemMessage() const
{
    return IsSystemMessage;
}

void message::setIsSystemMessage(bool value)
{
    IsSystemMessage = value;
}

bool message::getIsDiabled() const
{
    return IsDiabled;
}

void message::setIsDiabled(bool value)
{
    IsDiabled = value;
}

QString message::getHeader() const
{
    return header;
}

void message::setHeader(const QString &value)
{
    header = value;
}

QString message::getMsg() const
{
    return msg;
}

void message::setMsg(const QString &value)
{
    msg = value;
}

int message::getType() const
{
    return type;
}

void message::setType(const int &value)
{
    type = value;
}

int message::getDayOfWeek() const
{
    return dayOfWeek;
}

void message::setDayOfWeek(const int &value)
{
    dayOfWeek = value;
}

bool message::isDaySet(const msgBoxConfig::dayOfWeek day)
{
    if((dayOfWeek & day) != 0)
        return true;
    return false;
}

void message::setRepeatInterval(const int interval)
{
    this->interval = interval;
}

void message::addDates(int date){
    this->dates.append(date);
}

void message::addTimes(QTime time){
    this->times.append(time);
}

void message::setLastAlerted(const qint64 time){
    this->lastAlerted = time;
}

void message::setLastConfirmCompleted(const qint64 time){
    this->lastConfirmCompleted = time;
}

qint64 message::getLastConfirmCompleted(){
    return lastConfirmCompleted;
}

void message::updateMessageFileLastConfirmCompleted(){
    this->setLastConfirmCompleted((QDateTime::currentMSecsSinceEpoch()/1000));
    QSettings s(msgBoxConfig::configFileName(), QSettings::IniFormat);
    s.beginGroup("CONFIG");

    int count(s.value("Count",0).toInt());

    s.endGroup();

    for (int index(0); index < count; ++index)
    {
        static QString group("MESSAGE%0");
        s.beginGroup(group.arg(index));

        const int sUniqueId(s.value("UniqueId",QString()).toInt() );

        if(sUniqueId == this->uniqueId){
            s.setValue("LastConfirmCompleted", this->lastConfirmCompleted);
            //qDebug() << "Set last confirm completed for: " << this->header << " as: " << this->lastConfirmCompleted;
        }

        s.endGroup();
    }

}

void message::setUniqueId(const int uniqueId){
    this->uniqueId = uniqueId;
}

int message::getUniqueId(){
    return this->uniqueId;
}

void message::updateMessageFileLastAlerted(){
    //store in secs. Qt 5.5 does not support QDateTime::currentSecsSinceEpoch()
    setLastAlerted((QDateTime::currentMSecsSinceEpoch()/1000));
    QSettings s(msgBoxConfig::configFileName(), QSettings::IniFormat);
    s.beginGroup("CONFIG");

    int count(s.value("Count",0).toInt());

    s.endGroup();

    for (int index(0); index < count; ++index)
    {
        static QString group("MESSAGE%0");
        s.beginGroup(group.arg(index));

        const int sUniqueId(s.value("UniqueId",QString()).toInt() );


        if(sUniqueId == this->uniqueId){
            s.setValue("LastAlerted", this->lastAlerted);
            //qDebug() << "Set last alerted for: " << this->header << "--my id: " << this->uniqueId << "--checkid: " << sUniqueId;
        }


        s.endGroup();
    }

}

void message::slt_TimerExpired()
{
    timer->stop();
    if(overFlow){
        setInterval(remainder);
        overFlow = false;
        timer->start();
    }else{
        //If French, translator is set to convert to "dd/MM/yyyy -- hh:mm" instead
        QString lastCompletedTime = QDateTime::fromTime_t(this->lastConfirmCompleted).toString(tr("MM/dd/yyyy -- hh:mm"));
        if(this->lastConfirmCompleted != 0)
            Q_EMIT sig_DisplayMessage(header, msg, lastCompletedTime, uniqueId); //Emit signal to mainwindow
        else //If this is the first time message has ever been sent, set to Never
            Q_EMIT sig_DisplayMessage(header, msg, tr("Never"), uniqueId); //Emit signal to mainwindow
        std::cout << header.toStdString() << std::endl;

        updateMessageFileLastAlerted();

        //qDebug() << "Next Expire: " << getNextExpire();
        setInterval(getNextExpire());
        timer->start();
    }
}
