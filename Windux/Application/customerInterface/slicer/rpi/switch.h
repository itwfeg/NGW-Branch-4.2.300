#ifndef SWITCH_H
#define SWITCH_H

#include <QObject>
#include "inputgpio.h"

class Switch : public QObject
{
    Q_OBJECT

    friend void SwitchIsr();

public:
    enum state_e
    {
        On,
        Off
    };

    explicit Switch(RaspberryPi::pin_t pin);
    state_e State () const;

signals:
    void sig_On();
    void sig_Off();

private:
    InputGpio m_inputGpio;
};

#endif // SWITCH_H
