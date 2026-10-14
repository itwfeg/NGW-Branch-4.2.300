#include "HobartMath.h"

double hobround( double value )
{
	int i = ( int ) ( value * 10 );
	if ( i % 10 >= 5 )
		i += 10;
	return ( i / 10 );
}
