#ifndef SUPERVISORMENU_H
#define SUPERVISORMENU_H

#include <QDialog>
#include "hobart/usb/weigherconfiguration.h"
#include <QApplication>

namespace Ui {
class supervisorMenu;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
class supervisorMenu : public QDialog
{
    Q_OBJECT

    typedef supervisorMenu _Myt;
    typedef QDialog _Mybase;

    Ui::supervisorMenu *ui_;

public:
    explicit supervisorMenu(QWidget *parent = 0);
    ~supervisorMenu();
signals:
    void sig_MessageUpdated(); //Signal from msgboxconfig to msgbox
    void sig_UpdateLanguage(QString language); //Signal from generalconfig to mainwindow
    void sig_ReqConfig(); //Signal from generalconfig to mainwindow
    void sig_WriteConfig( WeigherConfiguration & config); //Signal from generalconfig to mainwindow
    void sig_WeigherReset(); //Signal from generalconfig to mainwindow
    void sig_WeigherConfigChanged(WeigherConfiguration & config); //Signal from Weigher to GeneralConfig

protected:
    void showEvent(QShowEvent *e);

private slots:
    void onAlertMsgSetup();
    void alertMsgSetup();

    void onSpeedKeySetup();
    void speedKeySetup();

    void onSetupUtility();
    void setupUtility();

    void onStats();
    void initStats();

private:
    void initialize();
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SUPERVISORMENU_H
