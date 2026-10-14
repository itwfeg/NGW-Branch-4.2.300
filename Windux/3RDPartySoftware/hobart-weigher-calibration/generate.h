#ifndef GENERATE_H
#define GENERATE_H

#include "flash.h"

#include <string>
#include <vector>


/// Generate fitting.
/**
 * @param[in] weigher_counts
 * @param[in] use_metric
 * @param[in] accel_values
 * @param[out] f1
 * @param[out] output
 * @return  @c true upon success, @c false otherwise
 */
bool generate_fit( const std::vector<double>& weigher_counts, bool use_metric, const std::vector<double>& accel_values, fit_flash_section &f1, std::string& output );

/// Test cases.
/**
 * @param[in] values
 * @param[in] use_metric
 * @param[out] f1
 * @param[out] output
 * @return  @c true upon success, @c false otherwise
 */
bool test_cases( const std::vector<double>& values, bool use_metric, const fit_flash_section &f1, std::string& str );

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // GENERATE_H
