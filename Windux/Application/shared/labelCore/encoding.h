/**
 * @file encoding.h
 * String encoding types for binary data.
 *
 * Label and graphics data are encoded into a string format. This file contains encoding types.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef ENCODING_H
#define ENCODING_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @struct no_encoding_t
 * @brief Create object from non-encoded string.
 */
struct no_encoding_t {};

/**
 * @struct base16_encoding_t
 * @brief Create object from base16 encoded string.
 */
struct base16_encoding_t {};

/**
 * @struct base64_encoding_t
 * @brief Create object from base64 encoded string.
 */
struct base64_encoding_t {};

/**
 * Used to construct object from a non-encoded string.
 */
static const no_encoding_t no_encoding = {};

/**
 * Used to construct object from a base16 encoded string.
 */
static const base16_encoding_t base16_encoding = {};

/**
 * Used to construct object from a base64 encoded string.
 */
static const base64_encoding_t base64_encoding = {};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // ENCODING_H
