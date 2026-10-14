#ifndef POPUPMESSAGE_H
#define POPUPMESSAGE_H

#include <QDialog>
#include <QDialogButtonBox>

namespace Ui {
class PopupMessage;
}

class PopupMessage : public QDialog
{
    Q_OBJECT

public:
    explicit PopupMessage(QWidget *parent = 0);
    ~PopupMessage();
    void setHeader(QString str);
    void setMessage(QString str);

    void setHeaderFont(QFont font);
    void setMessageFont(QFont font);

private:
    Ui::PopupMessage *ui;
};

#endif // POPUPMESSAGE_H
