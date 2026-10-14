#ifndef AUTOCALIBRATIONRIG_H
#define AUTOCALIBRATIONRIG_H

#include "flash.h"
#include "vector.h"

#include <iostream>
#include <vector>

/// Calibrate weigher.
/**
 * @param[in] counts  weigher counts
 * @param[in] metric  @c true to use kilogram weights, @c false for pounds
 * @param[out] f  fitting
 * @param[out] output  text output
 * @return  @c true upon success, @c false otherwise
 */
bool calib_weigher( const std::vector<double>& counts, bool metric, fit& f, std::ostream& os );

float roundf(float x);

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // AUTOCALIBRATIONRIG_H
