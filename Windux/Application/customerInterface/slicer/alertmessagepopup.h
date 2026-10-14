#ifndef ALERTMESSAGEPOPUP_H
#define ALERTMESSAGEPOPUP_H

#include <QDialog>

namespace Ui {
class AlertMessagePopup;
}

class AlertMessagePopup : public QDialog
{
    Q_OBJECT

public:
    explicit AlertMessagePopup(QWidget *parent = 0);
    ~AlertMessagePopup();
    void setHeader(QString header);
    void setMessage(QString message);
    void setUniqueId(const int id);
    void setLastCompleted(QString lastCompleted);

Q_SIGNALS:
    void sig_messageAccepted(int uid); //Emit signal to msgbox
    void sig_messageRejected(int uid); //Emit signal to ...nowhere?

private slots:
    void slt_messageAccepted();
    void slt_messageRejected();

private:
    Ui::AlertMessagePopup *ui;
    int uniqueId;
};

#endif // ALERTMESSAGEPOPUP_H
