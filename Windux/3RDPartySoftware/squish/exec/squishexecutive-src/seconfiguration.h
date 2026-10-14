#ifndef SECONFIGURATION_H
#define SECONFIGURATION_H

#include <QScopedPointer>
#include <QSettings>
#include <QString>

class SEConfiguration
{
public:
    SEConfiguration();

    bool verify() const;

    QString objectMapFileName() const;
    int maximumEventCount() const;

    QString squishPath() const;
    QString applicationName() const;

    quint16 serverPort() const;

private:
    enum ConfigurationKey {
        ObjectMapFileName,
        MaximumEventCount,
        SquishPath,
        ApplicationName,
        ServerPort,

        ConfigurationKeyCount // Always last!
    };

    QStringList m_configKeys;

    QScopedPointer<QSettings> m_settings;
};

#endif // SECONFIGURATION_H
