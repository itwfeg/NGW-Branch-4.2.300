/**
 * @file hobartweigher.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include <QApplication>
#include "hobartweigher.h"

#include <iostream>
#include <iomanip>

#include <memory.h>
#include <stdint.h>

static const bool DISPLAY_STATUS_MESSAGES = true;

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartWeigher::hobartWeigher()
{
    qRegisterMetaType<hobartWeigher::weigherState>("hobartWeigher::weigherState");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartWeigher::~hobartWeigher()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::handleValueMaxDate(uint32_t mfgDate)
{
    std::cout << "hobartWeigher::handleValueMaxDate(): Ready to pass on to Calibui -> so now EMIT updateValueMaxDate()" << std::endl;
    Q_EMIT updateValueMaxDate(mfgDate);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::handleValueMaxAssemblyNumber(QString assemNum)
{
    std::cout << "hobartWeigher::handleValueMaxAssemblyNumber(): Ready to pass on to Calibui -> so now EMIT updateValueMaxAssemblyNumber()" << std::endl;
    Q_EMIT updateValueMaxAssemblyNumber(assemNum);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::handleValueMaxHWVersion(QString hwVersion)
{
    std::cout << "hobartWeigher::handleValueMaxHWVersion(): Ready to pass on to Calibui -> so now EMIT updateValueMaxHWVersion()" << std::endl;
    Q_EMIT updateValueMaxHWVersion(hwVersion);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::handleValueMaxSerialNumber(QString serialNum)
{
    std::cout << "hobartWeigher::handleValueMaxSerialNumber(): Ready to pass on to Calibui -> so now EMIT updateValueMaxSerialNumber()" << std::endl;
    Q_EMIT updateValueMaxSerialNumber(serialNum);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::handleWeigherStatus(uint16_t flags, uint32_t rawAtoDCounts, int32_t nonZeroCalibCounts, int32_t zeroCalibCounts, int32_t acclXRawCounts, int32_t acclYRawCounts, int32_t acclZRawCounts, int16_t acclXTiltDegrees, int16_t acclYTiltDegrees )
{
    if( DISPLAY_STATUS_MESSAGES )
    {
        // really only using just one of these values right now, the rawAtoDCounts
        std::cout << "hobartWeighter::handleWeigherStatus() status: flags " << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << flags << std::dec <<
        "\n HW::handleWeigherStatus(): rawAtoDCts "      << rawAtoDCounts <<
        "\n HW::handleWeigherStatus(): acclXRawCts "     << acclXRawCounts <<
        "\n HW::handleWeigherStatus(): acclYRawCts "     << acclYRawCounts <<
        "\n HW::handleWeigherStatus(): acclZRawCts "     << acclZRawCounts << std::endl;
    }

    unsigned int rawWeight( rawAtoDCounts + 50 );          // NEW - but do same thing.
    rawWeight /= 100;

    // both routines do this
    float weight( rawWeight  );
    weight /= 100.0;

    //std::cout << "hobartWeigher::handleWeigherStatus(): Ready to pass on to Calibui -> so now EMIT updateValueMaxWeigherStatus()" << std::endl;

    Q_EMIT updateValueMaxWeigherStatus( flags, rawAtoDCounts, nonZeroCalibCounts, zeroCalibCounts, acclXRawCounts, acclYRawCounts, acclZRawCounts, acclXTiltDegrees, acclYTiltDegrees );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::weigherStateChanged( hobartWeigher::weigherState s, float weight )
{
    std::cout << "hobartWeigher::weigherStateChanged(): Ready to pass on to Calibui -> so now EMIT weigherStateChanged()" << std::endl;
    Q_EMIT weigherStateChanged(s, weight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::handleValueMaxFitParameters(int32_t a1_g, int32_t a2_m, int32_t a3_zerovalue, int32_t a4_spanvalue, int32_t a5_Z0, int32_t a6_D, int32_t a7_x2, int32_t a8_x1, int32_t a9_x0, int32_t a10_y2, int32_t a11_y1, int32_t a12_y0, int32_t a13_xrot0, int32_t a14_xrot1, int32_t a15_xrot2, int32_t a16_yrot0, int32_t a17_yrot1, int32_t a18_yrot2, int32_t a19_zrot0, int32_t a20_zrot1, int32_t a21_zrot2, int32_t a22_xgain, int32_t a23_ygain, int32_t a24_gcoeff, int32_t a25_checksum)
{
    std::cout << "hobartWeigher::handleValueMaxFitParameters(): Ready to pass on to Calibui -> so now EMIT updateValueMaxFitParameters()" << std::endl;
    Q_EMIT updateValueMaxFitParameters(a1_g, a2_m, a3_zerovalue, a4_spanvalue, a5_Z0, a6_D, a7_x2, a8_x1, a9_x0, a10_y2, a11_y1, a12_y0, a13_xrot0, a14_xrot1, a15_xrot2, a16_yrot0, a17_yrot1, a18_yrot2, a19_zrot0, a20_zrot1, a21_zrot2, a22_xgain, a23_ygain, a24_gcoeff, a25_checksum);
}
