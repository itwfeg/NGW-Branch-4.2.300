/**
 * @file systemstring.h
 * System string object that may be printed on a label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef SYSTEMSTRING_H
#define SYSTEMSTRING_H

#include "dataobject.h"
#include "tagdatarecord.h"

#include <QString>

DECLARE_DATA_OBJECT( systemString, dataObjectType::SYSTEM_STRING );                 ///< object type
DECLARE_TAG_DATA_RECORD_TYPE( systemString, tagDataCommand::READ_ALL, 'RT76' );     ///< tag data read all command
DECLARE_TAG_DATA_RECORD_TYPE( systemString, tagDataCommand::WRITE, 'RT9F' );        ///< tag data write command
DECLARE_TAG_DATA_RECORD_TYPE( systemString, tagDataCommand::DELETE_ALL, 'RT30' );   ///< tag data delete all command

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class systemString
 * @brief String object that may be printed on a label.
 */
class systemString : public dataObject< dataObjectType::SYSTEM_STRING, int >
{
    typedef systemString _Myt;
    typedef dataObject< dataObjectType::SYSTEM_STRING, int > _Mybase;

public:

    /**
     * List type.
     */
    typedef QList<_Myt> list_type;

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    systemString();

    /**
     * Constructor.
     *
     * @param[in] id  object id
     * @param[in] text  object text
     */
    systemString( int id, const QString& text );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    systemString( const _Myt& rhs );

    /**
     * Destructor
     */
    virtual ~systemString();

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Assignment operator.
     *
     * @param[in] rhs  value to assign from
     * @returns  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs );

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve the object key value.
     *
     * This key value is used for data store access.
     *
     * @returns  key value
     */
    virtual int objectKey() const {return id();}

    /**
     * Retrieve object id.
     *
     * @returns  object id
     */
    virtual int id() const {return id_;}

    /**
     * Set object id.
     *
     * @param[in] value  object id
     */
    virtual void setId( int value ) {id_ = value;}

    /**
     * Retrieve object text.
     *
     * @returns  object text
     */
    virtual QString text() const {return text_;}

    /**
     * Set object text.
     *
     * @param[in] value  object text
     */
    virtual void setText( const QString& value ) {text_ = value;}

    ///////////////////////////////////////////////////////////////////////////
    // Static Methods

    /**
     * Convert tag data into list of objects.
     *
     * @param[in] r  tag data
     * @returns  list of objects
     */
    static list_type toValueList( const tagDataList& r );

    /**
     * Convert list of objects into tag data.
     *
     * @param[in] values  list of objects
     * @returns  tag data
     */
    static tagDataRecordList toTagDataRecordList( const list_type& values );

private:

    int id_;
    QString text_;

    /**
     * Copy object.
     */
    void copy( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SYSTEMSTRING_H
