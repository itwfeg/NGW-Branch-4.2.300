#ifndef ON_PAUSE_BUTTON_SIM_H
#define ON_PAUSE_BUTTON_SIM_H

#include <QObject>
#include <QTimer>

class OnPauseButtonSim: public QObject
{
    Q_OBJECT

public:
    explicit OnPauseButtonSim();

public slots:
    void slt_SimulatePress();
    void slt_SimulateUnpress();

private:
    QTimer m_timer;

};

#endif // ON_PAUSE_BUTTON_SIM_H
