#ifndef ROTARYENCODERKNOB_H
#define ROTARYENCODERKNOB_H

#include <QObject>
#include <QTimer>
#include "raspberrypitypes.h"

class RotaryEncoderKnob : public QObject
{
    Q_OBJECT

    friend void RotaryEncoderKnobIsr();

public:
    RotaryEncoderKnob(RaspberryPi::pin_t a, RaspberryPi::pin_t b);

signals:
    void sig_Increment();
    void sig_Decrement();
    void sig_Isr();

private slots:
    void slt_Isr();
    void slt_NoKnobActuation();

private:
    int m_lastEncoded;
    RaspberryPi::pin_t m_a;
    RaspberryPi::pin_t m_b;

    //the following members represent a rudimentary "profile," where
    //the sensitivity of the knob at the start of actuation is
    //lower than the rest
    QTimer m_actuationTrackTimer;
    int m_startMomentumThreshold;
    int m_momentum;
};

#endif // ROTARYENCODERKNOB_H
