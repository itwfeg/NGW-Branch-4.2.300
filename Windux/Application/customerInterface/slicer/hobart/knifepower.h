#ifndef KNIFEPOWER_H
#define KNIFEPOWER_H

#include <QObject>

class KNIFE : public QObject
{
    Q_OBJECT

public:

    explicit KNIFE(const QString& pin);
    void SetPower(bool on);

public slots:
    void slt_On();
    void slt_Off();

private:
    QString m_pin;

    bool m_powerOn;

    void Refresh();
    void Off();

    static bool digitalRead( const QString& pin );
    static void digitalWrite( const QString& pin, bool value );

};

#endif // KNIFEPOWER_H
