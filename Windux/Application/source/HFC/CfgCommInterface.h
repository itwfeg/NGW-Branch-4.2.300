#ifndef CFGCOMMINTERFACE_H
#define CFGCOMMINTERFACE_H



#include "CfgBase.h"
#include "HfcObject.h"


#include "tags/Tags.h"



class CfgCommInterface : public CfgBase, public HfcObject
{
	public:
		/* --- Constructors and destructor --- */
		CfgCommInterface();
		CfgCommInterface(const CfgCommInterface& CfgCommInterface);
		CfgCommInterface(const char *data, int len, int formatType);
		~CfgCommInterface();


		/* --- Getters and setters --- */
		bool isClearProductClassTotalsClearsTransactions() const { return clearProductClassTotalsClearsTransactions_; }
		void setClearProductClassTotalsClearsTransaction(bool set) { if ( clearProductClassTotalsClearsTransactions_ != set ) { clearProductClassTotalsClearsTransactions_ = set; saveNeeded = true; } }

		bool isClearHourlyTotalsClearsTransactions() const { return clearHourlyTotalsClearsTransactions_; }
		void setClearHourlyTotalsClearsTransaction(bool set) { if ( clearHourlyTotalsClearsTransactions_ != set ) { clearHourlyTotalsClearsTransactions_ = set; saveNeeded = true; } }

		bool isClearOperatorTotalsClearsTransactions() const { return clearOperatorTotalsClearsTransactions_; }
		void setClearOperatorTotalsClearsTransaction(bool set) { if ( clearOperatorTotalsClearsTransactions_ != set ) { clearOperatorTotalsClearsTransactions_ = set; saveNeeded = true; } }

		bool isClearCOOLTotalsClearsTransactions() const { return clearCOOLTotalsClearsTransactions_; }
		void setClearCOOLTotalsClearsTransactions(bool set) { if ( clearCOOLTotalsClearsTransactions_ != set ) { clearCOOLTotalsClearsTransactions_ = set; saveNeeded = true; } }

		bool isClearOperatorChangesClearsTransactions() const { return clearOperatorChangesClearsTransactions_; }
		void setClearOperatorChangesClearsTransactions(bool set) { if ( clearOperatorChangesClearsTransactions_ != set ) { clearOperatorChangesClearsTransactions_ = set; saveNeeded = true; } }

		bool isChanged() { return isSaveNeeded(); }


		/* --- Public APIs --- */
		void load(DBAccess * dbAccess);        // Read the scale configuration data from the database.
		void save(DBAccess * dbAccess, const CfgBase * newValues = 0, bool scaleOnlyFields = false);

		void update(const CfgBase * newValues, bool scaleOnlyFields = false);
		void updateField(int fieldCode, const HobString & token);

		void getFieldAsString(int fieldCode, HobString & result, int format = UnknownFormat) const;
		const HobString& getField(int fieldCode, int format) const;

		void operator=(const CfgCommInterface & cfgFTP);

		virtual const char * serialize(int format) const;

		void show();


	protected:
		/* --- Instance data members --- */
		bool clearProductClassTotalsClearsTransactions_;
		bool clearHourlyTotalsClearsTransactions_;
		bool clearOperatorTotalsClearsTransactions_;
		bool clearCOOLTotalsClearsTransactions_;
		bool clearOperatorChangesClearsTransactions_;


	private:
		/* --- Instance data members --- */
		mutable HobString tempValue_;


		/* --- Class data members --- */
		static FieldOffsetsList _commConfigOffsets_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif


		/* --- Private function members --- */
		void init();
		static void initFieldList();
};



#endif //CFGCOMMINTERFACE_H
