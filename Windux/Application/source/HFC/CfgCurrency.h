#ifndef CFGCURRENCY_H
#define CFGCURRENCY_H



#include <list>


#include "CfgBase.h"
#include "HfcObject.h"


#include "Rounding.h"
#include "Power.h"



class CfgCurrency : public CfgBase, public HfcObject
{
	public:
		// --- Types and subtypes ---
		typedef enum
		{
			MonetaryRuleInvalid,

			MonetaryRuleLeading,
			MonetaryRuleTrailing,

			MonetaryRuleLast
		} MonetaryRule;


		// --- Constructors and destructor ---
		CfgCurrency();
		CfgCurrency(const CfgCurrency& cfgScale);
		CfgCurrency(const char *data, int len, int formatType);
		~CfgCurrency();


		// --- Getter and setter methods ---
		char getDecimalSeparator() const { return decimalSeparator_; }
		void setDecimalSeparator(char separator) { if (decimalSeparator_ != separator) { decimalSeparator_ = separator; saveNeeded = true;}}

		MonetaryRule getMonetaryRule() const { return monetaryRule_; }
		void setMonetaryRule(MonetaryRule rule) { if (monetaryRule_ != rule) { monetaryRule_ = rule; saveNeeded = true;}}

		short getTotalPriceTotalDigits() const { return totalPriceTotalDigits_; }
		void setTotalPriceTotalDigits(short digits) { if (totalPriceTotalDigits_ != digits) { totalPriceTotalDigits_ = digits; saveNeeded = true;}}

		short getUnitPriceTotalDigits() const { return unitPriceTotalDigits_; }
		void setUnitPriceTotalDigits(short digits) { if (unitPriceTotalDigits_ != digits) { unitPriceTotalDigits_ = digits; saveNeeded = true;}}

		short getRoundingFactor() const { return roundingFactor_; }
		void setRoundingFactor(short factor) { if (roundingFactor_ != factor) { roundingFactor_ = factor; saveNeeded = true;}}

		Rounding::Method getRoundingMethod() const { return roundingMethod_; }
		void setRoundingMethod(Rounding::Method method) { if (roundingMethod_ != method) { roundingMethod_ = method; saveNeeded = true;}}

		const HobString & getPrimaryMonetarySymbol() const { return primaryMonetarySymbol_; }
		void setPrimaryMonetarySymbol(const HobString & symbol) { if (primaryMonetarySymbol_ != symbol) { primaryMonetarySymbol_.assign(symbol); saveNeeded = true;}}

		short getPrimaryMonetaryDecimalDigits() const { return primaryMonetaryDecimalDigits_; }
		void setPrimaryMonetaryDecimalDigits(short digits) { if (primaryMonetaryDecimalDigits_ != digits) { primaryMonetaryDecimalDigits_ = digits; saveNeeded = true;}}

		const HobString & getSecondaryMonetarySymbol() const { return secondaryMonetarySymbol_; }
		void setSecondaryMonetarySymbol(const HobString & symbol) { if (secondaryMonetarySymbol_ != symbol) { secondaryMonetarySymbol_.assign(symbol); saveNeeded = true;}}

		short getSecondaryMonetaryDecimalDigits() const { return secondaryMonetaryDecimalDigits_; }
		void setSecondaryMonetaryDecimalDigits(short digits) { if (secondaryMonetaryDecimalDigits_ != digits) { secondaryMonetaryDecimalDigits_ = digits; saveNeeded = true;}}

		// --- Convenience functions ---
		const HobString & getMonetarySymbol(bool primary = true) const { return primary ? primaryMonetarySymbol_ : secondaryMonetarySymbol_; }

		short getPricePrecision(bool primary = true) const { return primary ? primaryMonetaryDecimalDigits_ : secondaryMonetaryDecimalDigits_; }
		double getPriceDivisor(bool primary = true) const
			{ return primaryMonetaryDecimalDigits_ >= 0 ? Power::power(10.0, (double)(primary ? primaryMonetaryDecimalDigits_ : secondaryMonetaryDecimalDigits_)) : 100.0; }

		HobPrice getMaxPrice(short digits) const;
		HobPrice getMaxUnitPrice() const;
		HobPrice getMaxTotalPrice() const;

		// --- Virtual methods from CfgBase base class ---
//		bool isFieldScaleOnly(int fieldCode) { return false; }

		void updateField(int fieldCode, const HobString & token);
		void update(const CfgBase * newValues, bool scaleOnlyFields = false);

		void load(DBAccess * dbAccess);
		void save(DBAccess * dbAccess, const CfgBase * newValues = 0, bool scaleOnlyFields = false);

		void show();


		// --- Virtual methods from HfcObject base class ---
		const char *serialize(int format) const;


		// --- Other Instance methods ---
		const HobString& getField(CfgFieldOffsets fieldCode, int format) const;
		void getFieldAsString(CfgFieldOffsets fieldCode, HobString &result, int format) const;
		CfgCurrency& operator=(const CfgCurrency & cfgScale);

		static void initFieldList();		

		bool isChanged();


	protected:
		char decimalSeparator_;                  // Typically '.' or ','.
		MonetaryRule monetaryRule_;              // Leading (1) or Trailing (2)
		short totalPriceTotalDigits_;            // 3-8 digits
		short unitPriceTotalDigits_;             // 3-7 digits
		short roundingFactor_;                   // 1 or 5
		Rounding::Method roundingMethod_;        // conventionsl (0), down (1), up (2), and unconventional (3)

		HobString primaryMonetarySymbol_;        // 0-3 characters
		short primaryMonetaryDecimalDigits_;     // 0-3 digits

		HobString secondaryMonetarySymbol_;      // 0-3 characters
		short secondaryMonetaryDecimalDigits_;   // 0-3 digits


		// --- Instance methods ---
		bool isFieldQuoted(CfgFieldOffsets fieldOffsetCfg) const;


	private:
		// --- Instance data members ---
		mutable HobString tempValue_;


		// --- Instance function members ---
		void init();


		// --- Class data members ---
		static std::list<CfgFieldOffsets> fieldList_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif //CFGCURRENCY_H
