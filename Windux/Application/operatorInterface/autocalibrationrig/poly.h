#ifndef POLY_H
#define POLY_H

#include "vector.h"

#include <iostream>
#include <vector>

/// Fit second order polynomial.
/**
 * @param[in] v  values
 * @param[out] x2  second order coefficient
 * @param[out] x1  first order coefficient
 * @param[out] x0  constant value
 * @param[out] os  output
 */
void fit_polynomial( const std::vector<vector2>& v, double& x2, double& x1, double& x0, std::ostream& os );

/// Fit linear.
/**
 * @param[in] v  values
 * @param[out] x2  second order coefficient
 * @param[out] x1  first order coefficient
 * @param[out] x0  constant value
 * @param[out] os  output
 */
void fit_linear( const std::vector<vector2>& v, double& x2, double& x1, double& x0, std::ostream& os );

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // POLY_H
