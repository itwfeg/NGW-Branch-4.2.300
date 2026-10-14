#ifndef EVOLWRAPPERDIAGNOSTICSTESTRESULT_H
#define EVOLWRAPPERDIAGNOSTICSTESTRESULT_H



#include <string>

#include "WrapperGeneric.h"


class HobThread;



class EvolWrapperDiagnosticsTestResult : public WrapperGeneric
{
	public:
		// --- Types and subtypes ---

		// --- Constructors and destructor ---
		EvolWrapperDiagnosticsTestResult(Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperDiagnosticsTestResult(const char* buffer);
		~EvolWrapperDiagnosticsTestResult();


		// --- Accessor methods ---
		int getTest() const { return test_; }
		void setTest(int value);

		bool getTestResult() const { return testResult_; }
		void setTestResult(bool value) { testResult_ = value; }

		bool getOpenLoopOverride() const { return openLoopOverride_; }
		void setOpenLoopOverride(bool value) { openLoopOverride_ = value; }

		int getDiag() const { return diag_; }
		void setDiag(int value) { diag_ = value; }

		int getDiagExpectedResult() const { return diagExpectedResult_; }
		void setDiagExpectedResult(int value) { diagExpectedResult_ = value; }

		int getDiagActualResult() const { return diagActualResult_; }
		void setDiagActualResult(int value) { diagActualResult_ = value; }

		int getErrorCode() const { return errorCode_; }
		void setErrorCode(int value);

		const char* serialize()const;
		void show() const;


	private:
		typedef std::pair<char*, size_t> memberPair;
		typedef std::list<memberPair> memberPairList;


		// --- Private members ---
		int test_;								// test the wrapper performed (error code)
		bool testResult_;						// test result (pass/fail)

		bool openLoopOverride_;					// true when running in open loop mode

		int diag_;								// wrapper internal diag test
		int diagExpectedResult_;				// expected result of internal diag test
		int diagActualResult_;					// actual result of internal diag test

		int errorCode_;							// diag test error code result

		// --- Class Members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif

		void initialize();

		memberPairList getMembers() const;

};



#endif /* EVOLWRAPPERDIAGNOSTICSTESTRESULT_H */
