/**
 * @file bluenicauploader.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "bluenica.h"
#include "bluenicadatarec.h"
#include "bluenicauploader.h"
#include "httputil.h"

#include <sys/time.h>

#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#define GrKey_Nop     0
#define GrKey_Err     1
#define GrKey_Sid     2
#define GrKey_Crc     3
#define GrKey_Utc     4

#define HTTP_ERR_NOERROR      0
#define HTTP_ERR_DATA_SHORT   1
#define HTTP_ERR_CRC_ERROR    2
#define HTTP_ERR_DB_FAILURE   3
#define HTTP_ERR_SSID_ERROR   254 // no valid SID found in the response
#define HTTP_ERR_UNAVAILABLE  255 // no valid response received

/// This is the filename we store queue information to.
const std::string bluenicaUploader::ADVERT_QUEUE_FILENAME( "bnupload.queue" );

///////////////////////////////////////////////////////////////////////////////////////////////////
void *startSendLoop( void *ptr )
{
    bluenicaUploader *obj( reinterpret_cast<bluenicaUploader*>( ptr ) );

    // go!
    obj->sendLoop();

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaUploader::bluenicaUploader() :
    state_( STATE_OFFLINE ),
    uploadsEnabled_( true ),
    running_( false ),
    sleeping_( 0 ),
    cycleTime_( time( NULL ) ),
    totalUploads_( 0 ),
    sendErrors_( 0 )
{
    int rc;

    // reset adapter
    memset( &adapter_, 0, sizeof(adapter_) );

    // reset application version info
    versionMajor_ = versionMinor_ = buildNumber_ = 0;

    // bluenica servers
    servers_.push_back( "novotrace.com" );
    servers_.push_back( "bn-gateway.com" );

    // pick server at random
    currentServer_ = (time( NULL ) % servers_.size());

    if ( 0 != (rc = pthread_mutex_init( &m_, NULL )) )
        std::cerr << "Failed to init mutex: " << rc << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaUploader::~bluenicaUploader()
{
    stop();

    pthread_mutex_destroy( &m_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaUploader::setAdapterAddress( const bdaddr_t *bdaddr )
{
    if ( bdaddr )
    {
        pthread_mutex_lock( &m_ );
        memcpy( &adapter_, bdaddr, sizeof(adapter_) );
        pthread_mutex_unlock( &m_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaUploader::setApplicationVersion( unsigned int versionMajor, unsigned int versionMinor, unsigned int buildNumber )
{
    pthread_mutex_lock( &m_ );

    versionMajor_ = versionMajor;
    versionMinor_ = versionMinor;
    buildNumber_ = buildNumber;

    pthread_mutex_unlock( &m_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaUploader::postEvent( const leAdvertInfo &e )
{
    // only handle bluenica sensors
    if ( !bluenicaDataRec::isBluenicaSensor( e ) )
        return;

    const std::string data( e.eir( GAP_ADTYPE_MANUFACTURER_SPECIFIC ) );

    const std::string addr( data.substr( 3, 6 ) );
    const std::string recId( data.substr( 14, 3 ) );

    // lock event list
    pthread_mutex_lock( &m_ );

    // filter out duplicate events
    // event is duplicate when same sensor (addr) and same record (recId)
    bool duplicate( false );

    for ( advertList::const_iterator a( le_adverts_.begin() ); a != le_adverts_.end(); ++a )
    {
        const std::string i_data( a->eir( GAP_ADTYPE_MANUFACTURER_SPECIFIC ) );

        const std::string i_addr( i_data.substr( 3, 6 ) );
        const std::string i_recId( i_data.substr( 14, 3 ) );

        if ( (duplicate |= ((addr == i_addr) && (recId == i_recId))) )
            break;
    }

    // add event to list
    if ( !duplicate )
    {
        le_adverts_.push_back( e );

#ifdef _DEBUG_SENSOR
        bluenicaDataRec *record;

        if ( (record = bluenicaDataRec::decode( e )) )
        {
            record->print();
            delete record;
        }
#endif
    }

    // unlock event list
    pthread_mutex_unlock( &m_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void bluenicaUploader::processCurrentState<bluenicaUploader::STATE_OFFLINE>()
{
    std::string resp;

    // switch servers
    if ( servers_.size() <= ++currentServer_ )
        currentServer_ = 0;

    // retrieve current server
    server_ = servers_[currentServer_];
    port_ = 80;

    // ping server
    if (( httpUtil::get( server_, port_, "exec.ashx?cmd=PING", resp ) ) &&
        ( "err=0;text=PONG" == resp ))
    {
        std::cout << "online to " << server_ << std::endl;

        // we are online!
        state_ = STATE_ONLINE;
    }
    else
    {
        std::cout << "server " << server_ << " is OFFLINE!" << std::endl;

        // sleep a bit before we try again
        sleeping_ = OFFLINE_TIMEOUT;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void bluenicaUploader::processCurrentState<bluenicaUploader::STATE_ONLINE>()
{
    std::string resp;

    // retrieve time from server
    // this command (par=0) retrieves a 4 byte unix utc timestamp (i.e. seconds since epoch)
    if (( !httpUtil::get( server_, port_, "exec.ashx?cmd=utc&par=0", resp ) ) ||
        ( 4 != resp.size() ))
    {
        std::cout << "error retrieving utc time!" << std::endl;
    }
    else
    {
        // get timestamp
        struct timeval now;
        gettimeofday( &now, NULL );

        // set clock skew
        skew_ = *((uint32_t *)resp.c_str()) - now.tv_sec;
        std::cout << "clock skew is " << skew_ << std::endl;

        // make sure skew is within proper tolerance (+/- 10m)
        int toler( 10 * 60 );

        if ( labs( skew_ ) <= toler )
        {
            state_ = STATE_WAITING;
            return;
        }

        std::cout << "clock skew exceeds tolerance" << std::endl;
    }

    // go offline
    state_ = STATE_OFFLINE;

    // sleep a bit before we try again
    sleeping_ = OFFLINE_TIMEOUT;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void bluenicaUploader::processCurrentState<bluenicaUploader::STATE_WAITING>()
{
    // check for queued events
    if ( eventCount() )
        state_ = STATE_UPLOADING;
    else
    {
        std::cout << "no events... sleeping..." << std::endl;

        // sleep a bit...
        sleeping_ = IDLE_TIMEOUT;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void bluenicaUploader::processCurrentState<bluenicaUploader::STATE_UPLOADING>()
{
    std::cout << "uploading to " << server_ << std::endl;

    // upload events
    uploadResult rc( upload() );

    // error
    if ( UPLOAD_ERROR == rc )
    {
        std::cout << "upload to server failed" << std::endl;
        state_ = STATE_OFFLINE;
        ++sendErrors_;

        // we are not online... sleep a bit...
        sleeping_ = OFFLINE_TIMEOUT;
    }

    // all queued events were uploaded
    else if ( UPLOAD_COMPLETE == rc )
    {
        std::cout << "upload complete... entering waiting state" << std::endl;
        state_ = STATE_WAITING;

        // save current state to filesystem
        save();

        // sleep a bit...
        sleeping_ = IDLE_TIMEOUT;

        // reset cycle time and error count
        cycleTime_ = time( NULL );
        sendErrors_ = 0;

        // increment upload count
        ++totalUploads_;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaUploader::sendLoop()
{
    running_ = true;

    try
    {
        // load queues from filesystem
        load();
    }
    catch ( ... )
    {
        std::cerr << "failed to load state" << std::endl;
        remove( ADVERT_QUEUE_FILENAME.c_str() );
    }

    // main read loop
    do
    {

        // thread is sleeping
        if ( 0 < sleeping_ )
        {
            usleep( 500000 );
            sleeping_ -= 500;
        }

        // uploads are disabled
        else if ( !uploadsEnabled_ )
        {
            state_ = STATE_OFFLINE;
            sleeping_ = OFFLINE_TIMEOUT;
        }

        // thread is active
        else
        {
            switch ( state_ )
            {
            case STATE_OFFLINE:
                processCurrentState<STATE_OFFLINE>();
                break;

            case STATE_ONLINE:
                processCurrentState<STATE_ONLINE>();
                break;

            case STATE_WAITING:
                processCurrentState<STATE_WAITING>();
                break;

            case STATE_UPLOADING:
                processCurrentState<STATE_UPLOADING>();
                break;

            default:
                std::cerr << "Unknown state" << std::endl;
                error_ = true;
                break;
            }
        }

    } while (( !done_ ) && ( !error_ ));

    // save queues to filesystem
    save();

    running_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bluenicaUploader::start()
{
    // we are not running (yet...)
    if ( !running_ )
    {
        int rc;

        // start thread
        done_ = false;
        error_ = false;

        if ( (rc = pthread_create( &thread_, NULL, startSendLoop, this )) )
        {
            std::cerr << "Error creating thread: " << rc << std::endl;
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaUploader::stop()
{
    // need to stop thread
    if ( running_ )
    {
        int rc;

        // stop thread
        done_ = true;

        if ( (rc = pthread_join( thread_, NULL )) )
            std::cerr << "Error joining thread: " << rc << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t bluenicaUploader::eventCount() const
{
    size_t count( 0 );

    pthread_mutex_lock( &m_ );
    count = le_adverts_.size();
    pthread_mutex_unlock( &m_ );

    return count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaUploader::load()
{
    std::ifstream ifs( ADVERT_QUEUE_FILENAME.c_str() );

    // open file
    if ( ifs.is_open() )
    {
        // read in all adverts
        while (( !ifs.eof() ) && ( !ifs.bad() ))
        {
            leAdvertInfo advert;
            ifs >> advert;

            postEvent( advert );
        }

        ifs.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bluenicaUploader::save() const
{
    struct timeval now;
    gettimeofday( &now, NULL );

    // only save last two weeks worth of data (14 days)
    const time_t filter( now.tv_sec - (14 * 24 * 60 * 60) );

    // open file
    std::ofstream ofs( ADVERT_QUEUE_FILENAME.c_str() );

    if ( ofs.is_open() )
    {
        pthread_mutex_lock( &m_ );

        // save each advert
        for ( advertList::const_iterator a( le_adverts_.begin() ); a != le_adverts_.end(); ++a )
            if ( filter <= a->timestamp() )
                ofs << (*a);

        pthread_mutex_unlock( &m_ );

        ofs.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bluenicaUploader::uploadResult bluenicaUploader::upload()
{
    uploadResult rc( UPLOAD_COMPLETE );

    // lock advert data
    pthread_mutex_lock( &m_ );

    // generate tag buffer
    std::string sendBuffer;
    size_t numEvents( createTagBuffer( sendBuffer ) );

    if ( numEvents )
    {
        std::string recvBuffer;

#ifdef _DEBUG_HTTP
        std::ofstream ofs( "/tmp/bluenicaSend.txt", std::ofstream::out );
        ofs << sendBuffer;
        ofs.close();
#endif

        // format address for posting
        char address[16];
        sprintf( address, "%02X%02X%02X%02X%02X%02X", adapter_.b[0], adapter_.b[1], adapter_.b[2], adapter_.b[3], adapter_.b[4], adapter_.b[5] );

        char sid[16];
        sprintf( sid, "%04x", (int)(time( NULL ) & 0xffff) );

        char url[1024];
        sprintf( url, "BinPost.ashx?fname=Hobart_g%d_%s_fb_s%d&dest=1&opt=1&crc=4&sid=%s", currentServer_, address, sendBuffer.length() - 4, sid );

        // post to server
        std::cout << "uploading " << numEvents << " records" << std::endl;

        if ( httpUtil::post( server_, port_, url, sendBuffer, recvBuffer ) )
        {
#ifdef _DEBUG_HTTP
            std::ofstream ofr( "/tmp/bluenicaRecv.txt", std::ofstream::out );
            ofr << recvBuffer;
            ofr.close();
#endif
            if ( parseResponse( sid, recvBuffer ) )
            {
                // we need to upload again if we processed max events
                if ( MAX_UPLOAD_EVENTS == numEvents )
                    rc = UPLOAD_AGAIN;

                do
                {
                    // dump processed adverts
                    le_adverts_.pop_front();

                } while ( 0 != --numEvents );
            }
            else
            {
                std::cout << "upload to '" << url << "' failed" << std::endl;
                rc = UPLOAD_ERROR;
            }
        }

        // post failed
        else
        {
            std::cout << "post to '" << url << "' failed" << std::endl;
            rc = UPLOAD_ERROR;
        }

    } // if events to process

    pthread_mutex_unlock( &m_ );

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t bluenicaUploader::createTagBuffer( std::string &sendBuffer ) const
{
    bluenicaFullAdvertDataHeader header;

    bluenicaTagDataHeader tagHeader;
    bluenicaTagIndex tagIndicies[MAX_TAGS];
    std::string tagDataBuffer;

    size_t numAdvertsProcessed( 0 );

    // buffer format:
    // advert data header
    // tag header (number of sensors, number of tag data readings)
    // tag id's (sensor id's)
    // tag data (tag data reading)
    // crc

    memset( &header, 0, sizeof(header) );
    memset( &tagHeader, 0, sizeof(tagHeader) );
    memset( &tagIndicies, 0, sizeof(tagIndicies) );

    tagHeader.rec_size = sizeof(bluenicaTagData);

    // process adverts
    for ( advertList::const_iterator a( le_adverts_.begin() ); a != le_adverts_.end(); ++a )
    {
        bluenicaTagData tagData;
        uint8_t tagId[6];

        memset( &tagData, 0, sizeof(tagData) );

        // process timestamp
        tagData.stamp = a->timestamp() + skew_;

        // process eirs
        std::list<std::string> eirs( a->eirs() );

        for ( std::list<std::string>::const_iterator eir( eirs.begin() ); eir != eirs.end(); ++eir )
        {
            if (( GAP_ADTYPE_LOCAL_NAME_SHORT == eir->at( 0 ) ) && ( 4 <= eir->length() ))
            {
                tagData.name[0] = eir->at( 1 );
                tagData.name[1] = eir->at( 3 );
            }

            else if (( GAP_ADTYPE_MANUFACTURER_SPECIFIC == eir->at( 0 ) ) && ( 21 <= eir->length() ))
            {
                size_t len( eir->length() - 9 );

                if ( sizeof(tagData.data) < len )
                    len = sizeof(tagData.data);

                // reverse the byte order of tag id
                // bluenica servers seem to require reverse byte order...
                for ( size_t i( 0 ); i < sizeof(tagId); ++i )
                    tagId[i] = (uint8_t) eir->at( 8-i );

                memcpy( tagData.data, &(eir->c_str()[9]), len );
            }
        }

        // figure out tag id
        // tag id's are indexed in tagIndicies therefore we must find the index
        bool found( false );

        for ( uint8_t idx( 0 ); idx < tagHeader.num_tags; ++idx )
            if ( 0 == memcmp( tagId, tagIndicies[idx].tag_id, sizeof(tagId) ) )
            {
                tagData.tag_index = idx;
                found = true;
            }

        if (( !found ) && ( tagHeader.num_tags < MAX_TAGS ))
        {
            uint8_t idx( tagHeader.num_tags++ );

            memcpy( tagIndicies[idx].tag_id, tagId, sizeof(tagId) );
            tagIndicies[idx].tag_index = idx;

            tagData.tag_index = idx;
            found = true;
        }

        // add to buffer
        if ( found )
        {
            tagDataBuffer.append( std::string( (char *) &tagData, sizeof(tagData) ) );
            ++tagHeader.num_recs;
        }

        // only process up to MAX_UPLOAD_EVENTS events
        if ( MAX_UPLOAD_EVENTS == ++numAdvertsProcessed )
            break;
    }

    // setup header
    struct timeval now;
    gettimeofday( &now, NULL );

    header.syntax = 1;
    header.type = RECTYPE_TRANSMISSION;
    header.subtype = RECSUBTYPE_XMIT_FULL_ADV;
    header.payload_len = sizeof(header) - sizeof(bluenicaHeader);
    header.stamp = now.tv_sec + skew_;

    // reverse the byte order of adapter address
    // bluenica servers seem to require reverse byte order...
    for ( size_t i( 0 ); i < sizeof(header.wasp_addr); ++i )
        header.wasp_addr[i] = adapter_.b[ 5-i ];

    header.versionMajor = versionMajor_;
    header.versionMinor = versionMinor_;
    header.versionBuild = buildNumber_;
    header.clock = now.tv_sec;

    header.cycleTime = time( NULL ) - cycleTime_;
    header.totalRecords = tagHeader.num_recs;
    header.totalUploads = totalUploads_;
    header.sendErrors = sendErrors_;

    // build buffer to send
    sendBuffer.append( std::string( (char *) &header, sizeof(header) ) );
    sendBuffer.append( std::string( (char *) &tagHeader, sizeof(tagHeader) ) );
    sendBuffer.append( std::string( (char *) &tagIndicies[0], tagHeader.num_tags * sizeof(bluenicaTagIndex) ) );
    sendBuffer.append( tagDataBuffer );

    // calculate crc
    uint32_t crc( 0 );

    for ( size_t i( 0 ); i < sendBuffer.length(); ++i )
        crc += (uint8_t) sendBuffer.at( i );

    // append crc
    sendBuffer.append( std::string( (char*) &crc, sizeof(crc) ) );

    return numAdvertsProcessed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bluenicaUploader::parseResponse( const std::string &sid, const std::string &resp )
{
    if ( resp.length() < 11 )
        return false;
    else if (( 'B' != resp.at( 0 ) ) || ( 'N' != resp.at( 1 ) ))
        return false;

    size_t pos( 2 );
    size_t key( 0 );

    size_t err( HTTP_ERR_UNAVAILABLE );
    bool sidOk( false );

    while ( (pos + 2) < resp.length() )
    {
        key = (size_t) resp.at( pos++ );

        size_t len( (size_t) resp.at( pos++ ) );

        // validate length
        if ( (pos + len) < resp.length() )
        {
            switch ( key )
            {
            case GrKey_Err:
                err = (size_t) resp.at( pos );
                break;

            case GrKey_Sid:
                sidOk = true;

                for ( size_t i( 0 ); i < sid.length(); ++i )
                    if ( !(sidOk &= (resp.at( pos + i ) == sid.at( i ))) )
                        break;

                break;

            case GrKey_Utc:

                {
                    // get timestamp
                    struct timeval now;
                    gettimeofday( &now, NULL );

                    // set clock skew
                    skew_ = *((uint32_t *) &resp.c_str()[pos]) - now.tv_sec;
                    std::cout << "clock skew is " << skew_ << std::endl;
                }

                break;

            default:
                break;
            }
        }

        pos += len;
    }

    if ( !sidOk )
    {
        std::cout << "bad sid" << std::endl;
        return false;
    }
    else if ( HTTP_ERR_NOERROR != err )
    {
        std::cout << "error code " << err << std::endl;
        return false;
    }

    return true;
}
