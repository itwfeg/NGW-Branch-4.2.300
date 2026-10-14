/*
 *  @file dsmsdk.h
 *
 *  DIGIMARC CORPORATION PROPRIETARY INFORMATION
 *  Copyright, Digimarc Corporation.  All Rights Reserved.
 *
 *  This software contains confidential information, including trade secrets, 
 *  of Digimarc that is protected by the confidentiality agreement entered into 
 *  between you and Digimarc.  It is important that this information be used only
 *  in accordance with your agreement with Digimarc.  Receipt of this software 
 *  conveys no license under Digimarc's patent, trademark, trade secret, or 
 *  copyright rights.  Use of this software - and the technology detailed therein - 
 *  requires a license from Digimarc. The technology embodied in this software is 
 *  protected by US and International Patents.
 *
 *  Digimarc Label SDK declarations
 */
#ifndef DSMSDK_H
#define DSMSDK_H

#if defined( __cplusplus )
extern "C" {
#endif // __cplusplus

#ifdef DSM_EXPORTS
#ifdef WIN32
#define DSM_API_METHOD __declspec(dllexport)
#else
#ifdef WINCE
#define DSM_API_METHOD __declspec(dllexport)
#else
#define DSM_API_METHOD
#endif
#endif
#else
#define DSM_API_METHOD
#endif

/**
 * @brief The API handle
 * @see DSM_Open
 
 * @ingroup SDK_Handles
 */
typedef void *DSM_API;

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ typedefs ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
typedef unsigned short DSM_UINT16;		///< 16-bit unsigned int	@ingroup SDK_TypeDefs	
#ifdef WIN32
typedef unsigned long DSM_UINT32;		///< 32-bit unsigned int	@ingroup SDK_TypeDefs
#else
typedef unsigned int DSM_UINT32;		///< 32-bit unsigned int	@ingroup SDK_TypeDefs
#endif

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ enumerations ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
/**
 * @brief The DSM SDK result enumeration.  The SDK APIs return these values to indicate success or failure.
 * @ingroup SDK_Enums
 */
typedef enum
{         						
    DSM_OK = 0,								///< Success
    DSM_INVALID_HANDLE,						///< Invalid handle
	DSM_INVALID_ARGUMENT,					///< Invalid argument
	DSM_IMAGE_RESOLUTION_OUT_OF_RANGE,		///< Image resolution out of range
	DSM_WATERMARK_RESOLUTION_OUT_OF_RANGE,	///< Watermark resolution out of range
	DSM_PAYLOAD_OUT_OF_RANGE,				///< Payload out of range
	DSM_INVALID_DATA_DESC,					///< Invalid data description
	DSM_MEMORY_ERROR,						///< Memory error
	DSM_INTERNAL_ERROR,						///< Internal error
	DSM_INVALID_PAYLOAD_ALIAS,				///< Payload Alias does not match
	DSM_INVALID_PAYLOAD_DATA,				///< Payload data is invalid
	DSM_PAYLOAD_BUFFER_NULL					///< The buffer to hold a converted payload is NULL
} DSM_RESULT;

/**
 * @brief Enumerates the image types that are currently supported by the SDK.
 * @ingroup SDK_Enums
 */
typedef enum
{                           
    DSM_DATADESC_GRAYSCALE,			///< White dots on a black background.  The image data is 8 bits per pixel.
	DSM_DATADESC_GRAYSCALE_INV,		///< Black dots on a white background.  The image data is 8 bits per pixel.
    DSM_DATADESC_ONE_BPP			///< One Bit Per Pixel. Black (0) dots on white (1) background. The image data is 1 bit per pixel.
									///< 
									///< 0 means black. 1 means white.
									///< Pixel values are stored in each bit.
									///< The pixels are packed from left to right into scanlines.
									///< The left most pixel is in the most significant bit of a byte.
									///< Scanlines begin on byte boundary based on BytesPerRow and go from top to bottom.
									///< The end of each scanline is padded to at least a byte.

} DSM_DATADESC_TYPE;

/**
 * @brief Enumerates the the dot patterns that are currently supported by the SDK.
 * @ingroup SDK_Enums
 */
typedef enum
{
    DSM_DOT_PATTERN_CIRCULAR,			///< Circular Dot Pattern.
    DSM_DOT_PATTERN_CHECKERBOARD		///< Checkerboard Dot Pattern.

} DSM_DOT_PATTERN;

/**
 * @brief Enumerates the image blend mode that are currently supported by the SDK.
 * @ingroup SDK_Enums
 */
typedef enum
{                           
    DSM_IMAGE_BLEND_MODE_OVERWRITE,		///< No blend. The image will be overwritten.
	DSM_IMAGE_BLEND_MODE_MERGE,			///< Simple blend mode. Dots will be added to the image.
	DSM_IMAGE_BLEND_MODE_DILATE			///< Blend but keep out from the dilated area of the image

} DSM_IMAGE_BLEND_MODE;

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ structs ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

/**
 * @brief This structure is used to describe the characteristics of the image data.
 * @ingroup SDK_Structs
 */
typedef struct
{                                       
    DSM_DATADESC_TYPE		DataDescType;           ///< A value which selects any of the data types enumerated in DSM_DATADESC_TYPE.

	DSM_UINT32				Width;                  ///< Specifies the width of the image, in pixels.

    DSM_UINT32				Height;                 ///< Specifies the height of the image, in pixels.

    DSM_UINT32				BytesPerRow;            ///< The offset between rows of the image data (i.e., stride).

													///< There may or may not be pad bytes at the end of each row.

    DSM_UINT16				Resolution;				///< Resolution of the image in dots per inch. 

	void					*pData;                 ///< A pointer to the raw data of image (the pixels).

} DSM_DATADESC;

/**
 * @brief This structure is used to describe a rectangular keep out region of the image.
 *
 * This structure is used to describe a rectangular region of the image that you do not want the SDK to embed.
 * To describe a keep out region, you specify the X and Y coordinates of the top left corner, and the width and height
 * of the rectangle.
 * 
 * You can specify multiple keep out regions up to a maximum of 10 keep out regions.
 * You specify the number of keep out regions using @ref DSM_EMBEDDER_OPTIONS::nNumKeepOutRegions "nNumKeepOutRegions"
 * and allocate a @ref DSM_KEEP_OUT_REGION array,
 * and then pass the @ref DSM_KEEP_OUT_REGION array pointer to @ref DSM_EMBEDDER_OPTIONS::pKeepOutRegionArray "pKeepOutRegionArray".
 * 
 * If there is no keep out region, just set the @ref DSM_EMBEDDER_OPTIONS::nNumKeepOutRegions "nNumKeepOutRegions" to 0
 * and @ref DSM_EMBEDDER_OPTIONS::pKeepOutRegionArray "pKeepOutRegionArray" to 0.
 * 
 * The SDK will not touch any part of the image within any of the keep out regions.
 *
 * <b>Caution:</b>
 *
 * <ul>
 * <li><b>Minimize the use of keep out region as much as possible.</b></li>
 * <li><b>Use only when necessary for critical label elements such as nutrition information.</b></li>
 * <li><b>For details, please see @ref EmbeddingGuide "Embedding Guide".</b></li>
 * </ul>
 *
 * 
 * @see DSM_EMBEDDER_OPTIONS
 * @ingroup SDK_Structs
 */
typedef struct
{                                       
    DSM_UINT32				TopLeftX;		///< Specifies the X coordinate of the Top Left corner of keep out region, in pixels.

    DSM_UINT32				TopLeftY;		///< Specifies the Y coordinate of the Top Left corner of keep out region, in pixels.

    DSM_UINT32				Width;			///< Specifies the width of the keep out region, in pixels.

	DSM_UINT32				Height;			///< Specifies the height of the keep out region, in pixels.

} DSM_KEEP_OUT_REGION;

/**
 * @brief This structure encapsulates the parameters for the call to DSM_GenerateSparseMark().
 *
 * @deprecated since 1.2
 * 
 * This is replaced by DSM_EMBEDDER_OPTIONS
 *
 * @see DSM_EMBEDDER_OPTIONS
 * @ingroup SDK_Structs
 */
typedef struct
{
    DSM_DATADESC			*pImageDesc;			///< Structure used to describe the characteristics of an image.

	const char				*pPayload;				///< @brief Pointer to a null-terminated UTF8 string that describes the watermark payload.
													///<
													///< This is a JSON string that describes the payload.
													///< Please use the provided functions in PayloadHelper.h to generate the JSON string.

	DSM_UINT16				nDotDensity;			///< @brief Density of the dots
													///<
													///< Valid values are in the range of 10 to 100.  Influences the number of dots and 
													///< the watermark strength. For a nWatermarkRes of 150 this is recommended to be 35,
													///< and for a nWatermarkRes of 75, this is recommended to be 40.

	DSM_UINT16				nDotSize;				///< @brief Diameter of the dot in pixels.
													///<
													///< Dot size must not be larger than the ratio of image resolution and watermark resolution.
													///< For example, at image resolution 75 and watermark resolution 75, the only valid dot size is 1.
													///< If for example, image resolution is 203 and watermark resolution is 67, 
													///< then the valid dot sizes are 1, 2 or 3.

	DSM_UINT16				nWatermarkRes;			///< @brief Watermark resolution
													///<
													///< Watermark resolution must be greater than 0 and less than or equal to the image resolution.
													///< The ratio of image resolution watermark resolution must be between 1 and 8.
													///< Default set to 75.

} DSM_GENERATOR_OPTIONS;

/**
 * @brief This structure encapsulates the parameters for the call to DSM_EmbedSparseMark().
 *
 * @note **DSM_InitializeEmbedderOptions() must be called prior to overriding all application specific values.**
 *
 * This replaces the deprecated DSM_GENERATOR_OPTIONS.
 *
 * @see DSM_GENERATOR_OPTIONS DSM_DATADESC
 * @ingroup SDK_Structs
 */
typedef struct
{
	const char				*pPayload;				///< @brief Pointer to a null-terminated UTF8 string that describes the watermark payload.
													///<
													///< This is a JSON string that describes the payload.
													///< Please use the provided functions in PayloadHelper.h to generate the JSON string.

	DSM_UINT16				nDotDensity;			///< @brief Density of the dots
													///< 
													///< Valid values are in the range of 10 to 100.  Influences the number of dots and 
													///< the watermark strength.
													///<
													///< Default = 40
													///<
													///< Please see @ref EmbeddingGuide "Embedding Guide" for recommended setting.

	DSM_UINT16				nDotSize;				///< @brief Diameter of the dot in pixels.
													///<
													///< Dot size must not be larger than the ratio of image resolution and watermark resolution.
													///< For example, at image resolution 75 and watermark resolution 75, the only valid dot size is 1.
													///< If for example, image resolution is 203 and watermark resolution is 67, 
													///< then the valid dot sizes are 1, 2 or 3.
													///<
													///< Default = 1
													///<
													///< Please see @ref EmbeddingGuide "Embedding Guide" for recommended setting.

    DSM_DOT_PATTERN         eDotPattern;			///< @brief Pattern of a single dot on the label
													///<
													///< Dot Patterns currently supported are Circular for all @ref nDotSize and Checkerboard for @ref nDotSize = 2.
													///<
													///< Default = DSM_DOT_PATTERN_CIRCULAR

	DSM_UINT16				nWatermarkRes;			///< @brief Watermark resolution
													///<
													///< Watermark resolution must be greater than 0 and less than or equal to the image resolution.
													///< The ratio of image resolution to watermark resolution must be between 1 and 8.
													///<
													///< Default = 75
													///<
													///< Please see @ref EmbeddingGuide "Embedding Guide" for recommended setting.

	DSM_IMAGE_BLEND_MODE	eImageBlendMode;		///< @brief Blend mode used to blend the original image and watermark
													///<
													///< Three blend modes are currently supported.
													///< DSM_IMAGE_BLEND_MODE_OVERWRITE:
													///< Original image will be overwritten.
													///< DSM_IMAGE_BLEND_MODE_MERGE:
													///< Dots will be placed on the original image.
													///< DSM_IMAGE_BLEND_MODE_DILATE:
													///< Dots will be placed on the original image,
													///< but existing content in the original image will be avoided.
													///< 
													///< Default = DSM_IMAGE_BLEND_MODE_OVERWRITE

	DSM_UINT16				nNumKeepOutRegions;		///< @brief Number of keep out regions in the keep out regions array
													///< 
													///< Set it to 0 if there is no keep out region.
													///< The maximum number of keep out regions allowed is 10.
													///< 
													///< Default = 0
													///<
													///< <b>Caution:</b>
													///< <ul>
													///< <li><b>Minimize the use of keep out region as much as possible.</b></li>
													///< <li><b>Use only when necessary for critical label elements such as nutrition information.</b></li>
													///< <li><b>For details, please see @ref EmbeddingGuide "Embedding Guide".</b></li>
													///< </ul>

	DSM_KEEP_OUT_REGION		*pKeepOutRegionArray;	///< @brief Array of keep out regions in the image
													///<
													///< Embedder will not embed in the keep out regions of the image.
													///< Set it to 0 if there is no keep out region.
													///<
													///< <b>Caution:</b>
													///< <ul>
													///< <li><b>Minimize the use of keep out region as much as possible.</b></li>
													///< <li><b>Use only when necessary for critical label elements such as nutrition information.</b></li>
													///< <li><b>For details, please see @ref EmbeddingGuide "Embedding Guide".</b></li>
													///< </ul>


} DSM_EMBEDDER_OPTIONS;

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DSM_Open() ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
/**
 * @brief Opens a new instance of the Digimarc Sparse Mark API.  Each call to this method must
 * be paired with call to @link DSM_Close @endlink.  Threads should open and use a unique DSM_API handle.
 * 
 * @param[out] phApi pointer to a DSM_API handle
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, a DSM_API handle 
 * will be allocated on the heap and returned indirectly.  Otherwise, the given pointer to a DSM_API handle 
 * will be set to NULL.  This method may also return:
 * 
 * - @link DSM_INVALID_HANDLE @endlink if the pointer to a DSM_API handle is not valid.
 * - @link DSM_MEMORY_ERROR @endlink if memory allocation fails.
 * 
 * @see DSM_Close
 * @ingroup DSM_Methods
 *
 * Example usage:
@code
DSM_API hApi = NULL;

if (DSM_Open(&hApi) == DSM_OK)
{
	// DSM_API handle was initialized.  DSM_Close() should be called.
}
else
{
	// DSM_API handle was NOT initialized and was instead set to NULL.  DSM_Close() should NOT be called.
}
@endcode
 *
 */
DSM_API_METHOD DSM_RESULT DSM_Open(DSM_API *phApi);

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DSM_Close() ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
/**
 * @brief Closes a DSM API instance. This call must be made for each DSM_API handle that is successfully 
 * initialized with a call to DSM_Open().
 * 
 * @param[in] phApi pointer to a DSM_API handle
 * @return @link DSM_RESULT @endlink enumeration.  If @link DSM_OK @endlink is returned, the DSM_API handle 
 * passed by pointer will be deallocated, and set to NULL.  This method may also return:
 * 
 * - @link DSM_INVALID_HANDLE @endlink if the pointer to a DSM_API handle is not valid.
 * 
 * @see DSM_Open
 * @ingroup DSM_Methods
 * Example usage:
 *
@code
if (DSM_Close(&hApi) == DSM_OK)
{
	// DSM_API handle has been released and was set to NULL
}
else
{
	// DSM_API handle has NOT been released and was probably invalid.
}
@endcode
 *
 */
DSM_API_METHOD DSM_RESULT DSM_Close(DSM_API *phApi);

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DSM_InitializeEmbedderOptions ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
/**
 * @brief Initialize embedder options to default values
 *
 * @note **DSM_InitializeEmbedderOptions() must be called prior to overriding all application specific values in DSM_EMBEDDER_OPTIONS.**
 *
 * @param[in] pOptions pointer to embed options
 * @return @ref DSM_RESULT enumeration.  @ref DSM_OK is returned if @ref DSM_EMBEDDER_OPTIONS is successfully initialized.  
 *		This method may also return:
 * 
 * - @ref DSM_INVALID_ARGUMENT if pOptions is NULL.
 * 
 * @see DSM_EMBEDDER_OPTIONS DSM_EmbedSparseMark
 * @ingroup DSM_Methods
 */
DSM_API_METHOD DSM_RESULT DSM_InitializeEmbedderOptions(DSM_EMBEDDER_OPTIONS *pOptions);

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DSM_GenerateSparseMark ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
/**
 * @brief Generates a sparse watermark.
 * 
 * @deprecated since 1.2
 *
 * This is replaced by DSM_EmbedSparseMark()
 *
 * @param[in] hApi the SDK API handle
 * @param[in] pOptions pointer to embed options
 * @param[in] pReserved reserved for future use (pass NULL)
 * @return @ref DSM_RESULT enumeration.  @ref DSM_OK is returned if a sparse mark is successfully generated.  
 *		This method may also return:
 * 
 * - @ref DSM_INVALID_HANDLE if the DCE_PAYLOAD handle is not valid.
 * - @ref DSM_INVALID_ARGUMENT if the DMS_EMBED_OPTIONS is NULL or contains invalid data.
 * - @ref DSM_IMAGE_RESOLUTION_OUT_OF_RANGE if the image resolution is not valid for watermarking.
 * - @ref DSM_WATERMARK_RESOLUTION_OUT_OF_RANGE if the watermark resolution is not valid for watermarking.
 * - @ref DSM_PAYLOAD_OUT_OF_RANGE if the given payload is not valid.
 * - @ref DSM_INVALID_DATA_DESC if the image description data is invalid.
 * - @ref DSM_MEMORY_ERROR if a memory allocation fails.
 * 
 * @see DSM_Open DSM_EmbedSparseMark
 * @ingroup DSM_Methods
 */
DSM_API_METHOD DSM_RESULT DSM_GenerateSparseMark(DSM_API hApi, DSM_GENERATOR_OPTIONS *pOptions, void *pReserved);

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DSM_EmbedSparseMark ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
/**
 * @brief Embeds a sparse watermark.
 *
 * This replaces DSM_GenerateSparseMark()
 * @note **DSM_InitializeEmbedderOptions() must be called prior to overriding all application specific values in DSM_EMBEDDER_OPTIONS.**
 *
 * @param[in] hApi the SDK API handle
 * @param[in] pImageDesc pointer to image descriptions
 * @param[in] pOptions pointer to embed options
 * @return @ref DSM_RESULT enumeration.  @ref DSM_OK is returned if a sparse mark is successfully generated.  
 *		This method may also return:
 * 
 * - @ref DSM_INVALID_HANDLE if the DCE_PAYLOAD handle is not valid.
 * - @ref DSM_INVALID_ARGUMENT if the DMS_EMBED_OPTIONS is NULL or contains invalid data.
 * - @ref DSM_IMAGE_RESOLUTION_OUT_OF_RANGE if the image resolution is not valid for watermarking.
 * - @ref DSM_WATERMARK_RESOLUTION_OUT_OF_RANGE if the watermark resolution is not valid for watermarking.
 * - @ref DSM_PAYLOAD_OUT_OF_RANGE if the given payload is not valid.
 * - @ref DSM_INVALID_DATA_DESC if the image description data is invalid.
 * - @ref DSM_MEMORY_ERROR if a memory allocation fails.
 * 
 * @see DSM_Open DSM_InitializeEmbedderOptions
 * @ingroup DSM_Methods
 */
DSM_API_METHOD DSM_RESULT DSM_EmbedSparseMark(DSM_API hApi, DSM_DATADESC *pImageDesc, DSM_EMBEDDER_OPTIONS *pOptions);


#if defined( __cplusplus )
}
#endif // __cplusplus
#endif // DSMSDK_H
