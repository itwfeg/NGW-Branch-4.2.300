/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGSCALE_H
#define CFGSCALE_H

#include "evolution.h"
#include "CfgBase.h"
#include "Graphic.h"
#include "Power.h"
#include "HobDateTime.h"
#include "Product.h"
#include "Wrapper.h"
#include "Weigher.h"
#include "WrapperIntakeMode.h"
#include "HobIpAddress.h"

#include <math.h>
#include <list>
using namespace edb;



class CfgScale : public CfgBase, public HfcObject
{
	public:

		CfgScale();
		CfgScale(const char *data, int len, int formatType);
		~CfgScale();


		void updateConfiguration() const;
		bool isSaveNeeded() const;

		virtual const char *serialize(int format) const;
		bool isFieldQuoted(int fieldOffsetCfg) const;
		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		bool isFieldScaleOnly(int fieldCode);

		static void initFieldList();		


	private:

		static std::list<CfgFieldOffsets> basicScaleConfigList_, normalScaleOpList_, upgradeScaleList_, synchronizationList_, scaleManagementList_;
		static std::list<CfgFieldOffsets> reportsSetupList_;

		void updateConfigurationUsingList(const list<CfgFieldOffsets>& fieldOffsetList) const;

		void serializeUsingList(const list<CfgFieldOffsets> fieldOffsetList, int format) const;


		// Do not allow the default copy constructor.
		CfgScale(const CfgScale&);
		// Do not allow the default assignment operator.
		CfgScale& operator=(const CfgScale&);


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGSCALE_H
