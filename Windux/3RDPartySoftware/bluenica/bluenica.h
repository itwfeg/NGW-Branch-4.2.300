/**
 * @file bluenica.h
 * Bluenica structures and definitions.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BLUENICA
#define BLUENICA

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

#define RECTYPE_TRANSMISSION            2           ///< Data transmission between two bluenica devices

#define RECSUBTYPE_XMIT_SIMPLE          1           ///< Simple transmission (data download)
#define RECSUBTYPE_XMIT_WASP_STATS      2           ///< WaSP statistics (since v.1.35.52)
#define RECSUBTYPE_XMIT_WASP_LOG        3           ///< WaSP activity log (since v.1.44.63)
#define RECSUBTYPE_XMIT_ADV_TEMP_DATA	4           ///< Advertisement-based Temperature Data (since v. 2.10.76)
#define RECSUBTYPE_XMIT_ADV_DATA        5           ///< Advertisement-based data (since v. 3.30)
#define RECSUBTYPE_XMIT_FULL_ADV        6           ///< Full advertisement data.

/// Upload header.
struct __attribute__ ((__packed__)) bluenicaHeader
{
    uint8_t syntax;                                 ///< Record syntax version. Currently 1.
    uint8_t id[3];                                  ///< Pseudo-unique record ID. B0 is LSB.
    uint8_t type;                                   ///< General record classification.
    uint8_t subtype;                                ///< Record subtype.
    uint8_t stamp[3];                               ///< Device clock value, in seconds. B0 is LSB.
    uint8_t payload_len;                            ///< The length of the following payload. Can be 0.
};

/// Advert data.
struct __attribute__ ((__packed__)) bluenicaAdvertDataHeader : public bluenicaHeader
{
    uint8_t sensor_addr[6];                         ///< The ID of the sensor.
    uint8_t wasp_addr[6];                           ///< The ID of the wasp.
    uint32_t stamp;                                 ///< UNIX-style epoch value of the current UTC date/time.
};

/// Full advert data.
struct __attribute__ ((__packed__)) bluenicaFullAdvertDataHeader : public bluenicaAdvertDataHeader
{
    uint8_t versionMajor;                           ///< Major code version.
    uint8_t versionMinor;                           ///< Minor code version.
    uint16_t versionBuild;                          ///< Code build number.
    uint32_t clock;                                 ///< System clock.
    uint16_t cycleTime;                             ///< Total time (seconds).
    uint16_t totalDiscoveredDevices;                ///< Total discovered devices.
    uint8_t maxDiscoveredDevices;                   ///< Max discovered devices.
    uint16_t totalRecords;                          ///< Number of records created.
    uint16_t totalUploads;                          ///< Upload attempts.
    uint16_t connErrors;                            ///< WiFi connection errors.
    uint16_t sendErrors;                            ///< Data send errors.
    uint8_t modemResets;                            ///< Number of modem resets.
    uint8_t rssi;                                   ///< RSSI.
};

/// Tag header.
struct __attribute__ ((__packed__)) bluenicaTagDataHeader
{
    uint8_t num_tags;                               ///< Number of tags the data will come from.
    uint32_t num_recs;                              ///< Total number of data records.
    uint8_t rec_size;                               ///< The size of an individual data record.
};

/// Sensor header.
struct __attribute__ ((__packed__)) bluenicaTagIndex
{
    uint8_t tag_id[6];                              ///< The ID of the peripheral (Tag) device.
    uint8_t tag_index;                              ///< An ordinal number assigned to the Tag in this set of data.
};

/// Tag data.
struct __attribute__ ((__packed__)) bluenicaTagData
{
    uint8_t tag_index;                              ///< The ID of the peripheral (Tag) device.
    uint32_t stamp;                                 ///< UNIX-style epoch value of the current UTC date/time.
    uint8_t name[2];                                ///< The first and third character of the device name.
    uint8_t data[16];                               ///< Full advertised data starting from offset 15 (Download age).
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BLUENICA

