
#ifndef EVOLWRAPPERPACKAGEDISCHARGE
#define EVOLWRAPPERPACKAGEDISCHARGE

#include "Wrapper.h"
#include "WrapperGeneric.h"

class HobThread;

class EvolWrapperPackageDischarge : public WrapperGeneric
{
	public:
		EvolWrapperPackageDischarge(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperPackageDischarge(const char *buffer);
		EvolWrapperPackageDischarge(const EvolWrapperPackageDischarge &ewpd);
		~EvolWrapperPackageDischarge();

		const char *serialize();
		void show();

		UINT16 getPackId() const;
		void setPackId(UINT16 id);


	private:
		UINT16  packId;	

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif // EVOLWRAPPERPACKAGEDISCHARGE
