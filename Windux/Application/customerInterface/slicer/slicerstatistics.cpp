#include "slicerstatistics.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

SlicerStatistics::SlicerStatistics(QObject *parent):
    QObject(parent)
{

}

void SlicerStatistics::writeStatistic(int stat, QVariant value){
    selectStatisticsFile();
    QSettings settings;
    settings.setValue(QString(stat), value);
    selectSettingsFile();
}

QString SlicerStatistics::readStatistic(int stat){
    selectStatisticsFile();
    QSettings settings;
    QString str = settings.value(QString(stat),0).toString();
    selectSettingsFile();
    return str;;
}

void SlicerStatistics::selectSettingsFile(){
    //changing the applicartion name changes the filename of the Qsettings File
    QCoreApplication::setApplicationName("Slicer");
}

void SlicerStatistics::selectStatisticsFile(){
    QCoreApplication::setApplicationName("SlicerStats");
}
