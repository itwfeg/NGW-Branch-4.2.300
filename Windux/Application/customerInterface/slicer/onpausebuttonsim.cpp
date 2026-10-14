#include "onpausebuttonsim.h"
#include <QSettings>

OnPauseButtonSim::OnPauseButtonSim()
{
    QSettings settings;
    m_timer.setInterval(settings.value("OnPausePressMs", 250).toInt());
    m_timer.setSingleShot(true);
    (void)connect(&m_timer, SIGNAL(timeout()), this, SLOT(slt_SimulateUnpress()));
}

void OnPauseButtonSim::slt_SimulatePress()
{
    m_timer.start();
}

void OnPauseButtonSim::slt_SimulateUnpress()
{
}
