#ifndef ACCELCALIB_H
#define ACCELCALIB_H

#include "flash.h"
#include "vector.h"

#include <iostream>
#include <vector>

/// Calibrate accelerometer.
/**
 * @param[in] v  accelerometer values
 * @param[out] f  fitting
 * @param[out] output  text output
 * @return  @c true upon success, @c false otherwise
 */
bool calib_accel( const std::vector<vector3>& v, fit &f, std::ostream& os );

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ACCELCALIB_H
