#ifndef KEYSIGNALHANDLER_H
#define KEYSIGNALHANDLER_H

#include <QObject>
#include <QMap>
#include <QString>

class KeySignalHandler : public QObject
{
    Q_OBJECT
public:
    explicit KeySignalHandler(QObject *parent = 0);
public Q_SLOTS:
    void slt_HandleKeyPress(QString qs);
    void slt_CapsLockStatus(bool toggle);
private:
    bool m_CapsLockOn = false;
    //QMaps QString ("A") to Qt::Key keycode (Qt::Key_A)
    const QMap<QString, int> m_KeyToKeyCode =
    {
        {"A", Qt::Key_A},
        {"B", Qt::Key_B},
        {"C", Qt::Key_C},
        {"D", Qt::Key_D},
        {"E", Qt::Key_E},
        {"F", Qt::Key_F},
        {"G", Qt::Key_G},
        {"H", Qt::Key_H},
        {"I", Qt::Key_I},
        {"J", Qt::Key_J},
        {"K", Qt::Key_K},
        {"L", Qt::Key_L},
        {"M", Qt::Key_M},
        {"N", Qt::Key_N},
        {"O", Qt::Key_O},
        {"P", Qt::Key_P},
        {"Q", Qt::Key_Q},
        {"R", Qt::Key_R},
        {"S", Qt::Key_S},
        {"T", Qt::Key_T},
        {"U", Qt::Key_U},
        {"V", Qt::Key_V},
        {"W", Qt::Key_W},
        {"X", Qt::Key_X},
        {"Y", Qt::Key_Y},
        {"Z", Qt::Key_Z},
        {"0", Qt::Key_0},
        {"1", Qt::Key_1},
        {"2", Qt::Key_2},
        {"3", Qt::Key_3},
        {"4", Qt::Key_4},
        {"5", Qt::Key_5},
        {"6", Qt::Key_6},
        {"7", Qt::Key_7},
        {"8", Qt::Key_8},
        {"9", Qt::Key_9},
        {"@", Qt::Key_At},
        {"#", Qt::Key_NumberSign},
        {"$", Qt::Key_Dollar},
        {"_", Qt::Key_Underscore},
        {"&&", Qt::Key_Ampersand},
        {"-", Qt::Key_Minus},
        {"+", Qt::Key_Plus},
        {".", Qt::Key_Period},
        {"(", Qt::Key_ParenLeft},
        {")", Qt::Key_ParenRight},
        {"/", Qt::Key_Slash},
        {"*", Qt::Key_Asterisk},
        {"\"", Qt::Key_QuoteDbl},
        {"'", Qt::Key_Apostrophe},
        {":", Qt::Key_Colon},
        {";", Qt::Key_Semicolon},
        {"!", Qt::Key_Exclam},
        {"?", Qt::Key_Question},
        {"BACKSPACE", Qt::Key_Backspace},
        {" ", Qt::Key_Space},
        {"CAPS", Qt::Key_CapsLock},
        {"ENTER", Qt::Key_Enter},
        {"À", 0x0C0},
        {"Â", 0x0C2},
        {"Æ", 0x0C6},
        {"Ç", 0x0C7},
        {"É", 0x0C9},
        {"È", 0x0C8},
        {"Ê", 0x0CA},
        {"Ë", 0x0CB},
        {"Œ", 0x08C}, //I DON'T KNOW WHAT QT HAS AGAINST THIS LETTER
        {"Î", 0x0CE},
        {"Ï", 0x0CF},
        {"Ô", 0x0D4},
        {"Ù", 0x0D9},
        {"Û", 0x0DB},
        {"Ü", 0x0DC},
        {"Ÿ", 0x0FF}
    };
};

#endif // KEYSIGNALHANDLER_H
