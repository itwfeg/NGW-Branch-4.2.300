#include "EvolMessageWrapper.h"

#include "ErrorHandling.h"

#include "PAL_platformheader.h"

#include "Debug.h"

//namespace Hobart
//{

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolMessageWrapper::dataType = 0;
std::string EvolMessageWrapper::dataTypeName("EvolMessageWrapper"); 
#endif

	EvolMessageWrapper::EvolMessageWrapper( const EvolMessage * msg )
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);
	
		util_acc::addInstance(dataType);
#endif		
		try
		{
			if ( msg != NULL )
			{
				const char * ptr = msg->serialize();
				evolData.append( ptr, msg->getSerializedLength() );
			}
		} 
		catch (...)
		{
	        Debug::trace("EvolMessageWrapper.error", "EvolMessageWrapper():  catch(...)");
			LogCatchAndThrow;
		}
	}

#ifdef MIN_WARN_FIX
	EvolMessageWrapper::EvolMessageWrapper( const char * ptr, size_t length )
#else
	EvolMessageWrapper::EvolMessageWrapper( const char * ptr, int length )
#endif
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);
	
		util_acc::addInstance(dataType);
#endif		
		try
		{
			if ( ptr != NULL )
				evolData.append( ptr, length );
		}
		catch (...)
		{
	        Debug::trace("EvolMessageWrapper.error", "EvolMessageWrapper(2):  catch(...)");
			LogCatchAndThrow;
		}
	}

	EvolMessageWrapper::~EvolMessageWrapper()
	{
	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
	#endif
	}

	EvolDeviceType EvolMessageWrapper::getDeviceType()
	{
		try
		{
#ifdef PAL_WIN32_RUN
			unsigned __int32 * pint = ( unsigned __int32 * ) evolData.data();
#else
			uint32_t * pint = ( uint32_t * ) evolData.data();
#endif
			return ( EvolDeviceType ) *pint;
		}
		catch (...)
		{
	        Debug::trace("EvolMessageWrapper.error", "~EvolMessageWrapper():  catch(...)");
			LogCatchAndThrow;
		}
	}


	UINT8 EvolMessageWrapper::getEventType()
	{
		try
		{
			const char * p = evolData.data();
			p += sizeof( EvolDeviceType );

			return  *  p;
		}
		catch (...)
		{
	        Debug::trace("EvolMessageWrapper.error", "getEventType():  catch(...)");
			LogCatchAndThrow;
		}
	}


	Hobart::CEvolMsgReceiver * EvolMessageWrapper::getSourceObject()
	{
		try
		{
			const char * p = evolData.data();
			p += sizeof( EvolDeviceType ) + sizeof( UINT8 );

			Hobart::CEvolMsgReceiver ** p_sender = ( Hobart::CEvolMsgReceiver ** ) p;

			return *p_sender;	
		}
		catch (...)
		{
	        Debug::trace("EvolMessageWrapper.error", "getSourceObject():  catch(...)");
			LogCatchAndThrow;
		}
	}


	void EvolMessageWrapper::setSourceObject(Hobart::CEvolMsgReceiver* source)
	{
		try
		{
			const char * p = evolData.data();
			p += sizeof( EvolDeviceType ) + sizeof( UINT8 );

			Hobart::CEvolMsgReceiver ** p_sender = ( Hobart::CEvolMsgReceiver ** ) p;

			*p_sender = source;	
		}
		catch (...)
		{
	        Debug::trace("EvolMessageWrapper.error", "setSourceObject():  catch(...)");
			LogCatchAndThrow;
		}
	}


	unsigned int EvolMessageWrapper::getSequenceNumber()
	{
		try
		{
			const char * p = evolData.data();
			p += sizeof( EvolDeviceType ) + sizeof( UINT8 ) + sizeof(Hobart::CEvolMsgReceiver *);

			return *(unsigned int *)p;	
		}
		catch (...)
		{
	        Debug::trace("EvolMessageWrapper.error", "getSequenceNumber():  catch(...)");
			LogCatchAndThrow;
		}
	}


	void EvolMessageWrapper::setSequenceNumber(unsigned int number)
	{
		try
		{
			const char * p = evolData.data();
			p += sizeof( EvolDeviceType ) + sizeof( UINT8 ) + sizeof(Hobart::CEvolMsgReceiver *);

			*(unsigned int *)p = number;	
		}
		catch (...)
		{
	        Debug::trace("EvolMessageWrapper.error", "setSequenceNumber():  catch(...)");
			LogCatchAndThrow;
		}
	}


	const char * EvolMessageWrapper::Data()
	{
		return evolData.data();
	}


	const char * EvolMessageWrapper::CStr()
	{
		return evolData.c_str();
	}


	int EvolMessageWrapper::Length()
	{
		return ( int ) evolData.size();
	}


//} //end of namespace 
