/**
 * @file nutrifact.h
 * Nutrifact data object.
 *
 * Nutrifact data is composed of one or more columns.  This object stores a map of all columns
 * by column number.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef NUTRIFACT_H
#define NUTRIFACT_H

#include "nutrifactcolumn.h"

#include "dataobject.h"
#include "tagdatarecord.h"

DECLARE_DATA_OBJECT( nutrifact, dataObjectType::NUTRIFACT );                        ///< object type
DECLARE_TAG_DATA_RECORD_TYPE( nutrifact, tagDataCommand::READ_ALL, 'RT60' );        ///< tag data read all command
DECLARE_TAG_DATA_RECORD_TYPE( nutrifact, tagDataCommand::WRITE, 'RT94' );           ///< tag data write command
DECLARE_TAG_DATA_RECORD_TYPE( nutrifact, tagDataCommand::DELETE_ONE, 'RT85' );      ///< tag data delete one command
DECLARE_TAG_DATA_RECORD_TYPE( nutrifact, tagDataCommand::DELETE_ALL, 'RT38' );      ///< tag data delete all command

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class nutrifact
 * @brief Nutrifact data object.
 */
class nutrifact : public dataObject< dataObjectType::NUTRIFACT, int >
{
    typedef nutrifact _Myt;
    typedef dataObject< dataObjectType::NUTRIFACT, int > _Mybase;

public:

    /**
     * Map of @c nutrifactColumn objects by id.
     */
    typedef QMap<int, nutrifactColumn> nutrifactColumnMap;

    /**
     * Key list type.
     */
    typedef QList<int> key_list_type;

    /**
     * List type.
     */
    typedef QList<_Myt> list_type;

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     */
    nutrifact();

    /**
     * Constructor.
     *
     * @param[in] id  nutrifacts id
     */
    nutrifact( int id );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    nutrifact( const _Myt& rhs );

    /**
     * Destructor
     */
    virtual ~nutrifact();

    ///////////////////////////////////////////////////////////////////////////////////////////////
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
    ///////////////////////////////////////////////////////////////////////////

    /**
     * Retrieve the object key value.
     *
     * This key value is used for data store access.
     *
     * @returns  key value
     */
    virtual int objectKey() const {return id();}

    /**
     * Retrieve nutrifacts id.
     *
     * @returns  nutrifacts id
     */
    virtual int id() const {return id_;}

    /**
     * Set nutrifacts id.
     *
     * @param[in] value  nutrifacts id
     */
    virtual void setId( int value ) {id_ = value;}

    /**
     * Retrieve nutrifact columns.
     *
     * @returns  constant reference to column map
     */
    virtual const nutrifactColumnMap columns() const {return columns_;}

    /**
     * Retrieve nutrifact columns.
     *
     * @returns  reference to column map
     */
    virtual nutrifactColumnMap& columns() {return columns_;}

    /**
     * Set nutrifact columns.
     *
     * @param[in] value  nutrifact column map
     */
    virtual void setColumns( const nutrifactColumnMap& value ) {columns_ = value;}

    /**
     * Retrieve nutrifact column.
     *
     * @param[in] id  column id to retrieve
     * @returns  constant reference to column
     */
    virtual const nutrifactColumn column( int id ) const {return columns_[ id ];}

    /**
     * Retrieve nutrifact column.
     *
     * @param[in] id  column id to retrieve
     * @returns  reference to column
     */
    virtual nutrifactColumn& column( int id ) {return columns_[ id ];}

    /**
     * Set nutrifact column.
     *
     * @param[in] id  column id to set
     * @param[in] value  nutrifact column
     */
    virtual void setColumn( int id, const nutrifactColumn& value ) {columns_[ id ] = value;}

    ///////////////////////////////////////////////////////////////////////////
    // Static Methods

    /**
     * Convert tag data into a list of object key values.
     *
     * @param[in] r  tag data
     * @returns  list of object key values
     */
    static key_list_type toKeyList( const tagDataList& r );

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
    nutrifactColumnMap columns_;

    /**
     * Copy object.
     */
    void copy( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // NUTRIFACT_H

