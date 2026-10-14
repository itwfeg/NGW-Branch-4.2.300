#ifndef KEYPAD_H
#define KEYPAD_H

#include <QFrame>
#include "keysignalhandler.h"

namespace Ui {
class Keypad;
}

class Keypad : public QFrame
{
    Q_OBJECT

public:
    explicit Keypad(QWidget *parent = 0);
    ~Keypad();

Q_SIGNALS:
    void sig_textEnterOrTab(QString s); //Emit signal to keysignalhandler

public Q_SLOTS:
    void invoke(QWidget* widget);
    void dismiss();

private:
    Ui::Keypad *ui;
    KeySignalHandler* ksh;
    void m_ShowNumbers(bool show);
    void m_ShowLetters(bool show);
    void m_ShowAltChars(bool show);

private slots:
    void slt_SetCapsLockBtns(bool capsOn);
    void slt_SetToggleNumbers(bool numbersOn);
    void slt_ToggleAtlChars(bool altOn);
    void slt_hideOnEnter();

protected:
    //virtual bool event (QEvent *e);
    QWidget * w_;
    QWidget *old_window;
    virtual void hideEvent(QHideEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual bool event(QEvent *e);

};

#endif // KEYPAD_H
