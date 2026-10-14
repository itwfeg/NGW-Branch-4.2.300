#ifndef VECTOR_H
#define VECTOR_H

struct matrix3x3;

///////////////////////////////////////////////////////////////////////////////////////////////////
struct vector2
{
    double x;
    double y;

    // ---- //

    vector2();
    vector2( double x, double y );
    vector2( const vector2& v );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
struct vector3
{
    double x;
    double y;
    double z;

    // ---- //

    vector3();
    vector3( double x, double y, double z );
    vector3( const vector3& v );

    double length() const;

    double x_angle() const;
    double y_angle() const;
    double z_angle() const;

    void normalize();
    vector3 normalized() const;

    double dot_product( const vector3& v ) const;

    vector3 cross_product( const vector3& v ) const;

    double angle( const vector3& v ) const;

    matrix3x3 skew_symmetric() const;

};

vector3 operator+ ( const vector3& lhs, const vector3& rhs );
vector3 operator- ( const vector3& lhs, const vector3& rhs );
vector3 operator* ( const vector3& lhs, double rhs );
vector3 operator/ ( const vector3& lhs, double rhs );

vector3 operator* ( const vector3& lhs, const matrix3x3& rhs );
vector3 operator* ( const matrix3x3& lhs, const vector3& rhs );

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // VECTOR_H
