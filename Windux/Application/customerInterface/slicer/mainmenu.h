#ifndef MAINMENU_H
#define MAINMENU_H

#include <QDialog>
#include <QApplication>
#include "hobart/usb/weigherconfiguration.h"
#include "msgbox.h"
#include "usbkey.h"

namespace Ui {
class mainMenu;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
class mainMenu : public QDialog
{
    Q_OBJECT

    typedef mainMenu _Myt;
    typedef QDialog _Mybase;

    Ui::mainMenu *ui_;

public:
    explicit mainMenu(QWidget *parent = 0);
    ~mainMenu();

protected:
    void showEvent(QShowEvent *e);

public slots:
//    void slt_WeigherKeyPressed();
//    void slt_WeigherConfigChanged(WeigherConfiguration &config);
    void slt_WriteConfig(WeigherConfiguration &config);
    void slt_ReqConfig();
    void slt_RezeroWeigher();
    void slt_WeigherStatus(char*buffer, int buffLen);
    void slt_SetWeigherMode(WeighingMode mode);
    void slt_WeigherReset();
    void supervisorScreen();

signals:
        void sig_WeigherKeyPressed();
        void sig_ReqConfig();
        void sig_WeigherConfigChanged(WeigherConfiguration & config); //Propagate signal from mainwindow
        void sig_WriteConfig( WeigherConfiguration & config);
        void sig_RezeroWeigher();
        void sig_WeigherStatus(char*buffer, int buffLen);
        void sig_SetWeigherMode(WeighingMode mode);
        void sig_WeigherReset();
        void sig_MessageUpdated(); //Signal from msgboxconfig to msgbox
        void sig_UpdateLanguage(QString language); //Propagate signal from generalconfig

private slots:
    void onServiceScreen();
    void serviceScreen();
    void onSupervisorScreen();
    void updateSupervisorKeyMessage();
    void updateServiceKeyMessage();

private:

    void initialize();
    void createLayout();

    QTimer *timerSupervisorKeyLabel;
    QTimer *timerServiceKeyLabel;

    UsbKey* m_usbKey;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // MAINMENU_H
