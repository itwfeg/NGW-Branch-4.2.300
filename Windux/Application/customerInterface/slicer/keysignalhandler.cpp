#include "keysignalhandler.h"

#include <QString>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>

KeySignalHandler::KeySignalHandler(QObject *parent) : QObject(parent)
{

}

void KeySignalHandler::slt_HandleKeyPress(QString qs){
    //key codes stored in uppercase
    QString changeQs = qs.toUpper();
    bool special_key = false;
    if(qs.length() != 1){
        special_key = true;
    }
    int keyCode = m_KeyToKeyCode[changeQs];

    QKeyEvent* keyPress;
    if(special_key){
        //special case: Qt::Key(Key_Ampersand) requires two && in the button to display
        if(keyCode == Qt::Key_Ampersand){
            keyPress = new QKeyEvent(QEvent::KeyPress, keyCode, Qt::NoModifier, "&");
        }
        //other keys: backspace/caps/enter
        else{
            keyPress = new QKeyEvent(QEvent::KeyPress, keyCode, Qt::NoModifier);
        }
    }
    else{
        //regular character key
        QString characterOut = QKeySequence(keyCode).toString();
        //special case: Qt::Key(Key_Space).toString == "Space"
        if(keyCode == Qt::Key_Space){
            characterOut = " ";
        }

        if(keyCode == 0x08C){ //Œ character that Qt doesn't like for some reason
            characterOut = QStringLiteral("Œ");
        }

        if(m_CapsLockOn){
            characterOut = characterOut.toUpper();
        } else {
            characterOut = characterOut.toLower();
        }

        keyPress = new QKeyEvent(QEvent::KeyPress, keyCode, Qt::NoModifier, characterOut);
    }
    QApplication::postEvent(QApplication::focusObject(), keyPress);
    QKeyEvent* keyRelease = new QKeyEvent(QEvent::KeyRelease, keyCode, Qt::NoModifier);
    QApplication::postEvent(QApplication::focusObject(), keyRelease);
}

void KeySignalHandler::slt_CapsLockStatus(bool toggle){
    m_CapsLockOn = toggle;
}
