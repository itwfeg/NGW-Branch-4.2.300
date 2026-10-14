#ifndef SLICERSTATISTICSVIEWER_H
#define SLICERSTATISTICSVIEWER_H

#include <QDialog>
#include "slicerstatistics.h"

namespace Ui {
class SlicerStatisticsViewer;
}

class SlicerStatisticsViewer : public QDialog
{
    Q_OBJECT

public:
    explicit SlicerStatisticsViewer(QWidget *parent = 0);
    ~SlicerStatisticsViewer();

    void addGeneralItem(QString name, QString value);
    void addSpeedKeyItem(QString name, QString value);
    void addMaintenanceItem(QString name, QString value);
    void initStats();
private:
    SlicerStatistics * currentStatistics;
    Ui::SlicerStatisticsViewer *ui;
};

#endif // SLICERSTATISTICSVIEWER_H
