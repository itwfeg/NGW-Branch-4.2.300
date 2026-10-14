#include "wrapperpacketdialog.h"

#include <QApplication>

#include <WinSock2.h>

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    memset( &wsaData, 0, sizeof(wsaData) );

    WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

    QApplication a(argc, argv);
    wrapperPacketDialog w;
    w.show();

    int result( a.exec() );

    WSACleanup();

    return result;
}
