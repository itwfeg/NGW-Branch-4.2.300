
#include "matrix.h"
#include "vector.h"

#include <math.h>

// ------------------------------------------------------------------------------------------------
//
// vector2 (two dimensional vector)
//
// ------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////
vector2::vector2()
{
    x = y = 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector2::vector2( double vx, double vy )
{
    x = vx;
    y = vy;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector2::vector2( const vector2& other )
{
    x = other.x;
    y = other.y;
}

// ------------------------------------------------------------------------------------------------
//
// vector3 (three dimensional vector)
//
// ------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3::vector3()
{
    x = y = z = 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3::vector3( double vx, double vy, double vz )
{
    x = vx;
    y = vy;
    z = vz;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3::vector3( const vector3& other )
{
    x = other.x;
    y = other.y;
    z = other.z;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double vector3::length() const
{
    return sqrt( pow( x, 2.0 ) + pow( y, 2.0 ) + pow( z, 2.0 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double vector3::x_angle() const
{
    return atan( x / sqrt( pow( y, 2.0 ) + pow( z, 2.0 ) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double vector3::y_angle() const
{
    return atan( y / sqrt( pow( x, 2.0 ) + pow( z, 2.0 ) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double vector3::z_angle() const
{
    return atan( sqrt( pow( x, 2.0 ) + pow( y, 2.0 ) ) / z );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void vector3::normalize()
{
    const double len( length() );

    x /= len;
    y /= len;
    z /= len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 vector3::normalized() const
{
    vector3 result( *this );
    result.normalize();

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double vector3::dot_product( const vector3& v ) const
{
    double result( 0.0 );
    result += x * v.x;
    result += y * v.y;
    result += z * v.z;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 vector3::cross_product( const vector3& v ) const
{
    vector3 result;
    result.x = y * v.z - z * v.y;
    result.y = z * v.x - x * v.z;
    result.z = x * v.y - y * v.x;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double vector3::angle( const vector3& v ) const
{
    return acos( dot_product( v ) / ( length() * v.length() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 vector3::skew_symmetric() const
{
    matrix3x3 result;

    result.x[0] = 0.0;
    result.y[0] = z;
    result.z[0] = -y;

    result.x[1] = -z;
    result.y[1] = 0.0;
    result.z[1] = x;

    result.x[2] = y;
    result.y[2] = -x;
    result.z[2] = 0.0;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 operator+ ( const vector3& lhs, const vector3& rhs )
{
    return vector3( lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 operator- ( const vector3& lhs, const vector3& rhs )
{
    return vector3( lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 operator* ( const vector3& lhs, double rhs )
{
    vector3 result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    result.z = lhs.z * rhs;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 operator/ ( const vector3& lhs, double rhs )
{
    vector3 result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    result.z = lhs.z / rhs;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 operator* ( const vector3& lhs, const matrix3x3& rhs )
{
    vector3 result;
    result.x = lhs.x * rhs.x[0] + lhs.y * rhs.x[1] + lhs.z * rhs.x[2];
    result.y = lhs.x * rhs.y[0] + lhs.y * rhs.y[1] + lhs.z * rhs.y[2];
    result.z = lhs.x * rhs.z[0] + lhs.y * rhs.z[1] + lhs.z * rhs.z[2];

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 operator* ( const matrix3x3& lhs, const vector3& rhs )
{
    return (rhs * lhs);
}
