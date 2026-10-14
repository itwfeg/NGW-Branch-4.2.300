#ifndef LED_H
#define LED_H

#include <QObject>
#include <QTimer>

class LED : public QObject
{
    Q_OBJECT

public:

    explicit LED(const QString& red, const QString& blue, const QString& green);
    void SetColor(bool redOn, bool blueOn, bool greenOn);

public slots:
    void slt_Off();
    void slt_Red();
    void slt_FlashRed();
    void slt_Blue();
    void slt_Green();
    void slt_White();

private slots:
    void slt_FlashToggle();

private:
    QString m_redPin;
    QString m_bluePin;
    QString m_greenPin;

    QTimer m_flashTimer;
    bool m_redOn;
    bool m_blueOn;
    bool m_greenOn;

    void Refresh();
    void Off();

    static bool digitalRead( const QString& pin );
    static void digitalWrite( const QString& pin, bool value );

};

#endif // LED_H
