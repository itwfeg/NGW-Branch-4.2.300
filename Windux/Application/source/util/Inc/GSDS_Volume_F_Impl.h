// Header file included by ANTLR UniLexer
#include <tchar.h>
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
$Archive: /Util/Inc/GSDS_Volume_F_Impl.h $
*
$History: GSDS_Volume_F_Impl.h $
 * 
 
*
*-------------------------------------------------------------------------------
*
********************************************************************************
*/

#ifdef XTRA_COM_INFO
	#pragma message( "Compiling " __FILE__) 
#endif

#include <strstream>


#include "ErrorHandling.h"

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const CHAR80& i_string)
{
	for(int i=0; i<StaticCHAR80Size; ++i )
		o_Stream << i_string.m_value[i];

	return o_Stream;

}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,CHAR80& o_string)
{
	for(int i=0; i<StaticCHAR80Size; ++i )
	{
#if _MSC_VER > 1100 && _MSC_VER < 1300
		i_Stream >> o_string.m_value[i];
#else
		short & lr_ = (short &)o_string.m_value[i];
		i_Stream >> lr_;
#endif
	}

	return i_Stream;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const CHAR255& i_string)
{
	for(int i=0; i<StaticCHAR255Size; ++i )
		o_Stream << i_string.m_value[i];

	return o_Stream;

}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,CHAR255& o_string)
{
	for(int i=0; i<StaticCHAR255Size; ++i )
	{
#if _MSC_VER > 1100 && _MSC_VER < 1300
		i_Stream >> o_string.m_value[i];
#else
		short & lr_ = (short &)o_string.m_value[i];
		i_Stream >> lr_;
#endif
	}

	return i_Stream;
}

inline std::wostream& operator << (std::wostream& o_Stream, const UNSIGNED_BYTE& i_unbyte)
{ 
	return o_Stream << i_unbyte.m_value;
}

inline std::wostream& operator << (std::wostream& o_Stream, const UNSIGNED_SHORT& i_unshort)
{
	return o_Stream << i_unshort.m_value;
}
inline std::wostream& operator << (std::wostream& o_Stream, const UNSIGNED& i_unsigned)
{
	return o_Stream << i_unsigned.m_value;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const BOOLEAN_BYTE& i_boolbyte)
{ 
	return o_Stream << _int8 (i_boolbyte.m_value);
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,BOOLEAN_BYTE& o_boolbyte)
{
	return i_Stream >> (o_boolbyte.m_value);
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const BOOLEAN_SHORT& i_boolshort)
{
	return o_Stream << _int16 (i_boolshort.m_value);
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,BOOLEAN_SHORT& o_boolshort)
{
	UINT16 temp;
	i_Stream >> temp;

	if (temp)
		o_boolshort = true;
	else
		o_boolshort = false;
	return i_Stream;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const BOOLEAN_LONG& i_boollong)
{
	return o_Stream << unsigned _int32  (i_boollong.m_value);
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,BOOLEAN_LONG& o_boollong)
{
	unsigned _int32 temp;
	i_Stream >> temp;
	if (temp)
		o_boollong = true;
	else
		o_boollong = false;
	return i_Stream;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED_BYTE& i_unbyte)
{
	return o_Stream << i_unbyte.m_value;
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED_BYTE& o_unbyte)
{
	return i_Stream >> o_unbyte.m_value;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED_SHORT& i_unshort)
{
	return o_Stream << i_unshort.m_value;
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED_SHORT& o_unshort)
{
	return i_Stream >> o_unshort.m_value;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED& i_unsigned)
{
	return o_Stream << i_unsigned.m_value;
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED& o_unsigned)
{
	return i_Stream >> o_unsigned.m_value;
}
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED_LONG& i_unsignedlong)
{
	return o_Stream << i_unsignedlong.m_value;
}
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const INTEGER_BYTE& i_intbyte)
{
	return o_Stream << i_intbyte.m_value;
}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,INTEGER_BYTE& o_intbyte)
{
	return i_Stream >> o_intbyte.m_value;
}
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const INTEGER& i_int)
{
	return o_Stream << i_int.m_value;
}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,INTEGER& o_int)
{
	return i_Stream >> o_int.m_value;
}


inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const INTEGER_SHORT& i_intshort)
{
	return o_Stream << i_intshort.m_value;
}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,INTEGER_SHORT& o_intshort)
{
	return i_Stream >> o_intshort.m_value;
}
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const INTEGER_DOUBLE& i_intdouble)
{
	return o_Stream << i_intdouble.m_value;
}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,INTEGER_DOUBLE& o_intdouble)
{
	return i_Stream  >> o_intdouble.m_value;
}
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const REAL& i_float)
{
	return o_Stream << i_float.m_value;
}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,REAL& o_float)
{
	return i_Stream >> o_float.m_value;
}
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const REAL_DOUBLE& i_double)
{
	return o_Stream << i_double.m_value;
}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,REAL_DOUBLE& o_double)
{
	return i_Stream >> o_double.m_value;
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED_LONG& o_unsignedlong)
{
	return i_Stream >> o_unsignedlong.m_value;
}

#if _MSC_VER > 1100 && _MSC_VER < 1300
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const UNSIGNED_DOUBLE& i_undouble)
{
	return o_Stream << i_undouble.m_value;
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,UNSIGNED_DOUBLE& o_undouble)
{
	return i_Stream >> o_undouble.m_value;
}
#endif
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_COMMAND_ID&  i_COMMAND_Id)
{
	return o_Stream << (__int32) i_COMMAND_Id.m_COMMAND_Id;
}
inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, GP_COMMAND_ID& i_COMMAND_Id)
{
	return i_Stream >> (__int32&) i_COMMAND_Id.m_COMMAND_Id;
}

//GP_COMMAND_FAIL_REASON
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_COMMAND_FAIL_REASON& i_CommandFReason)
{
	return o_stream << (__int16) i_CommandFReason.m_COMMAND_FAIL_REASON;
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
											GP_COMMAND_FAIL_REASON& o_CommandFReason)
{

	__int16 iTmp;
	i_stream >> iTmp;
	if((GP_COMMAND_FAIL_REASON::e_NoFailReason <= iTmp) && (iTmp < GP_COMMAND_FAIL_REASON::e_unknownCommandFailReason))
		o_CommandFReason.m_COMMAND_FAIL_REASON=(GP_COMMAND_FAIL_REASON::E_COMMAND_FAIL_REASON)iTmp;
	else
		o_CommandFReason.m_COMMAND_FAIL_REASON=GP_COMMAND_FAIL_REASON::e_unknownCommandFailReason;

	return i_stream;
}

//GP_EVENT_SEVERITY
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_EVENT_SEVERITY& i_EventSeverity)
{
	return o_stream << (UINT8) (i_EventSeverity.m_Severity);
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
											GP_EVENT_SEVERITY& o_EventSeverity)
{
	UINT8	temp;
	i_stream >> temp;

	if((GP_EVENT_SEVERITY::e_LogOnly <= temp) && (temp < GP_EVENT_SEVERITY::e_UnkownSeverity))
		o_EventSeverity.m_Severity = static_cast<GP_EVENT_SEVERITY::ESeverity> (temp);
	else
		o_EventSeverity.m_Severity=GP_EVENT_SEVERITY::e_UnkownSeverity;

	return i_stream;
}

//GP_FAC_ENV
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_FAC_ENV& i_FacEnv)
{
	return o_stream << (UINT8) (i_FacEnv.m_Fac_Env);
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
											GP_FAC_ENV& o_FacEnv)
{
	UINT8	temp;
	i_stream >> temp;

	if((GP_FAC_ENV::e_NoEnvironment <= temp) && (temp < GP_FAC_ENV::e_UnknownEnvironment ))
		o_FacEnv.m_Fac_Env=static_cast<GP_FAC_ENV::EGP_FAC_ENV> (temp);
	else
		o_FacEnv.m_Fac_Env=GP_FAC_ENV::e_UnknownEnvironment;

	return i_stream;
}

//GP_FAC_ID	
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_FAC_ID& i_FacId)
{
	return o_stream << (UINT8) (i_FacId.m_Fac_Id);
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
											GP_FAC_ID& o_FacId)
{
	UINT8	temp;
	i_stream >> temp;

	if((GP_FAC_ID::e_NoFacility <= temp) && (temp < GP_FAC_ID::e_UnknownFacility))
		o_FacId.m_Fac_Id=static_cast<GP_FAC_ID::EGP_FAC_ID> (temp);
	else
		o_FacId.m_Fac_Id=GP_FAC_ID::e_UnknownFacility;

	return i_stream;
}
//GP_SC_CHAN_ID
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_SC_CHAN_ID& i_SCChanId)
{
	return o_stream << (UINT8) (i_SCChanId.m_SC_CHAN_ID);
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
											GP_SC_CHAN_ID& o_SCChanId)
{
	UINT8	temp;
	i_stream >> temp;

	if((GP_SC_CHAN_ID::e_NO_CHANNEL <= temp) && (temp < GP_SC_CHAN_ID::e_UnknownChannel))
		o_SCChanId.m_SC_CHAN_ID=static_cast<GP_SC_CHAN_ID::E_SC_CHAN_ID> (temp);
	else
		o_SCChanId.m_SC_CHAN_ID=GP_SC_CHAN_ID::e_UnknownChannel;

	return i_stream;
}
//GP_SC_DETECT_ID
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_SC_DETECT_ID& i_SCDetectId)
{
	return o_stream << (UINT8) (i_SCDetectId.m_SC_DETECT_ID);
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
											GP_SC_DETECT_ID& o_SCDetectId)
{
	UINT8	temp;
	i_stream >> temp;

	if((GP_SC_DETECT_ID::e_No_Detector <= temp) && (temp < GP_SC_DETECT_ID::e_UnknownDetector))
		o_SCDetectId.m_SC_DETECT_ID=static_cast<GP_SC_DETECT_ID::E_SC_DETECT_ID> (temp);
	else
		o_SCDetectId.m_SC_DETECT_ID=GP_SC_DETECT_ID::e_UnknownDetector;

	return i_stream;
}
//GP_SC_ID
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_SC_ID& i_SCId)
{
	return o_stream << (UINT16) (i_SCId.m_SC_ID);
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
											GP_SC_ID& o_SCId)
{
	UINT16	temp;
	i_stream >> temp;

	if((GP_SC_ID::e_NoSpacecraft <= temp) && (temp < GP_SC_ID::e_UnknownSpacecraft))
		o_SCId.m_SC_ID=static_cast<GP_SC_ID::E_SC_ID> (temp);
	else
		o_SCId.m_SC_ID=GP_SC_ID::e_UnknownSpacecraft;
		

	return i_stream;
}

//GP_SU_ID
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, const GP_SU_ID& i_SUId)
{
	return o_Stream << (unsigned _int32) i_SUId.m_SU_ID;
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,GP_SU_ID& o_SUId)
{
	return i_Stream >> (unsigned _int32&) o_SUId.m_SU_ID;
}

//GP_SVCE_ID
inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_SVCE_ID& i_SVCEId)
{
	return o_stream << (UINT16) (i_SVCEId.m_ServiceID);
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
											GP_SVCE_ID& o_SVCEId)
{
	UINT16	temp;
	i_stream >> temp;

	if((GP_SVCE_ID::e_NoService <= temp) && (temp < GP_SVCE_ID::e_UnknownServiceID))
		o_SVCEId.m_ServiceID=static_cast<GP_SVCE_ID::E_SVCE_ID>(temp);
	else
		o_SVCEId.m_ServiceID=GP_SVCE_ID::e_UnknownServiceID;

	return i_stream;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& i_Stream, const GP_SVCE_TYPE& i_SType)
{
	return i_Stream << UINT8 (i_SType.m_SVCE_TYPE); 
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,GP_SVCE_TYPE& o_SType)
{
	UINT8 temp;
	i_Stream >> temp;

	if((GP_SVCE_TYPE::e_ServiceTypeNotDefined <= temp) && (temp < GP_SVCE_TYPE::e_UnknownServiceType))
		o_SType.m_SVCE_TYPE=static_cast<GP_SVCE_TYPE::E_SVCE_TYPE>(temp);
	else
		o_SType.m_SVCE_TYPE=GP_SVCE_TYPE::e_UnknownServiceType;
	
	return i_Stream;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& i_Stream, const GP_CPU_ADDRESS& i_Addr)
{
	return i_Stream << i_Addr.m_Qualifier_1 
					<< i_Addr.m_Qualifier_2 
					<< i_Addr.m_Qualifier_3 
					<< i_Addr.m_Qualifier_4;
}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,GP_CPU_ADDRESS& i_Addr)
{
	return i_Stream >> i_Addr.m_Qualifier_1 
					>> i_Addr.m_Qualifier_2 
					>> i_Addr.m_Qualifier_3 
					>> i_Addr.m_Qualifier_4;
}

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& i_Stream, const GP_COMMAND_ACK_FLAG& i_Ack)
{
	return i_Stream << i_Ack.m_Acceptance 
					<< i_Ack.m_Start 
					<< i_Ack.m_Progress 
					<< i_Ack.m_Completion;

}


inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream,GP_COMMAND_ACK_FLAG& i_Ack)
{
	return i_Stream >> i_Ack.m_Acceptance 
				    >> i_Ack.m_Start 
					>> i_Ack.m_Progress 
					>> i_Ack.m_Completion;
}

	//GP_CONFIG_ITEM_VERSION
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_CONFIG_ITEM_VERSION& i_ConfigIVersion)
	{
		return o_stream << i_ConfigIVersion.m_Issue << i_ConfigIVersion.m_Revision;
					
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_CONFIG_ITEM_VERSION& o_ConfigIVersion)
	{
		return i_stream >> o_ConfigIVersion.m_Issue >> o_ConfigIVersion.m_Revision;
						
	}

	//GP_DBASE_ID
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_DBASE_ID& i_DBaseId)
	{
		return o_stream << i_DBaseId.m_SpacecraftId << i_DBaseId.m_DBaseId;
					
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_DBASE_ID& o_DBaseId)
	{
		return i_stream >> o_DBaseId.m_SpacecraftId >> o_DBaseId.m_DBaseId	;		
	}



	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_SW_VERSION& i_SWVersion)
	{
		return o_stream << i_SWVersion.m_Issue
						<< i_SWVersion.m_Revision;
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_SW_VERSION& o_SWVersion)
	{
		return i_stream >> o_SWVersion.m_Issue
						>> o_SWVersion.m_Revision;
	}


	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_DU_ID& i_DUId)
	{
		return o_stream << (unsigned  _int16)i_DUId.m_DU_ID;
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_DU_ID& o_DUId)
	{
		UINT16	 l_tmp;
		i_stream >> l_tmp;

		GP_DU_ID l_tmpDUID((UINT32)l_tmp);

		o_DUId=l_tmpDUID;

		return i_stream;
	}

	//GP_FTP_LINK_MODE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_FTP_LINK_MODE& i_FTPLinkmode)
	{
		return o_stream << (UINT8) (i_FTPLinkmode.m_FTP_Link_Mode);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_FTP_LINK_MODE& o_FTPLinkmode)
	{
		return i_stream >> (UINT8&) (o_FTPLinkmode.m_FTP_Link_Mode);
	}

	//GP_FTP_LINK_STATE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_FTP_LINK_STATE& i_FTPLinkstate)
	{
		return o_stream << (UINT8) (i_FTPLinkstate.m_FTP_Link_State);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_FTP_LINK_STATE& o_FTPLinkstate)
	{
		return i_stream >> (UINT8&) (o_FTPLinkstate.m_FTP_Link_State);
	}

	//GP_PACKET_HEADER_FAILURE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_PACKET_HEADER_FAILURE& i_PacketHeaderFailure)
	{
		return o_stream << (_int8) (i_PacketHeaderFailure.m_PACKET_HEADER_FAILURE);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_PACKET_HEADER_FAILURE& o_PacketHeaderFailure)
	{
		return i_stream >> (_int8&) (o_PacketHeaderFailure.m_PACKET_HEADER_FAILURE);
	}

	//GP__SU_STATE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_SU_STATE& i_SUState)
	{
		return o_stream << (_int8) (i_SUState.m_SU_STATE);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_SU_STATE& o_SUState)
	{
		return i_stream >> (_int8&) (o_SUState.m_SU_STATE);
	}

	//GP_SU_MODE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_SU_MODE& i_SUMode)
	{
		return o_stream << (_int8) (i_SUMode.m_SU_MODE);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_SU_MODE& o_SUMode)
	{
		return i_stream >> (_int8&) (o_SUMode.m_SU_MODE);
	}

	//GP_TCP_LINK_MODE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_TCP_LINK_MODE& i_TCPLinkmode)
	{
		return o_stream << (_int8) (i_TCPLinkmode.m_TCPLinkMode);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_TCP_LINK_MODE& o_TCPLinkmode)
	{
		return i_stream >> (_int8&) (o_TCPLinkmode.m_TCPLinkMode);
	}

	//GP_TCP_LINK_STATE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_TCP_LINK_STATE& i_TCPLinkstate)
	{
		return o_stream << (_int8) (i_TCPLinkstate.m_TCPLinkState);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_TCP_LINK_STATE& o_TCPLinkstate)
	{
		return i_stream >> (_int8&) (o_TCPLinkstate.m_TCPLinkState);
	}

	//GP_TCP_SESSION_MODE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
												const GP_TCP_SESSION_MODE& i_TCPSessionMode)
	{
		return o_stream << (_int8) (i_TCPSessionMode.m_TCPSessionMode);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_TCP_SESSION_MODE& o_TCPSessionMode)
	{
		return i_stream >> (_int8&) (o_TCPSessionMode.m_TCPSessionMode);
	}

	//GP_TCP_REPLACE_MODE
	inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_stream,
											const GP_TCP_REPLACE_MODE& i_TCPReplacemode)
	{
		return o_stream << (_int8) (i_TCPReplacemode.m_TCPReplaceMode);
	}


	inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_stream,
												GP_TCP_REPLACE_MODE& o_TCPReplacemode)
	{
		return i_stream >> (_int8&) (o_TCPReplacemode.m_TCPReplaceMode);
	}



inline std::wstring GP_SU_ID::SUName() const
{
	switch(m_SU_ID)
	{
		case e_DCPE		: return _T("DCPE");
		case e_DDFC		: return _T("DDFC");
		case e_DSRX		: return _T("DSRX");
		case e_DSPL		: return _T("DSPL");
		case e_DSPH		: return _T("DSPH");
		case e_DSPM		: return _T("DSPM");
		case e_DSSP		: return _T("DSSP");
		case e_SYSE		: return _T("SYSE");
		case e_RAE		: return _T("RAE");
		case e_CMSE		: return _T("CMSE");
		case e_MMI		: return _T("MMI");
		default			: return _T("XXXX");	// Includes NULL_SU
	};
};


inline GP_CPU_ADDRESS::GP_CPU_ADDRESS( const UNSIGNED_BYTE   i_Qualifier_1,
					   const UNSIGNED_BYTE   i_Qualifier_2,
					   const UNSIGNED_BYTE   i_Qualifier_3,
					   const UNSIGNED_BYTE   i_Qualifier_4) :
m_Qualifier_1(i_Qualifier_1),
m_Qualifier_2(i_Qualifier_2),
m_Qualifier_3(i_Qualifier_3),
m_Qualifier_4(i_Qualifier_4)
{}

inline std::wstring GP_CPU_ADDRESS::dot_notation() const
{
	TCHAR	IPCharString[20];

#if defined(LINUX_FIX)
    swprintf(IPCharString, sizeof(IPCharString)/sizeof(TCHAR), _T("%d.%d.%d.%d"),
                (UINT8) m_Qualifier_1,
                (UINT8) m_Qualifier_2,
                (UINT8) m_Qualifier_3,
                (UINT8) m_Qualifier_4);
#else
	_stprintf (IPCharString, _T("%d.%d.%d.%d"),	(UINT8) m_Qualifier_1,
											(UINT8) m_Qualifier_2, 
											(UINT8) m_Qualifier_3, 
											(UINT8) m_Qualifier_4);
#endif

	return std::wstring(IPCharString);
}


inline GP_CPU_ADDRESS::GP_CPU_ADDRESS():
			m_Qualifier_1(0),m_Qualifier_2(0),m_Qualifier_3(0),m_Qualifier_4(0)
{
}

inline bool GP_CPU_ADDRESS::operator == (const GP_CPU_ADDRESS& i_Address) const
{
	return	m_Qualifier_1 == i_Address.m_Qualifier_1 &&
			m_Qualifier_2 == i_Address.m_Qualifier_2 &&
			m_Qualifier_3 == i_Address.m_Qualifier_3 &&
			m_Qualifier_4 == i_Address.m_Qualifier_4;
}

inline bool GP_CPU_ADDRESS::operator < (const GP_CPU_ADDRESS& i_Address) const
{
	// check the first component
	if(m_Qualifier_1 < i_Address.m_Qualifier_1)
	{
		return true;
	}
	else if(m_Qualifier_1 == i_Address.m_Qualifier_1)
	{
		// check the second component
		if(m_Qualifier_2 < i_Address.m_Qualifier_2)
		{
			return true;
		}
		else if(m_Qualifier_2 == i_Address.m_Qualifier_2)
		{
			// check the third component
			if(m_Qualifier_3 < i_Address.m_Qualifier_3)
			{
				return true;
			}
			else if(m_Qualifier_3 == i_Address.m_Qualifier_3)
			{
				// check the fourth component
				return (m_Qualifier_4 < i_Address.m_Qualifier_4);
			}
			else
				return false;	
		}
		else
			return false;
	}
	else
		return false;
}

inline std::wostream& operator << (std::wostream& out, const GP_COMMAND_ACK_FLAG& gp_command_ack_flag)
{	
	out <<_T(", Acceptance: ");
	if (gp_command_ack_flag.m_Acceptance == 0)
		out <<_T("false");
	else
		out <<_T("true");
	out <<_T(", Start: ");
	if (gp_command_ack_flag.m_Start == 0)
		out <<_T("false");
	else
		out <<_T("true");
	out <<_T(", Progress: ");
	if (gp_command_ack_flag.m_Progress == 0)
		out <<_T("false");
	else
		out <<_T("true");
	out <<_T(", Completion: ");
	if (gp_command_ack_flag.m_Completion == 0)
		out <<_T("false");
	else
		out <<_T("true");
	return out;
}

//	Convenience output streaming

inline std::wostream& operator << (std::wostream& o_Stream, const std::wstring& i_string)
{ 
	return o_Stream << i_string.c_str ();
}

inline std::wostream& operator << (std::wostream& out, const GP_SVCE_ID& i_DADFType)
{
	out  << i_DADFType.ReadableForm ();
	return out;
}

inline std::wostream& operator << (std::wostream& out, const GP_SVCE_TYPE& i_DADFType)
{
	out  << i_DADFType.ReadableForm ();
	return out;
}
	
inline std::wostream& operator << (std::wostream& out, const GP_CPU_ADDRESS& i_DADFType)
{
	out << i_DADFType.ReadableForm ();
	return out;
}

inline std::wostream& operator << (std::wostream& out, const GP_FAC_ID& i_DADFType)
{
	out << i_DADFType.ReadableForm ();
	return out;
}

inline std::wostream& operator << (std::wostream& out, const GP_FAC_ENV& i_DADFType)
{
	out << i_DADFType.ReadableForm ();
	return out;
}

inline std::wostream& operator << (std::wostream& out, const GP_SU_ID& i_DADFType)
{
	out << i_DADFType.ReadableForm ();
	return out;
}

inline std::wostream& operator << (std::wostream& out, const GP_DU_ID& i_DADFType)
{
	out << i_DADFType.ReadableForm ();
	return out;
}

unsigned int GP_FI_HEADER::ExtNetworkLength() const
{
	return (	m_HeaderVersionNo.ExtNetworkLength()+
				m_FileType.ExtNetworkLength()+
				m_SubHeaderType.ExtNetworkLength()+
				m_SourceFacilityId.ExtNetworkLength()+
				m_SourceEnvId.ExtNetworkLength()+
				m_SourceInstanceId.ExtNetworkLength()+
				m_SourceSUId.ExtNetworkLength()+
				m_SourceCPUId.ExtNetworkLength()+
				m_DestFacilityId.ExtNetworkLength()+
				m_DestEnvId.ExtNetworkLength()+
				m_DataFieldLength.ExtNetworkLength()+
				m_Description.ExtNetworkLength());

};

inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, 
										  const GP_FILE_TYPE&  i_File_Type)
{
	return o_Stream << (_int8) (i_File_Type.m_File_Type);
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, 
										  GP_FILE_TYPE& o_File_Type)
{
	return i_Stream >> (_int8&) (o_File_Type.m_File_Type);
}

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, 
										   const GP_SUBHEADER_TYPE&  i_Subheader_Type)
{
	return o_Stream << (_int8) (i_Subheader_Type.m_Subheader_Type);
}

inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, 
										   GP_SUBHEADER_TYPE& o_Subheader_Type)
{
	return i_Stream >> (_int8&) (o_Subheader_Type.m_Subheader_Type);
}


inline Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, 
										    const GP_FI_HEADER&  i_FIHeader)
{
	o_Stream << i_FIHeader.m_HeaderVersionNo
			 << i_FIHeader.m_FileType
			 << i_FIHeader.m_SubHeaderType
			 << i_FIHeader.m_SourceFacilityId
			 << i_FIHeader.m_SourceEnvId
			 << i_FIHeader.m_SourceInstanceId
			 << i_FIHeader.m_SourceSUId
			 << i_FIHeader.m_SourceCPUId
			 << i_FIHeader.m_DestFacilityId
			 << i_FIHeader.m_DestEnvId
			 << i_FIHeader.m_DataFieldLength;

	o_Stream.Write((BYTE*)i_FIHeader.m_Description.m_value.data(), (int)409);

	return o_Stream;
}

inline Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, 
										  GP_FI_HEADER& o_FIHeader)
{
	i_Stream >> o_FIHeader.m_HeaderVersionNo
			 >> o_FIHeader.m_FileType
			 >> o_FIHeader.m_SubHeaderType
			 >> o_FIHeader.m_SourceFacilityId
			 >> o_FIHeader.m_SourceEnvId
			 >> o_FIHeader.m_SourceInstanceId
			 >> o_FIHeader.m_SourceSUId
			 >> o_FIHeader.m_SourceCPUId
			 >> o_FIHeader.m_DestFacilityId
			 >> o_FIHeader.m_DestEnvId
			 >> o_FIHeader.m_DataFieldLength;

	std::wstring l_Buffer(409+1,0);
	i_Stream.Read((unsigned char*)&l_Buffer[0], (int)409);
	o_FIHeader.m_Description = l_Buffer;

	return i_Stream;
}


// CJC ifdef'd out on 10/18/00 7:03:22 PM
#ifdef UVWXYZ
	unsigned int GP_FI_SH1::ExtNetworkLength() const
	{
		return (m_SubheaderVersionNo.ExtNetworkLength()+
				m_ServiceType.ExtNetworkLength()+
				m_ServiceSubtype.ExtNetworkLength()+
				m_FileTime.ExtNetworkLength()+
				m_SpacecraftId.ExtNetworkLength()+
				m_Description.ExtNetworkLength());

};

#endif //UVWXYZ

inline 	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, 
										   const GP_FI_SH1&  i_FI_SH1)
{
	o_Stream << i_FI_SH1.m_SubheaderVersionNo
			 << i_FI_SH1.m_ServiceType
			 << i_FI_SH1.m_ServiceSubtype
			 << i_FI_SH1.m_FileTime
			 << i_FI_SH1.m_SpacecraftId;

	o_Stream.Write((BYTE*)i_FI_SH1.m_Description.m_value.data(), (int)187);

	return o_Stream;
}

inline 	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& i_Stream, 
										   GP_FI_SH1& o_FI_SH1)
{
	i_Stream >> o_FI_SH1.m_SubheaderVersionNo
			 >> o_FI_SH1.m_ServiceType
			 >> o_FI_SH1.m_ServiceSubtype
			 >> o_FI_SH1.m_FileTime
			 >> o_FI_SH1.m_SpacecraftId;

	std::wstring l_Buffer(187+1,0);
	i_Stream.Read((unsigned char*)&l_Buffer[0],(int)187);
	o_FI_SH1.m_Description = l_Buffer;

	return i_Stream;
}
