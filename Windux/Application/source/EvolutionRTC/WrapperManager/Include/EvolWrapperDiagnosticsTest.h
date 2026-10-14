#ifndef EVOLWRAPPERDIAGNOSTICSTEST_H
#define EVOLWRAPPERDIAGNOSTICSTEST_H



#include <string>

#include "WrapperGeneric.h"


class HobThread;



class EvolWrapperDiagnosticsTest : public WrapperGeneric
{
	public:
		// --- Types and subtypes ---

		// --- Constructors and destructor ---
		EvolWrapperDiagnosticsTest(int test, Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperDiagnosticsTest(const char* buffer);
		~EvolWrapperDiagnosticsTest();


		// --- Accessor methods ---
		int getTest() const { return test_; }
		void setTest(int value) { test_ = value; }

		const char* serialize()const;
		void show() const;


	private:
		// --- Private members ---
		int test_;



		// --- Class Members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* EVOLWRAPPERDIAGNOSTICSTEST_H */
