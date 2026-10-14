#include "rotaryencoderknob.h"
#include <QSettings>

void RotaryEncoderKnobIsr();

//the wiringPi API is limited, in that it does not support
//passing an argument to an "ISR"
//as a workaround, make this class essentially a singleton
static RotaryEncoderKnob* s_pKnob = NULL;

RotaryEncoderKnob::RotaryEncoderKnob(RaspberryPi::pin_t a, RaspberryPi::pin_t b) :
    m_lastEncoded(0),
    m_a(a),
    m_b(b),
    m_momentum(0)
{
    s_pKnob = this;

    QSettings settings;
    m_startMomentumThreshold = settings.value("KnobRotationThreshold", 3).toInt();

    m_actuationTrackTimer.setInterval(settings.value("KnobActuationTrackMs", 1000).toInt());
    m_actuationTrackTimer.setSingleShot(true);
    (void)connect(&m_actuationTrackTimer, SIGNAL(timeout()), this, SLOT(slt_NoKnobActuation()));

    //queue handling of interrupts
    connect(this, SIGNAL(sig_Isr()), this, SLOT(slt_Isr()), Qt::QueuedConnection);

    pinMode(m_a, INPUT);
    pinMode(m_b, INPUT);
    pullUpDnControl(m_a, PUD_UP);
    pullUpDnControl(m_b, PUD_UP);
    wiringPiISR(m_a, INT_EDGE_BOTH, RotaryEncoderKnobIsr);
    wiringPiISR(m_b, INT_EDGE_BOTH, RotaryEncoderKnobIsr);
}

void RotaryEncoderKnob::slt_Isr()
{
    int MSB = digitalRead (m_a);
    int LSB = digitalRead (m_b);

    int encoded = (MSB << 1) | LSB;
    int sum = (m_lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    {
        if (qAbs(m_momentum) >= m_startMomentumThreshold)
        {
            emit sig_Increment();
        }
        else
        {
            m_momentum = qMax(m_momentum, 0);
            ++m_momentum;
        }
    }
    else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
    {
        if (qAbs(m_momentum) >= m_startMomentumThreshold)
        {
            emit sig_Decrement();
        }
        else
        {
            m_momentum = qMin(m_momentum, 0);
            --m_momentum;
        }
    }

    m_actuationTrackTimer.start();

    m_lastEncoded = encoded;
}

void RotaryEncoderKnob::slt_NoKnobActuation()
{
    m_momentum = 0;
}

void RotaryEncoderKnobIsr()
{
    emit s_pKnob->sig_Isr();
}
