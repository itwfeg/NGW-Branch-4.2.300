#ifndef LED_H
#define LED_H

#include <QObject>
#include <QTimer>
#include "raspberrypitypes.h"

class LED : public QObject
{
    Q_OBJECT

public:

    explicit LED(RaspberryPi::pin_t redPin, RaspberryPi::pin_t bluePin, RaspberryPi::pin_t greenPin);
    void SetColor(bool redOn, bool blueOn, bool greenOn);

public slots:
    void slt_Off();
    void slt_Red();
    void slt_FlashRed();
    void slt_Blue();
    void slt_Green();
    void slt_White();

private slots:
    void slt_FlashToggle();

private:
    RaspberryPi::pin_t m_redPin;
    RaspberryPi::pin_t m_bluePin;
    RaspberryPi::pin_t m_greenPin;

    QTimer m_flashTimer;
    bool m_redOn;
    bool m_blueOn;
    bool m_greenOn;

    void Refresh();
    void Off();
};

#endif // LED_H
