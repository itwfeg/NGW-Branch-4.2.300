/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGITEM_H
#define CFGITEM_H

#include "evolution.h"
//#include "ConfigurationTags.h"
//#include "HfcObject.h"

#include "CCriticalSection.h"



class CfgItem // : public HfcObject
{
	public:

		// Types
		enum Type
		{
			STRING,
			INT,
			BOOL
		};

		// Constructors and destructor
		CfgItem(const std::string &name, int fieldOffsetCfgNumber, Type type, const std::string &value, const std::string &commTag = "");

		virtual ~CfgItem();

		// Getters and setters
		std::string getName() const { return name_; }
		int getFieldOffsetNumber() const { return fieldOffsetCfgNumber_; }
		Type getType() const { return type_; }
		std::string getCommTag() const { return commTag_; }

		bool isDefaulted() const { return defaulted_; }
		void setDefaulted(bool defaulted) { defaulted_ = defaulted; }

		bool isModified() const { return modified_; }
		void setModified(bool modified) { modified_ = modified; }

		bool isPersistent() const { return persistent_; }
		void setPersistent(bool persistent) { persistent_ = persistent; }

		std::string getValue() const { return value_; }
		void setValue(const std::string &value);
		void setValue(int value);
		void setValue(bool value);

		operator std::string() const { return value_; }
		operator int() const { return toInt(); }
		operator bool() const { return toBool(); }

		std::string toString() const { return value_; }
		int toInt() const { return (type_ == INT) ? valueInt_ : ((type_ == BOOL) ? ((valueBool_) ? 1 : 0) : 0); }
		bool toBool() const { return (type_ == BOOL) ? valueBool_ : ((type_ == INT) ? ((valueInt_ != 0) ? true : false) : false); }



	private:

		Util::CSmartCriticalSection  myCS_;		

		std::string name_;
		int fieldOffsetCfgNumber_;
		Type type_;
		std::string value_;  // This is always set, since it is stored in the database as a string.
		int  valueInt_;
		bool valueBool_;

		// Put the tag with the configuration item, then config items that don't have tags
		// are considered to not be allowed to be exported.
		std::string commTag_;

		bool defaulted_;  // Value is the application set default value; it has not been read from the database.

		bool modified_;  // Needs to be saved to the database (If not CfgI)

		bool persistent_;  // Is stored in the database, otherwise, only lasts as long as scale is booted.
		                   // (Initially used for weigher config parameters that no longer get stored to the database.)

		void syncValues();


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGITEM_H
