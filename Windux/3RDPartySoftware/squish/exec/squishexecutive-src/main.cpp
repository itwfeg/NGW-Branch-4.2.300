#include "seconfiguration.h"
#include "signalhandler.h"
#include "squishexecutive.h"

#include <QCoreApplication>
#include <QTcpServer>

#include <signal.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SEConfiguration config;
    if (!config.verify())
        return -1;

    SquishExecutive executive(&config);

    SignalHandler signalHandler(&executive);
    SignalHandler::setSignalHandlerInstance(&signalHandler);
    signal(SIGINT, SignalHandler::invokeSignalHandler);
    signal(SIGTERM, SignalHandler::invokeSignalHandler);

    QTcpServer server;
    if (!server.listen(QHostAddress::LocalHost, config.serverPort())) {
        qWarning("Could not start server: %s", qPrintable(server.errorString()));
        return -1;
    }

    QObject::connect(&server, SIGNAL(newConnection()), &executive, SLOT(acceptConnection()));

    return a.exec();
}
