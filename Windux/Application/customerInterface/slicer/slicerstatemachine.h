#ifndef SLICER_STATE_MACHINE_H
#define SLICER_STATE_MACHINE_H

#include <QObject>
#include <QStateMachine>

#include "kniferunin.h"
#include "onpausebuttonsim.h"
#include "scale.h"

class SlicerStateMachine : public QObject
{
    Q_OBJECT

public:
    enum incrementMode_e
    {
        IncrementBySingleSlice = 0,
        IncrementBySetpoint,
        IncrementBySetpointManual,


        NUMBER_INCREMENT_MODES
    };

    explicit SlicerStateMachine(
        OnPauseButtonSim& onPauseButtonSim,
        Scale& scale);

    incrementMode_e IncrementMode() const { return m_incrementMode; }

public slots:
    void slt_IncrementSetpoint();
    void slt_DecrementSetpoint();
    void slt_Zero();
    void slt_CycleIncrementMode();
    void slt_isKnifeRunning(bool isKnifeRunning);
    void slt_CheckUpdateSlicerType();

signals:
    void sig_SetpointChanged(ScaleInterface::weightInPounds_t weight);
    void sig_Stopped();
    void sig_Running();
    void sig_SetpointReached();
    void sig_Paused();
    void sig_Error(ScaleInterface::State);
    void sig_IncrementModeChanged(SlicerStateMachine::incrementMode_e incrementMode);
    void sig_CancelHalt();
    void sig_Asserted();
    void sig_Deasserted();

private slots:
    void slt_OnEnterNotRunning();
    void slt_OnExitNotRunning();
    void slt_OnExitStopped();
    void slt_OnEnterRunning();
    void slt_OnExitRunning();
    void slt_OnEnterStopped();
    void slt_SetpointChanged(ScaleInterface::weightInPounds_t weight);
    void slt_WeightUpdated(ScaleInterface::State state, ScaleInterface::weightInPounds_t weight);
    void slt_AboveSetpoint();
    void slt_KnifeRunError(ScaleInterface::State);
    void slt_OnErrorEntered();

private:
    static const QString s_incrementModeKey;
    ScaleInterface::weightInPounds_t m_noiseThreshold;
    QStateMachine m_stateMachine;
    Scale& m_scale;
    QTimer m_onPauseDelayTimer;
    int m_onPauseDelayMs;
    bool m_setpointAdjustEnabled;
    bool m_zeroEnabled;
    bool m_weighingEnabled;
    ScaleInterface::weightInPounds_t m_originalSetpoint;
    ScaleInterface::weightInPounds_t m_endWeight;
    incrementMode_e m_incrementMode;
    ScaleInterface::weightInPounds_t m_prevWeight;
};

#endif // SLICER_STATE_MACHINE_H
