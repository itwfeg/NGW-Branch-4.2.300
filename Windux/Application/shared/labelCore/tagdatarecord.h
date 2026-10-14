/**
 * @file tagdatarecord.h
 * Hobart tag data record classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef TAGDATARECORD_H
#define TAGDATARECORD_H

#include "tagdatafunctioncode.h"
#include "tagdatacommand.h"

#include <QList>
#include <QPair>
#include <QString>

class tagDataStream;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef tagDataRecordType
 * @brief Tag data record type.
 *
 * Record type is a 4 byte string that always starts with 'RT'. We represent it internally as a
 * 32 bit integer.
 */
typedef qint32 tagDataRecordType;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef tagData
 * @brief Tag data value class.
 *
 * Tag data is simply a function code and value. The function code indicates what the data is and
 * how it should be processed.
 *
 * The tag data function code @c tagDataFunctionCode::type can be found in @c first. The tag data
 * @c QString can be found in @c second.
 */
typedef QPair<tagDataFunctionCode::type, QString> tagData;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef tagDataList
 * @brief List of @c tagData pair values.
 */
typedef QList<tagData> tagDataList;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class tagDataRecord
 * @brief Hobart tagged data record.
 *
 * A tagged data record is composed of a record type followed by zero to many tag function code and
 * value pairs. An example record may look like the following:
 * @code
 *   RT89<US>p#37<US>rc999999<US>bc1<RS>
 * @endcode
 *
 * This class contains the record data in the form of a record type and tag data list pair. For the
 * above example you would have:
 * @arg record type RT89
 * @arg tag 0 [p#,37]
 * @arg tag 1 [rc,999999]
 * @arg tag 2 [bc,1]
 *
 * The tag data record type @c tagDataRecordType can be found in @c first. The tag data list
 * @c tagDataList can be found in @c second.
 */
class tagDataRecord : public QPair<tagDataRecordType, tagDataList>
{
    typedef tagDataRecord _Myt;
    typedef QPair<tagDataRecordType, tagDataList> _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    tagDataRecord();

    /**
     * Constructor.
     *
     * @param[in] rt  record type to construct
     */
    tagDataRecord( tagDataRecordType rt );

    /**
     * Constructor.
     *
     * @param[in] rt  record type to construct
     * @param[in] tags  tag values
     */
    tagDataRecord( tagDataRecordType rt, const tagDataList& tags );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    tagDataRecord( const _Myt& rhs );

    /**
     * Destructor.
     */
    virtual ~tagDataRecord();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if record is valid.
     *
     * @returns  @c true if valid, @c false otherwise.
     */
    virtual bool isValid() const;

    /**
     * Check if record is a header record.
     *
     * @returns  @c true if header record, @c false otherwise.
     */
    virtual bool isHeaderRecordType() const;

    /**
     * Check if record is an error record.
     *
     * @returns  @c true if error record, @c false otherwise.
     */
    virtual bool isErrorRecordType() const;

    /**
     * Check if record is a footer record.
     *
     * @returns  @c true if footer record, @c false otherwise.
     */
    virtual bool isFooterRecordType() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Serializes a tag data stream.
     *
     * @param [in,out] os  output stream
     */
    virtual void serialize( tagDataStream& os ) const;

    /**
     * Parses a tag data stream.
     *
     * @param[in,out] is  input stream
     */
    virtual void parse( tagDataStream& is );

    ///////////////////////////////////////////////////////////////////////////
    // Static Methods

    /**
     * Retrieve a header record.
     *
     * @param[in] scaleId  scale id (optional)
     * @returns  header record
     */
    static tagDataRecord header( const QString& scaleId = QString() );

    /**
     * Retrieve a error record.
     *
     * @param[in] errorCode  error code
     * @param[in] keyValue  key value (optional)
     * @param[in] failedTag  failed tag (optional)
     * @param[in] errorRecord  error record (optional)
     * @returns  header record
     */
    static tagDataRecord error( const QString& errorCode,
        const QString& keyValue = QString(),
        const QString& failedTag = QString(),
        const QString& errorRecord = QString() );

    /**
     * Retrieve a footer record.
     *
     * @returns  footer record.
     */
    static tagDataRecord footer();

protected:

    /**
     * Record seperator.
     */
    static const qint8 RS;

    /**
     * Unit seperator.
     */
    static const qint8 US;

};

/**
 * Data extraction operator.
 *
 * Serializes the tag data record into a tag data stream.
 *
 * @param [in,out] lhs  output stream
 * @param [in] rhs  object to serialize
 * @returns  reference to @p lhs
 */
tagDataStream& operator << ( tagDataStream& lhs, const tagDataRecord& rhs );

/**
 * Data insertion operator.
 *
 * Parses a tag data record from a tag data stream.
 *
 * @param[in,out] lhs  input stream
 * @param[out] rhs  object to parse into
 * @returns  reference to @p lhs
 */
tagDataStream& operator >> ( tagDataStream& lhs, tagDataRecord& rhs );

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef tagDataRecordList
 * @brief List of @c tagDataRecord values.
 */
typedef QList<tagDataRecord> tagDataRecordList;

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // TAGDATARECORD_H
