#ifndef FLASH_H
#define FLASH_H

#include "common.h"

struct fit
{
    double g;               // Gravity
    double m;               // Pounds/Counts conversion factor

    double zerovalue;       // Counts at zero load.
    double spanvalue;       // Cell span (max load - zero load)
    double Z0;              // Electronic offset
    double D;               // Dead weight.

    double x2;              // Polynomial for Front-Back tilt correction    // RESOLUTION_10M
    double x1;              // Polynomial for Front-Back tilt correction    // RESOLUTION_100K
    double x0;              // Polynomial for Front-Back tilt correction

    double y2;              // Polynomial for Left-Right tilt correction    // RESOLUTION_10M
    double y1;              // Polynomial for Left-Right tilt correction    // RESOLUTION_100K
    double y0;              // Polynomial for Left-Right tilt correction

    double xrot[3];         // Accel. Rotation Matrix
    double yrot[3];         // Accel. Rotation Matrix
    double zrot[3];         // Accel. Rotation Matrix

    double xgain;           // X Gain Factor
    double ygain;           // Y Gain Factor

    double g_coeff;         // Gravity Coefficient
};

struct fit_flash_section
{
    int32_t g;
    int32_t m;

    int32_t zerovalue;
    int32_t spanvalue;
    int32_t Z0;
    int32_t D;

    int32_t x2;
    int32_t x1;
    int32_t x0;

    int32_t y2;
    int32_t y1;
    int32_t y0;

    int32_t xrot[3];
    int32_t yrot[3];
    int32_t zrot[3];

    int32_t xgain;
    int32_t ygain;

    int32_t g_coeff;

    int32_t reserved[39];
    int32_t checksum;

    void from_fit( const fit& f )
    {
        std::memset( this, 0, sizeof(fit_flash_section) );

        g = (int32_t) (f.g * RESOLUTION_1M);
        m = (int32_t) (f.m * RESOLUTION_10M);

        zerovalue = (int32_t) f.zerovalue;
        spanvalue = (int32_t) f.spanvalue;
        Z0 = (int32_t) f.Z0;
        D = (int32_t) f.D;

        x2 = (int32_t) (f.x2 * RESOLUTION_10M);
        x1 = (int32_t) (f.x1 * RESOLUTION_100K);
        x0 = (int32_t) (f.x0 * RESOLUTION_100);

        y2 = (int32_t) (f.y2 * RESOLUTION_10M);
        y1 = (int32_t) (f.y1 * RESOLUTION_100K);
        y0 = (int32_t) (f.y0 * RESOLUTION_100);

        xrot[0] = (int32_t) (f.xrot[0] * RESOLUTION_10M);
        xrot[1] = (int32_t) (f.xrot[1] * RESOLUTION_10M);
        xrot[2] = (int32_t) (f.xrot[2] * RESOLUTION_10M);

        yrot[0] = (int32_t) (f.yrot[0] * RESOLUTION_10M);
        yrot[1] = (int32_t) (f.yrot[1] * RESOLUTION_10M);
        yrot[2] = (int32_t) (f.yrot[2] * RESOLUTION_10M);

        zrot[0] = (int32_t) (f.zrot[0] * RESOLUTION_10M);
        zrot[1] = (int32_t) (f.zrot[1] * RESOLUTION_10M);
        zrot[2] = (int32_t) (f.zrot[2] * RESOLUTION_10M);

        xgain = (int32_t) (f.xgain * RESOLUTION_100K);
        ygain = (int32_t) (f.ygain * RESOLUTION_100K);

        g_coeff = (int32_t) (f.g_coeff * RESOLUTION_100K);

        // ---- //

        uint8_t *p( (uint8_t*) this );

        while ( p < (uint8_t*) &checksum )
            checksum += *p++;
    }

    void to_fit( fit& f ) const
    {
        uint8_t *p( (uint8_t*) this );
        int32_t checksum_val( 0 );

        while ( p < (uint8_t*) &checksum )
            checksum_val += *p++;

        if ( checksum != checksum_val )
            return;

        f.g = (double) g / RESOLUTION_1M;
        f.m = (double) m / RESOLUTION_10M;

        f.zerovalue = (double) zerovalue;
        f.spanvalue = (double) spanvalue;
        f.Z0 = (double) Z0;
        f.D = (double) D;

        f.x2 = (double) x2 / RESOLUTION_10M;
        f.x1 = (double) x1 / RESOLUTION_100K;
        f.x0 = (double) x0 / RESOLUTION_100;

        f.y2 = (double) y2 / RESOLUTION_10M;
        f.y1 = (double) y1 / RESOLUTION_100K;
        f.y0 = (double) y0 / RESOLUTION_100;

        f.xrot[0] = (double) xrot[0] / RESOLUTION_10M;
        f.xrot[1] = (double) xrot[1] / RESOLUTION_10M;
        f.xrot[2] = (double) xrot[2] / RESOLUTION_10M;

        f.yrot[0] = (double) yrot[0] / RESOLUTION_10M;
        f.yrot[1] = (double) yrot[1] / RESOLUTION_10M;
        f.yrot[2] = (double) yrot[2] / RESOLUTION_10M;

        f.zrot[0] = (double) zrot[0] / RESOLUTION_10M;
        f.zrot[1] = (double) zrot[1] / RESOLUTION_10M;
        f.zrot[2] = (double) zrot[2] / RESOLUTION_10M;

        f.xgain = (double) xgain / RESOLUTION_100K;
        f.ygain = (double) ygain / RESOLUTION_100K;

        f.g_coeff = (double) g_coeff / RESOLUTION_100K;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FLASH_H
