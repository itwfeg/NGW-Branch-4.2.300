#ifndef WEIGHCALIB_H
#define WEIGHCALIB_H

#include <QDialog>
#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>

#include "servicemenu.h"
#include "hobart/usb/weigherconfiguration.h"

namespace Ui {
class weighcalib;
}

class weighcalib : public QDialog
{
    Q_OBJECT

public:
    explicit weighcalib(QWidget *parent = 0);
    ~weighcalib();
    void createLbsModel();
    void createKilogramsModel();

    //Set Style
    void initStyle();

    static uint32_t convertGainFactorToAvoir(const uint32_t gainFactor);
    static uint32_t convertGainFactorToMetric(const uint32_t gainFactor);

signals:
    void sig_ReqConfig();
    void sig_WriteConfig(WeigherConfiguration&);
    void sig_RezeroWeigher();
    void sig_SetWeigherMode(WeighingMode mode);
    void sig_WeigherKeyPressed();
    void sig_WeigherStatus(char *buffer, int buffLen);
    void sig_WeigherReset();
    void sig_WeigherWarmupFinished();


public slots:
    void slt_weight(bool check);
    void slt_WeigherConfigChanged(WeigherConfiguration&);
    void slt_WeigherKeyPressed();
    void slt_WeigherStatus(char * buffer, int buffLen);
    void slt_GainChanged(long gain);

private slots:
    void slt_BeginCalib();
    void slt_NextStep();
    void init();

private:
    Ui::weighcalib *ui;
    QStandardItemModel* model;
    QStandardItem* myitem1;
    QStandardItem* myitem2;
    QStandardItem* myitem3;
    QStandardItem* myitem4;
    QStandardItem* myitem5;
    QStandardItem* myitem6;
    QStandardItem* myitem7;
    QStandardItem* myitem8;

    WeigherConfiguration originConfig;
    WeigherConfiguration currentConfig;

    bool configReady;
    bool weightSetup;
    bool weigherInWarmup;
    bool readingCounts;

    long lastStep;
    long lastDelta;
    long lastZeroed;
    long lastNonZeroed;

    long currentStep;
    long currentDelta;
    long currentZeroed;
    long currentNonZeroed;

    uint32_t currentCounts;
    uint32_t currentFilteredUncalibratedCounts;

    long meanSquareError;
    long gainFactor;
    long centerOfMaintenanceZone;

    //The index of the array represents the current step e.g. The max change in delta at step 3
    // is equal to stepDeltaChange[3] = 400L
    const long stepDeltaChange[7] = {0L, 38300L, 200L, 400L, 200L, 200L, 200L}; //{0L, 76600L, 200L, 400L, 200L, 200L, 200L}; halved for 5, 10, 15 lb calibration 6/29/2018 EDL
    long stepDesiredCountsAvoir[7] = {0L, 50000L, 100000L, 150000L, 100000L, 50000L, 0L}; //{0L, 50000L, 100000L, 150000L, 100000L, 50000L, 0L}; // {0L, 100000L, 200000L, 300000L, 200000L, 100000L, 0L};
    long stepDesiredCountsMetric[7] = {0L, 40000L, 80000L, 120000L, 80000L, 40000L, 0L};//{0L, 60000L, 120000L, 180000L, 120000L, 60000L, 0L};
    long * stepDesiredCounts;
    // stores nonZeroedCounts from each step
    long stepNonZeroedCounts[7];

    // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
    // From: 14316593ULL -> To: 5377310ULL per T. Fink (haberrc, 04-26-2010)
    static const unsigned long long int ScalingConstant = 5377310ULL; // / 2ULL;

    void setDelta();
    long calculateOptimalGainFactor();
    void calculateMeanSquareError();

    long calculateError(long gainFactor, long baseCounts, int step);
    long calculateCenterOfMaintZone();


    void resetAndWaitForWeigher();
public:

    enum weigherDisplayMode
    {
        displayPounds,
        displayOunces,
        displayKilograms,
        displayGrams
    };

    weigherDisplayMode mode;
    QString weigh_mode;
protected:
    void showEvent(QShowEvent *e);

private slots:
    void on_btnCancel_clicked();
    void slt_CompleteCalibration();
};

#endif // WEIGHCALIB_H
