#include "common.h"

#include <cmath>
#include <math.h>
//#include <cstdint>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string format_double( double val, const std::string& fmt )
{
    char buffer[1024];
    sprintf( buffer, fmt.c_str(), val );

    return std::string( buffer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double acc_value( accAxis a, tiltType t, const std::vector<double>& rawValues )
{
    return rawValues[a+t];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double counts( appliedLoad a, tiltType t, const std::vector<double>& rawCounts )
{
    return rawCounts[a+t];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double net_load( appliedLoad a, bool /*use_metric*/ )
{
    // kg and lb range is [0-300000]
    // kg will have a slightly smaller scale coefficient due to this (the "m" value)
    // when the scale switches from kg to lb it will convert the coeffiecent as per selection
    switch ( a )
    {
    case LOAD_5KG:
        return 100000;
    case LOAD_10KG:
        return 200000;
    case LOAD_15KG:
        return 300000;
    default:
        break;
    }

    return 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double error( appliedLoad a, bool use_metric )
{
    double e( use_metric ? 20 : 25 ); // half of 0.002 kg or 0.005 lb

    // less precise when above 6 kg or 12 lb
    if ( 6 < a )
       e = 50; // half of 0.005 kg or 0.01 lb

    // per NTEP guidelines for Dual Range Class III Scales No Level Bubble
    //  6 x 0.002 kg  -or-  12 x 0.005 lb
    // 15 x 0.005 kg  -or-  30 x 0.01 lb
    //
    // Allowed Tolerances:
    //  0- 2 kg  -or-   0- 4 lb  :  +/- 0.5 divisions
    //  2- 4 kg  -or-   4- 8 lb  :  +/- 1.0 divisions
    //  4- 6 kg  -or-   8-12 lb  :  +/- 1.5 divisions
    //  6-10 kg  -or-  12-20 lb  :  +/- 1.0 divisions
    // 10-15 kg  -or-  20-30 lb  :  +/- 1.5 divisions

    if (( 2 < a ) && ( a <= 4 ))
        e *= 2.0;
    else if (( 4 < a ) && ( a <= 6 ))
        e *= 3.0;
    else if (( 6 < a ) && ( a <= 10 ))
        e *= 2.0;
    else if ( 10 < a )
        e *= 3.0;

    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double tilt_x( tiltType t )
{
    switch ( t )
    {
    case TILT_FRONT:
        return -1.0 * deg_to_rad_5;
    case TILT_BACK:
        return deg_to_rad_5;
    default:
        break;
    }

    return 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double tilt_y( tiltType t )
{
    switch ( t )
    {
    case TILT_LEFT:
        return -1.0 * deg_to_rad_5;
    case TILT_RIGHT:
        return deg_to_rad_5;
    default:
        break;
    }

    return 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double calc_average( const std::list<double>& values )
{
    double sum( 0.0 );

    // VC++11.0
    //for ( auto val : values )
    //    sum += val;

    // old school loop
    for ( std::size_t i = 0; i < values.size(); ++i )
    {
        sum += i++;
    }

    return sum / (double) values.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double calc_angle( double x, double y )
{
    return 1.0 / sqrt( 1.0 + pow(tan(x), 2) + pow(tan(y), 2) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double calc_angle_effect_factor( double Sx, double x, double Sy, double y )
{
    return calc_angle( x, y ) + Sx*sin( x ) + Sy*sin( y );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double calc_weight( double c, double Z0, double m, double A, double g, double m1, double D )
{
    const double c0( c - Z0 );

    return (c0*m)/(A*g) + m1*pow(c0, 2) - D;
}

float roundf(float x)
{
   return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
