#ifndef SLICERSTATISTICS_H
#define SLICERSTATISTICS_H

#include <QObject>

class SlicerStatistics : public QObject
{
public:
    explicit SlicerStatistics(QObject * parent);


    enum SlicerStatistic{
        SlicerRunningTime,
        LbsWeighed,
        LastStat
    };
    void writeStatistic(int stat, QVariant value);
    QString readStatistic(int stat);

private:

    void selectStatisticsFile();
    void selectSettingsFile();

public slots:

};

#endif // SLICERSTATISTICS_H
