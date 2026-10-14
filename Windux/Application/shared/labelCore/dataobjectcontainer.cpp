/**
 * @file dataobjectcontainer.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "dataobjectcontainer.h"

#include "tagdatarecord.h"
#include "tagdatastream.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
dataObjectContainerBase::dataObjectContainerBase() :
    _Mybase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dataObjectContainerBase::~dataObjectContainerBase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dataObjectContainerBase::empty() const
{
    return (0 == size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dataObjectContainerBase::processTagDataRecord( const tagDataRecord& command, tagDataStream& os )
{
    Q_UNUSED( command );
    Q_UNUSED( os );

    // base function does nothing
    return false;
}
