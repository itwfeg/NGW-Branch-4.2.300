#include "slicerstatemachine.h"
#include <QSignalTransition>
#include <QSettings>
#include <QDebug>
#include <QProcess>

const QString SlicerStateMachine::s_incrementModeKey = "IncrementMode";

SlicerStateMachine::SlicerStateMachine(
    OnPauseButtonSim& onPauseButtonSim,
    Scale& scale) :
    m_scale(scale),
    m_setpointAdjustEnabled(true),
    m_zeroEnabled(true),
    m_originalSetpoint(scale.Setpoint()),
    m_incrementMode(SlicerStateMachine::IncrementBySetpoint),
    m_prevWeight(0.0)
{
    qRegisterMetaType<SlicerStateMachine::incrementMode_e>("SlicerStateMachine::incrementMode_e");

    QSettings settings;
    m_incrementMode = (incrementMode_e)settings.value(s_incrementModeKey, (int)IncrementBySetpoint).toInt();
    m_noiseThreshold = (ScaleInterface::weightInPounds_t)settings.value("WeightNoiseThreshold", 0.01).toFloat();

    QState* pNotRunningState = new QState();
    QState* pStoppedState = new QState(pNotRunningState);
    QState* pHaltedState = new QState(pNotRunningState);
    QState* pSetpointReachedState = new QState();
    QState* pRunningState = new QState();
    QState* pErrorState = new QState(pHaltedState);
    QState* pPausedState = new QState(pHaltedState);

    m_stateMachine.addState(pNotRunningState);
    m_stateMachine.addState(pRunningState);
    m_stateMachine.addState(pSetpointReachedState);

    m_stateMachine.setInitialState(pNotRunningState);
    pNotRunningState->setInitialState(pStoppedState);
    pHaltedState->setInitialState(pPausedState);

    //transitions
    pNotRunningState->addTransition(this, SIGNAL(sig_Asserted()), pRunningState);
    pRunningState->addTransition(this, SIGNAL(sig_Deasserted()), pPausedState);
    pHaltedState->addTransition(this, SIGNAL(sig_CancelHalt()), pStoppedState);
    pPausedState->addTransition(this, SIGNAL(sig_CancelHalt()), pStoppedState);
    pNotRunningState->addTransition(this, SIGNAL(sig_CancelHalt()), pStoppedState);
    pRunningState->addTransition(&scale, SIGNAL(sig_KnifeRunError(ScaleInterface::State)),pStoppedState);
    //pRunningState->addTransition(&m_onPauseDelayTimer, SIGNAL(timeout()), pSetpointReachedState);
    pSetpointReachedState->addTransition(&scale, SIGNAL(sig_BelowSetpoint()), pStoppedState);
    //QSignalTransition* pIncrementTransition = pSetpointReachedState->addTransition(this, SIGNAL(sig_Asserted()), pRunningState);
    //(void)connect(pIncrementTransition, SIGNAL(triggered()), this, SLOT(slt_IncrementalSlice()));

    //enter/exit actions
    (void)connect(pNotRunningState, SIGNAL(entered()), this, SLOT(slt_OnEnterNotRunning()));
    (void)connect(pNotRunningState, SIGNAL(exited()), this, SLOT(slt_OnExitNotRunning()));
    (void)connect(pStoppedState, SIGNAL(entered()), this, SLOT(slt_OnEnterStopped()));
    (void)connect(pStoppedState, SIGNAL(exited()), this, SLOT(slt_OnExitStopped()));
    (void)connect(pRunningState, SIGNAL(entered()), this, SLOT(slt_OnEnterRunning()));
    (void)connect(pRunningState, SIGNAL(exited()), this, SLOT(slt_OnExitRunning()));
    //(void)connect(pSetpointReachedState, SIGNAL(entered()), &onPauseButtonSim, SLOT(slt_SimulatePress()));
    (void)connect(pRunningState, SIGNAL(entered()), this, SIGNAL(sig_Running()));
    (void)connect(pSetpointReachedState, SIGNAL(entered()), this, SIGNAL(sig_SetpointReached()));
    (void)connect(pPausedState, SIGNAL(entered()), this, SIGNAL(sig_Paused()));
    //(void)connect(pErrorState, SIGNAL(entered()), &onPauseButtonSim, SLOT(slt_SimulatePress()));
    (void)connect(pErrorState, SIGNAL(entered()), this, SLOT(slt_OnErrorEntered()));

    m_onPauseDelayMs = settings.value("OnPauseDelayMs", 1000).toInt();
    m_onPauseDelayTimer.setInterval(m_onPauseDelayMs);
    m_onPauseDelayTimer.setSingleShot(true);


    (void)connect(&scale, SIGNAL(sig_SetpointChanged(ScaleInterface::weightInPounds_t)), this, SLOT(slt_SetpointChanged(ScaleInterface::weightInPounds_t)));
    (void)connect(&scale, SIGNAL(sig_WeightChanged(ScaleInterface::State,ScaleInterface::weightInPounds_t)), this, SLOT(slt_WeightUpdated(ScaleInterface::State,ScaleInterface::weightInPounds_t)));
    (void)connect(&scale, SIGNAL(sig_KnifeRunError(ScaleInterface::State)), this, SLOT(slt_KnifeRunError(ScaleInterface::State)));
    (void)connect(&scale, SIGNAL(sig_AtOrAboveSetpoint()), this, SLOT(slt_AboveSetpoint()));

    m_stateMachine.start();
}

void SlicerStateMachine::slt_IncrementSetpoint()
{
    if (!m_setpointAdjustEnabled)
    {
        return; //do nothing
    }

    m_scale.slt_IncrementSetpoint();
    //emit sig_CancelHalt();
}

void SlicerStateMachine::slt_DecrementSetpoint()
{
    if (!m_setpointAdjustEnabled)
    {
        return; //do nothing
    }

    m_scale.slt_DecrementSetpoint();
    //emit sig_CancelHalt();
}

void SlicerStateMachine::slt_Zero()
{
    if (!m_zeroEnabled)
    {
        return; //do nothing
    }

    m_scale.slt_Zero();
    //emit sig_CancelHalt();
}

void SlicerStateMachine::slt_CheckUpdateSlicerType()
{
    QSettings settings;
    bool isManualSlicer = settings.value("isManualSlicer").toBool();
    bool changedIncrementMode = false;
    if(isManualSlicer && m_incrementMode == SlicerStateMachine::IncrementBySetpoint){
        m_incrementMode = SlicerStateMachine::IncrementBySetpointManual;
        changedIncrementMode = true;
    }
    else if(!isManualSlicer){
        m_incrementMode = SlicerStateMachine::IncrementBySetpoint;
        changedIncrementMode = true;
    }

    if(changedIncrementMode){
        settings.setValue(s_incrementModeKey, (int)m_incrementMode);
        settings.sync();
        //run the linux sync to ensure file saves on power shut off before written into persistent storage
        static const QString linuxSync( "/bin/sync");
        QProcess p(this);
        p.start(linuxSync);
        p.waitForFinished(30000);
        emit sig_IncrementModeChanged(m_incrementMode);
    }

}

void SlicerStateMachine::slt_CycleIncrementMode()
{
    QSettings settings;
    bool isManualSlicer = settings.value("isManualSlicer").toBool();

    m_incrementMode = (incrementMode_e)((m_incrementMode + 1) % NUMBER_INCREMENT_MODES);

    //skip regular setpoint mode if it is a manual slicer
    if(isManualSlicer){
        if(m_incrementMode == SlicerStateMachine::IncrementBySetpoint)
            m_incrementMode = (incrementMode_e)((m_incrementMode + 1) % NUMBER_INCREMENT_MODES);
    }
    //skip the manual setpoint mode if it is an automatic slicer
    if(!isManualSlicer){
        if(m_incrementMode == SlicerStateMachine::IncrementBySetpointManual)
            m_incrementMode = (incrementMode_e)((m_incrementMode + 1) % NUMBER_INCREMENT_MODES);
    }
    settings.setValue(s_incrementModeKey, (int)m_incrementMode);
    emit sig_IncrementModeChanged(m_incrementMode);
}

void SlicerStateMachine::slt_isKnifeRunning(bool isKnifeRunning)
{
    if(isKnifeRunning){
        Q_EMIT sig_Asserted();
    }else{
        Q_EMIT sig_Deasserted();
    }
}

void SlicerStateMachine::slt_OnEnterNotRunning()
{
    m_setpointAdjustEnabled = true;
    m_zeroEnabled = true;
    m_weighingEnabled = true;
}

void SlicerStateMachine::slt_OnExitNotRunning()
{
    m_setpointAdjustEnabled = false;
    m_zeroEnabled = false;
    m_weighingEnabled = false;
}

void SlicerStateMachine::slt_OnExitStopped()
{
    //store the original setpoint for later restoration, in case it needs to be incremented during the upcoming run
    m_originalSetpoint = m_scale.Setpoint();


    //qDebug() << "--ExitStopped--";
}

void SlicerStateMachine::slt_OnEnterRunning()
{
    //check for already at setpoint
    if (m_scale.Weight() >= m_scale.Setpoint())
    {
        m_onPauseDelayTimer.start(m_onPauseDelayMs); //delayed transition to SetpointReached, to ensure ON/PAUSE press is recognized
    }

    m_prevWeight = 0.0;
}

void SlicerStateMachine::slt_OnExitRunning()
{
    m_endWeight = m_scale.Weight();
}

void SlicerStateMachine::slt_OnEnterStopped()
{
    //restore the setpoint
    m_scale.slt_SetSetpoint(m_originalSetpoint);
    //qDebug() << "--Sig_Stopped--";
    m_scale.stopSlicing();
    emit sig_Stopped();
}

void SlicerStateMachine::slt_SetpointChanged(ScaleInterface::weightInPounds_t weight)
{
    if (m_setpointAdjustEnabled)
    {
        emit sig_SetpointChanged(weight);
    }
}

void SlicerStateMachine::slt_WeightUpdated(ScaleInterface::State state, ScaleInterface::weightInPounds_t weight)
{
    if (( ScaleInterface::zero == state ) || ( ScaleInterface::stable == state ))
    {

        if (weight < (m_endWeight - m_noiseThreshold))
        {
            emit sig_CancelHalt();
        }

        m_prevWeight = weight;
    }
}

void SlicerStateMachine::slt_AboveSetpoint()
{
    //qDebug() << "SlicerStateMachine::slt_AboveSetpoint()";
    if (!m_onPauseDelayTimer.isActive())
    {
        m_onPauseDelayTimer.start(0); //immediate timeout (and transition to SetpointReached)
    }
    //otherwise, delayed timeout (need to do our best to ensure ON/PAUSE press is recognized) (and transition to SetpointReached)
}

void SlicerStateMachine::slt_KnifeRunError(ScaleInterface::State state){
    //qDebug() << "slt_KnifeRunError: " << state;
    emit sig_Error(state);
}

void SlicerStateMachine::slt_OnErrorEntered(){
    //qDebug() << "slt_OnErrorEntered" ;
}
