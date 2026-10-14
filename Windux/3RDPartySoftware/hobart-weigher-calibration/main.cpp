#include "calibrationdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    calibrationDialog w;
    w.show();

    return a.exec();
}
