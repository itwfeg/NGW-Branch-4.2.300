/**
 * @file processtagdatacontainer.h
 * Method for processing tag data commands on a data object container.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup labelViewerCsu
 * @{
 */

#ifndef PROCESSTAGDATACONTAINER_H
#define PROCESSTAGDATACONTAINER_H

#include "dataobjectcontainer.h"

#include "tagdatarecord.h"
#include "tagdatastream.h"

// forward declaration of data conversion methods
namespace _impl {
    template <class T, int RECORD_TYPE> struct convert;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// implementation of the processTagDataRecord() method for class dataObjectContainer
template <class T>
bool dataObjectContainer<T>::processTagDataRecord( const tagDataRecord& command, tagDataStream& os )
{
    static const tagDataRecordType READ_ONE = tagDataCommand::toRecordType<T, tagDataCommand::READ_ONE>::TYPE;
    static const tagDataRecordType READ_ALL = tagDataCommand::toRecordType<T, tagDataCommand::READ_ALL>::TYPE;
    static const tagDataRecordType WRITE = tagDataCommand::toRecordType<T, tagDataCommand::WRITE>::TYPE;
    static const tagDataRecordType DELETE_ONE = tagDataCommand::toRecordType<T, tagDataCommand::DELETE_ONE>::TYPE;
    static const tagDataRecordType DELETE_ALL = tagDataCommand::toRecordType<T, tagDataCommand::DELETE_ALL>::TYPE;

    // validate command
    if ( !command.isValid() )
        return false;

    // process read one command
    if ( READ_ONE == command.first )
    {
        // get list of keys to read
        key_list_type keys = _impl::convert<T, READ_ONE>::toKeyList( command.second );

        // read the data
        if ( keys.size() )
        {
            value_list_type values = readData( keys );

            // convert data to tag format
            if ( values.size() )
            {
                tagDataRecordList rl( _impl::convert<T, READ_ONE>::toTagDataRecordList( values ) );

                // send data
                for ( tagDataRecordList::const_iterator r = rl.begin(); r != rl.end(); ++r )
                    if (( r->isValid() ) && ( r->second.size() ))
                        os << (*r);
            }
        }
    }

    // process read all command
    else if ( READ_ALL == command.first )
    {
        // read the data
        value_list_type values = readAllData();

        // convert data to tag format
        if ( values.size() )
        {
            tagDataRecordList rl( _impl::convert<T, READ_ALL>::toTagDataRecordList( values ) );

            // send data
            for ( tagDataRecordList::const_iterator r = rl.begin(); r != rl.end(); ++r )
                if (( r->isValid() ) && ( r->second.size() ))
                    os << (*r);
        }
    }

    // process write command
    else if ( WRITE  == command.first )
    {
        // get list of values to write
        value_list_type values = _impl::convert<T, WRITE>::toValueList( command.second );

        // write them!
        if ( values.size() )
            writeData( values );
    }

    // process delete one command
    else if ( DELETE_ONE == command.first )
    {
        // get list of keys to delete
        key_list_type keys = _impl::convert<T, DELETE_ONE>::toKeyList( command.second );

        // delete them!
        if ( keys.size() )
            deleteData( keys );
    }

    // process delete all command
    else if ( DELETE_ALL == command.first )
    {
        // delete!!
        deleteAllData();
    }

    // unhandled command
    else
    {
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @namespace _impl
 * @brief Non-public implementation code.
 */
namespace _impl
{
    /**
     * @struct convert
     * @brief This structure handles converting tag data to object keys or object values.
     *
     * The default implementation will call upon the data class object to perform the conversion. If
     * the command is not implemented the specialized structure below will be called instead and empty
     * lists will be returned.
     */
    template <class T, int RT>
    struct convert
    {
        typedef typename dataObjectTraits<T>::key_list_type key_list_type;           ///< Key list type.
        typedef typename dataObjectTraits<T>::value_list_type value_list_type;       ///< Value list type.

        /**
         * Convert tag data into key list.
         */
        static key_list_type toKeyList( const tagDataList& r ) {
            return T::toKeyList( r );
        }

        /**
         * Convert tag data into value list.
         */
        static value_list_type toValueList( const tagDataList& r ) {
            return T::toValueList( r );
        }

        /**
         * Convert value list into tag data.
         */
        static tagDataRecordList toTagDataRecordList( const value_list_type& values ) {
            return T::toTagDataRecordList( values );
        }

    };

    /**
     * @brief This is a specialization to handle data objects that have not implemented tag data.
     */
    template <class T>
    struct convert<T, 0>
    {
        typedef typename dataObjectTraits<T>::key_list_type key_list_type;           ///< Key list type.
        typedef typename dataObjectTraits<T>::value_list_type value_list_type;       ///< Value list type.

        /**
         * Convert tag data into key list.
         */
        static key_list_type toKeyList( const tagDataList& r ) {
            Q_UNUSED( r );
            return key_list_type();
        }

        /**
         * Convert tag data into value list.
         */
        static value_list_type toValueList( const tagDataList& r ) {
            Q_UNUSED( r );
            return value_list_type();
        }

        /**
         * Convert value list into tag data.
         */
        static tagDataRecordList toTagDataRecordList( const value_list_type& values ) {
            Q_UNUSED( values );
            return tagDataRecordList();
        }

    };

} // namespace _impl

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // TAGDATACONTAINERPROCESS_H
