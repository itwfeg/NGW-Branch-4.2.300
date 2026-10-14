#ifndef MESSAGE_H
#define MESSAGE_H

#define ONE_DAY_IN_MSEC 86400000

#include <QTimer>
#include <QTime>
#include <QDate>
#include <iostream>
#include "msgboxconfig.h"
#include "standardpopup.h"

class message : public QObject
{
    Q_OBJECT

public:
    explicit message(QObject *parent);

    void startTimer();
    int getDayOfWeek() const;
    void setDayOfWeek(const int &value);

    int getType() const;
    void setType(const int &value);

    QString getMsg() const;
    void setMsg(const QString &value);

    QString getHeader() const;
    void setHeader(const QString &value);

    bool getIsDiabled() const;
    void setIsDiabled(bool value);

    bool getIsSystemMessage() const;
    void setIsSystemMessage(bool value);

    void addDates(int date);
    void addTimes(QTime time);
    bool isDaySet(const msgBoxConfig::dayOfWeek day);


    void setRepeatInterval(const int interval);

    int getUniqueId();
    void setUniqueId(const int uniqueId);

    void setLastAlerted(const qint64 time);
    void setLastConfirmCompleted(const qint64 time);
    qint64 getLastConfirmCompleted();

    void updateMessageFileLastAlerted();
    void updateMessageFileLastConfirmCompleted();

signals:
    void sig_DisplayMessage(QString header, QString msg, QString lastCompleted, int uniqueId); //Emit signal to mainwindow

public slots:
    void slt_TimerExpired();

private:
    QTimer * timer;

    bool IsSystemMessage;
    bool IsDiabled;
    bool overFlow;

    int remainder;

    int interval;


    QString header;
    QString msg;

    int type = msgBoxConfig::Monthly;
    int dayOfWeek;

    QList<int> dates;

    QList<QTime> times;

    int uniqueId;

    //stores the secs since epoch
    qint64 lastAlerted;
    qint64 lastConfirmCompleted;

    long long getNextExpire();
    long long getNextExpireWeekly();
    long long getNextExpireMonthly();
    void setInterval(long long interval);


    void debugNextTimeAlert(long long msecs);

    const QList<msgBoxConfig::dayOfWeek> possibleDaysOfWeek =
    {
        msgBoxConfig::Sunday,
        msgBoxConfig::Monday,
        msgBoxConfig::Tuesday,
        msgBoxConfig::Wednesday,
        msgBoxConfig::Thursday,
        msgBoxConfig::Friday,
        msgBoxConfig::Saturday,
    };
};

#endif // MESSAGE_H
