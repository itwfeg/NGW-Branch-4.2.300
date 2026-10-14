#ifndef SETUPUTILITY_H
#define SETUPUTILITY_H

#include <QDialog>
#include "usbkey.h"
#include "hobart/usb/weigherconfiguration.h"

namespace Ui {
class SetupUtility;
}

class SetupUtility : public QDialog
{
    Q_OBJECT

public:
    explicit SetupUtility(QWidget *parent = 0);
    ~SetupUtility();

signals:
    void sig_UpdateLanguage(QString language); //Signal from generalconfig to mainwindow
    void sig_ReqConfig(); //Signal from generalconfig to mainwindow
    void sig_WriteConfig( WeigherConfiguration & config); //Signal from generalconfig to mainwindow
    void sig_WeigherReset(); //Signal from generalconfig to mainwindow
    void sig_WeigherConfigChanged(WeigherConfiguration & config); //Signal from Weigher to GeneralConfig

protected:
    void showEvent(QShowEvent *e);

private slots:
    void onCalibTouch();
    void onInstallTouch();
    void generalConfigure();
    void onTimeEdit();
    void timeEdit();
    void onConfigure();

private:
    Ui::SetupUtility *ui;

    UsbKey* m_usbKey;
};

#endif // SETUPUTILITY_H
