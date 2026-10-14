#ifndef BUTTON_H
#define BUTTON_H

#include <QObject>
#include <QTimer>
#include "inputgpio.h"

class Button : public QObject
{
    Q_OBJECT

    friend void ButtonIsr();

public:
    enum state_e
    {
        Pressed,
        Unpressed
    };

    explicit Button(RaspberryPi::pin_t pin);
    state_e State () const;

signals:
    void sig_Pressed();
    void sig_Unpressed();
    void sig_ShortClick();
    void sig_LongClick();

private slots:
    void slt_Release();

private:
    InputGpio m_inputGpio;
    QTimer m_clickTimer;
};

#endif // BUTTON_H
