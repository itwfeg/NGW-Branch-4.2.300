#include <QApplication>
#include "xpdfwidgetdemo2.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XpdfWidgetDemo2 w;
    w.show();
    
    return a.exec();
}
