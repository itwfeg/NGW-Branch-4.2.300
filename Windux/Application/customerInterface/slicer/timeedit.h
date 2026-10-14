#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QDialog>
#include <QTime>

namespace Ui {
class TimeEdit;
}

class TimeEdit : public QDialog
{
    Q_OBJECT

public:
    explicit TimeEdit(QWidget *parent = 0);
    ~TimeEdit();

private slots:
    void slt_UpdateTime();
    void slt_SetTime();
    void slt_ChangeCalendarMonth(int);
    void slt_ChangeCalendarYear(int);
    void slt_CalendarChanged(int, int);
    void slt_HourSelect();
    void slt_MinuteSelect();
    void slt_SecondSelect();
    void slt_TimeEditTimeChanged(QTime);

protected:
    void showEvent(QShowEvent *e);
    bool eventFilter(QObject *, QEvent *);

private:
    Ui::TimeEdit *ui;
    void initialize();

    enum months_{
        January = 1,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };
};

#endif // TIMEEDIT_H
