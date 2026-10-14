// Header file included by ANTLR UniLexer
#include <tchar.h>
#ifndef GSDS_VOLUME_F_HEADER
#define GSDS_VOLUME_F_HEADER

/*******************************************************************************
* Eumetsat Meteosat Second Generation
*
* Data Acquisition and Dissemination Facility: DADF
*
* This software is developed by VCS Nachrichtentechnik GmbH under contract
* VCS      : V97.103.1008
* Eumetsat : EUM/CO/97/499/YB
*
* Copyright VCS 1998
*
* Applicable Third-party Software Licence Information:
*
*-------------------------------------------------------------------------------
* Configuration Control Details from Visual Source Safe
*
$Archive: /Util/Inc/GSDS_Volume_F.h $
*
$History: GSDS_Volume_F.h $
 * 
*
*-------------------------------------------------------------------------------
*
********************************************************************************
*/

/**************************************************************************
****  DADF MODULE HEADER   ***

TYPE:			data types

PURPOSE:		defines the GSDS Volume F data types


FUNCTION:		In this header file the GSDS Volume F datatypes are
				defined.


INTERFACES:		See 'INTERFACES:' in the module declaration below
				The data types defined below model the data types 
				defined in 	GSDS, Volume F. 
				
				For sending these data-types across the network it 
				is necessary to define one function for each data-type which 
				returns the size of the type.
				It is also needed to define input/output operators for each 
				data-type to stream in/out the objects from a (or on to a)
				Util::CMSGByteStream object.
				These function/operators are not provided now, they will be 
				defined in the next phase.
				Below you see an example prototype of these functions: 
	
					// Length when sent across the network.
					unsigned int NetworkLength(...) const

					// input/output operators to send objects across the network.
					CMSGByteStream& operator << (CMSGByteStream& o_Stream, GP_FAC_ENV i_FAC_ENV)
					CMSGByteStream& operator >> (CMSGByteStream& o_Stream, GP_FAC_ENV o_FAC_ENV)
				

				The output operators defined in this file are only needed to tranform
				a data-type to a human-readable string. 
				
			
RESOURCES:		negligible


REFERENCES:		GSDS Volume F


PROCESSING:		no complex processing	

****  END MODULE HEADER   ***
**************************************************************************/

#ifdef XTRA_COM_INFO
	#pragma message( "Compiling " __FILE__ ) 
#endif

#include <string>
#include "CMSGByteStream.h"




	//	Some commonly needed sizes
	static const unsigned int	StaticInt8Size		= 1;
	static const unsigned int	StaticInt16Size		= 2;
	static const unsigned int	StaticInt32Size		= 4;
	static const unsigned int	StaticInt64Size		= 8;
	static const unsigned int	StaticFloatSize		= sizeof(float);
	static const unsigned int	StaticDoubleSize	= sizeof(double);
	static const unsigned int	StaticLongSize		= sizeof(long);
	static const unsigned int	StaticCHAR80Size	= 80;
	static const unsigned int	StaticCHAR255Size	= 255;

// mvd: necessary for event build tools which generates variables of type CHAR80
//##ModelId=39AD96140291
struct CHAR80
{
	//##ModelId=39AD961402B1
	std::wstring m_value;

	//##ModelId=39AD961402C6
	CHAR80(const std::wstring& i_value = _T(""))
	{
		m_value=i_value;
		m_value.resize(80);

	};

	//##ModelId=39AD961402C4
	std::wstring ReadableForm() const
	{	
		return m_value;
	}

	//##ModelId=39AD961402BC
	void operator = ( std::wstring& i_other)
		{m_value=i_other;m_value.resize(80);}

	//##ModelId=39AD961402BA
	const unsigned int IntNetworkLength() const {return StaticCHAR80Size;};
	//##ModelId=39AD961402B4
	const unsigned int ExtNetworkLength() const {return StaticCHAR80Size;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const CHAR80& i_string);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,CHAR80& o_string);


//##ModelId=39AD9605036C
struct CHAR255
{
	//##ModelId=39AD96050382
	std::wstring m_value;

	//##ModelId=39AD96050397
	CHAR255(const std::wstring& i_value = _T(""))
	{
		m_value=i_value;
		m_value.resize(255);

	};

	//##ModelId=39AD96050395
	std::wstring ReadableForm() const
	{	
		return m_value;
	}

	//##ModelId=39AD9605038E
	void operator = ( std::wstring& i_other)
		{m_value=i_other;m_value.resize(255);}

	//##ModelId=39AD9605038C
	const unsigned int IntNetworkLength() const {return StaticCHAR255Size;};
	//##ModelId=39AD9605038A
	const unsigned int ExtNetworkLength() const {return StaticCHAR255Size;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const CHAR255& i_string);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,CHAR255& o_string);


// Booleans

//##ModelId=39AD960902F9
struct BOOLEAN_BYTE
{
	//##ModelId=39AD96090304
	bool m_value;
	
	//##ModelId=39AD9609031B
	BOOLEAN_BYTE(bool i_value = true){m_value = i_value;};
	//##ModelId=39AD9609031A
	std::wstring ReadableForm()
	{
		if (m_value)
			return _T("TRUE");
		else
			return _T("FALSE");
	}

	//##ModelId=39AD96090318
	operator bool() const
	{
		return m_value;
	}

	//##ModelId=39AD96090310
	void operator = (bool i_other)
		{m_value = i_other;}


	//##ModelId=39AD9609030E
	const unsigned int IntNetworkLength() const {return StaticInt8Size ;};
	//##ModelId=39AD96090305
	const unsigned int ExtNetworkLength() const {return StaticInt8Size ;};

};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const BOOLEAN_BYTE& i_boolbyte);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,BOOLEAN_BYTE& o_boolbyte);


//##ModelId=39AD95FA0117
struct BOOLEAN_SHORT 
{
	//##ModelId=39AD95FA0122
	bool m_value;
	
	//##ModelId=39AD95FA0139
	BOOLEAN_SHORT(bool i_value = true){m_value = i_value;};
	//##ModelId=39AD95FA0138
	std::wstring ReadableForm()
	{
		if (m_value)
			return _T("TRUE");
		else
			return _T("FALSE");
	}

	//##ModelId=39AD95FA0136
	void operator = (bool i_other)
		{m_value = i_other;}


	//##ModelId=39AD95FA012E
	operator bool() const
	{
		return m_value;
	}


	//##ModelId=39AD95FA012C
	const unsigned int IntNetworkLength() const {return StaticInt16Size ;};
	//##ModelId=39AD95FA0123
	const unsigned int ExtNetworkLength() const {return StaticInt16Size ;};
};


inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const BOOLEAN_SHORT& i_boolshort);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,BOOLEAN_SHORT& o_boolshort);


//##ModelId=39AD961400C4
struct BOOLEAN_LONG
{
	//##ModelId=39AD961400D8
	bool m_value;
	
	//##ModelId=39AD961400ED
	BOOLEAN_LONG(bool i_value = true){m_value = i_value;};
	//##ModelId=39AD961400EC
	std::wstring ReadableForm()
	{
		if (m_value)
			return _T("TRUE");
		else
			return _T("FALSE");
	}

	//##ModelId=39AD961400E5
	void operator = (bool i_other)
		{m_value = i_other;}


	//##ModelId=39AD961400E3
	operator bool() const
	{
		return m_value;
	}

	//##ModelId=39AD961400DB
	const unsigned int IntNetworkLength() const {return StaticInt32Size ;};
	//##ModelId=39AD961400D9
	const unsigned int ExtNetworkLength() const {return StaticInt32Size ;};
};
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const BOOLEAN_LONG& i_boollong);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,BOOLEAN_LONG& o_boollong);



// Enumerated
typedef unsigned __int8		ENUMERATED_BYTE;


//##ModelId=39AD95FC005B
struct UNSIGNED_BYTE
{
	//##ModelId=39AD95FC0072
	UINT8 m_value;
	
	//##ModelId=39AD95FC0090
	UNSIGNED_BYTE (UINT8 i_value = 0){m_value = i_value;};
	//##ModelId=39AD95FC008E
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

		_itot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD95FC0086
	void operator = (UINT8 i_other)
		{m_value = i_other;}

	//##ModelId=39AD95FC0084
	operator UINT8 () const 
	{
		return m_value;
	}

	//##ModelId=39AD95FC007C
	const unsigned int IntNetworkLength() const {return StaticInt8Size ;};
	//##ModelId=39AD95FC007A
	const unsigned int ExtNetworkLength() const {return StaticInt8Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED_BYTE& i_unbyte);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED_BYTE& o_unbyte);

inline std::wostream& operator << (std::wostream& o_Stream, const UNSIGNED_BYTE& i_unbyte);


//##ModelId=39AD961A0285
struct UNSIGNED_SHORT
{
	//##ModelId=39AD961A029B
	UINT16 m_value;
	
	//##ModelId=39AD961A02CC
	UNSIGNED_SHORT(UINT16 i_value = 0){m_value = i_value;};

	//##ModelId=39AD961A02C2
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

		_itot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD961A02B7
	void operator = (UINT16 i_other)
		{m_value = i_other;}

	//##ModelId=39AD961A02B9
	void operator = (const UNSIGNED_SHORT i_other)
		{m_value = i_other.m_value;}

	//##ModelId=39AD961A02AF
	operator UINT16() const { return m_value; }

	//##ModelId=39AD961A02AD
	const unsigned int IntNetworkLength() const {return StaticInt16Size ;};
	//##ModelId=39AD961A02A3
	const unsigned int ExtNetworkLength() const {return StaticInt16Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED_SHORT& i_unshort);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED_SHORT& o_unshort);

inline std::wostream& operator << (std::wostream& o_Stream, const UNSIGNED_SHORT& i_unshort);


//##ModelId=39AD95FF0182
struct UNSIGNED
{
	//##ModelId=39AD95FF0198
	UINT32 m_value;
	
	//##ModelId=39AD95FF01B7
	UNSIGNED(UINT32 i_value = 0){m_value = i_value;};
	//##ModelId=39AD95FF01B5
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

		_ultot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD95FF01AD
	void operator = (UINT32 i_other)
		{m_value = i_other;}


	//##ModelId=39AD95FF01AB
	operator UINT32() const
	{
		return m_value;
	}

	//##ModelId=39AD95FF01A2
	const unsigned int IntNetworkLength() const {return StaticInt32Size ;};
	//##ModelId=39AD95FF01A0
	const unsigned int ExtNetworkLength() const {return StaticInt32Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED& i_unsigned);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED& o_unsigned);


//##ModelId=39AD960D0128
struct UNSIGNED_LONG
{
	//##ModelId=39AD960D013E
	UINT32 m_value;
	
	//##ModelId=39AD960D0165
	UNSIGNED_LONG(UINT32 i_value = 0){m_value = i_value;};
	//##ModelId=39AD960D015C
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

		_ultot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD960D015A
	void operator = (UINT32 i_other)
		{m_value = i_other;}


	//##ModelId=39AD960D0153
	operator UINT32() const
	{
		return m_value;
	}

	//##ModelId=39AD960D0151
	const unsigned int IntNetworkLength() const {return StaticInt32Size ;};
	//##ModelId=39AD960D0147
	const unsigned int ExtNetworkLength() const {return StaticInt32Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED_LONG& i_unsignedlong);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED_LONG& o_unsignedlong);

inline std::wostream& operator << (std::wostream& o_Stream, const UNSIGNED& i_unsigned);


//##ModelId=39AD961901EE
struct UNSIGNED_DOUBLE
{
	//##ModelId=39AD96190204
	UINT64 m_value;
	
	//##ModelId=39AD9619022A
	UNSIGNED_DOUBLE(UINT64 i_value = 0){m_value = i_value;};
	//##ModelId=39AD96190222
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[25];

		_ui64tot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD96190220
	void operator = (UINT64 i_other)
		{m_value = i_other;}

	
	//##ModelId=39AD96190219
	operator UINT64() const
	{
		return m_value;
	}

	//##ModelId=39AD96190217
	const unsigned int IntNetworkLength() const {return StaticInt64Size ;};
	//##ModelId=39AD9619020C
	const unsigned int ExtNetworkLength() const {return StaticInt64Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED_DOUBLE& i_undouble);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED_DOUBLE& o_undouble);



//##ModelId=39AD95FE0361
struct INTEGER_BYTE
{
	//##ModelId=39AD95FE0378
	 _int8 m_value;
	
	//##ModelId=39AD95FE0394
	INTEGER_BYTE( _int8 i_value = 0){m_value = i_value;};
	//##ModelId=39AD95FE038C
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

		_itot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD95FE038A
	void operator = ( _int8 i_other)
		{m_value = i_other;}

	operator _int8()
	{
		return m_value;
	}

	//##ModelId=39AD95FE0382
	const unsigned int IntNetworkLength() const {return StaticInt8Size ;};
	//##ModelId=39AD95FE0380
	const unsigned int ExtNetworkLength() const {return StaticInt8Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const INTEGER_BYTE& i_intbyte);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,INTEGER_BYTE& o_intbyte);



//##ModelId=39AD96180192
struct INTEGER_SHORT
{
	//##ModelId=39AD961801A8
	 _int16 m_value;
	
	//##ModelId=39AD961801C7
	INTEGER_SHORT( _int16 i_value = 0){m_value = i_value;};
	//##ModelId=39AD961801C5
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

		_itot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD961801BD
	void operator = ( _int16 i_other)
		{m_value = i_other;}

	operator _int16() const
	{
		return m_value;
	}

	//##ModelId=39AD961801BB
	const unsigned int IntNetworkLength() const {return StaticInt16Size ;};
	//##ModelId=39AD961801B0
	const unsigned int ExtNetworkLength() const {return StaticInt16Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const INTEGER_SHORT& i_intshort);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,INTEGER_SHORT& o_intshort);



//##ModelId=39AD95F70357
struct INTEGER
{
	//##ModelId=39AD95F7036D
	 _int32 m_value;
	
	//##ModelId=39AD95F70389
	INTEGER( _int32 i_value = 0){m_value = i_value;};
	//##ModelId=39AD95F70381
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

		_itot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD95F7037F
	void operator = ( _int32 i_other)
		{m_value = i_other;}

	operator _int32()
	{
		return m_value;
	}

	//##ModelId=39AD95F70377
	const unsigned int IntNetworkLength() const {return StaticInt32Size ;};
	//##ModelId=39AD95F70375
	const unsigned int ExtNetworkLength() const {return StaticInt32Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const INTEGER& i_int);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,INTEGER& o_int);



//##ModelId=39AD96170371
struct INTEGER_DOUBLE
{
	//##ModelId=39AD9617039A
	 _int64 m_value;
	
	//##ModelId=39AD961703B8
	INTEGER_DOUBLE( _int64 i_value = 0){m_value = i_value;};
	//##ModelId=39AD961703B0
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[25];

		_i64tot (m_value, buffer, 10);
		return buffer;
	}

	//##ModelId=39AD961703AE
	void operator = ( _int64 i_other)
		{m_value = i_other;}

	operator _int64() const
	{
		return m_value;
	}

	//##ModelId=39AD961703A6
	const unsigned int IntNetworkLength() const {return StaticInt64Size ;};
	//##ModelId=39AD961703A4
	const unsigned int ExtNetworkLength() const {return StaticInt64Size ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const INTEGER_DOUBLE& i_intdouble);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,INTEGER_DOUBLE& o_intdouble);




//##ModelId=39AD961803AF
struct REAL
{
	//##ModelId=39AD961803C3
	 float m_value;
	
	//##ModelId=39AD961803DA
	REAL( float i_value = 0){m_value = i_value;};
	//##ModelId=39AD961803D8
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

#if defined(LINUX_FIX)
        swprintf(buffer, sizeof(buffer)/sizeof(TCHAR), _T("%g"), m_value);
#else
		_stprintf(buffer, _T("%g"), m_value);
#endif
		return buffer;
	}

	//##ModelId=39AD961803D1
	void operator = ( float i_other)
		{m_value = i_other;}


	//##ModelId=39AD961803CF
	operator float() const
	{
		return m_value;
	}

	//##ModelId=39AD961803CD
	const unsigned int IntNetworkLength() const {return StaticFloatSize ;};
	//##ModelId=39AD961803C4
	const unsigned int ExtNetworkLength() const {return StaticFloatSize ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const REAL& i_float);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,REAL& o_float);



//##ModelId=39AD960C0311
struct REAL_DOUBLE
{
	//##ModelId=39AD960C031D
	 double m_value;
	
	//##ModelId=39AD960C033C
	REAL_DOUBLE( double i_value = 0){m_value = i_value;};
	//##ModelId=39AD960C033A
	std::wstring ReadableForm() const
	{	
		TCHAR	buffer[20];

#if defined(LINUX_FIX)
        swprintf(buffer, sizeof(buffer)/sizeof(TCHAR), _T("%g"), m_value);
#else
		_stprintf(buffer, _T("%g"), m_value);
#endif
		return buffer;
	}

	//##ModelId=39AD960C0332
	void operator = ( double i_other)
		{m_value = i_other;}


	//##ModelId=39AD960C0330
	operator double() const
	{
		return m_value;
	}

	//##ModelId=39AD960C0328
	const unsigned int IntNetworkLength() const {return StaticDoubleSize ;};
	//##ModelId=39AD960C0326
	const unsigned int ExtNetworkLength() const {return StaticDoubleSize ;};
};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const REAL_DOUBLE& i_double);
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, REAL_DOUBLE& o_double);


// Strings
typedef std::wstring			CHARACTERSTRING;


//##ModelId=39AD9606029B
struct GP_COMMAND_FAIL_REASON
{
	enum E_COMMAND_FAIL_REASON
	{
		e_NoFailReason				= 0,
		e_UnknownCommand			= 1,
		e_InvalidParameter			= 2,
		e_WrongLength				= 3,
		e_InvalidContext			= 4,
		e_CannotForward				= 5,
		e_unknownCommandFailReason	= 6
	} m_COMMAND_FAIL_REASON;

	//##ModelId=39AD960602CD
	GP_COMMAND_FAIL_REASON(){m_COMMAND_FAIL_REASON = e_NoFailReason;};
	//##ModelId=39AD960602CE
	GP_COMMAND_FAIL_REASON(E_COMMAND_FAIL_REASON i_CommandFailReason)
										{m_COMMAND_FAIL_REASON = i_CommandFailReason;};

	//##ModelId=39AD960602D0
	GP_COMMAND_FAIL_REASON(std::wstring i_string);
	//##ModelId=39AD960602C6
	std::wstring ReadableForm() const;
	
	//##ModelId=39AD960602BD
	GP_COMMAND_FAIL_REASON& operator = (const GP_COMMAND_FAIL_REASON i_fail)
	{m_COMMAND_FAIL_REASON = i_fail.m_COMMAND_FAIL_REASON;return *this;};

	//##ModelId=39AD960602C4
	GP_COMMAND_FAIL_REASON& operator = (const E_COMMAND_FAIL_REASON i_fail)
		{m_COMMAND_FAIL_REASON = i_fail;return *this;}

	//##ModelId=39AD960602B2
	bool operator == (const GP_COMMAND_FAIL_REASON& i_fail) const
		{return m_COMMAND_FAIL_REASON== i_fail.m_COMMAND_FAIL_REASON;};

	//##ModelId=39AD960602BA
	bool operator == (const GP_COMMAND_FAIL_REASON::E_COMMAND_FAIL_REASON& i_fail) const
		{return m_COMMAND_FAIL_REASON== i_fail;}


	//##ModelId=39AD960602B0
	const unsigned int IntNetworkLength() const {return StaticInt16Size;};
	//##ModelId=39AD960602A6
	const unsigned int ExtNetworkLength() const {return StaticInt16Size;}; 

};
inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_COMMAND_FAIL_REASON&  i_CommandFReason);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_COMMAND_FAIL_REASON& o_CommandFReason);

inline std::wostream& operator << (std::wostream& out, const GP_COMMAND_FAIL_REASON& gp_command_fail_reason);


//##ModelId=39AD96170191
struct  GP_COMMAND_ID
{
	enum EGP_COMMAND_ID
	{
		e_StartSU						= 30000,
		e_StopSU						= 30001,
		e_AbortSU						= 30002,
		e_EnableLink					= 30003,
		e_DisableLink					= 30004,
		e_SetConnectMode				= 30005,
		e_ChangeDU						= 30006,
		e_ChangeMonitoringFreq			= 30007,
		e_ChangeEventFilter				= 30008,
		e_ChangeTraceLevel				= 30009,
		e_ChangeDCPChannelAlarmsMode	= 30010,
		e_EnableCentralCommanding		= 30011,
		e_ForceToLocal					= 30012,
		e_GiveControl					= 30013,
		e_PurgeALQ						= 30014,
		e_EnableFTPLink					= 30015,
		e_DisableFTPLink				= 30016,
		e_Unknown						= 30017,
	} m_COMMAND_Id; 

	//##ModelId=39AD961701C4
	GP_COMMAND_ID ():m_COMMAND_Id(e_StartSU){};
	//##ModelId=39AD961701C5
	GP_COMMAND_ID (EGP_COMMAND_ID i_COMMAND_Id):m_COMMAND_Id(i_COMMAND_Id){};
	
	//##ModelId=39AD961701BC
	void operator = (GP_COMMAND_ID rhs){ m_COMMAND_Id = rhs.m_COMMAND_Id; };
	//##ModelId=39AD961701B9
	bool operator < (const GP_COMMAND_ID rhs) const { return m_COMMAND_Id < rhs.m_COMMAND_Id;};
	//##ModelId=39AD961701B1
	bool operator == (const GP_COMMAND_ID rhs) const 
		{return m_COMMAND_Id == rhs.m_COMMAND_Id;};
	//##ModelId=39AD961701B0
	operator UNSIGNED(){return UNSIGNED(m_COMMAND_Id);};

	//##ModelId=39AD961701A9
	const unsigned int IntNetworkLength() const {return StaticInt32Size;};
	//##ModelId=39AD961701A7
	const unsigned int ExtNetworkLength() const {return StaticInt32Size;}; 

	//##ModelId=39AD961701A5
	std::wstring ReadableForm() const;
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_COMMAND_ID&  i_COMMAND_Id);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_COMMAND_ID& i_COMMAND_Id);

//##ModelId=39AD95E50121
struct  GP_COMMAND_VERI_ServiceSubTypes
{
	enum EGP_COMMAND_VERI_ServiceSubTypes
	{
		e_AcceptanceSuccess			= 0,
		e_AcceptanceFail			= 1,
		e_StartSuccess				= 2,
		e_StartFail					= 3,
		e_ProgressSuccess			= 4,
		e_ProgressFail				= 5,
		e_CompletionSuccess			= 6,
		e_CompletionFail			= 7
	} m_Value; 

	//##ModelId=39AD95E50154
	GP_COMMAND_VERI_ServiceSubTypes ():m_Value(e_AcceptanceSuccess){};
	//##ModelId=39AD95E50155
	GP_COMMAND_VERI_ServiceSubTypes (EGP_COMMAND_VERI_ServiceSubTypes i_Value)
		:	m_Value(i_Value){};
	
	//##ModelId=39AD95E5014B
	void operator = (GP_COMMAND_VERI_ServiceSubTypes rhs){ m_Value = rhs.m_Value; };
	//##ModelId=39AD95E5014D
	GP_COMMAND_VERI_ServiceSubTypes& operator = (const EGP_COMMAND_VERI_ServiceSubTypes i_Value)
	{
		m_Value = i_Value; 
		return *this;
	}

	//##ModelId=39AD95E50142
	bool operator < (const GP_COMMAND_VERI_ServiceSubTypes rhs) const { return m_Value < rhs.m_Value;};
	//##ModelId=39AD95E5013F
	bool operator == (const GP_COMMAND_VERI_ServiceSubTypes rhs) const 
		{return m_Value == rhs.m_Value;};

	//##ModelId=39AD95E50139
	operator UNSIGNED_BYTE (){return _int8 (m_Value);};

	//##ModelId=39AD95E50137
	const unsigned int IntNetworkLength() const {return StaticInt8Size;};
	//##ModelId=39AD95E50135
	const unsigned int ExtNetworkLength() const {return StaticInt8Size;}; 

	//##ModelId=39AD95E5012C
	std::wstring ReadableForm() const;
};

//inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_COMMAND_VERI_ServiceSubTypes&  i_Value);
//inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_COMMAND_VERI_ServiceSubTypes& i_Value);


typedef UNSIGNED			GP_EVENT_ID;

//##ModelId=39AD961001FF
struct GP_EVENT_SEVERITY
{		 
	enum ESeverity
	{
		e_LogOnly				= 0,
		e_Warning				= 1,
		e_Error					= 2,
		e_Alarm					= 3,
		e_Maintenance			= 4,
		e_UnkownSeverity		= 5,
	} m_Severity;

	//##ModelId=39AD96100235
	GP_EVENT_SEVERITY(){m_Severity=e_LogOnly;};
	//##ModelId=39AD9610023B
	GP_EVENT_SEVERITY(ESeverity i_Severity){m_Severity=i_Severity;};
	//##ModelId=39AD9610023D
	GP_EVENT_SEVERITY(std::wstring i_string);

	//##ModelId=39AD96100231
	void operator = (GP_EVENT_SEVERITY rhs){ m_Severity = rhs.m_Severity; };
	//##ModelId=39AD96100229
	bool operator < (GP_EVENT_SEVERITY rhs) const { return m_Severity < rhs.m_Severity;};


	//##ModelId=39AD96100233
	GP_EVENT_SEVERITY& operator = (const ESeverity i_event)
		{m_Severity = i_event;return *this;};

	//##ModelId=39AD9610021E
	bool operator == (const GP_EVENT_SEVERITY& i_event) const
		{return m_Severity== i_event.m_Severity;};

	//##ModelId=39AD96100221
	bool operator == (const GP_EVENT_SEVERITY::ESeverity& i_event) const
		{return m_Severity== i_event;}

	//##ModelId=39AD96100216
	std::wstring ReadableForm() const;
	//##ModelId=39AD96100214
	const unsigned int IntNetworkLength() const {return StaticInt8Size;}; 
	//##ModelId=39AD9610020A
	const unsigned int ExtNetworkLength() const {return StaticInt8Size;}; 
};
	
inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_EVENT_SEVERITY& i_EventSeverity);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_EVENT_SEVERITY& o_EventSeverity);

inline std::wostream& operator << (std::wostream& out, const GP_EVENT_SEVERITY& gp_event_severity);


//##ModelId=39AD9607018E
struct GP_FAC_ENV
{
	enum EGP_FAC_ENV
	{
		e_NoEnvironment			= 0,
		e_Operational_On_line	= 1,
		e_Operational_Off_line	= 2,
		e_Validation_On_line	= 254,
		e_Validation_Offline	= 255,
		e_Backup_On_line		= 128,
		e_Backup_Off_line		= 129,
		e_Development			= 130,
		e_ConfigurationControl	= 131,
		e_UnknownEnvironment	= 132
	}	m_Fac_Env;

	//##ModelId=39AD960701B8
	GP_FAC_ENV():m_Fac_Env(e_NoEnvironment){};
	//##ModelId=39AD960701B9
	GP_FAC_ENV(EGP_FAC_ENV i_Fac_env):m_Fac_Env(i_Fac_env){};
	//##ModelId=39AD960701C1
	GP_FAC_ENV(std::wstring i_string);
	
	//##ModelId=39AD960701B6
	void operator = (GP_FAC_ENV rhs){ m_Fac_Env = rhs.m_Fac_Env; };
	
	//##ModelId=39AD960701AE
	bool operator < (const GP_FAC_ENV rhs) const 
		{return m_Fac_Env < rhs.m_Fac_Env;};
	
	//##ModelId=39AD960701A6
	bool operator == (const GP_FAC_ENV rhs) const 
		{return m_Fac_Env == rhs.m_Fac_Env;};

	
	//##ModelId=39AD960701A4
	std::wstring ReadableForm() const;
	//##ModelId=39AD960701A2
	const unsigned int IntNetworkLength() const 	{return StaticInt8Size;}; 
	//##ModelId=39AD96070199
	const unsigned int ExtNetworkLength() const 	{return StaticInt8Size;}; 
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_FAC_ENV&  i_FacEnv);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_FAC_ENV& o_FacEnv);

inline std::wostream& operator << (std::wostream& out, const GP_FAC_ENV& gp_fac_env);

//##ModelId=39AD95EA00C4
struct  GP_FAC_ID
{
	enum EGP_FAC_ID
	{
		e_NoFacility			= 0,
		e_BRGS					= 1,
		e_CF					= 2,
		e_DADF					= 3,
		e_IMPF					= 4,
		e_MARF					= 5,
		e_MPEF					= 6,
		e_PGS					= 7,
		e_SSF					= 8,
		e_RGS					= 32,
		e_SAF_NWC				= 64,
		e_SAF_OSI				= 65,
		e_SAF_Ozone				= 66,
		e_SAF_4					= 67,
		e_ECMWF					= 128,
		e_EGSE					= 129,
		e_GGSPS					= 130,
		e_RTH					= 131,
		e_IQGSE					= 132,
		e_RFS					= 133,
		e_SGS					= 254,	//	TBD
		e_UnknownFacility		= 255	//	TBD
	} m_Fac_Id; 

	//##ModelId=39AD95EA00F8
	GP_FAC_ID():m_Fac_Id(e_DADF){};
	//##ModelId=39AD95EA00F9
	GP_FAC_ID(EGP_FAC_ID i_Fac_Id):m_Fac_Id(i_Fac_Id){};
	//##ModelId=39AD95EA0100
	GP_FAC_ID(std::wstring i_string);
	
	//##ModelId=39AD95EA00F6
	void operator = (GP_FAC_ID rhs){ m_Fac_Id = rhs.m_Fac_Id; };
	//##ModelId=39AD95EA00EE
	bool operator < (const GP_FAC_ID rhs) const { return m_Fac_Id < rhs.m_Fac_Id;};
	//##ModelId=39AD95EA00E6
	bool operator > (const GP_FAC_ID rhs) const { return m_Fac_Id > rhs.m_Fac_Id;};
	//##ModelId=39AD95EA00E3
	bool operator == (const GP_FAC_ID rhs) const 
		{return m_Fac_Id == rhs.m_Fac_Id;}

	//##ModelId=39AD95EA00DB
	std::wstring ReadableForm() const;
	//##ModelId=39AD95EA00D9
	const unsigned int IntNetworkLength() const {return StaticInt8Size;}; 
	//##ModelId=39AD95EA00CF
	const unsigned int ExtNetworkLength() const {return StaticInt8Size;}; 
};


inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_FAC_ID&  i_FacId);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_FAC_ID& o_FacId);
inline std::wostream& operator << (std::wostream& out, const GP_FAC_ID& gp_fac_id);



typedef UNSIGNED			GP_MONITORING_ID;


//##ModelId=39AD95F601B1
struct GP_SC_CHAN_ID
{
	enum E_SC_CHAN_ID
	{
		e_NO_CHANNEL				= 0,
		e_VIS_0_6					= 1,
		e_VIS_0_8					= 2,
		e_IR_1_6					= 3,
		e_IR_3_9					= 4,
		e_WV_6_2					= 5,
		e_WV_7_3					= 6,
		e_IR_8_7					= 7,
		e_IR_9_7					= 8,
		e_IR_10_8					= 9,
		e_IR_12_0					= 10,
		e_IR_13_4					= 11,
		e_HRV						= 12,
		e_UnknownChannel			= 13
	} m_SC_CHAN_ID;

	//##ModelId=39AD95F601E6
	GP_SC_CHAN_ID(){m_SC_CHAN_ID = e_NO_CHANNEL;};
	//##ModelId=39AD95F601ED
	GP_SC_CHAN_ID(E_SC_CHAN_ID i_SCChanId){m_SC_CHAN_ID = i_SCChanId;};
	//##ModelId=39AD95F601EF
	GP_SC_CHAN_ID(std::wstring i_string);

	//##ModelId=39AD95F601DD
	GP_SC_CHAN_ID& operator = (const GP_SC_CHAN_ID i_SCchan)
		{m_SC_CHAN_ID = i_SCchan.m_SC_CHAN_ID;return *this;};

	//##ModelId=39AD95F601E4
	GP_SC_CHAN_ID& operator = (const E_SC_CHAN_ID i_SCchan)
		{m_SC_CHAN_ID = i_SCchan;return *this;}

	//##ModelId=39AD95F601D0
	bool operator == (const GP_SC_CHAN_ID& i_SCchan) const
		{return m_SC_CHAN_ID== i_SCchan.m_SC_CHAN_ID;};

	//##ModelId=39AD95F601DA
	bool operator == (const GP_SC_CHAN_ID::E_SC_CHAN_ID& i_SCchan) const
		{return m_SC_CHAN_ID== i_SCchan;}
	
	//##ModelId=39AD95F601C7
	std::wstring ReadableForm() const;
	//##ModelId=39AD95F601C5
	const unsigned int IntNetworkLength() const {return StaticInt8Size;}; 
	//##ModelId=39AD95F601BC
	const unsigned int ExtNetworkLength() const {return StaticInt8Size;}; 
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SC_CHAN_ID&  i_SCChanId);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_SC_CHAN_ID& o_SCChanId);
inline std::wostream& operator << (std::wostream& out, const GP_SC_CHAN_ID& gp_sc_chan_id);


//##ModelId=39AD960201A5
struct GP_SC_DETECT_ID
{
	enum E_SC_DETECT_ID
	{	
		e_No_Detector				= 0,
		e_VIS06SouthDetector		= 1,
		e_VIS06MiddleDetector		= 2,
		e_VIS06NorthDetector		= 3,
		e_VIS08SouthDetector		= 4,
		e_VIS08MiddleDetector		= 5,
		e_VIS08NorthDetector		= 6,
		e_IR16SouthDetector			= 7,
		e_IR16MiddleDetector		= 8,
		e_IR16NorthDetector			= 9,
		e_IR39SouthDetector			= 10,
		e_IR39MiddleDetector		= 11,
		e_IR39NorthDetector			= 12,
		e_WV62SouthDetector			= 13,
		e_WV62MiddleDetector		= 14,
		e_WV62NorthDetector			= 15,	
		e_WV73SouthDetector			= 16,	
		e_WV73MiddleDetector		= 17,
		e_WV73NorthDetector			= 18,
		e_IR87SouthDetector			= 19,
		e_IR87MiddleDetector		= 20,
		e_IR87NorthDetector			= 21,
		e_IR97SouthDetector			= 22,
		e_IR97MiddleDetector		= 23,
		e_IR97NorthDetector			= 24,
		e_IR108SouthDetector		= 25,
		e_IR108MiddleDetector		= 26,
		e_IR108NorthDetector		= 27,	
		e_IR120SouthDetector		= 28,
		e_IR120MiddleDetector		= 29,
		e_IR120NorthDetector		= 30,
		e_IR134SouthDetector		= 31,
		e_IR134MiddleDetector		= 32,
		e_IR134NorthDetector		= 33,
		e_HRVSouthernDetector		= 34,
		e_HRV1AboveSouthernDetector	= 35,	
		e_HRV2AboveSouthernDetector	= 36,
		e_HRV3AboveSouthernDetector	= 37,
		e_HRV4AboveSouthernDetector	= 38,		
		e_HRV3BelowNorthernDetector	= 39,
		e_HRV2BelowNorthernDetector	= 40,
		e_HRV1BelowNorthernDetector	= 41,
		e_HRVNorthernDetector		= 42,
		e_UnknownDetector			= 43
	}	m_SC_DETECT_ID;

	//##ModelId=39AD960201D9
	GP_SC_DETECT_ID(){m_SC_DETECT_ID = e_No_Detector;};
	//##ModelId=39AD960201DA
	GP_SC_DETECT_ID(E_SC_DETECT_ID i_SCDetectId){m_SC_DETECT_ID = i_SCDetectId;};
	//##ModelId=39AD960201E2
	GP_SC_DETECT_ID(std::wstring i_string);

	//##ModelId=39AD960201D0
	GP_SC_DETECT_ID& operator = (const GP_SC_DETECT_ID i_SCdetect)
		{m_SC_DETECT_ID = i_SCdetect.m_SC_DETECT_ID;return *this;};

	//##ModelId=39AD960201D7
	GP_SC_DETECT_ID& operator = (const E_SC_DETECT_ID i_SCdetect)
		{m_SC_DETECT_ID = i_SCdetect;return *this;}

	//##ModelId=39AD960201C5
	bool operator == (const GP_SC_DETECT_ID& i_SCdetect) const
	{return m_SC_DETECT_ID== i_SCdetect.m_SC_DETECT_ID;};

	//##ModelId=39AD960201CD
	bool operator == (const GP_SC_DETECT_ID::E_SC_DETECT_ID& i_SCdetect) const
		{return m_SC_DETECT_ID== i_SCdetect;}

	//##ModelId=39AD960201C3
	 std::wstring ReadableForm() const;
	//##ModelId=39AD960201BB
	 const unsigned int IntNetworkLength() const {return StaticInt8Size;}; 
	//##ModelId=39AD960201B9
	const unsigned int ExtNetworkLength() const {return StaticInt8Size;}; 
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SC_DETECT_ID&  i_SCDetectId);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_SC_DETECT_ID& o_SCDetectId);

inline std::wostream& operator << (std::wostream& out, const GP_SC_DETECT_ID& gp_sc_detect_id);


//##ModelId=39AD95FA03A2
struct GP_SC_ID
{
	enum E_SC_ID
	{	
		e_NoSpacecraft	= 0,
		e_METEOSAT_3	= 16,
		e_METEOSAT_4	= 19,
		e_METEOSAT_5	= 20, 
		e_METEOSAT_6	= 21,
		e_MTP_1			= 150,
		e_MTP_2			= 151,
		e_MSG_1			= 321,
		e_MSG_2			= 322,
		e_MSG_3			= 323,
		e_MSG_4			= 324,
		e_METOP_1		= 11,
		e_METOP_2		= 12,
		e_METOP_3		= 13,
		e_NOAA_12		= 17012,
		e_NOAA_13		= 17013,
		e_NOAA_14		= 17014,
		e_NOAA_15		= 17015,
		e_GOES_7		= 18007,
		e_GOES_8		= 18008,
		e_GOES_9		= 18009,
		e_GOES_10		= 18010,
		e_GOMS_1		= 19001,
		e_GOMS_2		= 19002,
		e_GOMS_3		= 19003,
		e_GMS_4			= 20004,
		e_GMS_5			= 20005,
		e_GMS_6			= 20006,
		e_MTSAT_1		= 21001,
		e_MTSAT_2		= 21002,
		e_UnknownSpacecraft	= 21003
	} m_SC_ID;

	//##ModelId=39AD95FA03CA
	GP_SC_ID(E_SC_ID i_SCId = e_NoSpacecraft){m_SC_ID = i_SCId;};
	//##ModelId=39AD95FA03CC
	GP_SC_ID(std::wstring i_string);

	//##ModelId=39AD95FA03C1
	bool operator == (const GP_SC_ID i_SCId) const 
		{return m_SC_ID == i_SCId.m_SC_ID;};
	
	//##ModelId=39AD95FA03BA
	GP_SC_ID& operator = (const GP_SC_ID i_SCId)
		{m_SC_ID = i_SCId.m_SC_ID;return *this;};
		
	//##ModelId=39AD95FA03B8
	const unsigned int IntNetworkLength () const { return StaticInt16Size;}
	//##ModelId=39AD95FA03B6
	const unsigned int ExtNetworkLength () const { return StaticInt16Size;}

	//##ModelId=39AD95FA03AD
	std::wstring ReadableForm 
	(
	)
	const
	// Returns:		Corresponding GP_SC_NAME value.
	// Exceptions:	No specific ones.
	{
		switch (m_SC_ID)
		{
			case e_NoSpacecraft	: return std::wstring(_T("NoSpacecraft"));
			case e_METEOSAT_3	: return std::wstring(_T("METEOSAT3"));
			case e_METEOSAT_4	: return std::wstring(_T("METEOSAT4"));
			case e_METEOSAT_5	: return std::wstring(_T("METEOSAT5"));
			case e_METEOSAT_6	: return std::wstring(_T("METEOSAT6"));
			case e_MTP_1		: return std::wstring(_T("MTP1"));
			case e_MTP_2		: return std::wstring(_T("MTP2"));
			case e_MSG_1		: return std::wstring(_T("MSG1"));
			case e_MSG_2		: return std::wstring(_T("MSG2"));
			case e_MSG_3		: return std::wstring(_T("MSG3"));
			case e_MSG_4		: return std::wstring(_T("MSG4"));
			case e_METOP_1		: return std::wstring(_T("METOP1"));
			case e_METOP_2		: return std::wstring(_T("METOP2"));
			case e_METOP_3		: return std::wstring(_T("METOP3"));
			case e_NOAA_12		: return std::wstring(_T("NOAA12"));
			case e_NOAA_13		: return std::wstring(_T("NOAA13"));
			case e_NOAA_14		: return std::wstring(_T("NOAA14"));
			case e_NOAA_15		: return std::wstring(_T("NOAA15"));
			case e_GOES_7		: return std::wstring(_T("GOES7"));
			case e_GOES_8		: return std::wstring(_T("GOES8"));
			case e_GOES_9		: return std::wstring(_T("GOES9"));
			case e_GOES_10		: return std::wstring(_T("GOES10"));
			case e_GOMS_1		: return std::wstring(_T("GOMS1"));
			case e_GOMS_2		: return std::wstring(_T("GOMS2"));
			case e_GOMS_3		: return std::wstring(_T("GOMS3"));
			case e_GMS_4		: return std::wstring(_T("GMS4"));
			case e_GMS_5		: return std::wstring(_T("GMS5"));
			case e_GMS_6		: return std::wstring(_T("GMS6"));
			case e_MTSAT_1		: return std::wstring(_T("MTSAT1"));
			case e_MTSAT_2		: return std::wstring(_T("MTSAT2"));
		}
		return std::wstring(_T("UnknownSpacecraft"));
	}
};


inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SC_ID&  i_SCId);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_SC_ID& o_SCId);

inline std::wostream& operator << (std::wostream& out, const GP_SC_ID& gp_sc_id);


//##ModelId=39AD95F70177
struct GP_SU_ID
{
	enum E_SU_ID
	{
		e_NULL_SU	= 0,
		e_DCPE		= 30000,
		e_DDFC		= 30001,
		e_DSRX		= 30002,
		e_DSPL		= 30003,
		e_DSPH		= 30004,
		e_DSPM		= 30005,
		e_DSSP		= 30006,
		e_SYSE		= 30007,
		e_RAE		= 30008,
		e_CMSE		= 30009,
		e_MMI		= 30010
	}				m_SU_ID;

	//##ModelId=39AD95F701AB
	GP_SU_ID ():m_SU_ID(e_NULL_SU) {};
	//##ModelId=39AD95F701AC
	GP_SU_ID (E_SU_ID i_SU_ID): m_SU_ID (i_SU_ID)  {};
	//##ModelId=39AD95F701B4
	GP_SU_ID(std::wstring i_string);
	//##ModelId=39AD95F701B6
	GP_SU_ID (UINT32 i_SU_ID)
	{
		m_SU_ID = static_cast<GP_SU_ID::E_SU_ID> (i_SU_ID);
	}

	//##ModelId=39AD95F701AA
	operator GP_SU_ID (){ return m_SU_ID; }

	// Copy a SU Id
	//##ModelId=39AD95F701A3
	void operator = (GP_SU_ID rhs){ m_SU_ID = rhs.m_SU_ID; }

	//##ModelId=39AD95F701A0
	bool operator < (const GP_SU_ID rhs) const 
		{return m_SU_ID < rhs.m_SU_ID;}

	//##ModelId=39AD95F70198
	bool operator == (const GP_SU_ID rhs) const 
		{return m_SU_ID == rhs.m_SU_ID;}
	
	// A string representation of the SU id that can be used in the constructor
	// to obtain the same value.
	//##ModelId=39AD95F70196
	inline std::wstring SUName() const;
	//##ModelId=39AD95F7018E
	std::wstring ReadableForm() const { return SUName(); }

	//##ModelId=39AD95F7018C
	const unsigned int IntNetworkLength() const {return StaticInt32Size;}; 
	//##ModelId=39AD95F70183
	const unsigned int ExtNetworkLength() const {return StaticInt32Size;}; 

	//##ModelId=39AD95F70182
	int get() {return int(m_SU_ID);}
};
inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SU_ID& i_SUId);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,GP_SU_ID& o_SUId);


inline std::wostream& operator << (std::wostream& out, const GP_SU_ID& gp_su_id);

//##ModelId=39AD95E60245
struct GP_SVCE_ID
{
	enum E_SVCE_ID
	{
		//	GP_SVCE_ID values as defined in GSDS, Volume F
		e_NoService						= 0,
		e_15_Data						= 1,	//	TCP/IP 
		e_CONTROL						= 5,	//	TCP/IP
		e_DCP							= 6,	//	TCP/IP 
		e_GTS_DATA_IN					= 10,	//	TCP/IP 
		e_GTS_DATA_OUT					= 11,	//	TCP/IP 
		e_HRIT_DISSEMINATION			= 12,	//	TCP/IP 
		e_LRIT_DISSEMINATION			= 13,	//	TCP/IP 
		e_METPRODUCT					= 14,	//	FTP
		e_MONITORING					= 15,	//	TCP/IP
		e_RETRIEVAL						= 17,	//	FTP
		e_SGS							= 22,	//	FTP
		e_UTC_CORR						= 23,	//	TCP/IP
		e_ECMWF							= 30,	//	

		//	DADF Internal GP_SVCE_IDs (where required).  If one of the
		//	general service IDs are also appropriate for internal DADF
		//	usage, then it is used for internal use as well.

		//	DCP Messages / Bulletins to DDFC
		e_DCPE_DDFC						= 3000,

		//	DCPE Products (raw DCP Messages and GTS bulletins) from DDFC to DSRX
		e_DDFC_DSRX_dcp					= 3001,

		//	RTH Products (GTS messages) from DDFC to DSRX
		e_DDFC_DSRX_gts					= 3002,

		e_DSRX_DSPH = 3003,						// DSRX to DSPH (requests to process HRIT files)
		e_DSRX_DSPL = 3004,						// DSRX to DSPL (requests to process LRIT files)
		e_DSRX_DSSP = 3005,						// DSRX to DSSP (input expectation information,
												//               input file status data,
												//               overview status messages)
		e_DSPH_DSSP = 3006,						// DSPH to DSSP (HRIT file status data)
		e_DSPL_DSSP = 3007,						// DSPL to DSSP (LRIT file status data)
		e_DSPM_DSSP = 3008,						// DSPM to DSSP (HRIT/LRIT reception monitoring
												//               data from US)
		e_DSPM_US_MonitoringDataHRIT = 3009,	// US   to DSPM (HRIT Monitoring Data)
		e_DSPM_US_MonitoringDataLRIT = 3010,	// US   to DSPM (LRIT Monitoring Data)
		e_DSPM_US_FileTransferHRIT   = 3011,	// US   to DSPM (HRIT File Transfer)
		e_DSPM_US_FileTransferLRIT   = 3012,	// US   to DSPM (LRIT File Transfer)

		//	SYSE to DCPE (Control Link)
		e_SYSE_DCPE_Control = 3013,

		//	SYSE to DDFC (Control Link)
		e_SYSE_DDFC_Control = 3014,

		//	SYSE to DSRX (Control Link)
		e_SYSE_DSRX_Control	= 3015,
		
		//	SYSE to DSPL (Control Link)
		e_SYSE_DSPL_Control = 3016,

		//	SYSE to DSPH (Control Link)
		e_SYSE_DSPH_Control	= 3017,

		//	SYSE to DSSP (Control Link)
		e_SYSE_DSSP_Control = 3018,

		//	SYSE to DSPM (Control Link)
		e_SYSE_DSPM_Control = 3019,

		//	SYSE to DCPE (Monitoring Link)
		e_SYSE_DCPE_Monitoring = 3020,

		//	SYSE to DDFC (Monitoring Link)
		e_SYSE_DDFC_Monitoring = 3021,

		//	SYSE to DSRX (Monitoring Link)
		e_SYSE_DSRX_Monitoring = 3022,
		
		//	SYSE to DSPL (Monitoring Link)
		e_SYSE_DSPL_Monitoring = 3023,

		//	SYSE to DSPH (Monitoring Link)
		e_SYSE_DSPH_Monitoring = 3024,

		//	SYSE to DSSP (Monitoring Link)
		e_SYSE_DSSP_Monitoring = 3025,

		//	SYSE to DSPM (Monitoring Link)
		e_SYSE_DSPM_Monitoring = 3026,


		//	***   Other FTP Services   ***

		e_DDFC_DSRX_sgs			= 3028,
		e_DDFC_DSRX_mpef		= 3029,

		//	DDFC to OFL
		e_OFL_DDFC				= 3030,
		// link for administrative messages 
		// send from an MMI to DDFC
		e_DDFC_MMI_AdminMessages,
		
		//	Absorb Status Data from other DISE SUs
		e_DSSP_StatusDataInput	= 3032,


		// MMI Service Ids.
		e_MMI_Service			= 3500,

		e_SYSE_OverviewStatus,
		e_SYSE_SU_Statuses,
		e_SYSE_LogServer,
		e_SYSE_EventServer,
		e_SYSE_MonitoringPackets,
		e_SYSE_LinkStatuses,
		e_SYSE_MMI_Commanding,
		e_DSSP_OverviewStatus,
		e_DSSP_DataStreamMonitoringHRIT,
		e_DSSP_DataStreamMonitoringLRIT,
		e_DSSP_xRITAligned,
		e_DSPH_DataStreamCommandingHRIT,
		e_DSPL_DataStreamCommandingLRIT,
		e_DSRX_QuickLook_VIS_0_6,
		e_DSRX_QuickLook_VIS_0_8,
		e_DSRX_QuickLook_IR_1_6,
		e_DSRX_QuickLook_IR_3_9,
		e_DSRX_QuickLook_WV_6_2,
		e_DSRX_QuickLook_WV_7_3,
		e_DSRX_QuickLook_IR_8_7,
		e_DSRX_QuickLook_IR_9_7,
		e_DSRX_QuickLook_IR_10_8,
		e_DSRX_QuickLook_IR_12_0,
		e_DSRX_QuickLook_IR_13_4,
		e_DSRX_QuickLook_HRV,
		e_DSRX_SEVIRIRepeatCycle,
		e_DCPE_MessageSummary,
		e_DCPE_BulletinSummary,
		e_DCPE_ChannelStatus,
		e_DDFC_TCPReception,
		e_DDFC_FTPReception,
		e_DDFC_TCPDispatch,
		e_DDFC_FTPDispatch,
		//	RAE MMI control IF
		e_RAE_Control,
		
		e_UnknownServiceID

	} m_ServiceID;

	
	//##ModelId=39AD95E60278
	GP_SVCE_ID():m_ServiceID(e_DCPE_DDFC){};
	//##ModelId=39AD95E60279
	GP_SVCE_ID(E_SVCE_ID i_ServiceID):m_ServiceID(i_ServiceID){};
	//##ModelId=39AD95E60281
	GP_SVCE_ID(std::wstring i_string);

	//##ModelId=39AD95E60271
	void operator = (GP_SVCE_ID rhs)
		{ m_ServiceID = rhs.m_ServiceID; };
	
	//##ModelId=39AD95E6026E
	bool operator < (const GP_SVCE_ID rhs) const 
		{ return m_ServiceID < rhs.m_ServiceID;};

	//##ModelId=39AD95E60266
	bool operator > (const GP_SVCE_ID rhs) const 
	{ return m_ServiceID > rhs.m_ServiceID;};

	//##ModelId=39AD95E60263
	bool operator == (const GP_SVCE_ID rhs) const
		{return m_ServiceID == rhs.m_ServiceID;}

	//##ModelId=39AD95E6025C
	std::wstring ReadableForm() const;
	//##ModelId=39AD95E6025A
	const unsigned int IntNetworkLength() const {return StaticInt16Size;}; 
	//##ModelId=39AD95E60250
	const unsigned int ExtNetworkLength() const {return StaticInt16Size;}; 
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SVCE_ID&  i_SVCEId);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_SVCE_ID& o_SVCEId);


inline std::wostream& operator << (std::wostream& out, const GP_SVCE_ID& gp_svce_id);

//##ModelId=39AD96120202
struct GP_SVCE_TYPE
{
	enum E_SVCE_TYPE
	{
		e_ServiceTypeNotDefined			= 0,

		//	Service Types as defined in GSDS_F
		e_CommandVerification			= 129,
		e_GenericCommand				= 130,
		e_MonitoringandDiagnostic		= 131,
		e_SubsystemAlive				= 142,
		e_SpacecraftTelemetry			= 150,
		e_SpacecraftTelecommand			= 151,
		e_SpacecraftTelecommandAck		= 152,
		e_10ImageDelivery				= 153,
		e_15ImageDelivery				= 154,
		e_AuxDataDelivery				= 155,
		e_CalibrationMonFeedback		= 156,
		e_CatalogueDataDelivery			= 157,
		e_DcpDataDelivery				= 158,
		e_FlightDynamicsDataDelivery	= 159,
		e_FlightDynamicsDataInput		= 160,
		e_GERBDataDelivery				= 161,
		e_GTSDataDelivery				= 162,
		e_HistoricalDCPDelivery 		= 163,
		e_HistoricalGERBDelivery		= 164,
		e_HistoricalHKDelivery			= 165,
		e_HistoricalImageDelivery		= 166,
		e_HistoricalProductDelivery		= 167,
		e_HritDataDelivery				= 168,
		e_IQGSERunResult				= 169,
		e_LritDataDelivery				= 170,
		e_MetProductsDelivery			= 171,
		e_OrbitDataDelivery				= 172,
		e_SGSDataDelivery				= 173,
		e_SpinRate						= 174,
		e_UTCCorrelation				= 175,
		e_UnknownServiceType			= 176

	} m_SVCE_TYPE;
	
	//##ModelId=39AD96120234
	GP_SVCE_TYPE(){m_SVCE_TYPE = e_ServiceTypeNotDefined;};
	//##ModelId=39AD96120235
	GP_SVCE_TYPE(E_SVCE_TYPE i_SVCEType){m_SVCE_TYPE = i_SVCEType;};
	//##ModelId=39AD96120237
	GP_SVCE_TYPE(std::wstring i_string);

	//##ModelId=39AD96120224
	bool operator == (const E_SVCE_TYPE i_SVCEType) const 
		{return m_SVCE_TYPE == i_SVCEType;}

	//##ModelId=39AD9612022C
	bool operator == (const GP_SVCE_TYPE i_SVCEType) const
		{return m_SVCE_TYPE == i_SVCEType.m_SVCE_TYPE;}


	//##ModelId=39AD96120222
	GP_SVCE_TYPE& operator = (const GP_SVCE_TYPE i_SVCEType)
		{m_SVCE_TYPE = i_SVCEType.m_SVCE_TYPE;return *this;};



	//##ModelId=39AD96120220
	const unsigned int IntNetworkLength () const { return StaticInt8Size ;}
	//##ModelId=39AD96120218
	const unsigned int ExtNetworkLength () const { return StaticInt8Size ;}

	//##ModelId=39AD96120216
	std::wstring ReadableForm() const;
};
inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& i_Stream, const GP_SVCE_TYPE& i_SType);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,GP_SVCE_TYPE& o_SType);


inline std::wostream& operator << (std::wostream& out, const GP_SVCE_TYPE& gp_svce_type);


//##ModelId=39AD95E90126
struct GP_CPU_ADDRESS
{
	//##ModelId=39AD95E901B3
	GP_CPU_ADDRESS();
	//##ModelId=39AD95E901B4
	GP_CPU_ADDRESS( const UNSIGNED_BYTE   i_Qualifier_1,
					const UNSIGNED_BYTE   i_Qualifier_2,
					const UNSIGNED_BYTE   i_Qualifier_3,
					const UNSIGNED_BYTE   i_Qualifier_4);

	//##ModelId=39AD95E901D1
	GP_CPU_ADDRESS(const std::wstring& IP_Address);

	//##ModelId=39AD95E9016F
    UNSIGNED_BYTE   m_Qualifier_1;
	//##ModelId=39AD95E9015B
    UNSIGNED_BYTE   m_Qualifier_2;
	//##ModelId=39AD95E90151
    UNSIGNED_BYTE   m_Qualifier_3;
	//##ModelId=39AD95E9013C
    UNSIGNED_BYTE   m_Qualifier_4;

	//##ModelId=39AD95E901A9
	inline std::wstring dot_notation() const;


	//##ModelId=39AD95E9019F
	GP_CPU_ADDRESS& operator = (const GP_CPU_ADDRESS i_cpu)
		{m_Qualifier_1	= i_cpu.m_Qualifier_1;
		m_Qualifier_2	= i_cpu.m_Qualifier_2;
		m_Qualifier_3	= i_cpu.m_Qualifier_3;
		m_Qualifier_4	= i_cpu.m_Qualifier_4;
		return *this;};

	//##ModelId=39AD95E90195
	bool operator == (const GP_CPU_ADDRESS& i_Address) const;

	//##ModelId=39AD95E9018B
	bool operator < (const GP_CPU_ADDRESS& i_Address) const;

	//##ModelId=39AD95E90183
	const unsigned int IntNetworkLength() const  { return  m_Qualifier_1.IntNetworkLength() * 4;}; 
	//##ModelId=39AD95E90181
	const unsigned int ExtNetworkLength() const  { return  m_Qualifier_1.ExtNetworkLength() * 4;}; 

	//##ModelId=39AD95E90177
	inline std::wstring ReadableForm() const { return dot_notation(); }
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& i_Stream, const GP_CPU_ADDRESS& i_Addr);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,GP_CPU_ADDRESS& i_Addr);



typedef GP_CPU_ADDRESS GP_SVCE_ADDRESS;

inline std::wostream& operator << (std::wostream& out, const GP_CPU_ADDRESS& gp_cpu_address);

//##ModelId=39AD960E00F8
struct GP_COMMAND_ACK_FLAG
{
	//##ModelId=39AD960E014A
	BOOLEAN_BYTE			m_Acceptance;
	//##ModelId=39AD960E0136
	BOOLEAN_BYTE			m_Start;
	//##ModelId=39AD960E0122
	BOOLEAN_BYTE			m_Progress;
	//##ModelId=39AD960E010E
	BOOLEAN_BYTE			m_Completion;

	//##ModelId=39AD960E0171
	GP_COMMAND_ACK_FLAG() : 
		m_Acceptance(0),
		m_Start(0),
		m_Progress(0),
		m_Completion(0)
	{}

	//##ModelId=39AD960E017A
	GP_COMMAND_ACK_FLAG(GP_COMMAND_ACK_FLAG& i_CommandAck) : 
		m_Acceptance(i_CommandAck.m_Acceptance),
		m_Start(i_CommandAck.m_Start),
		m_Progress(i_CommandAck.m_Progress),
		m_Completion(i_CommandAck.m_Completion)
	{}

	//##ModelId=39AD960E0170
	~GP_COMMAND_ACK_FLAG(){}


	//##ModelId=39AD960E0166
	bool operator ==(const GP_COMMAND_ACK_FLAG& i_AckFlag) const 
	{
		return (m_Acceptance == i_AckFlag.m_Acceptance &&
				m_Start == i_AckFlag.m_Start &&
				m_Progress == i_AckFlag.m_Progress &&
				m_Completion == i_AckFlag.m_Completion);
	}

	//##ModelId=39AD960E015E
	GP_COMMAND_ACK_FLAG& operator =(const GP_COMMAND_ACK_FLAG& i_AckFlag)
	{
		m_Acceptance = i_AckFlag.m_Acceptance;
		m_Start = i_AckFlag.m_Start;
		m_Progress = i_AckFlag.m_Progress ;
		m_Completion = i_AckFlag.m_Completion;
		return *this;
	}

	//##ModelId=39AD960E015C
	const unsigned int IntNetworkLength() const {return  m_Acceptance.IntNetworkLength() * 4;};
	//##ModelId=39AD960E0152
	const unsigned int ExtNetworkLength() const {return  m_Acceptance.ExtNetworkLength() * 4;};
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& i_Stream, const GP_COMMAND_ACK_FLAG& i_Ack);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,GP_COMMAND_ACK_FLAG& i_Ack);



//##ModelId=39AD95E50329
struct GP_CONFIG_ITEM_VERSION
{	
	//##ModelId=39AD95E50349
	UNSIGNED_SHORT			m_Issue;
	//##ModelId=39AD95E5033F
	UNSIGNED_SHORT			m_Revision;

	//##ModelId=39AD95E50371
	GP_CONFIG_ITEM_VERSION (UNSIGNED_SHORT i_Issue = 0, UNSIGNED_SHORT i_Revision = 0) 
		:	m_Issue (i_Issue),
			m_Revision (i_Revision) {}

	//##ModelId=39AD95E50369
	GP_CONFIG_ITEM_VERSION& operator = (const GP_CONFIG_ITEM_VERSION i_Config)
		{m_Issue = i_Config.m_Issue;
		m_Revision = i_Config.m_Revision;
		return *this;};

	//##ModelId=39AD95E50366
	bool operator == (const GP_CONFIG_ITEM_VERSION& i_Config) const
		{return m_Issue== i_Config.m_Issue &&
				m_Revision == i_Config.m_Revision;};

	//##ModelId=39AD95E5035E
	const unsigned int IntNetworkLength() const  
	{ 
		return m_Issue.IntNetworkLength () + m_Revision.IntNetworkLength ();
	}
	//##ModelId=39AD95E5035C
	const unsigned int ExtNetworkLength() const  
	{ 
		return m_Issue.ExtNetworkLength () + m_Revision.ExtNetworkLength ();
	}

	//##ModelId=39AD95E50352
	std::wstring ReadableForm() const
	{
		return m_Issue.ReadableForm () + _T(".") + m_Revision.ReadableForm ();
	}
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_CONFIG_ITEM_VERSION&  i_ConfigIVersion);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_CONFIG_ITEM_VERSION& o_ConfigIVersion);
inline std::wostream& operator << (std::wostream& out, const GP_CONFIG_ITEM_VERSION& gp_config_item_version);


//##ModelId=39AD96010009
struct GP_DBASE_ID
{
	//##ModelId=39AD9601001F
	GP_SC_ID				m_SpacecraftId;
	//##ModelId=39AD96010014
	UNSIGNED				m_DBaseId;

	//##ModelId=39AD9601003D
	GP_DBASE_ID& operator = (const GP_DBASE_ID i_DBase)
		{m_SpacecraftId = i_DBase.m_SpacecraftId;
		m_DBaseId = i_DBase.m_DBaseId;
		return *this;};

	//##ModelId=39AD96010034
	bool operator == (const GP_DBASE_ID& i_DBase) const
		{return m_SpacecraftId== i_DBase.m_SpacecraftId &&
				m_DBaseId == i_DBase.m_DBaseId;};

	//##ModelId=39AD96010032
	const unsigned int IntNetworkLength() const 	
	{
		return		m_SpacecraftId.IntNetworkLength() + m_DBaseId.IntNetworkLength();
	}; 
	//##ModelId=39AD96010027
	const unsigned int ExtNetworkLength() const 
	{
		return		m_SpacecraftId.ExtNetworkLength() + m_DBaseId.ExtNetworkLength();
	};

};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_DBASE_ID&  i_DBaseId);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_DBASE_ID& o_DBaseId);
inline std::wostream& operator << (std::wostream& out, const GP_DBASE_ID& gp_dbase_id);


//##ModelId=39AD961500B2
struct GP_SW_VERSION
{
	//##ModelId=39AD961500E6
	UNSIGNED_SHORT				m_Issue;
	//##ModelId=39AD961500D2
	UNSIGNED_SHORT				m_Revision;

	//##ModelId=39AD96150104
	GP_SW_VERSION (UNSIGNED_SHORT i_Issue = 0, UNSIGNED_SHORT i_Revision = 0) 
		:	m_Issue (i_Issue),
			m_Revision (i_Revision) {}

	//##ModelId=39AD96150102
	GP_SW_VERSION& operator = (const GP_SW_VERSION i_SWVersion)
		{m_Issue = i_SWVersion.m_Issue;
		 m_Revision = i_SWVersion.m_Revision;	
		return *this;};

	//##ModelId=39AD961500F9
	bool operator == (const GP_SW_VERSION& i_SWVersion) const
		{return m_Issue== i_SWVersion.m_Issue &&
				m_Revision == i_SWVersion.m_Revision;};

	//##ModelId=39AD961500F2
	const unsigned int IntNetworkLength() const  
	{ 
		return m_Issue.IntNetworkLength () + m_Revision.IntNetworkLength ();
	}
	//##ModelId=39AD961500F0
	const unsigned int ExtNetworkLength() const  
	{ 
		return m_Issue.ExtNetworkLength () + m_Revision.ExtNetworkLength ();
	}
	//##ModelId=39AD961500EE
	std::wstring ReadableForm() const
	{
		return m_Issue.ReadableForm () + _T(".") + m_Revision.ReadableForm ();
	}
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SW_VERSION&  i_SWVersion);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_SW_VERSION& o_SWVersion);
inline std::wostream& operator << (std::wostream& out, const GP_SW_VERSION& gp_sw_version);


typedef CHARACTERSTRING			GP_VOLUME_DESCRIPTION;
// VARIABLE CHARACTER STRING (65536)

typedef CHARACTERSTRING			GP_VOLUME_HEADER;
// CHARACTER STRING (50)


//##ModelId=39AD95FD02B6
struct GP_DU_ID
{
	enum E_DU_ID
	{
		e_NULL_DU								= 0,
		e_DCPE_ControlDB						= 31000,
		e_DCPE_Links							= 31001,
		e_DCPE_IBConfig							= 31002,
		e_DDFC_ControlDB						= 31003,
		e_DDFC_Links							= 31004,
		e_DDFC_IBConfig							= 31005,
		e_DSSP_IBConfig							= 31006,
		e_DSPH_ProcessHRIT						= 31007,
		e_DSPL_ProcessLRIT						= 31008,
		e_DSRX_ProcessingSchemeHRIT				= 31009,
		e_DSRX_ProcessingSchemeLRIT				= 31010,
		e_DSRX_Receive							= 31011,
		e_DSRX_Links							= 31012,
		e_DSPL_Links							= 31013,
		e_DSPH_Links							= 31014,
		e_DSSP_Links							= 31015,
		e_DSPM_Links							= 31016,
		e_KCE_EncryptionExportLRIT				= 31017,
		e_KCE_EncryptionExportHRIT				= 31018,
		e_SYSE_IBConfig							= 31019,
		e_SYSE_Links							= 31020,
		e_SYSE_MandC							= 31021,
		e_SYSE_ConfigurationControl				= 31022,
		e_SYSE_MMI_Configuration				= 31023,
		e_RAE_ConfigDB							= 31024,
		e_RAE_IBConfig							= 31025,
		e_RAE_Links								= 31026,
		e_RAE_Events							= 31027,

	}								m_DU_ID;

	//##ModelId=39AD95FD02E9
	GP_DU_ID () {};
	//##ModelId=39AD95FD02EA
	GP_DU_ID (E_DU_ID i_DU_ID): m_DU_ID (i_DU_ID)  {};
	//##ModelId=39AD95FD02F3
	GP_DU_ID(std::wstring i_string);
	//##ModelId=39AD95FD02E8
	operator GP_DU_ID (){ return m_DU_ID; }

	//##ModelId=39AD95FD02F5
	GP_DU_ID (UINT32 i_DU_ID)
	{
		m_DU_ID = static_cast<GP_DU_ID::E_DU_ID> (i_DU_ID);
	}

	// Copy a DU Id
	//##ModelId=39AD95FD02E1
	void operator = (GP_DU_ID rhs){ m_DU_ID = rhs.m_DU_ID; }

	//##ModelId=39AD95FD02DE
	bool operator< (GP_DU_ID rhs) const { return m_DU_ID < rhs.m_DU_ID; }

	//##ModelId=39AD95FD02D6
	bool operator== (GP_DU_ID rhs) const { return m_DU_ID == rhs.m_DU_ID; }

	//##ModelId=39AD95FD02D4
	std::wstring ReadableForm() const;

	//##ModelId=39AD95FD02CC
	const unsigned int IntNetworkLength() const  { return StaticInt16Size; }
	//##ModelId=39AD95FD02CA
	const unsigned int ExtNetworkLength() const  { return StaticInt16Size; }

	//##ModelId=39AD95FD02C1
	int get() {return int(m_DU_ID);}

};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_DU_ID&  i_DUId);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_DU_ID& o_DUId);


//##ModelId=39AD96010293
struct GP_FTP_LINK_MODE
{
	enum E_FTP_Link_Mode 
	{
		e_FTP_Enable		= 0,
		e_FTP_LinkDisable	= 1,
	} m_FTP_Link_Mode;

	//##ModelId=39AD960102C0
	GP_FTP_LINK_MODE(E_FTP_Link_Mode i_link_mode = e_FTP_Enable){m_FTP_Link_Mode = i_link_mode;};
	//##ModelId=39AD960102C7
	GP_FTP_LINK_MODE(std::wstring i_string);
	
	//##ModelId=39AD960102BE
	std::wstring ReadableForm() const;
	

	//##ModelId=39AD960102B5
	GP_FTP_LINK_MODE& operator = (const E_FTP_Link_Mode i_FTPLinkmode)
		{m_FTP_Link_Mode = i_FTPLinkmode;return *this;};

	//##ModelId=39AD960102BC
	GP_FTP_LINK_MODE& operator = (const GP_FTP_LINK_MODE i_FTPLinkmode)
		{m_FTP_Link_Mode = i_FTPLinkmode.m_FTP_Link_Mode;return *this;};

	//##ModelId=39AD960102A9
	bool operator == (const GP_FTP_LINK_MODE::E_FTP_Link_Mode& i_FTPLinkmode) const
		{return m_FTP_Link_Mode== i_FTPLinkmode;};

	//##ModelId=39AD960102B2
	bool operator == (const GP_FTP_LINK_MODE& i_FTPLinkmode) const
		{return m_FTP_Link_Mode== i_FTPLinkmode.m_FTP_Link_Mode;};

	//##ModelId=39AD960102A1
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD9601029F
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }

};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_FTP_LINK_MODE&  i_FTPLinkmode);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_FTP_LINK_MODE& o_FTPLinkmode);


//##ModelId=39AD96110341
struct GP_FTP_LINK_STATE
{
	enum E_FTP_Link_State
	{
		e_FTP_Connecting					= 0,
		e_FTP_ConnectError					= 1,
		e_FTP_ConnectionAttemptsExceeded	= 2,
		e_FTP_Transferring					= 3,
		e_FTP_TransferComplete				= 4,
		e_FTP_TransferError					= 5,
		e_FTP_TransferAttemptsExceeded		= 6,
	} m_FTP_Link_State;

	//##ModelId=39AD9611036D
	GP_FTP_LINK_STATE(E_FTP_Link_State i_link_state = e_FTP_Connecting){m_FTP_Link_State = i_link_state;};
	//##ModelId=39AD96110374
	GP_FTP_LINK_STATE(std::wstring i_string);
	
	//##ModelId=39AD9611036B
	std::wstring ReadableForm() const;

	//##ModelId=39AD96110362
	GP_FTP_LINK_STATE& operator = (const E_FTP_Link_State i_FTPLinkstate)
		{m_FTP_Link_State = i_FTPLinkstate;return *this;};

	//##ModelId=39AD96110369
	GP_FTP_LINK_STATE& operator = (const GP_FTP_LINK_STATE i_FTPLinkstate)
		{m_FTP_Link_State = i_FTPLinkstate.m_FTP_Link_State;return *this;};

	//##ModelId=39AD96110356
	bool operator == (const GP_FTP_LINK_STATE::E_FTP_Link_State& i_FTPLinkstate) const
		{return m_FTP_Link_State== i_FTPLinkstate;};

	//##ModelId=39AD9611035F
	bool operator == (const GP_FTP_LINK_STATE& i_FTPLinkstate) const
		{return m_FTP_Link_State== i_FTPLinkstate.m_FTP_Link_State;};

	//##ModelId=39AD9611034E
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD9611034C
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }

};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_FTP_LINK_STATE&  i_FTPLinkstate);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_FTP_LINK_STATE& o_FTPLinkstate);

//##ModelId=39AD960A0305
struct GP_PACKET_HEADER_FAILURE
{
	enum E_PACKET_HEADER_FAILURE
	{
		e_HeaderVersion				= 0,
		e_PacketType				= 1,
		e_SubHeaderType				= 2,
		e_SourceFacility			= 3,
		e_SourceEnvironment			= 4,
		e_SourceInstance			= 5,
		e_SourceSU					= 6,
		e_SourceCPU					= 7,
		e_DestinationFacility		= 8,
		e_DestinationEnvironment	= 9,
		e_SequenceCount				= 10,
		e_Length					= 11,
		e_SubHeaderVersion			= 12,
		e_ServiceType				= 13,
		e_ServiceSubType			= 14,
		e_InvalidTimestamp			= 15
	};

	//##ModelId=39AD960A0343
	GP_PACKET_HEADER_FAILURE(const E_PACKET_HEADER_FAILURE i_PACKET_HEADER_FAILURE = e_HeaderVersion) :
		m_PACKET_HEADER_FAILURE(i_PACKET_HEADER_FAILURE){}

	//##ModelId=39AD960A0339
	GP_PACKET_HEADER_FAILURE& operator = (const E_PACKET_HEADER_FAILURE i_PACKET_HEADER_FAILURE)
		{m_PACKET_HEADER_FAILURE = i_PACKET_HEADER_FAILURE;return *this;}

	//##ModelId=39AD960A0341
	GP_PACKET_HEADER_FAILURE& operator = (const GP_PACKET_HEADER_FAILURE i_PACKET_HEADER_FAILURE)
		{m_PACKET_HEADER_FAILURE = i_PACKET_HEADER_FAILURE.m_PACKET_HEADER_FAILURE;return *this;}

	//##ModelId=39AD960A032D
	bool operator == (const GP_PACKET_HEADER_FAILURE::E_PACKET_HEADER_FAILURE& i_PACKET_HEADER_FAILURE) const
		{return m_PACKET_HEADER_FAILURE== i_PACKET_HEADER_FAILURE;}

	//##ModelId=39AD960A0330
	bool operator == (const GP_PACKET_HEADER_FAILURE& i_PACKET_HEADER_FAILURE) const
		{return m_PACKET_HEADER_FAILURE== i_PACKET_HEADER_FAILURE.m_PACKET_HEADER_FAILURE;}

	//##ModelId=39AD960A0311
	E_PACKET_HEADER_FAILURE		m_PACKET_HEADER_FAILURE;

	//##ModelId=39AD960A0326
	std::wstring ReadableForm() const;
	//##ModelId=39AD960A0324
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD960A0319
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_PACKET_HEADER_FAILURE&  i_PACKET_HEADER_FAILURE);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_PACKET_HEADER_FAILURE& o_PACKET_HEADER_FAILURE);

//##ModelId=39AD960B017F
struct GP_SU_STATE
{
	enum E_SU_STATE
	{
		e_OFF			= 1,
		e_ON			= 2,
		e_DEGRADED		= 3,
		e_OTHER			= 4,
	}				m_SU_STATE;

	//##ModelId=39AD960B01AB
	GP_SU_STATE(const E_SU_STATE i_SU_STATE = e_OFF) : 
	m_SU_STATE(i_SU_STATE){}
	//##ModelId=39AD960B01B3
	GP_SU_STATE(std::wstring i_string);

	//##ModelId=39AD960B01A2
	GP_SU_STATE& operator = (const E_SU_STATE i_SU_STATE)
		{m_SU_STATE = i_SU_STATE; return *this;}

	//##ModelId=39AD960B01A9
	GP_SU_STATE& operator = (const GP_SU_STATE i_SU_STATE)
		{m_SU_STATE = i_SU_STATE.m_SU_STATE; return *this;}

	//##ModelId=39AD960B019F
	bool operator == (const GP_SU_STATE::E_SU_STATE i_SU_STATE) const 
	{
		return m_SU_STATE == i_SU_STATE;
	}

	//##ModelId=39AD960B0197
	std::wstring ReadableForm() const;
	//##ModelId=39AD960B0195
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD960B018A
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SU_STATE&  i_SUState);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_SU_STATE& o_SUState);


//##ModelId=39AD96110070
struct GP_SU_MODE
{
	enum E_SU_MODE
	{
		e_OFF		= 0,
		e_ON		= 1,
		e_OTHER		= 2,
	}				m_SU_MODE;

	//##ModelId=39AD9611009A
	GP_SU_MODE(const E_SU_MODE i_SU_MODE = e_OFF) : 
	m_SU_MODE(i_SU_MODE){}
	//##ModelId=39AD961100A2
	GP_SU_MODE(std::wstring i_string);

	//##ModelId=39AD96110091
	GP_SU_MODE& operator = (const E_SU_MODE i_SU_MODE)
		{m_SU_MODE = i_SU_MODE; return *this;}

	//##ModelId=39AD96110098
	GP_SU_MODE& operator = (const GP_SU_MODE i_SU_MODE)
		{m_SU_MODE = i_SU_MODE.m_SU_MODE; return *this;}

	//##ModelId=39AD9611008E
	bool operator == (const GP_SU_MODE::E_SU_MODE i_SU_MODE) const 
	{
		return m_SU_MODE == i_SU_MODE;
	}

	//##ModelId=39AD96110087
	std::wstring ReadableForm() const;
	//##ModelId=39AD96110085
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD9611007B
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SU_MODE&  i_SUMode);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_SU_MODE& o_SUMode);


//##ModelId=39AD95FB021D
struct GP_TCP_LINK_MODE
{
	enum E_TCP_LINK_MODE
	{
		e_ENABLED		= 0,
		e_DISABLED		= 1
	};

	//##ModelId=39AD95FB025B
	GP_TCP_LINK_MODE(const E_TCP_LINK_MODE i_TCPLinkMode = e_ENABLED) : 
	m_TCPLinkMode(i_TCPLinkMode){}
	//##ModelId=39AD95FB0264
	GP_TCP_LINK_MODE(std::wstring i_string);

	//##ModelId=39AD95FB0251
	GP_TCP_LINK_MODE& operator = (const E_TCP_LINK_MODE i_TCPLinkMode)
		{m_TCPLinkMode = i_TCPLinkMode; return *this;}

	//##ModelId=39AD95FB0259
	GP_TCP_LINK_MODE& operator = (const GP_TCP_LINK_MODE i_TCPLinkMode)
		{m_TCPLinkMode = i_TCPLinkMode.m_TCPLinkMode; return *this;}

	//##ModelId=39AD95FB0249
	bool operator == (const GP_TCP_LINK_MODE::E_TCP_LINK_MODE i_TCPLinkMode) const 
	{
		return m_TCPLinkMode == i_TCPLinkMode;
	}


	//##ModelId=39AD95FB0233
	E_TCP_LINK_MODE m_TCPLinkMode;

	//##ModelId=39AD95FB0247
	std::wstring ReadableForm() const;
	//##ModelId=39AD95FB0245
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD95FB023B
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};


inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_TCP_LINK_MODE&  i_TCPLinkmode);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_TCP_LINK_MODE& o_TCPLinkmode);


//##ModelId=39AD961300B9
struct GP_TCP_LINK_STATE
{
	enum E_TCP_LINK_STATE
	{
		e_OFF_STATE						= 0,
		e_CANCELLING_STATE				= 1,
		e_WAIT_STATE					= 2,
		e_RELEASING_STATE				= 3,
		e_ESTABLISHED_STATE_CONNECT		= 4,
		e_ESTABLISHED_STATE_RECONNECT	= 5
	};

	//##ModelId=39AD96130101
	GP_TCP_LINK_STATE(const E_TCP_LINK_STATE i_TCPLinkState = e_OFF_STATE) : 
		m_TCPLinkState(i_TCPLinkState){}
	//##ModelId=39AD9613010A
		GP_TCP_LINK_STATE(std::wstring i_string);

	//##ModelId=39AD961300F7
	GP_TCP_LINK_STATE& operator = (const E_TCP_LINK_STATE i_TCPLinkState)
		{m_TCPLinkState = i_TCPLinkState; return *this;}

	//##ModelId=39AD961300FF
	GP_TCP_LINK_STATE& operator = (const struct GP_TCP_LINK_STATE& i_TCPLinkState)
		{m_TCPLinkState = i_TCPLinkState.m_TCPLinkState; return *this;}

	//##ModelId=39AD961300EB
	bool operator == (const GP_TCP_LINK_STATE::E_TCP_LINK_STATE i_TCPLinkState) const
		{return m_TCPLinkState == i_TCPLinkState;}

	//##ModelId=39AD961300EE
	bool operator == (const GP_TCP_LINK_STATE& i_TCPLinkState) const
		{return m_TCPLinkState == i_TCPLinkState.m_TCPLinkState;}

	//##ModelId=39AD961300CF
	E_TCP_LINK_STATE m_TCPLinkState;

	//##ModelId=39AD961300E4
	std::wstring ReadableForm() const;
	//##ModelId=39AD961300E2
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD961300D7
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_TCP_LINK_STATE&  i_TCPLinkstate);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_TCP_LINK_STATE& o_TCPLinkstate);


//##ModelId=39AD95FE0177
struct GP_TCP_SESSION_MODE
{
	enum E_TCP_SESSION_MODE
	{
		e_CONNECT_MODE		= 0,
		e_RECONNECT_MODE	= 1
	};
				
	//##ModelId=39AD95FE01B5
	GP_TCP_SESSION_MODE(const E_TCP_SESSION_MODE i_TCPSessionMode =e_CONNECT_MODE) :
		m_TCPSessionMode(i_TCPSessionMode){}
	//##ModelId=39AD95FE01BD
	GP_TCP_SESSION_MODE(std::wstring i_string);

	//##ModelId=39AD95FE01AB
	GP_TCP_SESSION_MODE& operator = (const E_TCP_SESSION_MODE i_TCPSessionMode)
		{m_TCPSessionMode = i_TCPSessionMode;return *this;}

	//##ModelId=39AD95FE01B3
	GP_TCP_SESSION_MODE& operator = (const GP_TCP_SESSION_MODE i_TCPSessionMode)
		{m_TCPSessionMode = i_TCPSessionMode.m_TCPSessionMode;return *this;}

	//##ModelId=39AD95FE019F
	bool operator == (const GP_TCP_SESSION_MODE::E_TCP_SESSION_MODE& i_TCPSessionMode) const
		{return m_TCPSessionMode== i_TCPSessionMode;}

	//##ModelId=39AD95FE01A2
	bool operator == (const GP_TCP_SESSION_MODE& i_TCPSessionMode) const
		{return m_TCPSessionMode== i_TCPSessionMode.m_TCPSessionMode;}

	//##ModelId=39AD95FE0183
	E_TCP_SESSION_MODE m_TCPSessionMode;

	//##ModelId=39AD95FE0198
	std::wstring ReadableForm() const;
	//##ModelId=39AD95FE0196
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD95FE018B
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};


inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_TCP_SESSION_MODE&  i_TCPSessionMode);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_TCP_SESSION_MODE& o_TCPSessionMode);



//##ModelId=39AD96160294
struct GP_TCP_REPLACE_MODE
{
	enum E_TCP_REPLACE_MODE
	{
		e_REPLACE_MODE		= 0,
		e_REJECT_MODE		= 1
	};

	//##ModelId=39AD961602EF
	GP_TCP_REPLACE_MODE(const E_TCP_REPLACE_MODE i_TCPReplaceMode = e_REPLACE_MODE) :
		m_TCPReplaceMode(i_TCPReplaceMode){}
	//##ModelId=39AD961602F8
	GP_TCP_REPLACE_MODE(std::wstring i_string);


	//##ModelId=39AD961602E4
	GP_TCP_REPLACE_MODE& operator = (const E_TCP_REPLACE_MODE i_TCPReplaceMode)
		{m_TCPReplaceMode = i_TCPReplaceMode;return *this;}

	//##ModelId=39AD961602E6
	GP_TCP_REPLACE_MODE& operator = (const GP_TCP_REPLACE_MODE i_TCPReplaceMode)
		{m_TCPReplaceMode = i_TCPReplaceMode.m_TCPReplaceMode;return *this;}

	//##ModelId=39AD961602D1
	bool operator == (const GP_TCP_REPLACE_MODE::E_TCP_REPLACE_MODE& i_TCPReplaceMode) const
		{return m_TCPReplaceMode== i_TCPReplaceMode;}

	//##ModelId=39AD961602DA
	bool operator == (const GP_TCP_REPLACE_MODE& i_TCPReplaceMode) const
		{return m_TCPReplaceMode== i_TCPReplaceMode.m_TCPReplaceMode;}

	//##ModelId=39AD961602B4
	E_TCP_REPLACE_MODE m_TCPReplaceMode;

	//##ModelId=39AD961602C9
	std::wstring ReadableForm() const;
	//##ModelId=39AD961602C7
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD961602BC
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_TCP_REPLACE_MODE&  i_TCPReplacemode);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_TCP_REPLACE_MODE& o_TCPReplacemode);


//##ModelId=39AD95EA0359
struct  GP_FILE_TYPE
{
	enum E_FILE_TYPE
	{
		e_Monitoring			= 0,
		e_Command				= 1,
		e_MissionData			= 2,
		e_Simulated_Monitoring	= 128,
		e_Simulated_Command		= 129,
		e_Simulated_MissionData	= 130
	}; 

	//##ModelId=39AD95EA03A0
	GP_FILE_TYPE(const E_FILE_TYPE i_File_Type = e_Monitoring) :
		m_File_Type(i_File_Type){}
	//##ModelId=39AD95EA03A2
	GP_FILE_TYPE(std::wstring i_string);


	//##ModelId=39AD95EA0395
	GP_FILE_TYPE& operator = (const E_FILE_TYPE i_File_Type)
		{m_File_Type = i_File_Type;return *this;}

	//##ModelId=39AD95EA0397
	GP_FILE_TYPE& operator = (const GP_FILE_TYPE i_File_Type)
		{m_File_Type = i_File_Type.m_File_Type;return *this;}

	//##ModelId=39AD95EA0384
	bool operator == (const GP_FILE_TYPE::E_FILE_TYPE& i_File_Type) const
		{return m_File_Type== i_File_Type;}

	//##ModelId=39AD95EA038C
	bool operator == (const GP_FILE_TYPE& i_File_Type) const
		{return m_File_Type== i_File_Type.m_File_Type;}

	//##ModelId=39AD95EA036F
	E_FILE_TYPE m_File_Type;

	//##ModelId=39AD95EA0382
	std::wstring ReadableForm() const;
	//##ModelId=39AD95EA0379
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD95EA0377
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_FILE_TYPE&  i_File_Type);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_FILE_TYPE& o_File_Type);

//##ModelId=39AD95EB021A
struct  GP_SUBHEADER_TYPE
{
	enum E_SUBHEADER_TYPE
	{
		e_NoSubheader			= 0,
		e_GP_FI_SH1				= 1
	}; 

	//##ModelId=39AD95EB0260
	GP_SUBHEADER_TYPE(const E_SUBHEADER_TYPE i_Subheader_Type = e_NoSubheader) :
		m_Subheader_Type(i_Subheader_Type){}
	//##ModelId=39AD95EB0262
	GP_SUBHEADER_TYPE(std::wstring i_string);


	//##ModelId=39AD95EB0256
	GP_SUBHEADER_TYPE& operator = (const E_SUBHEADER_TYPE i_Subheader_Type)
		{m_Subheader_Type = i_Subheader_Type;return *this;}

	//##ModelId=39AD95EB0258
	GP_SUBHEADER_TYPE& operator = (const GP_SUBHEADER_TYPE i_Subheader_Type)
		{m_Subheader_Type = i_Subheader_Type.m_Subheader_Type;return *this;}

	//##ModelId=39AD95EB0244
	bool operator == (const GP_SUBHEADER_TYPE::E_SUBHEADER_TYPE& i_Subheader_Type) const
		{return m_Subheader_Type == i_Subheader_Type;}

	//##ModelId=39AD95EB024D
	bool operator == (const GP_SUBHEADER_TYPE& i_Subheader_Type) const
		{return m_Subheader_Type == i_Subheader_Type.m_Subheader_Type;}

	//##ModelId=39AD95EB0230
	E_SUBHEADER_TYPE m_Subheader_Type;

	//##ModelId=39AD95EB0242
	std::wstring ReadableForm() const;
	//##ModelId=39AD95EB023A
	const unsigned int IntNetworkLength() const  { return StaticInt8Size; }
	//##ModelId=39AD95EB0238
	const unsigned int ExtNetworkLength() const  { return StaticInt8Size; }
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SUBHEADER_TYPE&  i_Subheader_Type);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_SUBHEADER_TYPE& o_Subheader_Type);


//##ModelId=39AD960800DB
struct GP_FI_HEADER
{
	struct CHAR409
	{
		std::wstring m_value;

		CHAR409(const std::wstring& i_value = _T(""))
		{
			m_value=i_value;
			m_value.resize(409,' ');	
		};

		std::wstring ReadableForm() const
		{	
			return m_value;
		}

		void operator = ( std::wstring& i_other)
			{m_value=i_other;m_value.resize(409);}

		const unsigned int IntNetworkLength() const {return 409;};
		const unsigned int ExtNetworkLength() const {return 409;};
	};


	//##ModelId=39AD960801B9
	UNSIGNED_BYTE		m_HeaderVersionNo;
	//##ModelId=39AD960801A5
	GP_FILE_TYPE		m_FileType;
	//##ModelId=39AD9608018F
	GP_SUBHEADER_TYPE	m_SubHeaderType;
	//##ModelId=39AD9608017B
	GP_FAC_ID			m_SourceFacilityId;
	//##ModelId=39AD96080167
	GP_FAC_ENV			m_SourceEnvId;
	//##ModelId=39AD9608014B
	UNSIGNED_BYTE		m_SourceInstanceId;
	//##ModelId=39AD96080137
	GP_SU_ID			m_SourceSUId;
	//##ModelId=39AD9608012B
	GP_CPU_ADDRESS		m_SourceCPUId;
	//##ModelId=39AD96080117
	GP_FAC_ID			m_DestFacilityId;
	//##ModelId=39AD96080105
	GP_FAC_ENV			m_DestEnvId;
	//##ModelId=39AD960800F2
	UNSIGNED			m_DataFieldLength;
	//##ModelId=39AD960800EF
	CHAR409				m_Description;

	//##ModelId=39AD960801CD
	void SetDescriptionField();

	//##ModelId=39AD960801CB
	inline unsigned int ExtNetworkLength()const;
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_FI_HEADER&  i_FIHeader);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_FI_HEADER& o_FIHeader);


//##ModelId=39AD95EC022F
struct GP_FI_SH1
{
	struct CHAR187
	{
		std::wstring m_value;

		CHAR187(const std::wstring& i_value = _T(""))
		{
			m_value=i_value;
			m_value.resize(187,' ');	
		};

		std::wstring ReadableForm() const
		{	
			return m_value;
		}

		void operator = ( std::wstring& i_other)
			{m_value=i_other;m_value.resize(187);}

		const unsigned int IntNetworkLength() const {return 187;};
		const unsigned int ExtNetworkLength() const {return 187;};
	};

	//##ModelId=39AD95EC0295
	UNSIGNED_BYTE  m_SubheaderVersionNo;
	//##ModelId=39AD95EC0281
	GP_SVCE_TYPE   m_ServiceType;
	//##ModelId=39AD95EC0275
	UNSIGNED_BYTE  m_ServiceSubtype;
	//##ModelId=39AD95EC0263
	Util::CMSGTime m_FileTime;
	//##ModelId=39AD95EC024F
	GP_SC_ID       m_SpacecraftId;
	//##ModelId=39AD95EC0245
	CHAR187		   m_Description;

	//##ModelId=39AD95EC02A8
	void SetDescriptionField();

	//##ModelId=39AD95EC029D
	inline unsigned int ExtNetworkLength()const;
};

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_FI_SH1&  i_FI_SH1);
inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_FI_SH1& o_FI_SH1);


#include "GSDS_Volume_F_Impl.h"

#endif	// GSDS_VOLUME_F_HEADER




