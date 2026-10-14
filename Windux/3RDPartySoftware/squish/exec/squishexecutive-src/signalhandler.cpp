#include "signalhandler.h"

#include "squishexecutive.h"

#include <QCoreApplication>

SignalHandler *SignalHandler::m_signalHandler = 0;

SignalHandler::SignalHandler(SquishExecutive *squishExecutive)
    : m_squishExecutive(squishExecutive)
{
}

void SignalHandler::setSignalHandlerInstance(SignalHandler *signalHandler)
{
    m_signalHandler = signalHandler;
}

void SignalHandler::invokeSignalHandler(int sig)
{
    if (m_signalHandler)
        m_signalHandler->handleSignal(sig);
}

void SignalHandler::handleSignal(int sig)
{
    Q_UNUSED(sig);

    m_squishExecutive->stopAll();
    qApp->exit(0);
}
