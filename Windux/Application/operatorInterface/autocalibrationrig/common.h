#ifndef COMMON_H
#define COMMON_H

#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

// VC++11 format
// static constexpr double deg_to_rad_1( 1.0 * 0.0174533 );
// static constexpr double deg_to_rad_5( 5.0 * 0.0174533 );            // test fixture tilt
// static constexpr double deg_to_rad_10( 10.0 * 0.0174533 );
// static constexpr double deg_to_rad_90( 90.0 * 0.0174533 );
// static constexpr double deg_to_rad_180( 180.0 * 0.0174533 );

// old school format
static const double deg_to_rad_1   = 0.0174533;
static const double deg_to_rad_5   = 0.0872665;            // test fixture tilt
static const double deg_to_rad_10  = 0.174533;
static const double deg_to_rad_90  = 1.570797;
static const double deg_to_rad_180 = 3.141594;

enum appliedLoad
{
    LOAD_0KG = 0,       // or 0 lb
    LOAD_5KG = 5,       // or 10 lb
    LOAD_10KG = 10,     // or 20 lb
    LOAD_15KG = 15,     // or 30 lb
};

enum tiltType
{
    TILT_NONE = 0,
    TILT_FRONT = 1,     // x
    TILT_BACK = 2,      // x
    TILT_LEFT = 3,      // y
    TILT_RIGHT = 4,     // y
};

enum accAxis
{
    X_AXIS = 0,
    Y_AXIS = 5,
    Z_AXIS = 10,
};

// VC++11 format
// static constexpr double RESOLUTION_100( 100.0 );
// static constexpr double RESOLUTION_100K( 100000.0 );
// static constexpr double RESOLUTION_1M( 1000000.0 );
// static constexpr double RESOLUTION_10M( 10000000.0 );
// static constexpr double RESOLUTION_1B( 1000000000.0 );
// static constexpr double RESOLUTION_10B( 10000000000.0 );

// old school format
static const double RESOLUTION_100  = 100.0;
static const double RESOLUTION_100K = 100000.0;
static const double RESOLUTION_1M   = 1000000.0;
static const double RESOLUTION_10M  = 10000000.0;
static const double RESOLUTION_1B   = 1000000000.0;
static const double RESOLUTION_10B  = 10000000000.0;

std::string format_double( double val, const std::string& fmt );

double acc_value( accAxis a, tiltType t, const std::vector<double>& rawValues );
double counts( appliedLoad a, tiltType t, const std::vector<double>& rawCounts );

double net_load( appliedLoad a, bool use_metric );
double error( appliedLoad a, bool use_metric );

double tilt_x( tiltType t );
double tilt_y( tiltType t );

double calc_average( const std::list<double>& values );
double calc_angle( double x, double y );
double calc_angle_effect_factor( double Sx, double x, double Sy, double y );
double calc_weight( double c, double Z0, double m, double A, double g, double m1, double D );

float roundf(float x);

#endif // COMMON_H
