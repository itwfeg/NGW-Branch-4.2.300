
#include "accelcalib.h"
#include "common.h"
#include "generate.h"
#include "matrix.h"
#include "weighercalib.h"

#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <cmath>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
bool generate_fit( const std::vector<double>& raw_counts, bool use_metric, double grav_const, const std::vector<double>& acc_values, fit_flash_section &f1, std::string& str )
{
    bool result( false );

    std::stringbuf os_buffer;
    std::ostream os( &os_buffer );

    std::list<appliedLoad> loadList;
    loadList.push_back( LOAD_0KG );
    loadList.push_back( LOAD_5KG );
    loadList.push_back( LOAD_10KG );
    loadList.push_back( LOAD_15KG );

    std::list<tiltType> tiltList;
    tiltList.push_back( TILT_NONE );
    tiltList.push_back( TILT_FRONT );
    tiltList.push_back( TILT_BACK );
    tiltList.push_back( TILT_LEFT );
    tiltList.push_back( TILT_RIGHT );

    std::map<appliedLoad, std::string> loadDescriptionList;
    loadDescriptionList[LOAD_0KG] = (use_metric ? "0 kg" : "0 lb");
    loadDescriptionList[LOAD_5KG] = (use_metric ? "5 kg" : "10 lb");
    loadDescriptionList[LOAD_10KG] = (use_metric ? "10 kg" : "20 lb");
    loadDescriptionList[LOAD_15KG] = (use_metric ? "15 kg" : "30 lb");

    std::map<tiltType, std::string> tiltDescriptionList;
    tiltDescriptionList[TILT_NONE] = "None";
    tiltDescriptionList[TILT_FRONT] = "Front";
    tiltDescriptionList[TILT_BACK] = "Back";
    tiltDescriptionList[TILT_LEFT] = "Left";
    tiltDescriptionList[TILT_RIGHT] = "Right";

    // convert accel values to vectors
    std::vector<vector3> acc_vectors;

    // TODO 2 DONE
    // VC++11
    //for ( auto t : tiltList )
    // old school
    for ( std::list<tiltType>::iterator t( tiltList.begin() ); t != tiltList.end(); t++ )
    {
        vector3 v( acc_value( X_AXIS, *t, acc_values ), acc_value( Y_AXIS, *t, acc_values ), acc_value( Z_AXIS, *t, acc_values ) );

        acc_vectors.push_back( v );
    }

    // Gravity Constants
    // https://www.ngs.noaa.gov/cgi-bin/grav_pdx.prl
    //
    // Troy, OH
    // Latitude: 40.031641
    // Longitude: -84.214363
    // Predicted gravity:  980104 +/-  2  milligals
    //
    // Hillsboro, OH
    // Latitude: 39.233218
    // Longitude: -83.614655
    // Predicted gravity:  980023 +/-  4  milligals

    // we cant be using the gravity constant from Hillsboro or Troy, can we? Get it from the accelerometer
    //const double g( 9.80104 );
    double g = grav_const;

    // compute fitting and validate
    os << "---------- FITTING ----------" << std::endl;
    os << std::endl;

    bool passed_fitting( true );

    fit f0;
    f0.g = g;

    passed_fitting &= calib_weigher( raw_counts, use_metric, f0, os );
    passed_fitting &= calib_accel( acc_vectors, f0, os );

    f1.from_fit( f0 );

    fit f;
    f1.to_fit( f );

    // perform validation
    os << "---------- VALIDATION ----------" << std::endl;
    os << std::endl;

    int32_t tested( 0 );
    int32_t passed( 0 );

    // compute zero and span
    const int32_t zerovalue( counts( LOAD_0KG, TILT_NONE, raw_counts ) );
    const int32_t spanvalue( counts( LOAD_15KG, TILT_NONE, raw_counts ) - zerovalue );

    // compute scaling factor
    const double scalingFactor( net_load( LOAD_15KG, use_metric ) / spanvalue );

    // compute gravity conversion
    const double g_conv( scalingFactor / f.m );

    const int32_t DW( roundd( f.D * g_conv ) );
    const int32_t Eoff( zerovalue - DW );

    // compute accelerometer rotation
    matrix3x3 rot;
    rot.x[0] = f.xrot[0];
    rot.x[1] = f.xrot[1];
    rot.x[2] = f.xrot[2];
    rot.y[0] = f.yrot[0];
    rot.y[1] = f.yrot[1];
    rot.y[2] = f.yrot[2];
    rot.z[0] = f.zrot[0];
    rot.z[1] = f.zrot[1];
    rot.z[2] = f.zrot[2];


    os << " " << std::setw( 6 ) << "Weight";
    os << " " << std::setw( 8 ) << "Tilt";
    os << " " << std::setw( 8 ) << "Counts";
    os << " " << std::setw( 12 ) << "x";
    os << " " << std::setw( 12 ) << "y";
    os << " " << std::setw( 12 ) << "g";
    os << " " << std::setw( 8 ) << "Wc";
    os << " " << std::setw( 8 ) << "Wb";
    os << " " << std::setw( 8 ) << "Sx";
    os << " " << std::setw( 8 ) << "Sy";
    os << " " << std::setw( 8 ) << "Wb Adj";
    os << " " << std::setw( 8 ) << "Error";
    os << " " << std::setw( 8 ) << "Counts";
    os << " " << std::setw( 8 ) << "Error";
    os << std::endl;

    // TODO 6 DONE
    // VC11
    //for ( auto a : loadList )
    //    for ( auto t : tiltList )
    // old school
    for ( std::list<appliedLoad>::iterator a( loadList.begin() ); a != loadList.end(); a++ )
    {
        for ( std::list<tiltType>::iterator t( tiltList.begin() ); t != tiltList.end(); t++ )
        {
            const int32_t c( counts( *a, *t, raw_counts ) );
            const int32_t c_off( c - Eoff );

            // no counts, skip data point
            if (( -1 < c ) && ( c < 1 ))
                continue;

            vector3 tilt( acc_vectors[*t] );
            tilt = tilt * rot;

            const double xtilt( f.xgain * atan( tilt.x / sqrt( pow( tilt.y, 2.0 ) + pow( tilt.z, 2.0 ) ) ) );
            const double ytilt( f.ygain * atan( tilt.y / sqrt( pow( tilt.x, 2.0 ) + pow( tilt.z, 2.0 ) ) ) );

            //const double xtilt( TILT_FRONT == t ? -tilt_rad : (TILT_BACK == t ? tilt_rad : 0.0 ) );
            //const double ytilt( TILT_LEFT == t ? -tilt_rad : (TILT_RIGHT == t ? tilt_rad : 0.0 ) );

            const double angle( calc_angle( xtilt, ytilt ) );

            // measured load
            const double Wb( ((double) c_off) / angle );

            const double Sx_load( ((xtilt < 0.0) ? -1.0 : 1.0) * (Wb/g_conv) );
            //const double Sx_error( (f.x2/RESOLUTION_10M)*pow(Sx_load,2.0) + (f.x1/RESOLUTION_100K)*Sx_load ); // avery
            const double Sx_error( (f.x2/RESOLUTION_10M)*pow(Sx_load,2.0) + (f.x1/RESOLUTION_100K)*Sx_load + f.x0 );
            const double Sx( sin( xtilt ) * Sx_error );

            const double Sy_load( ((ytilt < 0.0) ? -1.0 : 1.0) * (Wb/g_conv) );
            //const double Sy_error( (f.y1/RESOLUTION_100K)*Sy_load ); // avery
            const double Sy_error( (f.y2/RESOLUTION_10M) * pow(Sy_load,2.0) + (f.y1/RESOLUTION_100K)*Sy_load + f.y0 );
            const double Sy( std::abs( sin( ytilt ) ) * Sy_error );

            const double Wb_adj( Wb + Sx + Sy );

            // expected load
            // (applied load + dead load) * cos( angle )
            const double Wc( net_load( *a, use_metric )/scalingFactor + DW );

            // adjusted counts
            const int32_t c_adj( (int32_t) ((Wb_adj - DW) * scalingFactor) );

            os << " " << std::setw( 6 ) << loadDescriptionList[*a];
            os << " " << std::setw( 8 ) << tiltDescriptionList[*t];
            os << " " << std::setw( 8 ) << c;
            os << " " << std::setw( 12 ) << xtilt;
            os << " " << std::setw( 12 ) << ytilt;
            os << " " << std::setw( 12 ) << f.g_coeff * tilt.length();
            os << " " << std::setw( 8 ) << (int32_t) Wc;
            os << " " << std::setw( 8 ) << (int32_t) Wb;
            os << " " << std::setw( 8 ) << (int32_t) Sx;
            os << " " << std::setw( 8 ) << (int32_t) Sy;
            os << " " << std::setw( 8 ) << (int32_t) Wb_adj;
            os << " " << std::setw( 8 ) << (int32_t) (Wc - Wb_adj);
            os << " " << std::setw( 8 ) << c_adj;
            os << " " << std::setw( 8 ) << (int32_t) ((Wc - Wb_adj) * scalingFactor);

            // calculate error
            const double e( error( *a, use_metric ) );
            double myload(net_load(*a,use_metric));
            double myabs( std::abs( net_load( *a, use_metric ) - c_adj));

            //loadList.push_back( LOAD_0KG );  = TILT_NONE,TILT_FRONT,TILT_BACK,TILT_LEFT,TILT_RIGHT
            //loadList.push_back( LOAD_5KG );  = TILT_NONE,TILT_FRONT,TILT_BACK,TILT_LEFT,TILT_RIGHT
            //loadList.push_back( LOAD_10KG ); = TILT_NONE,TILT_FRONT,TILT_BACK,TILT_LEFT,TILT_RIGHT
            //loadList.push_back( LOAD_15KG ); = TILT_NONE,TILT_FRONT,TILT_BACK,TILT_LEFT,TILT_RIGHT

            std::cout << "a loadlist: " << *a << std::endl;
            std::cout << "use_metric: " << use_metric << std::endl;
            std::cout << "net_load(a,metric): " << myload << std::endl;
            std::cout << "c_adj: " << c_adj << std::endl;
            std::cout << "abs: " << myabs << std::endl;
            std::cout << "e: " << e << std::endl;

            if ( std::abs(net_load( *a, use_metric ) - c_adj) < e )
            {
                os << " pass";
                ++passed;
            }
            else
            {
                os << " FAIL";
            }

            ++tested;
            os << std::endl;
        }
    }
    os << std::endl;

    // ---- //

    if (( !passed_fitting ) || ( passed != tested ))
    {
        os << "!!! CALIBRATION FAILED" << std::endl;
        os << std::endl;

        result = false;
    }
    else
    {
        os << "!!! CALIBRATION PASSED" << std::endl;
        os << std::endl;
        os << "fitParamFlash.g = " << f1.g << ";" << std::endl;
        os << "fitParamFlash.m = " << f1.m << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.zerovalue = " << f1.zerovalue << ";" << std::endl;
        os << "fitParamFlash.spanvalue = " << f1.spanvalue << ";" << std::endl;
        os << "fitParamFlash.Z0 = " << f1.Z0 << ";" << std::endl;
        os << "fitParamFlash.D = " << f1.D << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.x2 = " << f1.x2 << ";" << std::endl;
        os << "fitParamFlash.x1 = " << f1.x1 << ";" << std::endl;
        os << "fitParamFlash.x0 = " << f1.x0 << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.y2 = " << f1.y2 << ";" << std::endl;
        os << "fitParamFlash.y1 = " << f1.y1 << ";" << std::endl;
        os << "fitParamFlash.y0 = " << f1.y0 << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.xrot[0] = " << f1.xrot[0] << ";" << std::endl;
        os << "fitParamFlash.xrot[1] = " << f1.xrot[1] << ";" << std::endl;
        os << "fitParamFlash.xrot[2] = " << f1.xrot[2] << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.yrot[0] = " << f1.yrot[0] << ";" << std::endl;
        os << "fitParamFlash.yrot[1] = " << f1.yrot[1] << ";" << std::endl;
        os << "fitParamFlash.yrot[2] = " << f1.yrot[2] << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.zrot[0] = " << f1.zrot[0] << ";" << std::endl;
        os << "fitParamFlash.zrot[1] = " << f1.zrot[1] << ";" << std::endl;
        os << "fitParamFlash.zrot[2] = " << f1.zrot[2] << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.xgain = " << f1.xgain << ";" << std::endl;
        os << "fitParamFlash.ygain = " << f1.ygain << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.g_coeff = " << f1.g_coeff << ";" << std::endl;
        os << std::endl;
        os << "fitParamFlash.checksum = " << f1.checksum << ";" << std::endl;
        os << std::endl;

        result = true;
    }

    /* TESTING
    double xgainAvgNone = f.xgain;
    double ygainAvgNone = f.ygain;
    double xgainAvg0 = f0.xgain;
    double ygainAvg0 = f0.ygain;
    double xgainAvg1 = f1.xgain;
    double ygainAvg1 = f1.ygain;
    */

    double xVal_ = f0.xgain;;
    double yVal_ = f0.ygain;;

    std::cout << "generate_fit xgain: " << xVal_ << std::endl;
    std::cout << "generate_fit ygain: " << yVal_ << std::endl;

    str = os_buffer.str();

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool test_cases( const std::vector<double>& values, bool use_metric, const fit_flash_section &f1, std::string& str )
{
    std::stringbuf os_buffer;
    std::ostream os( &os_buffer );

    fit f;
    f1.to_fit( f );

    // perform validation
    os << "---------- TEST CASES ----------" << std::endl;
    os << std::endl;

    int32_t tested( 0 );
    int32_t passed( 0 );

    // compute zero and span
    const int32_t zerovalue( f.zerovalue );
    const int32_t spanvalue( f.spanvalue );

    // compute scaling factor
    const double scalingFactor( net_load( LOAD_15KG, use_metric ) / spanvalue );

    // compute gravity conversion
    const double g_conv( scalingFactor / f.m );

    const int32_t DW( roundd( f.D * g_conv ) );
    const int32_t Eoff( zerovalue - DW );

    // compute accelerometer rotation
    matrix3x3 rot;
    rot.x[0] = f.xrot[0];
    rot.x[1] = f.xrot[1];
    rot.x[2] = f.xrot[2];
    rot.y[0] = f.yrot[0];
    rot.y[1] = f.yrot[1];
    rot.y[2] = f.yrot[2];
    rot.z[0] = f.zrot[0];
    rot.z[1] = f.zrot[1];
    rot.z[2] = f.zrot[2];


    os << " " << std::setw( 6 ) << "Weight";
    os << " " << std::setw( 8 ) << "Counts";
    os << " " << std::setw( 12 ) << "x";
    os << " " << std::setw( 12 ) << "y";
    os << " " << std::setw( 8 ) << "Wc";
    os << " " << std::setw( 8 ) << "Wb";
    os << " " << std::setw( 8 ) << "Sx";
    os << " " << std::setw( 8 ) << "Sy";
    os << " " << std::setw( 8 ) << "Wb Adj";
    os << " " << std::setw( 8 ) << "Error";
    os << " " << std::setw( 8 ) << "Counts";
    os << " " << std::setw( 8 ) << "Error";
    os << std::endl;

    for ( size_t i( 0 ); i < values.size(); i += 4 )
    {
        const int32_t applied( values[i + 0] );
        const appliedLoad a( (appliedLoad) applied );

        const int32_t c( values[i + 1] );
        const int32_t c_off( c - Eoff );

        const double xtilt( values[i + 2] );
        const double ytilt( values[i + 3] );

        const double angle( calc_angle( xtilt, ytilt ) );

        // measured load
        const double Wb( ((double) c_off) / angle );

        const double Sx_load( ((xtilt < 0.0) ? -1.0 : 1.0) * (Wb/g_conv) );
        const double Sx_error( (f.x2/RESOLUTION_10M)*pow(Sx_load,2.0) + (f.x1/RESOLUTION_100K)*Sx_load + f.x0 );
        const double Sx( sin( xtilt ) * Sx_error );

        const double Sy_load( ((ytilt < 0.0) ? -1.0 : 1.0) * (Wb/g_conv) );
        const double Sy_error( (f.y2/RESOLUTION_10M)*pow(Sy_load,2.0) + (f.y1/RESOLUTION_100K)*Sy_load + f.y0 );
        const double Sy( std::abs( sin( ytilt ) ) * Sy_error );

        const double Wb_adj( Wb + Sx + Sy );

        // expected load
        // (applied load + dead load) * cos( angle )
        const double Wc( net_load( a, use_metric )/scalingFactor + DW );

        // adjusted counts
        const int32_t c_adj( (int32_t) ((Wb_adj - DW) * scalingFactor) );

        os << " " << std::setw( 6 ) << applied;
        os << " " << std::setw( 8 ) << c;
        os << " " << std::setw( 12 ) << xtilt;
        os << " " << std::setw( 12 ) << ytilt;
        os << " " << std::setw( 8 ) << (int32_t) Wc;
        os << " " << std::setw( 8 ) << (int32_t) Wb;
        os << " " << std::setw( 8 ) << (int32_t) Sx;
        os << " " << std::setw( 8 ) << (int32_t) Sy;
        os << " " << std::setw( 8 ) << (int32_t) Wb_adj;
        os << " " << std::setw( 8 ) << (int32_t) (Wc - Wb_adj);
        os << " " << std::setw( 8 ) << c_adj;
        os << " " << std::setw( 8 ) << (int32_t) ((Wc - Wb_adj) * scalingFactor);

        // calculate error
        const double e( error( a, use_metric ) );

        if ( std::abs(net_load( a, use_metric ) - c_adj) < e )
        {
            os << " pass";
            ++passed;
        }
        else
        {
            os << " FAIL";
        }

        ++tested;
        os << std::endl;
    }

    os << std::endl;

    // ---- //

    if ( passed != tested )
    {
        os << "!!! ONE OR MORE TEST CASES FAILED" << std::endl;
        os << std::endl;
    }

    str = os_buffer.str();

    return (tested == passed);
}

float roundd(double x)
{
    return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
