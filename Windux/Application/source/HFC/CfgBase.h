/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGBASE_H
#define CFGBASE_H

#include "evolution.h"
#include "HfcObject.h"
#include "e_string/utilstring.h"

#include "CCriticalSection.h"

#include "CfgBaseFieldOffsets.h"

using namespace util_string; 

class DBAccess;
namespace edb 
{
class DBConfigurationsRM;
}


class CfgBase
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		CfgBase();
		virtual ~CfgBase();

		// Read the configuration data from the database.
		virtual void load(DBAccess *dbAccess);

		virtual bool isFieldScaleOnly(int /*fieldCode*/) {return false;}

		virtual void updateField(int /*fieldCode*/, const HobString & /*token*/) {} // = 0;
		virtual void update(const CfgBase * /*newValues*/, bool /*scaleOnlyFields*/ = false) {}

		virtual void save(DBAccess * /*dbAccess*/, const CfgBase * /*newValues*/ = (CfgBase *)NULL, bool /*scaleOnlyFields*/ = false) {} // = 0;

		virtual void show() {}
		
		static const std::string& yesNoValue(const std::string& value)
				{ return stringtobool(value) ? yesValue : noValue; }

		virtual bool isSaveNeeded() const {return saveNeeded;}


	protected:
	
		std::string receivedData_;
		// The following indicates which set of fields was received, the message's format (see MessageFormat.h)
		int receivedFormat_;


		bool loadField(DBAccess *dbAccess, const std::string &fieldName, std::string &value);
		void saveField(DBAccess *dbAccess, const std::string &fieldName, const std::string &value, bool batchIt = false);
		void deleteField(DBAccess *dbAccess, const std::string &fieldName);

		// If any field in this class is modified, this flag should be set to true,
		// indicating the values still need saved to the database.
		bool saveNeeded;


	private:
		static const std::string yesValue;
		static const std::string noValue;

		Util::CSmartCriticalSection  cfgBaseCS;		

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGBASE_H
