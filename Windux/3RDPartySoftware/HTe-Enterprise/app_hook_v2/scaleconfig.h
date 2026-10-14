/**
 * @file scaleconfig.h
 * Scale configuration for HTe servers.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALECONFIG_H
#define SCALECONFIG_H

#include <QDateTime>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QMutex>
#include <QObject>

class QJsonDocument;
class QString;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale configuration.
class scaleConfig : public QObject
{
    Q_OBJECT

    typedef scaleConfig _Myt;
    typedef QObject _Mybase;

signals:

    /// Signal for config changed.
    void configChanged();

public:

    /// Mapping of string values.
    typedef QMap<QString, QString> stringMap;

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve country code.
    /**
     * @return  country code
     */
    QString countryCode() const;

    /// Retrieve scale model from flash.
    /**
     * Retrieve the HOBART scale model.
     * @return  scale model
     */
    QString fullModel() const {return fullModel_;}

    /// Check if configuration has been initialized.
    /**
     * @return  @c true if initialized, @c false otherwise
     */
    bool isInitialized() const;

    /// Retrieve scale model.
    /**
     * Retrieve the HTe scale model.
     * @return  scale model
     */
    QString model() const {return model_;}

    /// Retrieve scale serial from flash.
    /**
     * @return  scale serial
     */
    unsigned int serialNum() const {return serialNum_;}

    /// Set current configuration.
    /**
     * @param[in] value  config
     * @return  @c true if config valid, @c false otherwise
     */
    bool setCurrentConfig( const QJsonDocument& value );

    /// Retrieve config timestamp.
    /**
     * @return  timestamp of configuration file
     */
    QString timestamp() const;

    /// Retrieve config value.
    /**
     * @param[in] path  config path
     * @param[out] okay  @c true when value found, @c false otherwise
     * @return  value
     */
    template <class T>
    T value( const QString& path, bool *okay = nullptr ) const;

    /// Retrieve scale vendor.
    /**
     * Retrieve the HTe scale vendor.
     * @return  vendor
     */
    QString vendor() const;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Create request info from config.
    /**
     * @return  request info
     */
    QJsonObject createRequestInfo() const;

    /// Create event info from config.
    /**
     * @return  event info
     */
    QJsonObject createEventInfo() const;

    /// Convert to @c QJsonObject.
    /**
     * @return  json object
     */
    QJsonObject toJsonObject() const;

    /// Validate request info.
    /**
     * @param[in] doc  document to validate
     * @param[in] requestUuid  request uuid (optional)
     * @return  @c true if valid, @c false otherwise
     */
    bool validateRequestInfo( const QJsonDocument& doc, const QString& requestUuid = QString() ) const;

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Convert @c QDateTime to @c QString using configuration options.
    /**
     * @param[in] value  @c QDateTime to convert
     * @return  string value
     */
    static QString dateTimeToString( const QDateTime& value );

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

    /// Parse a config file.
    /**
     * @param[in] file  config file
     * @param[in] delim  delimiter
     * @param[out] config  config values
     * @return  @c true upon success, @c false otherwise
     */
    static bool parseConfigFile( const QString& file, const QString& delim, stringMap& config );

private:

    static QMutex instanceMutex_;
    static _Myt *instance_;

    mutable QMutex m_;

    QJsonObject config_;
    QDateTime configStamp_;

    unsigned int serialNum_;
    QString fullModel_;

    QString vendor_;
    QString model_;

    /// Constructor.
    scaleConfig( QObject *parent = nullptr );

    /// Destructor.
    ~scaleConfig();

    /// Create common request / event info from config.
    /**
     * @return  request info
     */
    QJsonObject createCommonInfo() const;

    /// Read configuration.
    bool readConfiguration();

    /// Write configuration.
    bool writeConfiguration( const QJsonObject& config, const QDateTime& configStamp, bool save );

    /// Validate configuration.
    bool validate( const QJsonObject& value );

    /// Read flash information.
    void readFlashInfo();

    /// Update cron jobs.
    void updateCronJobs() const;

    /// Lookup config value.
    QJsonValue lookupValue( const QString& path, bool *okay ) const;

    /// Generate uuid.
    static QString uuid( bool random = false );

    // not implemented
    scaleConfig( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

/// Specialization for @c QString objects.
template <>
inline QString scaleConfig::value( const QString& path, bool *okay ) const
{
    QString result;

    QMutexLocker guard( &m_ );

    const QJsonValue val( lookupValue( path, okay ) );

    if ( val.isString() )
        result = val.toString();
    else if ( okay )
        *okay = false;

    return result;
}

/// Specialization for @c double objects.
template <>
inline double scaleConfig::value( const QString& path, bool *okay ) const
{
    double result( 0.0 );

    QMutexLocker guard( &m_ );

    const QJsonValue val( lookupValue( path, okay ) );

    if ( val.isDouble() )
        result = val.toDouble();
    else if ( okay )
        *okay = false;

    return result;
}

/// Specialization for @c int objects.
template <>
inline int scaleConfig::value( const QString& path, bool *okay ) const
{
    int result( 0 );

    QMutexLocker guard( &m_ );

    const QJsonValue val( lookupValue( path, okay ) );

    if ( val.isDouble() )
        result = val.toInt();
    else if ( okay )
        *okay = false;

    return result;
}

/// Specialization for @c bool objects.
template <>
inline bool scaleConfig::value( const QString& path, bool *okay ) const
{
    bool result( false );

    QMutexLocker guard( &m_ );

    const QJsonValue val( lookupValue( path, okay ) );

    if ( val.isBool() )
        result = val.toBool();
    else if ( okay )
        *okay = false;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALECONFIG_H
