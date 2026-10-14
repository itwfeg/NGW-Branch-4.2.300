/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGSYNCHRONIZATION_H
#define CFGSYNCHRONIZATION_H

#include <string>
#include <list>

#include "CfgBase.h"
#include "HfcObject.h"

#include "../core/Synchronization/ScaleSynchronizationType.h"
#include "HobIpAddress.h"
#include "HobString.h"



class CfgSynchronization : public CfgBase, public HfcObject
{
	public:

		CfgSynchronization();
		CfgSynchronization(const char * data, int len, int formatType);
		~CfgSynchronization();


		void updateConfiguration();
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;

		static void initFieldList();		
		 		 


		/* --- Getters and setters --- */

		ScaleSynchronizationType getReceivedScaleSynchronizationType() const { return receivedScaleSynchronizationType_; }
		void setReceivedScaleSynchronizationType(ScaleSynchronizationType type) { receivedScaleSynchronizationType_ = type; }

		const std::string& getReceivedPrimaryScale() const { return receivedPrimaryScale_; }
		void setReceivedPrimaryScale(const std::string& pri) { receivedPrimaryScale_ = pri; }

		bool getSavedSyncTypeChanged() const {return savedSyncTypeChanged_;}
		void setSavedSyncTypeChanged(bool changed) {savedSyncTypeChanged_ = changed;}

		bool getSavedPrimaryScaleChanged() const {return savedPrimaryScaleChanged_;}
		void setSavedPrimaryScaleChanged(bool changed) {savedPrimaryScaleChanged_ = changed;}

		ScaleSynchronizationType getSavedNewSyncType() const {return savedNewSyncType_;}
		void setSavedNewSyncType(ScaleSynchronizationType type) {savedNewSyncType_ = type;}

		const std::string& getSavedNewPrimaryScale() const {return savedNewPrimaryScale_;}
		void setSavedNewPrimaryScale(const std::string& primary) {savedNewPrimaryScale_ = primary;}


	private:

		static std::list<CfgFieldOffsets> fieldList, cfgItemCheckList;

		// Do not allow the default copy constructor.
		CfgSynchronization(const CfgSynchronization&);
		// Do not allow the default assignment operator.
		CfgSynchronization& operator=(const CfgSynchronization&);


#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif


		ScaleSynchronizationType receivedScaleSynchronizationType_;
		std::string receivedPrimaryScale_;

		bool savedSyncTypeChanged_;
		bool savedPrimaryScaleChanged_;
		ScaleSynchronizationType savedNewSyncType_;
		std::string savedNewPrimaryScale_;
};

#endif /* CFGSYNCHRONIZATION_H */
