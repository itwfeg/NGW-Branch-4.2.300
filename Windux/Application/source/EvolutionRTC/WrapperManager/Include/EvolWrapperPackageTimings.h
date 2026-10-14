#ifndef EVOLWRAPPERPACKAGETIMINGS_H
#define EVOLWRAPPERPACKAGETIMINGS_H



#include <string>
#include <list>
#include <functional>
#include <algorithm>


#include "WrapperGeneric.h"
#include "WrapperEvent.h"
#include "Wrapper.h"
#include "WrapBox.h"
#include "EvolWrapperMessages.h"


class HobThread;

using namespace std;

class EvolWrapperPackageTimings : public WrapperGeneric
{
	public:
		// --- Types and subtypes ---
		typedef std::list<WrapBox> WrapBoxContainer_type;
		typedef WrapBoxContainer_type::iterator iterator;
		typedef WrapBoxContainer_type::const_iterator const_iterator;

		static const int _WrSettingsWrite_ = 163;
		class isIndex : public std::binary_function<WrapBox, int, bool>
		{
			public:
				isIndex() : index_(0) {}
            bool operator() (const WrapBox& UNUSED(wrapBox), int index) const
						{ UNREFERENCED_PARAMETER(wrapBox);
						  return index_++ == index; }

			private:
				mutable int index_;
		};


		// --- Constructors and destructor ---
		EvolWrapperPackageTimings(Hobart::CEvolMsgReceiver* pHobThread = 0);
		EvolWrapperPackageTimings(const char* buffer);
		EvolWrapperPackageTimings(EvolWrapperPackageTimings* ewpt);
		~EvolWrapperPackageTimings();


		// --- Accessor methods ---
		int getNumberWrapBoxes() const { return (int)wrapBoxes_.size(); }
		WrapBox* getWrapBox(int index);
		const WrapBox* getWrapBox(int index) const;
//		const WrapBox* getWrapBox(const std::string& name) const;


		// --- Processing methods ---
		iterator begin() { return wrapBoxes_.begin(); }
		const_iterator begin() const { return wrapBoxes_.begin(); }

		iterator end() { return wrapBoxes_.end(); }
		const_iterator end() const { return wrapBoxes_.end(); }

		void initDummy();
		bool changePrecedence(int wrapBox, int delta);
		void addWrapBox(const WrapBox& wrapBox) { wrapBoxes_.push_back(wrapBox); }
		bool insertWrapBox(const WrapBox& newWrapBox, int precedence);
		bool updateWrapBox(const WrapBox& wrapBox, int index);
		void removeWrapBox(int wb);

		const char* serialize()const;
		UINT32 getSerializedLength()const;

		void show(const std::string& title) const;

		UINT16 getDisposition() const;
		void setDisposition(UINT16 value);

		void update(const char* buffer, unsigned int version = 0);


	private:

		// --- Instance members ---
		WrapBoxContainer_type wrapBoxes_;
		UINT16 disposition;



		// --- Class members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* EVOLWRAPPERPACKAGETIMINGS_H */
