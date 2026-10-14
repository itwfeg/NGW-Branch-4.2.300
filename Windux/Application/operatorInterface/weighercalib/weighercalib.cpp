
#include "common.h"
#include "poly.h"
#include "weighercalib.h"

#include <cmath>
#include <math.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
bool calib_weigher( const std::vector<double>& raw_counts, bool use_metric, fit& f, std::ostream& os )
{
    std::list<appliedLoad> loadList;
    loadList.push_back( LOAD_0KG );
    loadList.push_back( LOAD_5KG );
    loadList.push_back( LOAD_10KG );
    loadList.push_back( LOAD_15KG );

    std::list<tiltType> tiltList;
    tiltList.push_back( TILT_FRONT );
    tiltList.push_back( TILT_BACK );
    tiltList.push_back( TILT_LEFT );
    tiltList.push_back( TILT_RIGHT );

    std::map<appliedLoad, std::string> loadDescriptionList;
    loadDescriptionList[LOAD_0KG] = (use_metric ? "0 kg" : "0 lb");
    loadDescriptionList[LOAD_5KG] = (use_metric ? "5 kg" : "10 lb");
    loadDescriptionList[LOAD_10KG] = (use_metric ? "10 kg" : "20 lb");
    loadDescriptionList[LOAD_15KG] = (use_metric ? "15 kg" : "30 lb");

    // compute zero and span
    const int32_t zerovalue( counts( LOAD_0KG, TILT_NONE, raw_counts ) );
    const int32_t spanvalue( counts( LOAD_15KG, TILT_NONE, raw_counts ) - zerovalue );

    // average counts at tilt
    std::map<appliedLoad, int32_t> avgCounts;

    // TODO 1 DONE
    // VC++11 format
    //for ( auto a : loadList )

    // old school format
    for ( std::list<appliedLoad>::iterator a(loadList.begin()); a != loadList.end(); a++ )
    {
        avgCounts[*a] = 0;

        //for ( auto t : tiltList )
        for ( std::list<tiltType>::iterator t( tiltList.begin() ); t != tiltList.end(); t++ )
            avgCounts[*a] += counts( *a, *t, raw_counts );

        avgCounts[*a] /= tiltList.size();
    }

    // compute dead weight and electronic offset
    const int32_t DW( roundf( (double)( zerovalue - avgCounts[LOAD_0KG] ) / ( 1.0 - cos( deg_to_rad_5 ) ) ) );
    const int32_t Eoff( zerovalue - DW );

    const double scalingFactor( net_load( LOAD_15KG, use_metric ) / spanvalue );

    os << "zerovalue: " << zerovalue << std::endl;
    os << "spanvalue: " << spanvalue << std::endl;
    os << "DW: " << DW << std::endl;
    os << "Eoff: " << Eoff << std::endl;
    os << "m: " << scalingFactor << std::endl;
    os << std::endl;

    // TODO 7 DONE
    // expected load
    // applied load + dead load
    std::map<appliedLoad, double> expected;

    // VC++11 standard
    //for ( auto a : loadList )
    //    expected[a] = net_load( a, use_metric )/scalingFactor + DW;

    // old school format
    for ( std::list<appliedLoad>::iterator a(loadList.begin()); a != loadList.end(); a++ )
        expected[*a] = net_load( *a, use_metric )/scalingFactor + DW;

    // ---- //

    const double gain_factor( std::abs( sin( deg_to_rad_5 ) ) );

    std::vector<appliedLoad> Sx_applied;
    std::vector<vector2> Sx_values;

    std::vector<appliedLoad> Sy_applied;
    std::vector<vector2> Sy_values;

    os << " " << std::setw( 6 ) << "Weight";
    os << " " << std::setw( 8 ) << "Counts";
    os << " " << std::setw( 10 ) << "x";
    os << " " << std::setw( 10 ) << "y";
    os << " " << std::setw( 8 ) << "Wc";
    os << " " << std::setw( 8 ) << "Wb";
    os << " " << std::setw( 8 ) << "Error";
    os << std::endl;

    // TODO 8 DONE
    // compute error
    //for ( auto t : tiltList )
    //    for ( auto a : loadList )
    for ( std::list<tiltType>::iterator t( tiltList.begin() ); t != tiltList.end(); t++ )
        for ( std::list<appliedLoad>::iterator a( loadList.begin() ); a != loadList.end(); a++ )
        {
            const int32_t c( counts( *a, *t, raw_counts ) );
            const int32_t c_off( c - Eoff );

            // no counts, skip data point
            if (( -1 < c ) && ( c < 1 ))
                continue;

            const double angle( calc_angle( tilt_x( *t ), tilt_y( *t ) ) );

            // measured load
            const double Wb( ((double) c_off) / angle );

            // expected load and delta
            const double Wc( expected[*a] );
            const double delta( Wc - Wb );

            os << " " << std::setw( 6 ) << loadDescriptionList[*a];
            os << " " << std::setw( 8 ) << c;
            os << " " << std::setw( 10 ) << tilt_x( *t );
            os << " " << std::setw( 10 ) << tilt_y( *t );
            os << " " << std::setw( 8 ) << (int32_t) Wc;
            os << " " << std::setw( 8 ) << (int32_t) Wb;
            os << " " << std::setw( 8 ) << (int32_t) delta;
            os << std::endl;

            // save values
            if (( TILT_FRONT == *t ) || ( TILT_BACK == *t ))
            {
                const double sign( ((tilt_x( *t ) < 0.0) ? -1.0 : 1.0) );

                Sx_applied.push_back( *a );
                Sx_values.push_back( vector2( sign * Wb, sign * (delta / gain_factor) ) );
            }
            else if (( TILT_LEFT == *t ) || ( TILT_RIGHT == *t ))
            {
                const double sign( ((tilt_y( *t ) < 0.0) ? -1.0 : 1.0) );

                Sy_applied.push_back( *a );
                Sy_values.push_back( vector2( sign * Wb, delta / gain_factor ) );
            }
        }

    os << std::endl;

compute_poly:

    // multiple passes
    static const bool multiple_pass( true );

    // compute polynomials
    double x2, x1, x0;
    double y2, y1, y0;    

    fit_polynomial( Sx_values, x2, x1, x0, os );
    fit_linear( Sy_values, y2, y1, y0, os );

    os << "x2: " << x2 << std::endl;
    os << "x1: " << x1 << std::endl;
    os << "x0: " << x0 << std::endl;
    os << "y2: " << y2 << std::endl;
    os << "y1: " << y1 << std::endl;
    os << "y0: " << y0 << std::endl;
    os << std::endl;

    // ---- //

    bool passed( true );

    std::map<double, size_t> x_error;
    std::map<double, size_t> y_error;

    // show Sx points
    os << "X-Axis Error (Front-Back):" << std::endl;

    os << " " << std::setw( 10 ) << "Wb";
    os << " " << std::setw( 12 ) << "Meas Error";
    os << " " << std::setw( 12 ) << "Comp Error";
    os << " " << std::setw( 10 ) << "Wc";
    os << " " << std::setw( 10 ) << "Wb Adj";
    os << " " << std::setw( 8 ) << "Error";
    os << std::endl;

    for ( size_t i( 0 ); i < Sx_values.size(); ++i )
    {
        const double Wb( std::abs( Sx_values[i].x ) );

        const double measuredError( gain_factor * Sx_values[i].y );
        const double computedError( gain_factor * ( x2*pow(Sx_values[i].x, 2.0) + x1*Sx_values[i].x + x0 ) );

        const double Wb_adj( Wb + ((Sx_values[i].x < 0.0) ? -1.0 : 1.0) * computedError );

        // expected load and delta
        const double Wc( expected[ Sx_applied[i] ] );
        const double delta( Wc - Wb_adj );

        const double err( error( Sx_applied[i], use_metric ) / scalingFactor );

        // track error
        x_error[ pow( delta, 2.0 ) ] = i;

        os << " " << std::setw( 10 ) << (int32_t) Wb;
        os << " " << std::setw( 12 ) << measuredError;
        os << " " << std::setw( 12 ) << computedError;
        os << " " << std::setw( 10 ) << (int32_t) Wc;
        os << " " << std::setw( 10 ) << (int32_t) Wb_adj;
        os << " " << std::setw( 8 ) << (int32_t) delta;

        if ( std::abs( delta ) < err )
            os << " " << "pass";
        else
        {
            os << " " << "FAIL";
            passed = false;
        }

        os << std::endl;
    }

    os << std::endl;

    os << "XVAL";
    for ( size_t i( 0 ); i < Sx_values.size(); ++i )
        os << ((0 < i) ? "," : " ") << (int32_t) Sx_values[i].x;
    os << std::endl;

    os << "YVAL";
    for ( size_t i( 0 ); i < Sx_values.size(); ++i )
        os << ((0 < i) ? "," : " ") << Sx_values[i].y;
    os << std::endl;

    os << std::endl;

    // show Sy points
    os << "Y-Axis Error (Left-Right):" << std::endl;

    os << " " << std::setw( 10 ) << "Wb";
    os << " " << std::setw( 12 ) << "Meas Error";
    os << " " << std::setw( 12 ) << "Comp Error";
    os << " " << std::setw( 10 ) << "Wc";
    os << " " << std::setw( 10 ) << "Wb Adj";
    os << " " << std::setw( 8 ) << "Error";
    os << std::endl;

    for ( size_t i( 0 ); i < Sy_values.size(); ++i )
    {
        const double Wb( std::abs( Sy_values[i].x ) );

        const double measuredError( gain_factor * Sy_values[i].y );
        const double computedError( gain_factor * ( y2*pow(Sy_values[i].x, 2.0) + y1*Sy_values[i].x + y0 ) );

        const double Wb_adj( Wb + computedError );

        // expected load and delta
        const double Wc( expected[ Sy_applied[i] ] );
        const double delta( Wc - Wb_adj );

        const double err( error( Sy_applied[i], use_metric ) / scalingFactor );

        // track error
        y_error[ pow( delta, 2.0 ) ] = i;

        os << " " << std::setw( 10 ) << (int32_t) Wb;
        os << " " << std::setw( 12 ) << measuredError;
        os << " " << std::setw( 12 ) << computedError;
        os << " " << std::setw( 10 ) << (int32_t) Wc;
        os << " " << std::setw( 10 ) << (int32_t) Wb_adj;
        os << " " << std::setw( 8 ) << (int32_t) delta;

        if ( std::abs( delta ) < err )
            os << " " << "pass";
        else
        {
            os << " " << "FAIL";
            passed = false;
        }

        os << std::endl;
    }

    os << std::endl;

    os << "XVAL";
    for ( size_t i( 0 ); i < Sy_values.size(); ++i )
        os << ((0 < i) ? "," : " ") << (int32_t) Sy_values[i].x;
    os << std::endl;

    os << "YVAL";
    for ( size_t i( 0 ); i < Sy_values.size(); ++i )
        os << ((0 < i) ? "," : " ") << Sy_values[i].y;
    os << std::endl;

    os << std::endl;

    // ---- //

    // for 20pt fitting remove highest error and re-compute.
    if (( multiple_pass ) && ( 6 < x_error.size() ) && ( 6 < y_error.size() ))
    {
        const double err( error( LOAD_0KG, use_metric ) / scalingFactor ); // half division

        bool compute( false );

        // erase worse offender
        if ( err <= sqrt( x_error.rbegin()->first ) )
        {
            const size_t x_idx( x_error.rbegin()->second );

            Sx_applied.erase( Sx_applied.begin()+x_idx );
            Sx_values.erase( Sx_values.begin()+x_idx );

            compute = true;
        }

        // erase worse offender
        if ( err <= sqrt( y_error.rbegin()->first ) )
        {
            const size_t y_idx( y_error.rbegin()->second );

            Sy_applied.erase( Sy_applied.begin()+y_idx );
            Sy_values.erase( Sy_values.begin()+y_idx );

            compute = true;
        }

        // perform additional fitting
        if ( compute )
        {
            os << "ADDITIONAL FITTING" << std::endl;
            os << std::endl;

            goto compute_poly;
        }
    }

    // ---- //

    f.m = scalingFactor;

    f.zerovalue = zerovalue;
    f.spanvalue = spanvalue;
    f.Z0 = Eoff;
    f.D = DW;

    f.x2 = x2*RESOLUTION_10M;
    f.x1 = x1*RESOLUTION_100K;
    f.x0 = x0;

    f.y2 = y2*RESOLUTION_10M;
    f.y1 = y1*RESOLUTION_100K;
    f.y0 = y0;

    return passed;
}

