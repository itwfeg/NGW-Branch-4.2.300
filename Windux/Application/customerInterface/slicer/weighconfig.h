#ifndef WEIGHCONFIG_H
#define WEIGHCONFIG_H

#include <QMainWindow>
#include "weighcalib.h"
#include "hobart/usb/weigherconfiguration.h"
#define ONEGRAMASLBS 0.0022

#define SMALL_MOTION                            0x0001
#define LARGE_MOTION                            0x0002
#define NEGATIVE_GROSS_WEIGHT                   0x0004
#define WITHIN_QUARTER_GRADUATION_OF_ZERO       0x0008
#define OVER_GROSS_WEIGHT_LIMIT                 0x0010
#define ZERO_OUT_OF_MAINTENANCE_ZONE            0x0020
#define SCALE_WARMUP                            0x0040
#define NEW_ZERO                                0x0080
#define WEIGHER_CALIBRATION_FAILED              0x0100
#define WEIGHER_EEP_READ_FAILED                 0x0200
#define WEIGHER_DISABLED                        0x0400
#define WEIGHER_KEY_PRESSED                     0x0800

namespace Ui {
class weighconfig;
}

class weighconfig : public QDialog
{
    Q_OBJECT

    typedef weighconfig _Myt;
    typedef QDialog _Mybase;

    Ui::weighconfig *ui_;

public:
    explicit weighconfig(QWidget *parent = 0);
    ~weighconfig();

    void createModel();
    void setweight(QString weight);

    void saveWeigherConfig();
    void saveFromGeneralConfig();

Q_SIGNALS:
    void sig_ReqConfig(); //Emit signal to mainwindow
    void sig_WriteConfig(WeigherConfiguration & currentConfig); //Emit signal to mainwindow
    void sig_WeigherReset(); //Emit signal to mainwindow

public Q_SLOTS:
    void slt_WeigherConfigChanged(WeigherConfiguration & config);

protected:
    void showEvent(QShowEvent *e);

private slots:

     void on_backBtn_clicked();
     void on_initBtn_clicked();

     void onEnableConfig();

     void onWeigherModelChanged(int index);

     void onMaxzWeightChanged();

     void slt_initConfigReq();
     void slt_matchSettings();
private:
    Ui::weighconfig *ui;

    WeigherConfiguration currentConfig;
    bool ConfigNeedsUpdated;
    bool writingConfig;

    /// Weigher display modes.

    WeighMode mode_;

    enum MaximumWeightCapacity
    {
        capNone = 0,
        cap30lb,
        cap50lb,
        cap300lb,
        cap15kg,
        cap25kg,
        cap150kg
    };

    enum DivisionSizeFactor
    {
        div0_005_lb = 0,
        div0_01_lb,
        div0_1_lb,
        div0_002_kg,
        div0_005_kg,
        div0_05_kg
    };


    void PopulateDivisionSize();
    void PopulateMaximumWeight();
    void updateSettingDivSize();

    void init();

    int weigherModel;
    QString weigherFilterSpeed;
    int weigherScaleFactor; // Used for Division Size
    QString weigherMaximumWt;
    QString weigherMininumWt;

    bool weigherKeyPressed;

    QString weight;
};

#endif // WEIGHCONFIG_H
