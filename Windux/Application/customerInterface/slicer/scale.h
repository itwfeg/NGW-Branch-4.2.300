#ifndef SCALE_H
#define SCALE_H

#include <QObject>
#include <QTimer>
#include "scaleinterface.h"
#include "hobart/usb/weigherconfiguration.h"
#include "slicerstatistics.h"
#include <QElapsedTimer>

#define PULSE_WIDTH 50 //in miliseconds
#define POLLING_SPEED 100 //in miliseconds
#define MILISECONDS_DELAY_SINGLE_SLICE_STOPPED 1400
#define MILISECONDS_DELAY_SINGLE_SLICE_CONTINUED 1000
class Scale : public QObject
{
    Q_OBJECT

public:
    explicit Scale(ScaleInterface& scaleInterface, bool filtering = false);
    ScaleInterface::weightInPounds_t Weight() const { return m_weight; }
    ScaleInterface::weightInPounds_t Setpoint() const { return m_setpoint; }
    void stopSlicing();

    enum InfinityModeState{
        noInfinityMode = 0,
        infinityFromBottomMode = 1,
        infinityFromTopMode = 2,
        infinityFromSpeedKey
    };

signals:
    void sig_SetpointChanged(ScaleInterface::weightInPounds_t weight);
    void sig_WeightChanged(ScaleInterface::State state,ScaleInterface::weightInPounds_t weight);
    void sig_AtOrAboveSetpoint();
    void sig_BelowSetpoint();
    void sig_Zeroing();
    void sig_WeigherKeyPressed();
    void sig_ReqConfig();
    void sig_WeigherConfigChanged(WeigherConfiguration & config);
    void sig_WriteConfig(WeigherConfiguration & config);
    void sig_WeigherStatus(char*buffer, int buffLen);
    void sig_SetWeigherMode(WeighingMode mode);
    void sig_WeigherReset();
    void sig_isKnifeRunning(bool isKnifeRunning);
    void sig_InfinityMode(Scale::InfinityModeState mode);
    void sig_KnifeRunError(ScaleInterface::State);
    void sig_TimeoutPopup();

public slots:
    void slt_IncrementSetpoint();
    void slt_DecrementSetpoint();
    void slt_Zero();
    void slt_SetSetpoint(ScaleInterface::weightInPounds_t weight);
    void slt_updateSetPoint(double value);
    void slt_updateWeightTolerance(double value);
    void slt_WeigherKeyPressed();
    void slt_ReqConfig();
    void slt_WriteConfig(WeigherConfiguration & config);
    void slt_WeigherStatus(char*buffer, int buffLen);
    void slt_SetWeigherMode(WeighingMode mode);
    void slt_WeigherReset();
    void slt_IncrementModeChanged(int incMode);
    void slt_IncrementStepChanged(ScaleInterface::weightInPounds_t step);
    void slt_ModeChanged(int mode);

private:
    int mode_;
    ScaleInterface& m_scaleInterface;
    ScaleInterface::weightInPounds_t m_maximumSupportedWeight;
    ScaleInterface::weightInPounds_t m_weightStep;
    ScaleInterface::weightInPounds_t m_setpoint;
    ScaleInterface::weightInPounds_t m_prevSetpoint;
    ScaleInterface::weightInPounds_t m_weightTolerance;
    ScaleInterface::weightInPounds_t m_weight;
    ScaleInterface::weightInPounds_t m_startingWeight;
    ScaleInterface::weightInPounds_t m_belowSetpointHysteresis;
    ScaleInterface::weightInPounds_t m_weightBeforeExtra;
    ScaleInterface::weightInPounds_t m_totalWeight;

    ScaleInterface::State m_state;
    bool m_filtering;
    int m_setpointFilterLength;
    int m_setpointCount;

    int m_incrementMode;

    unsigned long long runTimeMs;

    bool isSingleSliceMode;
    bool isKnifeRunning;
    bool wasKnifeRunning;
    bool firstSlice;
    bool signalSent;

    QTimer knifeRunPolling_;
    QTimer knifeRunError_;
    QElapsedTimer runningTimer;

    int repitionsCounter;

    bool KnifeRunning();

    InfinityModeState m_infinityMode;

    SlicerStatistics * stats;

    void handleSingleSlice(ScaleInterface::weightInPounds_t weight);
    void handleSetpoint(ScaleInterface::weightInPounds_t weight);
    void handleSetpointManual(ScaleInterface::weightInPounds_t weight);


private slots:
    void slt_WeightChanged(ScaleInterface::State state,ScaleInterface::weightInPounds_t weight);
    void slt_WeigherConfigChanged(WeigherConfiguration &config);

    void onPause();
    void slt_knifeRunPolling();
    void slt_handleKnifeStop(bool);
    void slt_KnifeRunError();


};

#endif // SCALE_H
