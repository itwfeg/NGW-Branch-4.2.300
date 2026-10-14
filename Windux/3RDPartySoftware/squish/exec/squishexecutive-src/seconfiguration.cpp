#include "seconfiguration.h"

#include <QFileInfo>
#include <QSettings>

static const char *CONFIGURATION_FILE = "squishexecutive.conf";

SEConfiguration::SEConfiguration()
    : m_settings(new QSettings(QString::fromLatin1(CONFIGURATION_FILE), QSettings::IniFormat))
{
    m_configKeys.insert(ObjectMapFileName, QStringLiteral("objectMapFileName"));
    m_configKeys.insert(MaximumEventCount, QStringLiteral("maximumEventCount"));
    m_configKeys.insert(SquishPath,        QStringLiteral("squishPath"));
    m_configKeys.insert(ApplicationName,   QStringLiteral("applicationName"));
    m_configKeys.insert(ServerPort,        QStringLiteral("serverPort"));
}

bool SEConfiguration::verify() const
{
    if (!QFileInfo(QString::fromLatin1(CONFIGURATION_FILE)).exists()) {
        qWarning("No configuration file found, make sure \"%s\" exists next to the application binary", CONFIGURATION_FILE);
        return false;
    }

    bool configComplete = true;
    for (int i = 0; i < ConfigurationKeyCount; ++i) {
        const QString configKey = m_configKeys[i];
        if (m_settings->value(configKey, QVariant()).isNull()) {
            qWarning("Missing key \"%s\" in configuration file", qPrintable(configKey));
            configComplete = false;
        }
    }

    return configComplete;
}

QString SEConfiguration::objectMapFileName() const
{
    return m_settings->value(m_configKeys[ObjectMapFileName], QStringLiteral("")).toString();
}

int SEConfiguration::maximumEventCount() const
{
    return m_settings->value(m_configKeys[MaximumEventCount], 0).toInt();
}

QString SEConfiguration::squishPath() const
{
    return m_settings->value(m_configKeys[SquishPath], QStringLiteral("")).toString();
}

QString SEConfiguration::applicationName() const
{
    return m_settings->value(m_configKeys[ApplicationName], QStringLiteral("")).toString();
}

quint16 SEConfiguration::serverPort() const
{
    return static_cast<quint16>(m_settings->value(m_configKeys[ServerPort], 0).toUInt());
}
