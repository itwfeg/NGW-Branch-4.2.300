#ifndef EVOLWRAPPERREQUESTPACKAGETIMINGS_H
#define EVOLWRAPPERREQUESTPACKAGETIMINGS_H



#include <string>

#include "WrapperGeneric.h"


class HobThread;



class EvolWrapperRequestPackageTimings : public WrapperGeneric
{
	public:
		// --- Types and subtypes ---
		typedef enum
		{
			CurrentTimings,		//	PERMANENT - Persistent memory
			TemporaryTimings,	//	TEMPORARY - RAM
			DefaultTimings,		//	DEFAULT - Factory settings

			InvalidTimings
		} TimingsType;


		// --- Constructors and destructor ---
		EvolWrapperRequestPackageTimings(TimingsType type = CurrentTimings, Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperRequestPackageTimings(const char* buffer);
		~EvolWrapperRequestPackageTimings();


		// --- Accessor methods ---
		TimingsType getTimingsType() const { return timingsType_; }
		void setTimingsType(TimingsType type) { timingsType_ = type; }

		const char* serialize()const;
		void show() const;


	private:
		// --- Private members ---
		TimingsType timingsType_;



		// --- Class Members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* EVOLWRAPPERREQUESTPACKAGETIMINGS_H */
