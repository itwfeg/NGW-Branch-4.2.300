//
//      A very simple unit test module
//
#if !defined ( __linux__ )
#include <crtdbg.h>
#include "stdafx.h" 
#endif
#include "PAL_platformheader.h"
#include "Test.h" // This is the UnitTester's header
#include "ErrorHandling.h"
// If you do not need to check memory leaks
// then remove the following
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace Test // required namespace
{
    //
    //  CSimpleTest derives from the abstract class CUnitTest.
    //  
    //  
    class CSimpleTest : public CUnitTest
    {
        public:
            CSimpleTest(){};
            virtual ~CSimpleTest(){};
            // Required Implementation of Name
            const TCHAR * Name() { return _T("SimpleTest");};
            // Required Implementation of Description
            const TCHAR * Description() { return _T("A Simple Test ");};
            // Required Implementation of TestCode
            bool TestCode() 
            {
                try
                {
                    // Insert your test code here, in a try block
                    int x = 2,  y = 2;
                    return 4 == ( x+y );
                }
                // Catch any expected exceptions
                /*
                   catch(   )
                   {

                   }
                   */
                catch(...)  // an unexpected exception
                {
                    return false;
                }

            }


    };
    //
    //  Register the UnitTest objects with the application
    //  The user MUST implement this function.
    //
    void DoRegistration( CUnitTestManager & manager)
    {
        // Note : Do not use a pointer to an object that
        //        is already constructed. The CUnitTestManager
        //        assumes ownership of the object, and will
        //        destroy it when the application exits.
        manager.Register(new CSimpleTest);
    }
} // end of namespace TEST
