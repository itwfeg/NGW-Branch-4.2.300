/**
 * @file labelformflags.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labelformflags.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
labelFormFlags::type labelFormFlags::fromString( const QString& flags )
{
    type result = (type) 0;

    checkAppendFlag<TEXT_ENTITY>( flags, result );
    checkAppendFlag<BARCODE_ENTITY>( flags, result );
    checkAppendFlag<GRAPHIC_ENTITY>( flags, result );
    checkAppendFlag<LINE_ENTITY>( flags, result );
    checkAppendFlag<PRE_IMAGE>( flags, result );
    checkAppendFlag<POST_IMAGE>( flags, result );
    checkAppendFlag<FIXED>( flags, result );
    checkAppendFlag<FLOATING>( flags, result );
    checkAppendFlag<STRETCH>( flags, result );
    checkAppendFlag<VARIABLE>( flags, result );
    checkAppendFlag<RECT_BORDER>( flags, result );
    checkAppendFlag<INVERTED>( flags, result );
    checkAppendFlag<DIGIMARC_KEEP_OUT>( flags, result );
    checkAppendFlag<CODE_CHECKER_DISABLED>( flags, result );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labelFormFlags::toString( type flags )
{
    QString result;
    QTextStream os( &result );

    checkAppendFlag<TEXT_ENTITY>( flags, os );
    checkAppendFlag<BARCODE_ENTITY>( flags, os );
    checkAppendFlag<GRAPHIC_ENTITY>( flags, os );
    checkAppendFlag<LINE_ENTITY>( flags, os );
    checkAppendFlag<PRE_IMAGE>( flags, os );
    checkAppendFlag<POST_IMAGE>( flags, os );
    checkAppendFlag<FIXED>( flags, os );
    checkAppendFlag<FLOATING>( flags, os );
    checkAppendFlag<STRETCH>( flags, os );
    checkAppendFlag<VARIABLE>( flags, os );
    checkAppendFlag<RECT_BORDER>( flags, os );
    checkAppendFlag<INVERTED>( flags, os );
    checkAppendFlag<DIGIMARC_KEEP_OUT>( flags, os );
    checkAppendFlag<CODE_CHECKER_DISABLED>( flags, os );

    // remove trailing seperator
    if ( result.length() )
        result.chop( 1 );

    return result;
}
