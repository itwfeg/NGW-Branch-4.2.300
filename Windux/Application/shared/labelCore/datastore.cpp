/**
 * @file datastore.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "datastore.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
dataStoreBase::dataStoreBase() :
    containers_( new containerPtr[dataObjectType::_MAX_TYPES] )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dataStoreBase::~dataStoreBase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dataStoreBase::empty() const
{
    // find a container that is not empty
    for ( int i = 0; i < dataObjectType::_MAX_TYPES; ++i )
        if ( !containers_[i]->empty() )
            return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dataStoreBase::clear()
{
    LOG_CLASS_INFO( "dataStoreBase", "clear" );

    LOG_INFO( "clearing all items from the store...", NULL );

    // clear all items
    for ( int i = 0; i < dataObjectType::_MAX_TYPES; ++i )
        containers_[i]->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dataStoreBase::processTagDataRecord( const tagDataRecord& command, tagDataStream& os )
{
    LOG_CLASS_INFO( "dataStoreBase", "processTagDataRecord" );

    // header
    if ( command.isHeaderRecordType() )
        LOG_DEBUG( "got header record", NULL );

    // error
    else if ( command.isErrorRecordType() )
        LOG_WARN( "got error record 0x%08x", command.first );

    // footer
    else if ( command.isFooterRecordType() )
    {
        LOG_DEBUG( "got footer record", NULL );
        Q_EMIT complete();
    }

    // data
    else
    {
        // find a container that will process this command
        for ( int i = 0; i < dataObjectType::_MAX_TYPES; ++i )
            if ( containers_[i]->processTagDataRecord( command, os ) )
            {
                LOG_DEBUG( "got data record 0x%08x for container %d", command.first, i );

                Q_EMIT processedRecord( (dataObjectType::type) i );
                return true;
            }
    }

    LOG_WARN( "could not find container that would accept the tag command 0x%08x", command.first );
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dataStoreBase::containerPtrArray& dataStoreBase::containers()
{
    return containers_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dataStoreBase::containerPtr& dataStoreBase::container( int i )
{
    return containers_[i];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataStream& operator >> ( tagDataStream& lhs, dataStoreBase& rhs )
{
    while ( !lhs.atEnd() )
    {
        tagDataRecord command;
        lhs >> command;

        if ( !command.isValid() )
            break;

        rhs.processTagDataRecord( command, lhs );
    }

    return lhs;
}
