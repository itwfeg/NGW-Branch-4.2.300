/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCUPDATESTATUS_H
#define HFCUPDATESTATUS_H

#include "evolution.h"
#include "HfcObject.h"


class HfcUpdateStatus : public HfcObject
{
	public:

		HfcUpdateStatus();
		HfcUpdateStatus(bool success);
		~HfcUpdateStatus();

		virtual const char *serialize(int format) const;


		bool isUpdateSuccessful() const { return updateSuccessful; }
		void setUpdateSuccessful(bool success) { updateSuccessful = success; }

		//static void initFieldList();


	private:

		bool updateSuccessful;

		//static std::list<HfcUpdateStatusFieldOffsets> browserFieldList;

		//void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCUPDATESTATUS_H
