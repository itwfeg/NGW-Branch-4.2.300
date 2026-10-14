/**
 * @file dbquery.h
 * Data query for SQLITE.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DBQUERY
#define DBQUERY

#include <QList>
#include <QString>

class QSqlDatabase;

/// Profile information.
struct profileInfo
{
    unsigned int id;
    QString description;
    qreal tempWarningFrom;
    qreal tempWarningTo;
    qreal tempNormalFrom;
    qreal tempNormalTo;
    qreal tempStandard;
};

/// Sensor point information.
struct sensorPointInfo
{
    unsigned int id;
    QString description;
    unsigned int profile;
    QString sensorAddress;
    bool generateAlerts;
};

/// Sensor point state info.
struct sensorPointStateInfo
{
    unsigned int stamp;
    qreal temp;
    unsigned int batteryLevel;
    unsigned int state;
};

/// Sensor information.
struct sensorInfo
{
    unsigned int id;
    QString address;
    unsigned int type;
    unsigned int lastComm;
    qreal temperature;
    unsigned int batteryLevel;
};

/// Temperature information.
struct tempInfo
{
    unsigned int sensor;
    unsigned int stamp;
    qreal temperature;
    unsigned int sensorPoint;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Data query for SQLITE.
class dbQuery
{
    typedef dbQuery _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    dbQuery();

    /// Destructor.
    virtual ~dbQuery();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Delete profile.
    /**
     * @param[in] id  profile id
     * @return  @c true upon successful delete, @c false otherwise
     */
    virtual bool deleteProfile( unsigned int id );

    /// Delete sensor point info.
    /**
     * @param[in] id  sensor point id
     * @return  @c true upon successful delete, @c false otherwise
     */
    virtual bool deleteSensorPoint( unsigned int id );

    /// Delete sensor info.
    /**
     * @param[in] id  sensor id
     * @return  @c true upon successful delete, @c false otherwise
     */
    virtual bool deleteSensor( unsigned int id );

    /// Insert profile info.
    /**
     * @param[in] value  profile info value
     * @return  new record id, or zero upon failure
     */
    virtual unsigned int insertProfile( const profileInfo &value );

    /// Insert sensor point info.
    /**
     * @param[in] value  sensor point info value
     * @return  new record id, or zero upon failure
     */
    virtual unsigned int insertSensorPoint( const sensorPointInfo &value );

    /// Check if profile is being used.
    /**
     * @param[in] id  profile id
     * @return  @c true if in use, @c false otherwise
     */
    virtual bool isProfileUsed( unsigned int id );

    /// Check if sensor is being used.
    /**
     * @param[in] address  sensor address
     * @return  @c true if in use, @c false otherwise
     */
    virtual bool isSensorUsed( const QString &address );

    /// Query for config value.
    /**
     * @param[in] id  config key
     * @return  value
     */
    virtual QString queryConfigValue( const QString &id );

    /// Query for profile info.
    /**
     * @param[in] id  profile id
     * @return  profile
     */
    virtual profileInfo queryProfile( unsigned int id );

    /// Query for profile info.
    /**
     * @return  list of profiles
     */
    virtual QList<profileInfo> queryProfiles();

    /// Query for sensor point info.
    /**
     * @param[in] id  sensor point id
     * @return  sensor point
     */
    virtual sensorPointInfo querySensorPoint( unsigned int id );

    /// Query for sensor point info.
    /**
     * @return  list of sensor points
     */
    virtual QList<sensorPointInfo> querySensorPoints();

    /// Query for sensor point state.
    /**
     * @param[in] id  sensor point id
     * @return  sensor point state
     */
    virtual sensorPointStateInfo querySensorPointState( unsigned int id );

    /// Query for sensor info.
    /**
     * @param[in] id  sensor id
     * @return  sensor
     */
    virtual sensorInfo querySensor( unsigned int id );

    /// Query for sensor info.
    /**
     * @param[in] id  sensor id
     * @return  sensor
     */
    virtual sensorInfo querySensor( const QString &address );

    /// Query for sensor info.
    /**
     * @param[in] age  max age of last comm
     * @return  list of sensors
     */
    virtual QList<sensorInfo> querySensors( unsigned int age = 0 );

    /// Query for temperature info.
    /**
     * @param[in] sensor  sensor id
     * @param[in] sensorPoint  sensor point id
     * @param[in] bottom  filter bottom (epoch time in seconds)
     * @param[in] top  filter top (epoch time in seconds)
     * @return  list of temps
     */
    virtual QList<tempInfo> queryTemps( unsigned int sensor, unsigned int sensorPoint, unsigned int bottom = 0, unsigned int top = 0 );

    /// Update config value.
    /**
     * @param[in] id  config key
     * @param[in] value  config value
     * @return  @c true upon successful update, @c false otherwise
     */
    virtual bool updateConfigValue( const QString &id, const QString &value );

    /// Update profile info.
    /**
     * @param[in] value  profile info value
     * @return  @c true upon successful update, @c false otherwise
     */
    virtual bool updateProfile( const profileInfo &value );

    /// Update sensor point info.
    /**
     * @param[in] value  sensor point info value
     * @return  @c true upon successful update, @c false otherwise
     */
    virtual bool updateSensorPoint( const sensorPointInfo &value );

private:

    static const QString DB_FILENAME;

    QSqlDatabase *db_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBQUERY
