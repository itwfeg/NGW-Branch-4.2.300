/**
 * @file bluenicauploader.h
 * Data uploader for Bluenica sensors.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BLUENICAUPLOADER
#define BLUENICAUPLOADER

#include "leadvertinfo.h"

#include <pthread.h>

#include <list>
#include <map>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Data uploader for Bluenica sensors.
class bluenicaUploader
{
    typedef bluenicaUploader _Myt;

public:

    /// States.
    enum state
    {
        STATE_OFFLINE,                              ///< Offline to servers.
        STATE_ONLINE,                               ///< Online to servers.
        STATE_WAITING,                              ///< Waiting for sensor data.
        STATE_UPLOADING                             ///< Uploading sensor data.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    bluenicaUploader();

    /// Destructor.
    virtual ~bluenicaUploader();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if uploading is enabled.
    /**
     * @return  @c true to enable, @c false otherwise
     */
    virtual bool enableUploads() const {return uploadsEnabled_;}

    /// Retrieve state.
    /**
     * @return  state
     */
    virtual state currentState() const {return state_;}

    /// Set adapter address.
    /**
     * @param[in] bdaddr  address
     */
    virtual void setAdapterAddress( const bdaddr_t *bdaddr );

    /// Set application version.
    /**
     * @param[in] versionMajor  major
     * @param[in] versionMinor  minor
     * @param[in] buildNumber  build
     */
    virtual void setApplicationVersion( unsigned int versionMajor, unsigned int versionMinor, unsigned int buildNumber );

    /// Set if uploading is enabled.
    /**
     * @param[in] value  @c true to enable, @c false otherwise
     */
    virtual void setEnableUploads( bool value ) {uploadsEnabled_ = value;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Pull event from queue.
    /**
     * @return  queued event
     */
    virtual void postEvent( const leAdvertInfo &e );

    /// Send loop.
    virtual void sendLoop();

    /// Start thread.
    /**
     * @return  @c true if started succesfully, @c false otherwise
     */
    virtual bool start();

    /// Stop thread.
    virtual void stop();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve event count.
    /**
     * @return  number of events queued
     */
    virtual size_t eventCount() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Load events from filesystem.
    virtual void load();

    /// Save events to filesystem.
    virtual void save() const;

private:

    static const std::string ADVERT_QUEUE_FILENAME;

    enum {OFFLINE_TIMEOUT = 60 * 1000}; // 1m
    enum {IDLE_TIMEOUT = 5 * 60 * 1000}; // 5m

    enum {MAX_TAGS = 255};
    enum {MAX_UPLOAD_EVENTS = 32768};

    enum uploadResult
    {
        UPLOAD_ERROR,
        UPLOAD_COMPLETE,
        UPLOAD_AGAIN
    };

    typedef std::list<leAdvertInfo> advertList;

    std::vector<std::string> servers_;              ///< List of bluenica servers.
    size_t currentServer_;                          ///< Index of server we will try to communicate with.

    bdaddr_t adapter_;                              ///< Adapter address.

    unsigned int versionMajor_;                     ///< Version major.
    unsigned int versionMinor_;                     ///< Version minor.
    unsigned int buildNumber_;                      ///< Build number.

    mutable pthread_mutex_t m_;

    advertList le_adverts_;                         ///< Adverts.
    state state_;                                   ///< Uploader state.

    bool uploadsEnabled_;                           ///< Uploads are enabled or not.

    pthread_t thread_;

    bool done_;                                     ///< Thread is done.
    bool error_;                                    ///< Thread has an error.
    bool running_;                                  ///< Thread is running.

    unsigned int sleeping_;                         ///< Thread sleep time in milliseconds.

    std::string server_;                            ///< Server we are communicating to.
    unsigned short port_;                           ///< Port we are communicating on.

    int skew_;                                      ///< Clock skew between this machine and server in seconds.

    unsigned int cycleTime_;
    unsigned int totalUploads_;
    unsigned int sendErrors_;

    /// Process current state.
    template <state S>
    void processCurrentState();

    /// Upload queued events.
    uploadResult upload();

    /// Create tag buffer ready to be uploaded to bluenica.
    size_t createTagBuffer( std::string &buffer ) const;

    /// Parse and validate response.
    bool parseResponse( const std::string &sid, const std::string &resp );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BLUENICAUPLOADER

