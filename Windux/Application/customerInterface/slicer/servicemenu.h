#ifndef SERVICEMENU_H
#define SERVICEMENU_H

#include <QWidget>
#include "hobart/usb/weigherconfiguration.h"
#include "mainmenu.h"

namespace Ui {
class servicemenu;
}

class servicemenu : public QDialog
{
    Q_OBJECT
    typedef servicemenu _Myt;
    typedef QDialog _Mybase;

public slots:
    void slt_WeigherKeyPressed();

    void slt_WeigherConfigChanged(WeigherConfiguration &config);
    void slt_WriteConfig(WeigherConfiguration &config);
    void slt_ReqConfig();
    void slt_RezeroWeigher();
    void slt_WeigherStatus(char*buffer, int buffLen);
    void slt_SetWeigherMode(WeighingMode mode);
    void slt_WeigherReset();

private slots:
        void onConfigButtonClicked();
        void onCalibButtonClicked();

        void onSetupUtility();
        void setupUtility();

signals:
        void sig_WeigherKeyPressed();
        void sig_RezeroWeigher();
        void sig_ReqConfig();
        void sig_WeigherConfigChanged(WeigherConfiguration & config);
        void sig_WriteConfig( WeigherConfiguration & config);
        void sig_WeigherStatus(char* buffer ,int buffLen);
        void sig_SetWeigherMode(WeighingMode mode);
        void sig_WeigherReset();
        void sig_UpdateLanguage(QString language); //Propagate signal from generalconfig

public:
    explicit servicemenu(QWidget *parent = 0);
    ~servicemenu();

protected:
        void showEvent(QShowEvent *e);
private:
        Ui::servicemenu *ui;

};

#endif // SERVICEMENU_H
