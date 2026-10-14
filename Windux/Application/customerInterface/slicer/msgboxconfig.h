#ifndef MSGBOXCONFIG_H
#define MSGBOXCONFIG_H

#include <QDialog>
#include <QList>
#include <QStringList>

namespace Ui {
class msgBoxConfig;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
class msgBoxConfig : public QDialog
{
    Q_OBJECT

    typedef msgBoxConfig _Myt;
    typedef QDialog _Mybase;

    Ui::msgBoxConfig *ui_;

public:
    typedef QList<QStringList> messageList;

    enum type
    {
        Monthly = 0,
        Weekly = 1,
        Daily = 2,

        MAX_MESSAGE_TYPES
    };

    enum dayOfWeek
    {
        Sunday = 0x01,
        Monday = 0x02,
        Tuesday = 0x04,
        Wednesday = 0x08,
        Thursday = 0x10,
        Friday = 0x20,
        Saturday = 0x40
    };

    explicit msgBoxConfig(QWidget *parent = 0);
    ~msgBoxConfig();

    // Message Ordering: -- used for array indexing
    enum MessageType
    {
        IsSystemMsg = 0,
        IsDisabled,
        Header,
        Message,
        Type,
        DayOfWeek,
        Date1,
        Date2,
        Time1,
        Time2,
        Time3,
        Time4,
        Time5,
        Time6,
        Time7,
        Time8,
        Interval,
        UniqueId,
        LastAlerted,
        LastConfirmCompleted,
        NumberMessageTypes
    };

    static QString configFileName();
    static messageList readMessages();
    static void writeMessages(const messageList &values);

signals:
    void sig_MessageUpdated(); //Emit signal to msgbox

protected:
    void showEvent(QShowEvent *e);

private slots:
    void onLeftArrowClicked();
    void onRightArrowClicked();
    void onDeleteClicked();
    void onNewClicked();
    void onAddTimeClicked();
    void onAddDateClicked();

    void onCancel();
    void onSave();

    void onTypeChanged(int type) const;

    void slt_UpdateTime();

private:
    enum {MAX_KEYS = 8};

    void initialize();
    void createLayout();

    void updateCurrent() const;
    void saveCurrent();

    messageList messages_;
    messageList messagesOrig_;

    void initSystemMessages();

    int current_;

    int maxUniqueId_;
    void updateMaxUniqueId();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // MSGBOXCONFIG_H
