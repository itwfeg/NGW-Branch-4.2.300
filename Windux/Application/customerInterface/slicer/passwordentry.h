#ifndef PASSWORDENTRY_H
#define PASSWORDENTRY_H

#include <QDialog>
#include "mainmenu.h"

namespace Ui {
class passwordEntry;
}

class passwordEntry : public QDialog
{
    Q_OBJECT

public:
    typedef enum {supervisor, service} passwordType;
    passwordEntry(QWidget *parent = 0, passwordType entryType=supervisor);
    ~passwordEntry();
    static QString configFileName();
    static bool passwordConfigured(passwordType entryType);
    bool validPassword();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void onLogin();

private:
    Ui::passwordEntry *ui;
    passwordType loginType;
    bool passwordValid;
    void initialize();
};

#endif // PASSWORDENTRY_H
