#ifndef INPUT_GPIO_H
#define INPUT_GPIO_H

#include <QObject>
#include <QTimer>
#include "raspberrypitypes.h"

class InputGpio : public QObject
{
    Q_OBJECT

public:
    typedef void (*pfIsrHandler)();

    explicit InputGpio(RaspberryPi::pin_t pin, RaspberryPi::gpioPullUpType_e pud);
    RaspberryPi::gpioValue_e Value() const { return m_value; }
    void EnableIsr(RaspberryPi::gpioEdgeType_e edgeType, pfIsrHandler isrHandler);
    void Isr();

signals:
    void sig_High();
    void sig_Low();
    void sig_Isr();

private slots:
    void slt_DebounceTimerExpired();
    void slt_Isr();

private:
    int m_debounceTimeMs;
    RaspberryPi::pin_t m_pin;
    RaspberryPi::gpioValue_e m_value;
    QTimer m_debounceTimer;
};

#endif // INPUT_GPIO_H
