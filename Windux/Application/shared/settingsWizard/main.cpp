/*
PROLOGUE: QmSettings (aka Setup Wizard)
SRD: 117
Date: Q4 2012
AUTHOR: Todd Davis
COMPANY: Hobart

PURPOSE: This application serves the REQ for a System settings software wizard
         for the QM Series scales to configure the system hardware's settings for:

         - Time & Date
         - Sound Volume
         - Network Configuration
         - Touchscreen Calibration
         - Administration: Users
         - Timestamp of Last Settings
*/

#include <QtGui/QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
