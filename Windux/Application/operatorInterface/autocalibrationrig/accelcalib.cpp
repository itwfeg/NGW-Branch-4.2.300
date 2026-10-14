
#include "accelcalib.h"
#include "common.h"
#include "matrix.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <cmath>

// desired mounting
// x: back-front +/-
// y: right-left +/-
// z: up-down +/-

// ADXL312 accel physical mounting
// x: up-down +/-
// y: front-back +/-
// z: right-left +/-
static const vector3 r_adxl312( deg_to_rad_90, 0.0, -deg_to_rad_90 );

// ADXL355 accel physical mounting
// x: back-front +/-
// y: up-down +/-
// z: right-left +/-
static const vector3 r_adxl355( deg_to_rad_90, 0.0, 0.0 );

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 find_accel_rotation_quick( const std::vector<vector3>& c, const vector3 &r0 )
{
    vector3 r( r0 );

    // generate rotation matrix to change physical frame to desired frame
    // rotation is applied z, y, x order
    const double range( 6.0 * deg_to_rad_1 );
    const double i( range / pow( 2.0, 12.0 ) );

    std::map<double, vector3> rotations;

    // z rotation
    for ( double z( -range ); z <= range; z += i )
    {
        const vector3 rot( r.x + 0.0, r.y + 0.0, r.z + z );
        const matrix3x3 rot_frame( matrix3x3::rotation( rot.x, rot.y, rot.z ) );

        const vector3 none( c[TILT_NONE] * rot_frame );
        const vector3 front( c[TILT_FRONT] * rot_frame );
        const vector3 back( c[TILT_BACK] * rot_frame );
        const vector3 left( c[TILT_LEFT] * rot_frame );
        const vector3 right( c[TILT_RIGHT] * rot_frame );

        // compute error of expected versus actual
        double error( 0.0 );
        error += pow( none.y, 2.0 );
        error += pow( front.y, 2.0 );
        error += pow( back.y, 2.0 );
        error += pow( none.x, 2.0 );
        error += pow( left.x, 2.0 );
        error += pow( right.x, 2.0 );

        rotations[error] = rot;
    }

    // keep best rotation
    r = rotations.begin()->second;
    rotations.clear();

    // y rotation
    for ( double y( -range ); y <= range; y += i )
    {
        const vector3 rot( r.x + 0.0, r.y + y, r.z + 0.0 );
        const matrix3x3 rot_frame( matrix3x3::rotation( rot.x, rot.y, rot.z ) );

        const vector3 none( c[TILT_NONE] * rot_frame );
        const vector3 front( c[TILT_FRONT] * rot_frame );
        const vector3 back( c[TILT_BACK] * rot_frame );
        const vector3 left( c[TILT_LEFT] * rot_frame );
        const vector3 right( c[TILT_RIGHT] * rot_frame );

        // compute error of expected versus actual
        double error( 0.0 );
        error += pow( none.y, 2.0 );
        error += pow( front.y, 2.0 );
        error += pow( back.y, 2.0 );
        error += pow( none.x, 2.0 );
        error += pow( left.x, 2.0 );
        error += pow( right.x, 2.0 );

        rotations[error] = rot;
    }

    // keep best rotation
    r = rotations.begin()->second;
    rotations.clear();

    // x rotation
    for ( double x( -range ); x <= range; x += i )
    {
        const vector3 rot( r.x + x, r.y + 0.0, r.z + 0.0 );
        const matrix3x3 rot_frame( matrix3x3::rotation( rot.x, rot.y, rot.z ) );

        const vector3 none( c[TILT_NONE] * rot_frame );
        const vector3 front( c[TILT_FRONT] * rot_frame );
        const vector3 back( c[TILT_BACK] * rot_frame );
        const vector3 left( c[TILT_LEFT] * rot_frame );
        const vector3 right( c[TILT_RIGHT] * rot_frame );

        // compute error of expected versus actual
        double error( 0.0 );
        error += pow( none.y, 2.0 );
        error += pow( front.y, 2.0 );
        error += pow( back.y, 2.0 );
        error += pow( none.x, 2.0 );
        error += pow( left.x, 2.0 );
        error += pow( right.x, 2.0 );

        rotations[error] = rot;
    }

    // keep best rotation
    r = rotations.begin()->second;
    rotations.clear();

    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector3 find_accel_rotation( const std::vector<vector3>& c, const vector3 &r0 )
{
    vector3 r( r0 );

    // generate rotation matrix to change physical frame to desired frame
    // rotation is applied z, y, x order
    double range( 8.0 * deg_to_rad_1 );
    double i( 0.5 * deg_to_rad_1 );

    for ( size_t loops( 11 ); loops--; )
    {
        std::map<double, vector3> rotations;

        for ( double x( -range ); x <= range; x += i )
            for ( double y( -range ); y <= range; y += i )
                for ( double z( -range ); z <= range; z += i )
                {
                    const vector3 rot( r.x + x, r.y + y, r.z + z );
                    const matrix3x3 rot_frame( matrix3x3::rotation( rot.x, rot.y, rot.z ) );

                    const vector3 none( c[TILT_NONE] * rot_frame );
                    const vector3 front( c[TILT_FRONT] * rot_frame );
                    const vector3 back( c[TILT_BACK] * rot_frame );
                    const vector3 left( c[TILT_LEFT] * rot_frame );
                    const vector3 right( c[TILT_RIGHT] * rot_frame );

                    // compute error of expected versus actual
                    double error( 0.0 );
                    error += pow( none.y, 2.0 );
                    error += pow( front.y, 2.0 );
                    error += pow( back.y, 2.0 );
                    error += pow( none.x, 2.0 );
                    error += pow( left.x, 2.0 );
                    error += pow( right.x, 2.0 );

                    rotations[error] = rot;
                }

        // keep best rotation
        r = rotations.begin()->second;

        range /= 2.0;
        i /= 2.0;
    }

    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool calib_accel( const std::vector<vector3>& c, fit &f, std::ostream& os )
{
    std::list<tiltType> tiltList;
    tiltList.push_back( TILT_NONE );
    tiltList.push_back( TILT_FRONT );
    tiltList.push_back( TILT_BACK );
    tiltList.push_back( TILT_LEFT );
    tiltList.push_back( TILT_RIGHT );

    std::map<tiltType, std::string> tiltDescriptionList;
    tiltDescriptionList[TILT_NONE] = "None";
    tiltDescriptionList[TILT_FRONT] = "Front";
    tiltDescriptionList[TILT_BACK] = "Back";
    tiltDescriptionList[TILT_LEFT] = "Left";
    tiltDescriptionList[TILT_RIGHT] = "Right";

    std::vector<vector3> expected;
    expected.push_back( vector3( 0.0, 0.0, 0.0 ) );
    expected.push_back( vector3( -deg_to_rad_5, 0.0, -deg_to_rad_5 ) );     //  0.0872665 = -deg_to_rad_5 = (5.0 * 0.0174533)
    expected.push_back( vector3( deg_to_rad_5, 0.0, -deg_to_rad_5 ) );
    expected.push_back( vector3( 0.0, -deg_to_rad_5, -deg_to_rad_5 ) );
    expected.push_back( vector3( 0.0, deg_to_rad_5, -deg_to_rad_5 ) );

    static const vector3 expected_level( 0.0, 0.0, -1.0 );

    // ---- //

    // compute length
    std::vector<double> length;
    double avg_length( 0.0 );

    // TODO 3 DONE
    //for ( auto t : tiltList )
    for ( std::list<tiltType>::iterator t( tiltList.begin() ); t != tiltList.end(); t++ )
    {
        const double len( c[*t].length() );

        length.push_back( len );
        avg_length += len;
    }

    avg_length /= (double) tiltList.size();

    os << " " << std::setw( 12 ) << "Min Len";
    os << " " << std::setw( 12 ) << "Max Len";
    os << " " << std::setw( 12 ) << "Avg Len";
    os << std::endl;

    os << " " << std::setw( 12 ) << *std::min_element( length.begin(), length.end() );
    os << " " << std::setw( 12 ) << *std::max_element( length.begin(), length.end() );
    os << " " << std::setw( 12 ) << avg_length;
    os << std::endl;

    os << std::endl;

    // calculate gravity coefficient
    double g_coeff( f.g / avg_length );

    os << "Coeff Gravity: " << g_coeff << std::endl;
    os << std::endl;

    // compute rotation matrix
    const vector3 r( find_accel_rotation_quick( c, r_adxl355 ) );

    os << "X Rotation: " << r.x << std::endl;
    os << "Y Rotation: " << r.y << std::endl;
    os << "Z Rotation: " << r.z << std::endl;
    os << std::endl;

    // rotate all vectors
    const matrix3x3 rot( matrix3x3::rotation( r.x, r.y, r.z ) );

    std::vector<vector3> c_rot;

    // TODO 4 DONE
    //for ( auto t : tiltList )
    for ( std::list<tiltType>::iterator t( tiltList.begin() ); t != tiltList.end(); t++ )
        c_rot.push_back( c[*t] * rot );

    // calculate gain
    const double xgain( deg_to_rad_10 / (c_rot[TILT_BACK].x_angle() - c_rot[TILT_FRONT].x_angle()) );
    const double ygain( deg_to_rad_10 / (c_rot[TILT_RIGHT].y_angle() - c_rot[TILT_LEFT].y_angle()) );

    os << "X Gain: " << xgain << std::endl;
    os << "Y Gain: " << ygain << std::endl;
    os << std::endl;

    os << "const deg_to_rad_1: " << deg_to_rad_1 << std::endl;            // 0.0174533;
    os << "err = (0.1 * deg_to_rad_1): " << (deg_to_rad_1 * 0.15) << std::endl;      // 0.00174533;
    os << std::endl;

    // ---- //

    bool passed( true );

    os << " " << std::setw( 8 ) << "Tilt";
    os << " " << std::setw( 12 ) << "Meas x";
    os << " " << std::setw( 12 ) << "Meas y";
    os << " " << std::setw( 12 ) << "Meas z";
    os << " " << std::setw( 10 ) << "Meas ang";
    os << " " << std::setw( 12 ) << "Adj x";
    os << " " << std::setw( 12 ) << "Adj y";
    os << " " << std::setw( 12 ) << "Err x";
    os << " " << std::setw( 12 ) << "Err y";
    os << std::endl;


    std::string failhere = "";

    // TODO 5
    // for ( auto t : tiltList )
    for ( std::list<tiltType>::iterator t( tiltList.begin() ); t != tiltList.end(); t++ )
    {
        static const double err( 0.15 * deg_to_rad_1 );
        failhere = "";
        const vector3 v( c_rot[*t] );

        const double ax( v.x_angle() * xgain );
        const double ay( v.y_angle() * ygain );

        const double angle( v.angle( expected_level ) );

        bool passed_tilt( false );

        if (( expected[*t].x - err < ax ) && ( ax < expected[*t].x + err ))
        {
            if (( expected[*t].y - err < ay ) && ( ay < expected[*t].y + err ))
                passed_tilt = true;
            else
                failhere = " Y";
        }
        else
        {
            failhere = " X";
        }

        passed &= passed_tilt;

        os << " " << std::setw( 8 ) << tiltDescriptionList[*t];
        os << " " << std::setw( 12 ) << v.x_angle();
        os << " " << std::setw( 12 ) << v.y_angle();
        os << " " << std::setw( 12 ) << v.z_angle();
        os << " " << std::setw( 10 ) << angle / deg_to_rad_1;
        os << " " << std::setw( 12 ) << ax;
        os << " " << std::setw( 12 ) << ay;
        os << " " << std::setw( 12 ) << expected[*t].x - ax;
        os << " " << std::setw( 12 ) << expected[*t].y - ay;
        os << " " << (passed_tilt ? "pass" : "FAIL");
        os << " " << failhere;
        // ADDED
        os << " " << "exp.x="  << expected[*t].x << "   exp.x-err="  << expected[*t].x - err << "  is exp.x-err < ax=" << ((expected[*t].x - err) < ax) << "  exp.x+err=" << expected[*t].x + err <<  "  is ax < exp.x+err=" << ( ax < (expected[*t].x + err)) << "  exp.y="  << expected[*t].y << "   exp.y-err="  << expected[*t].y - err << "  exp.y+err=" << expected[*t].y + err;
        os << std::endl;
    }

    os << std::endl;

    // ---- //

    f.xrot[0] = rot.x[0];
    f.xrot[1] = rot.x[1];
    f.xrot[2] = rot.x[2];

    f.yrot[0] = rot.y[0];
    f.yrot[1] = rot.y[1];
    f.yrot[2] = rot.y[2];

    f.zrot[0] = rot.z[0];
    f.zrot[1] = rot.z[1];
    f.zrot[2] = rot.z[2];

    f.xgain = xgain;
    f.ygain = ygain;

    f.g_coeff = g_coeff;

    return passed;
}
