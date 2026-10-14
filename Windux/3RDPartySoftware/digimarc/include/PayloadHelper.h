// - - - - - - - - - - - - - - - - - - - -
//
// Digimarc Confidential
// Copyright Digimarc Corporation, 2019
//
// - - - - - - - - - - - - - - - - - - - -

#ifndef PAYLOADHELPER_H
#define PAYLOADHELPER_H

#include "dsmsdk.h"

#if defined( __cplusplus )
extern "C" {
#endif // __cplusplus

static const unsigned int DSM_GTIN_13_LENGTH = 13;
static const unsigned int DSM_SELLBY_DATE_LENGTH = 6;

/**
 * @brief MAX_PAYLOAD_OUT_BUFFER Specifies the longest JSON string that would be returned in PayloadOut for the DSM_Get***Json functions.
 *	This constant should be used when creating a buffer to hold the intermediate payload JSON that needs to be passed to @link DSM_GENERATOR_OPTIONS::pPayload @endlink
 *
 * @code
 * char convertedPaylodBuffer[MAX_PAYLOAD_OUT_BUFFER] = {'\0'};
 * DSM_RESULT successfulGtinConversion = DSM_GetGtinJson(MyPayloadAliasString, MyGtinString, convertedPayloadBuffer);
 *
 * if(successfulGtinConversion == DSM_OK) {
 *	// continue using SDK
 * } else {
 *	// handle error. Likely a malformed GTIN-14 or payload alias.
 * }
 * @endcode
 */
static const DSM_UINT32 MAX_PAYLOAD_OUT_BUFFER = 256;

/**
 * @brief Converts a GTIN-14 with valid check digit (i.e. 08525960043024) to a JSON string.
 *
 * Converts an ASCII Decimal string representing a GTIN-14 with valid check digit (i.e. 08525960043024) to a JSON string that is usable by the underlying encoder.
 *
 * @param[in] Gtin An ASCII Decimal string representing a GTIN-14 with valid check digit (i.e. 08525960043024).
 * @param[out] PayloadOut A pointer to a buffer to place the generated JSON string in, for use with the encoder.
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, The PayloadOut buffer is ready for use in the DLSDK. Buffer length must be as long as @ref MAX_PAYLOAD_OUT_BUFFER.
 */
DSM_API_METHOD DSM_RESULT DSM_GetGtinJson(const char* Gtin, char* PayloadOut);

/**
 * @brief Converts a GTIN-14 with valid check digit (i.e. 08525960043024), a filter value, a company prefix length and a serial number to a JSON string.
 *
 * Converts an ASCII Decimal string representing a GTIN-14 with valid check digit (i.e. 08525960043024), a filter value, a company prefix length and a serial number to a JSON string that is usable by the underlying encoder.
 *
 * @param[in] Gtin An ASCII Decimal string representing a GTIN-14 with valid check digit (i.e. 08525960043024).
 * @param[in] FilterValue SGTIN Filter Value (Valid Range: 0 to 7 (1 for POS)).
 * @param[in] CompanyPrefixLength SGTIN Company Prefix Length.
 * @param[in] SerialNumber SGTIN Serial Number (Valid Range: 0 to 274877906943 (12 decimal digits)).
 * @param[out] PayloadOut A pointer to a buffer to place the generated JSON string in, for use with the encoder.
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, The PayloadOut buffer is ready for use in the DLSDK. Buffer length must be as long as @ref MAX_PAYLOAD_OUT_BUFFER.
 */
DSM_API_METHOD DSM_RESULT DSM_GetSerializedGtinJson(const char* Gtin, const char* FilterValue, const char* CompanyPrefixLength, const char* SerialNumber, char* PayloadOut);

/**
 * @brief Converts an ASCII Decimal string representing a UPC-E (00012386) to a JSON string that is usable by the underlying encoder.
 *
 * @note UPC-E always starts with 0, followed by a 6-digit code and the check digit.
 *
 * @param[in] Upce An ASCII Decimal string representing a UPC-E (i.e. 00012386).
 * @param[out] PayloadOut A pointer to a buffer to place the generated JSON string in, for use with the encoder.
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, The PayloadOut buffer is ready for use in the DLSDK. Buffer length must be as long as @ref MAX_PAYLOAD_OUT_BUFFER.
 */
DSM_API_METHOD DSM_RESULT DSM_GetUpceJson(const char* Upce, char* PayloadOut);

/**
 * @brief Converts an ASCII Decimal string representing a GTIN-13 with valid check digit (for example, 8525960043024) and certain combinations of the other parameters
 * to a JSON string that is usable by the underlying encoder.
 *
 * @note Gtin13
 * 	- Required
 *	- Allowable range: Any valid GTIN-13.
 *		- Other symbols that can be zero padded (UPC-A, GTIN-12, GTIN-8, etc.) are supported, but must be provided with padded zeros.
 *		- Value provided **Must have 13 digits and include valid check digit**.
 *	- Input format:
 *		- 13 character string, null terminated.
 *	- Example:
 *		- UPC-A (padded): "0852596004050"
 *		- GTIN-13: "5901234123497"
 *
 * @note SellbyDate
 *	- Allowable range: **January 01, 2016** through **May 08, 2106**.
 *		- If it is provided, then it **Must have 6 digits**.
 *	- Use NULL or 0 or "0" to specify unused.
 *	- Input format:
 *		- YYMMDD See [GS1 General Specification: Sell by date: AI (16)](https://www.gs1.org/docs/barcodes/GS1_General_Specifications.pdf).
 *		- 6 character string, null terminated.
 *	- Example:
 *		- "170415" = April 15, 2017.
 *
 * @note Price
 *	- Allowable range: 0.00 to 99999.99
 *	- Use NULL or 0 or "0" to specify unused.
 *	- Denotes local currency
 *	- Input format:
 *		- 1 to 7 digits (0-9) in null terminated string, no decimal point.
 *	- Example:
 *		- "12345" = 123.45 (local currency).
 *
 * @note WeightInPounds/WeightInKiloGrams support weights up to 22.767 lbs or kg, with 3 decimals of precision.
 *	- Allowable range: 1 to 22767
 *	- Use NULL or 0 or "0" to specify unused.
 *	- Numerical range is the same for pounds and kilograms.
 *	- Only specify weight in a single unit. If pounds and kilograms are provided, it is treated as an error and PayloadOut will not be valid. Weights are not converted.
 *	- Input format:
 *		- 1 to 5 digits (0-9) in null terminated string, no decimal point.
 *	- Example:
 *		- "2345" = 2.345 (unit specified).
 *
 * @note Count
 *	- Allowable range: 1 to 99
 *	- Use NULL or 0 or "0" to specify unused.
 *	- Count is mutually exclusive with either weight. If count is provided with either weight, it is treated as an error and PayloadOut will not be valid. Weights are not converted.
 *	- Input format:
 *		- 1 to 2 digits (0-9) in null terminated string, no decimal point.
 *	- Example:
 *		- "15" = 15 qty.
 *
 * Example:
 *	Encode an item that should be sold by April 15, 2017, costs 123.45, weighs 2.345 lbs.
 * @code{.cpp}
 * char encodedJson[MAX_PAYLOAD_OUT_BUFFER] = {'\0'};
 * DSM_RESULT result = DSM_GetExpandedFreshJson("0852596004050", "170415", "12345", "2345", 0, 0, encodedJson);
 *
 * if(result != DSM_OK) {
 *	// Use encodedJson with API
 * } else {
 *	// Handle Error
 * }
 * @endcode
 *
 * @param[in] Gtin13 An ASCII Decimal string representing a GTIN-13 (i.e. 0852596004050). Value provided **Must include valid check digit**.
 * @param[in] SellbyDate The day the item must be sold by (YYMMDD). See [GS1 General Specification: Sell by date: AI (16)](https://www.gs1.org/docs/barcodes/GS1_General_Specifications.pdf) for further details.
 * @param[in] Price The price of the item (1234567, where the last two digits represent the fractional portion of the amount. i.e. the decoded value would be 12345.67).
 *	Max price is 9999999.
 * @param[in] WeightInPounds The weight of the item in US Pounds (12345, where the last three digits represent the fractional portion of the amount. i.e. the decoded value would be 12.345 lbs)
 * @param[in] WeightInKiloGrams The weight of the item in Metric kilograms (12345, where the last three digits represent the fractional portion of the amount. i.e. the decoded value would be 12.345 kg)
 * @param[in] Count The count of the items labeled (12)
 * @param[out] PayloadOut A pointer to a buffer to place the generated JSON string in, for use with the encoder.
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, The PayloadOut buffer is ready for use in the DLSDK. Buffer length must be as long as @ref MAX_PAYLOAD_OUT_BUFFER.
 */
DSM_API_METHOD DSM_RESULT DSM_GetExpandedFreshJson(const char* Gtin13
												   , const char* SellbyDate
												   , const char* Price
												   , const char* WeightInPounds
												   , const char* WeightInKiloGrams
												   , const char* Count
												   , char* PayloadOut);
/**
 * @brief Converts an ASCII Decimal string representing a GTIN-13 with valid check digit (for example, 8525960043024) and certain combinations of the other parameters
 * to a JSON string that is usable by the underlying encoder.
 *
 * @note WeightInPoundsRange2/WeightInKiloGramsRange2 support weights up to 227.67 lbs or kg, with 2 decimals of precision.
 *	- Allowable range: 1 to 22767
 *	- Use NULL or 0 or "0" to specify unused.
 *	- Numerical range is the same for pounds and kilograms.
 *	- Only specify weight in a single unit. If pounds and kilograms are provided, it is treated as an error and PayloadOut will not be valid. Weights are not converted.
 *	- Input format:
 *		- 1 to 5 digits (0-9) in null terminated string, no decimal point.
 *	- Example:
 *		- "2345" = 23.45 (unit specified).
 *
 * The rest of the parameters function the same as in @ref DSM_GetExpandedFreshJson.
 *
 * @param[in] Gtin13 An ASCII Decimal string representing a GTIN-13 (i.e. 0852596004050). Value provided **Must include valid check digit**.
 * @param[in] SellbyDate The day the item must be sold by (YYMMDD). See [GS1 General Specification: Sell by date: AI (16)](https://www.gs1.org/docs/barcodes/GS1_General_Specifications.pdf) for further details.
 * @param[in] Price The price of the item (1234567, where the last two digits represent the fractional portion of the amount. i.e. the decoded value would be 12345.67).
 *	Max price is 9999999.
 * @param[in] WeightInPoundsRange2 The weight of the item in US Pounds (12345, where the last two digits represent the fractional portion of the amount. i.e. the decoded value would be 123.45 lbs)
 * @param[in] WeightInKiloGramsRange2 The weight of the item in Metric kilograms (12345, where the last two digits represent the fractional portion of the amount. i.e. the decoded value would be 123.45 kg)
 * @param[in] Count The count of the items labeled (12)
 * @param[out] PayloadOut A pointer to a buffer to place the generated JSON string in, for use with the encoder.
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, The PayloadOut buffer is ready for use in the DLSDK. Buffer length must be as long as @ref MAX_PAYLOAD_OUT_BUFFER.
 */
DSM_API_METHOD DSM_RESULT DSM_GetExpandedFreshJson_WeightRange2(const char* Gtin13
												   , const char* SellbyDate
												   , const char* Price
												   , const char* WeightInPoundsRange2
												   , const char* WeightInKiloGramsRange2
												   , const char* Count
												   , char* PayloadOut);

/**
 * @brief Converts a Simple Product ID containing a GTIN-14 with valid check digit and extra data to a JSON string.
 *
 * Converts an ASCII Decimal string representing a GTIN-14 with valid check digit (i.e. 08525960043024) and Numeric or Alpha-Numeric extra data to a JSON string that is usable by the underlying encoder.
 *
 * @note Valid decimal range of Numeric extra data is 0 to 281474976710655.\n
 * Alpha-numeric extra data can have up to 8 characters, each taken from the ASCII range spanning from Space (ASCII 32) to '_' (ASCII 95).
 * This includes the numeric digits, all the capital English alphabet characters, and some other punctuation characters. However,
 * leading and trailing spaces (ASCII 32) are not allowed.
 *
 * @param[in] Gtin14 An ASCII Decimal string representing a GTIN-14 with valid check digit (i.e. 08525960043024).
 * @param[in] ExtraDataType Determines the type of extra data. (0 means extra data is Numeric and 1 means extra data is Alpha-Numeric)
 * @param[in] ExtraData Numeric or Alpha-Numeric extra data.
 * @param[out] PayloadOut A pointer to a buffer to place the generated JSON string in, for use with the encoder.
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, The PayloadOut buffer is ready for use in the DLSDK. Buffer length must be as long as @ref MAX_PAYLOAD_OUT_BUFFER.
 */
DSM_API_METHOD DSM_RESULT DSM_GetSimpleProductIdJson_Gtin(const char* Gtin14, const char* ExtraDataType, const char* ExtraData, char* PayloadOut);

/**
 * @brief Converts a Simple Product ID containing a UPC-E with valid check digit and extra data to a JSON string.
 *
 * Converts an ASCII Decimal string representing a UPC-E (i.e. 00012386) and Numeric or Alpha-Numeric extra data to a JSON string that is usable by the underlying encoder.
 *
 * @note UPC-E always starts with 0, followed by a 6-digit code and the check digit.\n
 * Valid decimal range of Numeric extra data is 0 to 281474976710655.\n
 * Alpha-Numeric extra data can have up to 8 characters, each taken from the ASCII range spanning from Space (ASCII 32) to '_' (ASCII 95).
 * This includes the numeric digits, all the capital English alphabet characters, and some other punctuation characters. However,
 * leading and trailing spaces (ASCII 32) are not allowed.
 *
 * @param[in] Upce An ASCII Decimal string representing a UPC-E (i.e. 00012386).
 * @param[in] ExtraDataType Determines the type of extra data. (0 means extra data is Numeric and 1 means extra data is Alpha-Numeric)
 * @param[in] ExtraData Numeric or Alpha-Numeric extra data.
 * @param[out] PayloadOut A pointer to a buffer to place the generated JSON string in, for use with the encoder.
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, The PayloadOut buffer is ready for use in the DLSDK. Buffer length must be as long as @ref MAX_PAYLOAD_OUT_BUFFER.
 */
DSM_API_METHOD DSM_RESULT DSM_GetSimpleProductIdJson_Upce(const char* Upce, const char* ExtraDataType, const char* ExtraData, char* PayloadOut);

#if defined( __cplusplus )
}
#endif // __cplusplus

#endif
