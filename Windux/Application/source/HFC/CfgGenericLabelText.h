/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGGENERICLABELTEXT_H
#define CFGGENERICLABELTEXT_H

#include "evolution.h"
#include "CfgBase.h"

class HobString;
class GenericLabelText;


class CfgGenericLabelText : public CfgBase, public HfcObject
{
	public:

		CfgGenericLabelText();
		~CfgGenericLabelText();

		// Read the generic label text configuration data from the database.
		void load(DBAccess *dbAccess);

		void reloadLabelText(DBAccess *dbAccess);

		short getMaxLabelTexts() const {return maxLabelTexts;}
		void setMaxLabelTexts(DBAccess *dbAccess, short max);

		HobString getLabelText(int labelTextNum) const;
		void setLabelText(int labelTextNum, GenericLabelText* labelText);

		virtual const char *serialize(int format) const;

		void show();

		// pure virtual definitions from CfgBase class
		void updateField(int /*fieldCode*/, const HobString& /*token*/) {}
		void update(const CfgBase* /*newValues*/, bool /*scaleOnlyFields*/ = false) {}

		void save(DBAccess* /*dbAccess*/, const CfgBase* /*newValues*/ = (CfgBase *)NULL, bool /*scaleOnlyFields*/ = false) {}



	private:

		short maxLabelTexts;
		std::vector<GenericLabelText *> labelTexts;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGGENERICLABELTEXT_H
