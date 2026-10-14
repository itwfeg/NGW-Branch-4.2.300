#include "scale.h"
#include "slicerstatemachine.h"
#include "slicerstatistics.h"
#include "popupmessage.h"

#include "float.h"

#include <unistd.h>
#include <qdebug.h>
#include <fstream>
#include <iostream>
#include <QtMath>
#include <QSettings>
#include <QFont>
#include <QErrorMessage>
#include <QMessageBox>
#include <QDebug>

Scale::Scale(ScaleInterface& scaleInterface,bool filtering) :
    m_scaleInterface(scaleInterface),
    m_weight(0.0),
    m_filtering(filtering),
    m_setpointCount(0),
    m_infinityMode(noInfinityMode)
{
    QSettings settings;
    stats = new SlicerStatistics(nullptr);
    m_maximumSupportedWeight = settings.value("MaximumWeight", 10.0).toFloat();
    m_weightStep = settings.value("WeightStep", 0.125).toFloat();
    m_setpoint = settings.value("DefaultWeightSetpoint", 0.5).toFloat();
    m_weightTolerance= 0;
    m_setpointFilterLength = settings.value("SetpointFilterLength", 3).toInt();
    m_belowSetpointHysteresis = settings.value("BelowSetpointHysteresis", 0.1).toFloat();

    knifeRunPolling_.setInterval( POLLING_SPEED );
    connect( &knifeRunPolling_, SIGNAL(timeout()), SLOT(slt_knifeRunPolling()) );
    knifeRunPolling_.start();

    //For debugging, set error interval to 5 seconds
    knifeRunError_.setInterval(29000); //29 sec
    //knifeRunError_.setInterval(5000);//5 sec
    knifeRunError_.setSingleShot(true);
    connect(&knifeRunError_, SIGNAL(timeout()), SLOT(slt_KnifeRunError()));//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTODO? Maybe

    runTimeMs = stats->readStatistic(SlicerStatistics::SlicerRunningTime).toLongLong();
    m_totalWeight = stats->readStatistic(SlicerStatistics::LbsWeighed).toFloat();

    firstSlice = true;
    signalSent = true;
    m_startingWeight = 0;
    m_prevSetpoint = 0;
    isKnifeRunning = false;
    connect(
                dynamic_cast<QObject*>(&scaleInterface),
                SIGNAL(sig_weightUpdated(ScaleInterface::State,ScaleInterface::weightInPounds_t)),
                SLOT(slt_WeightChanged(ScaleInterface::State,ScaleInterface::weightInPounds_t)) );
    connect(
                dynamic_cast<QObject*>(&scaleInterface),
                SIGNAL(sig_OnWeigherKeyPressed()),
                SLOT(slt_WeigherKeyPressed()));
    connect(this,
            SIGNAL(sig_ReqConfig()),
            dynamic_cast<QObject*>(&scaleInterface),
            SLOT(slt_ReqConfig()));
    connect(dynamic_cast<QObject*>(&scaleInterface),
            SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)),
            this,
            SLOT(slt_WeigherConfigChanged(WeigherConfiguration&)));
    connect(dynamic_cast<QObject*>(&scaleInterface),
            SIGNAL(sig_WeigherStatus(char *, int)),
            this,
            SLOT(slt_WeigherStatus(char*,int)));
    connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)),
            dynamic_cast<QObject*>(&scaleInterface),
            SLOT(slt_WriteConfig(WeigherConfiguration&)));
    connect(this, SIGNAL(sig_SetWeigherMode(WeighingMode)),
            dynamic_cast<QObject*>(&scaleInterface),
            SLOT(slt_SetWeigherMode(WeighingMode)));
    connect(this, SIGNAL(sig_WeigherReset()),
            dynamic_cast<QObject*>(&scaleInterface),
            SLOT(slt_WeigherReset()));
    connect(this, SIGNAL(sig_isKnifeRunning(bool)), this, SLOT(slt_handleKnifeStop(bool)));


    //connect(m_scaleInterface, SIGNAL(sig_updateSetPoint(double)),this,SLOT(slt_updateSetPoint(double));
}


void Scale::slt_IncrementSetpoint()
{
    //qDebug() << "Increment Mode: " << m_infinityMode;
    ///enter infinity mode
    if( m_setpoint == m_maximumSupportedWeight){
        m_setpoint = FLT_MAX;
        m_infinityMode = infinityFromTopMode;
        emit sig_InfinityMode(m_infinityMode);
        emit sig_SetpointChanged(m_setpoint);
        return;
    }
    else if(m_setpoint == FLT_MAX){
        ///exit infinity mode
        if(m_infinityMode == infinityFromBottomMode){
            m_setpoint = 0;
            m_infinityMode = noInfinityMode;
            emit sig_InfinityMode(m_infinityMode);
            emit sig_SetpointChanged(m_setpoint);
            return;
        }else if(m_infinityMode == infinityFromSpeedKey){
            //qDebug() << "Increment out of speed key";
            m_setpoint = m_prevSetpoint;
            m_infinityMode = noInfinityMode;
            emit sig_InfinityMode(m_infinityMode);
            emit sig_SetpointChanged(m_setpoint);
            return;
        }
        ///stay in infinity mode
        else
            return;
    }

    ///regular setting
    ScaleInterface::weightInPounds_t weight_delta = m_weightStep;
    //qDebug() <<"setpoint "<< m_setpoint << "step "<< m_weightStep << "fmod " << fmod(m_setpoint, m_weightStep);
    float setMod = fmod(m_setpoint, m_weightStep);
    //qDebug() << "setMod: " << setMod;
    setMod = round(setMod * 1000);
    //qDebug() << "setMod: " << setMod;
    setMod /= 1000;
    //qDebug() << "setMod: " << setMod;

    if(setMod != 0.0 && setMod != m_weightStep){
        weight_delta = m_weightStep - setMod;
        //qDebug() << "Adjusting to multiple " << weight_delta;
    }
    if ((m_setpoint + weight_delta) < m_maximumSupportedWeight)
    {
        m_setpoint += weight_delta;
        m_infinityMode = noInfinityMode;
        emit sig_InfinityMode(m_infinityMode);
        emit sig_SetpointChanged(m_setpoint);
        return;
    }
    else if((m_setpoint + weight_delta) >= m_maximumSupportedWeight){
        m_setpoint = m_maximumSupportedWeight;
        m_infinityMode = noInfinityMode;
        emit sig_InfinityMode(m_infinityMode);
        emit sig_SetpointChanged(m_setpoint);
        return;
    }else{
        //qDebug() << "Being dumb and not working";
    }
}

void Scale::slt_DecrementSetpoint()
{
    ///enter infinity mode
    if( m_setpoint == 0){
        m_setpoint = FLT_MAX;
        m_infinityMode = infinityFromBottomMode;
        emit sig_InfinityMode(m_infinityMode);
        emit sig_SetpointChanged(m_setpoint);
        return;
    }
    else if(m_setpoint == FLT_MAX){
        ///exit infinity mode
        if(m_infinityMode == infinityFromTopMode){
            m_setpoint = m_maximumSupportedWeight;
            m_infinityMode = noInfinityMode;
            emit sig_InfinityMode(m_infinityMode);
            emit sig_SetpointChanged(m_setpoint);
            return;
        }
        else if(m_infinityMode == infinityFromSpeedKey){
                    //qDebug() << "Increment out of speed key";
                    m_setpoint = m_prevSetpoint;
                    m_infinityMode = noInfinityMode;
                    emit sig_InfinityMode(m_infinityMode);
                    emit sig_SetpointChanged(m_setpoint);
                    return;
        }
        ///stay in infinity mode
        else
            return;
    }

    if(m_infinityMode != infinityFromBottomMode){
        ///regular setting
        ScaleInterface::weightInPounds_t weight_delta = m_weightStep;
        ////qDebug() <<"setpoint "<< m_setpoint << "step "<< m_weightStep << "fmod " << fmod(m_setpoint, m_weightStep);
        if(round(fmod(m_setpoint, m_weightStep) *1000) /1000 != 0.0){
            weight_delta = round(fmod(m_setpoint, m_weightStep) * 1000) / 1000;
            qDebug() << "Adjusting to multiple " << weight_delta;
        }
        if ((m_setpoint - weight_delta) > 0)
        {
            m_setpoint -= weight_delta;
            m_infinityMode = noInfinityMode;
            emit sig_InfinityMode(m_infinityMode);
            emit sig_SetpointChanged(m_setpoint);
            return;
        }
        else if((m_setpoint - weight_delta) <= 0){
            ////qDebug() << "setpoint 0";
            m_setpoint = 0;
            m_infinityMode = noInfinityMode;
            emit sig_InfinityMode(m_infinityMode);
            emit sig_SetpointChanged(m_setpoint);
            return;
        }else{
            //qDebug() << "Being dumb and not working";
        }
    }
}

void Scale::slt_Zero()
{
    m_scaleInterface.ZeroScale();
    emit sig_Zeroing();
}

void Scale::slt_SetSetpoint(ScaleInterface::weightInPounds_t weight)
{
    qDebug() << "sltSetSetpoint";
    if(weight != FLT_MAX){
        if(weight > m_maximumSupportedWeight)
            m_setpoint = m_maximumSupportedWeight;
        else if(weight <= 0)
            m_setpoint = 0;
        else
            m_setpoint = weight;
        m_setpointCount = 0;
        emit sig_InfinityMode(noInfinityMode);
        emit sig_SetpointChanged(m_setpoint);
    }

}

void Scale::slt_updateWeightTolerance(double value){
    //qDebug() << "New Tolerance " << value;
    m_weightTolerance = value;
}

void Scale::slt_updateSetPoint(double value) {
    //qDebug() << "updateSetpoint value:" << value;
    //qDebug() << "updateSetpoint mode" << m_infinityMode;
    if(value != FLT_MAX){
        if(value > m_maximumSupportedWeight)
            m_setpoint = m_maximumSupportedWeight;
        else if(value < 0)
            m_setpoint = 0;
        else
            m_setpoint = value;
        m_infinityMode = noInfinityMode;
        emit sig_InfinityMode(noInfinityMode);
        emit sig_SetpointChanged(m_setpoint);
    }else{
        if(m_infinityMode == noInfinityMode){

            m_prevSetpoint = m_setpoint;
            m_setpoint = FLT_MAX;
            m_infinityMode = infinityFromSpeedKey;
            emit sig_InfinityMode(infinityFromSpeedKey);
            emit sig_SetpointChanged(m_setpoint);
        }
    }
}

//not stacking, so flash whenever more weight is added
void Scale::handleSingleSlice(ScaleInterface::weightInPounds_t weight){
    if(weight + m_weightTolerance >= m_setpoint && !signalSent){
        signalSent = true;
        emit sig_AtOrAboveSetpoint();
        stopSlicing();
        firstSlice = false;
    }
}

void Scale::handleSetpoint(ScaleInterface::weightInPounds_t weight){
    //qDebug() << "Scale::handleSetpoint()";
    //qDebug() << "Weight: " << weight << " m_weightTolerance: " << m_weightTolerance << " m_setpoint: " << m_setpoint << " m_startingWeight: " << m_startingWeight;
    if(isKnifeRunning){
        if(weight + m_weightTolerance >= m_setpoint + m_startingWeight && !signalSent){
            //qDebug() << "Scale::handleSetpoint() Time to stop slicing";
            if(mode_ < 3){
                m_totalWeight += (m_weight - m_startingWeight);
            }else{
                m_totalWeight += (m_weight - m_startingWeight) * 2.20462;
            }
            signalSent = true;
            emit sig_AtOrAboveSetpoint();
            stopSlicing();
            m_startingWeight = weight;
            firstSlice = false;
        }
    }
    else{
        m_startingWeight = weight; //automatically tare
    }
}

//no knowledge of knife running -- no automatic tare
void Scale::handleSetpointManual(ScaleInterface::weightInPounds_t weight){
    //qDebug() << "handleSetpointManual()";
    if(weight + m_weightTolerance >= m_setpoint + m_startingWeight && !signalSent){
        //qDebug() << "handleSetpointManual(): stoping slicer";
        //saved in lbs so if kg mode convert to lbs
        if(mode_ < 3){
            m_totalWeight += (m_weight - m_startingWeight);
        }else{
            m_totalWeight += (m_weight - m_startingWeight) * 2.20462;
        }
        signalSent = true;
        //qDebug() << "handleSetpointManual(): emit sig_AtOrAboveSetpoint";
        emit sig_AtOrAboveSetpoint();
        //qDebug() << "handleSetpointManual(): callling stop Slicing";
        stopSlicing();
        m_startingWeight = weight;
        firstSlice = false;
        //qDebug() << "handleSetpointManual(): done";
    }
}

void Scale::slt_WeightChanged(ScaleInterface::State state,ScaleInterface::weightInPounds_t weight)
{
    //qDebug() << "slt_WeightChanged(): Weight Changed";
    emit sig_WeightChanged(state,weight);
    m_state = state;
    if(ScaleInterface::stable == state){
        //SOF-3129 Only restart timer if significant weight change
        if (isKnifeRunning && fabs(weight - m_weight) >= .005){
            knifeRunError_.start();//restart the timer
        }
        m_weight = weight;
        ///if weight is removed, reset m_startingWeight
        if(weight + .3 <  m_startingWeight){
            firstSlice = true;
            m_startingWeight = weight;
            Q_EMIT sig_BelowSetpoint();
        }
        if(this->m_incrementMode == SlicerStateMachine::IncrementBySingleSlice){
            handleSingleSlice(weight);
        }
        else if(this->m_incrementMode == SlicerStateMachine::IncrementBySetpoint){
            handleSetpoint(weight);
        }
        else if(this->m_incrementMode == SlicerStateMachine::IncrementBySetpointManual){
            handleSetpointManual(weight);
        }
    }
    if ((ScaleInterface::zero == state)){
        if (isKnifeRunning && !(this->m_incrementMode == SlicerStateMachine::IncrementBySetpointManual)){
            knifeRunError_.start();//restart the timer
        }
        m_startingWeight = 0;
        m_weightBeforeExtra = 0;
        firstSlice = true;
        m_weight = 0.0;
        Q_EMIT sig_BelowSetpoint();
    }
}

void Scale::stopSlicing(){
    if(isKnifeRunning){
        int delay = 0;
        if(this->m_incrementMode == SlicerStateMachine::IncrementBySingleSlice){
            if(firstSlice) //stopping via initial loop
                delay = 0;//MILISECONDS_DELAY_SINGLE_SLICE_CONTINUED;
            else
                delay = MILISECONDS_DELAY_SINGLE_SLICE_STOPPED;
        }
        QTimer::singleShot(delay,this,SLOT(onPause()));
    }
}

/* DO NOT CALL THIS FUNCTION DIRECTLY AS IT COULD START THE SLICER
CALL stopSlicing() INSTEAD. THIS IS HERE FOR FUTURE EXPANSION WHEN
WE MOVE THE PHYSICAL ONPAUSE BUTTON TO THE UI
*/
void Scale::onPause(){
    system("echo 1 >> /usr/local/gpio/on_pause/value");
    usleep(PULSE_WIDTH * 1000);
    system("echo 0 >> /usr/local/gpio/on_pause/value");
}

bool Scale::KnifeRunning(){
    std::ifstream knifeRunning;
    std::string line;
    //read in knife running file
    knifeRunning.open("/usr/local/gpio/knife_run/value", std::ios::in);
    if(knifeRunning.is_open()){
        std::getline(knifeRunning, line);
        knifeRunning.close();
        //if knife is running
        return strcmp(line.data(), "1") == 0;
    }
    return NULL;
}

void Scale::slt_knifeRunPolling(){
    isKnifeRunning = KnifeRunning();
    //std::cout << "SS  :: " << isSingleSliceMode << std::endl;
    //std::cout << "FS  :: " << (!firstSlice) << std::endl;
    //std::cout << "SIG :: " << (!signalSent) << std::endl;
    if(!isKnifeRunning){
        signalSent = false;
    }
    else if (m_incrementMode == SlicerStateMachine::IncrementBySingleSlice && !(firstSlice) && !(signalSent)){
        signalSent = true;
        stopSlicing();
    }

    if (isKnifeRunning != wasKnifeRunning){
        //Manual Mode Does not have an error timeout
        if(isKnifeRunning){ //&& !(this->m_incrementMode == SlicerStateMachine::IncrementBySetpointManual)
            //qDebug() << "timer started";
            knifeRunError_.start();
            runningTimer.start();
        }else{
            //qDebug() << "timer stopped";
            //qDebug() << "Slicer Total Weight" << m_totalWeight;
            stats->writeStatistic(SlicerStatistics::LbsWeighed, m_totalWeight);
            if (runningTimer.isValid()){
                runTimeMs += runningTimer.elapsed();
                runningTimer.invalidate();
                stats->writeStatistic(SlicerStatistics::SlicerRunningTime, runTimeMs);
                //qDebug() << "Slicer Running Time" << runTimeMs;
                //If the timer was about to expire and the slicer stopped assume it was the slicers timeout
                if (knifeRunError_.isActive() && knifeRunError_.remainingTime() <= 3000){
                    slt_KnifeRunError();
                }
            }
            knifeRunError_.stop();
        }
        wasKnifeRunning = isKnifeRunning;
        Q_EMIT sig_isKnifeRunning(isKnifeRunning);
    }

}

void Scale::slt_handleKnifeStop(bool)
{

}

void Scale::slt_WeigherConfigChanged(WeigherConfiguration & config)
{
    //qDebug() << "Scale::slt_WeigherConfigChanged()";
    Q_EMIT sig_WeigherConfigChanged(config);
}

void Scale::slt_WeigherKeyPressed(){
    emit sig_WeigherKeyPressed();
}

void Scale::slt_ReqConfig(){
    emit sig_ReqConfig();
}

void Scale::slt_WriteConfig(WeigherConfiguration &config)
{
    Q_EMIT sig_WriteConfig(config);
}

void Scale::slt_WeigherStatus(char *buffer, int buffLen)
{
    Q_EMIT sig_WeigherStatus(buffer, buffLen);
}

void Scale::slt_SetWeigherMode(WeighingMode mode)
{
    Q_EMIT sig_SetWeigherMode(mode);
}

void Scale::slt_WeigherReset()
{
    Q_EMIT sig_WeigherReset();
}

void Scale::slt_IncrementModeChanged(int incMode){
    this->m_incrementMode = incMode;
}

void Scale::slt_KnifeRunError(){
    QSettings settings;
    bool usr30secTimeoutEnabled = settings.value("usr30secTimeoutEnabled", true).toBool();

    qDebug() << "User timeout on? " << usr30secTimeoutEnabled;

    if( usr30secTimeoutEnabled ){
        Q_EMIT sig_KnifeRunError(m_state);
    }else{
        Q_EMIT sig_TimeoutPopup();
    }
}

void Scale::slt_IncrementStepChanged(ScaleInterface::weightInPounds_t step){
    //qDebug() << "New Increment Step " << step;
    m_weightStep = step;
}

void Scale::slt_ModeChanged(int mode){
    //qDebug() << "slt_ModeChanged("<< mode <<")";
    mode_ = mode;
}
