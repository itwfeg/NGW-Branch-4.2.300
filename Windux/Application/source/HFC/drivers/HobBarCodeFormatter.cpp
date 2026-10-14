
#include "HobBarCodeFormatter.h"

#include "HobDateTime.h"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////////////////
HobBarCodeFormatter::HobBarCodeFormatter( const formatMapping &mapping ) :
	mapping_( mapping )
{
	static const char *n( "dEKkLMNOoPpQRSTUVWwXYZz" );

	for ( size_t i( 0 ); n[i]; ++i )
		numericValues_.push_back( n[i] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HobBarCodeFormatter::~HobBarCodeFormatter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string HobBarCodeFormatter::value( char type, bool &isNumeric ) const
{
	std::string v;

	// default to string
	isNumeric = false;

	// current date time does not pull from mapping
	if ( 'D' == type )
	{
		HobDateTime time;
		v += time.formatDate( FormatDate_YYMMDD_NoDelimiter );
		v += time.formatTime( FormatTime_HHMMSS_NoDelimiter, HourFormat_MILITARY );
	}
	else
	{
		// locate value in mapping
		formatMapping::const_iterator i( mapping_.find( type ) );

		if ( i != mapping_.end() )
			v = i->second;

		// check if numeric
		isNumeric = (numericValues_.end() != std::find( numericValues_.begin(), numericValues_.end(), type ));

		if ( isNumeric )
		{
			// empty numeric values are zero
			if ( v.empty() )
				v = "0";
			else
			{
				// trim leading zeros
				while (( 1 < v.length() ) && ( '0' == v.front() ))
					v.erase( 0, 1 );
			}
		}
    }

	return v;
};
