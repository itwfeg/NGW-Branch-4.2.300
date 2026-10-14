#ifndef CALIBRATETYPEPOPUP_H
#define CALIBRATETYPEPOPUP_H

#include <QDialog>

namespace Ui {
class calibrateTypePopup;
}

class calibrateTypePopup : public QDialog
{
    Q_OBJECT

public:
    explicit calibrateTypePopup(QWidget *parent = 0);
    ~calibrateTypePopup();

private:
    Ui::calibrateTypePopup *ui;
};

#endif // CALIBRATETYPEPOPUP_H
