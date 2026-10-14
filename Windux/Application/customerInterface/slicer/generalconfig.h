#ifndef GENERALCONFIG_H
#define GENERALCONFIG_H

#include <QDialog>
#include "hobart/usb/weigherconfiguration.h"

namespace Ui {
class GeneralConfig;
}

class GeneralConfig : public QDialog
{
    Q_OBJECT

public:
    explicit GeneralConfig(QWidget *parent = 0);
    ~GeneralConfig();

signals:
    void sig_UpdateLanguage(QString language); //Emit signal to mainwindow
    void sig_ReqConfig(); //Emit signal to mainwindow
    void sig_WriteConfig( WeigherConfiguration & config); //Emit signal to mainwindow
    void sig_WeigherReset(); //Emit signal to mainwindow
    void sig_WeigherConfigChanged(WeigherConfiguration & config); //Signal from Weigher to GeneralConfig

protected:
    void showEvent(QShowEvent *);

private slots:
    void slt_ModelChanged(QString model);
    void closeDialog();

private:
    Ui::GeneralConfig *ui;
    void initialize();
    void loadSettings();
    void save();
};

#endif // GENERALCONFIG_H
