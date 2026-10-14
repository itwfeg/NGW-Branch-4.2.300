#ifndef STANDARDPOPUP_H
#define STANDARDPOPUP_H

#include <QDialog>
#include <QDialogButtonBox>


namespace Ui {
class standardPopup;
}

class standardPopup : public QDialog
{
    Q_OBJECT

public slots:
    void slt_WeigherKeyPressed();
public:
    explicit standardPopup(QWidget *parent = 0);
    void setMessage(QString message);
    ~standardPopup();

    void setStandardButtons(QDialogButtonBox::StandardButtons buttons);
    void setHeader(QString header);
private:
    Ui::standardPopup *ui;
};

#endif // STANDARDPOPUP_H
