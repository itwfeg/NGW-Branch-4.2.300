/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGEMAIL_H
#define CFGEMAIL_H

#include "evolution.h"
#include "CfgBase.h"
#include "HobIpAddress.h"

#include <list>


class CfgEmail : public CfgBase, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		CfgEmail();
		CfgEmail(const CfgEmail& cfgEmail);
		CfgEmail(const char *data, int len, int formatType);
		~CfgEmail();


		static void initFieldList();

		// Read the scale configuration data from the database.
		void load(DBAccess *dbAccess);

		void updateField(int fieldCode, const HobString &token);
		void update(const CfgBase *newValues, bool scaleOnlyFields = false);

		void save(DBAccess *dbAccess, const CfgBase *newValues = (CfgBase *)NULL, bool scaleOnlyFields = false);

		void operator=(const CfgEmail &cfgEmail);

		void getFieldAsString(int fieldCode, HobString &result, int format = UnknownFormat) const;
		const HobString& getField(int fieldCode, int format) const;

		virtual const char *serialize(int format) const;

		bool isChanged();

		void show();

		const HobString &getFrom() const;
		void setFrom(const HobString &name);

		const HobString &getToList() const;
		void setToList(const HobString &name);

		const HobIpAddress &getSmtpIpAddrOrDnsName() const;
		void setSmtpIpAddrOrDnsName(const HobString &address);
		void setSmtpIpAddrOrDnsName(const HobIpAddress &newAddress);


	protected:

		HobString		from;
		HobString		toList;
		HobIpAddress	smtpIpAddrOrDnsName;


	private:
		mutable HobString tempValue_;

		static std::list<CfgFieldOffsets> basicEmailConfigList, synchronizationList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGEMAIL_H
