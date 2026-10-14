#ifndef MATRIX_H
#define MATRIX_H

struct vector3;

///////////////////////////////////////////////////////////////////////////////////////////////////
struct matrix3x3
{
    double x[3];
    double y[3];
    double z[3];

    // ---- //

    matrix3x3();
    matrix3x3( const matrix3x3& other );

    vector3 euler_angles() const;

    static matrix3x3 identity();
    static matrix3x3 rotation( const vector3& v0, const vector3& v1 );
    static matrix3x3 rotation( const double& x, const double& y, const double& z );

};

matrix3x3 operator+ ( const matrix3x3& lhs, const matrix3x3& rhs );
matrix3x3 operator- ( const matrix3x3& lhs, const matrix3x3& rhs );
matrix3x3 operator* ( const matrix3x3& lhs, double rhs );
matrix3x3 operator/ ( const matrix3x3& lhs, double rhs );

matrix3x3 operator* ( const matrix3x3& lhs, const matrix3x3& rhs );

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MATRIX_H
