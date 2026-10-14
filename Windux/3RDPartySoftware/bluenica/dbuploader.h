/**
 * @file dbuploader.h
 * Data uploader for SQLITE.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DBUPLOADER
#define DBUPLOADER

#include <map>
#include <string>

class bluenicaConfigDataRec;
class bluenicaTempDataRec;
class leAdvertInfo;

class QSqlDatabase;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Data uploader for SQLITE.
class dbUploader
{
    typedef dbUploader _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    dbUploader();

    /// Destructor.
    virtual ~dbUploader();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Pull event from queue.
    /**
     * @return  queued event
     */
    virtual void postEvent( const leAdvertInfo &e );

    /// Perform database housekeeping.
    virtual void housekeeping();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process sensor data.
    /**
     * @param[in] data  sensor data
     */
    template <class T>
    void processSensorData( const T &data );

private:

    static const std::string DB_FILENAME;

    typedef std::map<std::string, unsigned int> sensorRecordMap;

    QSqlDatabase *db_;

    sensorRecordMap records_;

    /// Initialize.
    void initialize();

    /// Create database tables.
    void createTables( bool populate = true );

    /// Upgrade database tables from previous version.
    void upgradeTables( unsigned int oldVersion );

    /// Query for config value.
    std::string queryConfigValue( const std::string &id );

    /// Check for duplicate sensor record.
    bool isDuplicateRecord( const std::string &address, unsigned int record );

    /// Insert sensor to table.
    unsigned int insertSensor( const std::string &address, unsigned int type );

    /// Update sensor table.
    unsigned int updateSensor( const std::string &address, unsigned int type, unsigned int stamp, float temp, unsigned int batteryLevel, unsigned int &sensorPoint );

    /// Insert temp reading to table.
    bool insertTemperature( unsigned int sensor, unsigned int stamp, float temp, unsigned int sensorPoint = 0 );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBUPLOADER

