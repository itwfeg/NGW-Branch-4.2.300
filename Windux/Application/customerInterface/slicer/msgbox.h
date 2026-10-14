#ifndef MSGBOX_H
#define MSGBOX_H

#include "message.h"

#include <QMessageBox>
#include <QDialog>
#include <QSettings>
#include <QTimer>

class msgbox : public QObject
{
    Q_OBJECT
public:
    explicit msgbox(QObject* parent = 0);
    ~msgbox();

    void start();
     void readSettings();

     void initMessageTimer();
     void createMessageList();
     void init();

     void updateMessageList();
     const QList<message*> getMessageList(){return * messageList;}
signals:
     void sig_DisplayMessage(QString, QString, QString, int); //Signal from message to mainwindow

public slots:
    void slt_MessageUpdated(); //From msgboxconfig
    void slt_messageAccepted(int id);

private:
    QList<message*> * messageList;

};




#endif // MSGBOX_H
