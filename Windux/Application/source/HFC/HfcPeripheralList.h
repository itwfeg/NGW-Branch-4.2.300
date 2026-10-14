/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPERIPHERALLIST_H
#define HFCPERIPHERALLIST_H

#include "HfcObject.h"
#include "EvolMessage.h"

#include "Event.h"
#include "Weigher.h"
#include "Printer.h"
#include "LabelApplier.h"
#include "Wrapper.h"
#include "Numeric.h"
#include "Marquee.h"
#include "GCD.h"
#include "CCriticalSection.h"


class HfcPeripheralList : public HfcObject, public EvolMessage
{
	public:
		HfcPeripheralList(const HfcPeripheralList& obj);
		HfcPeripheralList(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		HfcPeripheralList(const char *buffer);
		~HfcPeripheralList();

		const char *serialize() const;
		const char *serialize(int format = InternalFormat) const;
		void show() const;

		void setFirstWeigher(WeigherKind wgKind);
		WeigherKind getFirstWeigher() const;

		void setSecondWeigher(WeigherKind wgKind);
		WeigherKind getSecondWeigher() const;

		void setFirstPrinter(PrinterKind prKind);
		PrinterKind getFirstPrinter() const;

		void setSecondPrinter(PrinterKind prKind);
		PrinterKind getSecondPrinter() const;

		void setThirdPrinter(PrinterKind prKind);
		PrinterKind getThirdPrinter() const;

		void setLabelApplier(LabelApplierKind laKind);
		LabelApplierKind getLabelApplier() const;

		void setWrapper(WrapperKind wrKind);
		WrapperKind getWrapper() const;

		void setNumeric(NumericKind nmKind);
		NumericKind getNumeric() const;

		void setMarquee(MarqueeKind mqKind);
		MarqueeKind getMarquee() const;

		void setBakeryLabelApplier(LabelApplierKind laKind);
		LabelApplierKind getBakeryLabelApplier() const;
		void operator=(const HfcPeripheralList &periphList);

		void setGCD(GCDKind gcdKind);
		GCDKind getGCD() const;
		bool isCustomerDisplayAttached() const;		//	GCDR-158

		bool isWeigherAttached() const;
		bool isWeigherAttached(WeigherType weigherNumber) const;
		bool isPrinterAttached() const;
		bool isPrinterAttached(PrinterType printerNumber) const;
		bool isWrapperAttached() const;
		bool isEvolutionWrapperAttached() const;
#ifdef SUPPORT_ULTIMA_WRAPPERS
		bool isUltimaWrapperAttached() const;
#endif
		bool isLabelApplierAttached() const;
		bool isCLAAttached() const;
		bool isCLAEPSAC2Attached() const;
		bool isILAAttached() const;
		bool isEvolLAAttached() const;


	private:

		mutable Util::CSmartCriticalSection listCS;

		typedef enum HfcPeripheralListFieldOffsets
		{
			fieldOffsetFirstWeigher,
			fieldOffsetSecondWeigher,
			fieldOffsetFirstPrinter,
			fieldOffsetSecondPrinter,
			fieldOffsetThirdPrinter,
			fieldOffsetLabelApplier,
			fieldOffsetWrapper,
			fieldOffsetNumeric,
			fieldOffsetMarquee,
			fieldOffsetGCD,
			EndHfcPeripheralListFieldOffsets	//	MUST be last!
		}   HfcPeripheralListFieldOffsets_t;

		void initialize();

		EvolMessage *deviceKinds[EndHfcPeripheralListFieldOffsets];



#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//HFCPERIPHERALLIST_H
