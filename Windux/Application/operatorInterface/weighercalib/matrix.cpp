
#include "matrix.h"
#include "vector.h"

#include <math.h>
#include <stddef.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3::matrix3x3()
{
    for ( size_t i( 0 ); i < 3; ++i )
        x[i] = y[i] = z[i] = 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3::matrix3x3( const matrix3x3& other )
{
    for ( size_t i( 0 ); i < 3; ++i )
    {
        x[i] = other.x[i];
        y[i] = other.y[i];
        z[i] = other.z[i];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 matrix3x3::euler_angles() const
{
    const double ax( atan2( z[1], z[2] ) );
    const double ay( atan2( -z[0], sqrt( pow( z[1], 2.0 ) + pow( z[2], 2.0 ) ) ) );
    const double az( atan2( y[0], x[0] ) );

    return vector3( ax, ay, az );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 matrix3x3::identity()
{
    matrix3x3 result;
    result.x[0] = result.y[1] = result.z[2] = 1.0;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 matrix3x3::rotation( const vector3& v0, const vector3& v1 )
{
    const vector3 a( v0.normalized() );
    const vector3 b( v1.normalized() );

    const vector3 axb( a.cross_product( b ) );

    const matrix3x3 v( axb.skew_symmetric() );

    const matrix3x3 v2( v * v );
    const double c( a.dot_product( b ) );

    const matrix3x3 result( matrix3x3::identity() + v + v2*(1.0 - c)/pow( axb.length(), 2 ) );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 matrix3x3::rotation( const double& x, const double& y, const double& z )
{
    matrix3x3 rot;

    rot.x[0] = cos( y ) * cos( z );
    rot.x[1] = cos( y ) * sin( z );
    rot.x[2] = -sin( y );

    rot.y[0] = sin( x ) * sin( y ) * cos( z ) - cos( x ) * sin( z );
    rot.y[1] = sin( x ) * sin( y ) * sin( z ) + cos( x ) * cos( z );
    rot.y[2] = sin( x ) * cos( y );

    rot.z[0] = cos( x ) * sin( y ) * cos( z ) + sin( x ) * sin( z );
    rot.z[1] = cos( x ) * sin( y ) * sin( z ) - sin( x ) * cos( z );
    rot.z[2] = cos( x ) * cos( y );

    return rot;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 operator+ ( const matrix3x3& lhs, const matrix3x3& rhs )
{
    matrix3x3 result;

    for ( size_t i( 0 ); i < 3; ++i )
    {
        result.x[i] = lhs.x[i] + rhs.x[i];
        result.y[i] = lhs.y[i] + rhs.y[i];
        result.z[i] = lhs.z[i] + rhs.z[i];
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 operator- ( const matrix3x3& lhs, const matrix3x3& rhs )
{
    matrix3x3 result;

    for ( size_t i( 0 ); i < 3; ++i )
    {
        result.x[i] = lhs.x[i] - rhs.x[i];
        result.y[i] = lhs.y[i] - rhs.y[i];
        result.z[i] = lhs.z[i] - rhs.z[i];
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 operator* ( const matrix3x3& lhs, double rhs )
{
    matrix3x3 result;

    for ( size_t i( 0 ); i < 3; ++i )
    {
        result.x[i] = lhs.x[i] * rhs;
        result.y[i] = lhs.y[i] * rhs;
        result.z[i] = lhs.z[i] * rhs;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 operator/ ( const matrix3x3& lhs, double rhs )
{
    matrix3x3 result;

    for ( size_t i( 0 ); i < 3; ++i )
    {
        result.x[i] = lhs.x[i] / rhs;
        result.y[i] = lhs.y[i] / rhs;
        result.z[i] = lhs.z[i] / rhs;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
matrix3x3 operator* ( const matrix3x3& lhs, const matrix3x3& rhs )
{
    matrix3x3 result;

    result.x[0] = lhs.x[0] * rhs.x[0] + lhs.x[1] * rhs.y[0] + lhs.x[2] * rhs.z[0];
    result.x[1] = lhs.x[0] * rhs.x[1] + lhs.x[1] * rhs.y[1] + lhs.x[2] * rhs.z[1];
    result.x[2] = lhs.x[0] * rhs.x[2] + lhs.x[1] * rhs.y[2] + lhs.x[2] * rhs.z[2];

    result.y[0] = lhs.y[0] * rhs.x[0] + lhs.y[1] * rhs.y[0] + lhs.y[2] * rhs.z[0];
    result.y[1] = lhs.y[0] * rhs.x[1] + lhs.y[1] * rhs.y[1] + lhs.y[2] * rhs.z[1];
    result.y[2] = lhs.y[0] * rhs.x[2] + lhs.y[1] * rhs.y[2] + lhs.y[2] * rhs.z[2];

    result.z[0] = lhs.z[0] * rhs.x[0] + lhs.z[1] * rhs.y[0] + lhs.z[2] * rhs.z[0];
    result.z[1] = lhs.z[0] * rhs.x[1] + lhs.z[1] * rhs.y[1] + lhs.z[2] * rhs.z[1];
    result.z[2] = lhs.z[0] * rhs.x[2] + lhs.z[1] * rhs.y[2] + lhs.z[2] * rhs.z[2];

    return result;
}
