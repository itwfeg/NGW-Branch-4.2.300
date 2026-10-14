#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

class SquishExecutive;

class SignalHandler
{
public:
    SignalHandler(SquishExecutive *squishExecutive);

    static void setSignalHandlerInstance(SignalHandler *signalHandler);
    static void invokeSignalHandler(int sig);

private:
    void handleSignal(int sig);

    static SignalHandler *m_signalHandler;

    SquishExecutive *m_squishExecutive;
};

#endif // SIGNALHANDLER_H
