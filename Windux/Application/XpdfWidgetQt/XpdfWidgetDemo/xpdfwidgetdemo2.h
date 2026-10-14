#ifndef XPDFWIDGETDEMO2_H
#define XPDFWIDGETDEMO2_H

#include <QWidget>

namespace Ui {
class XpdfWidgetDemo2;
}

class XpdfWidgetDemo2 : public QWidget
{
    Q_OBJECT
    
public:
    explicit XpdfWidgetDemo2(QWidget *parent = 0);
    ~XpdfWidgetDemo2();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::XpdfWidgetDemo2 *ui;
};

#endif // XPDFWIDGETDEMO2_H
