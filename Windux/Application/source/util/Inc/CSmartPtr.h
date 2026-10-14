// Header file included by ANTLR UniLexer

#include "PAL_platformheader.h"

#if defined ( _WIN32 )
#include <tchar.h>
#endif

#include "../Debug.h"

#ifndef CSmartPtr_included
#define CSmartPtr_included

/*******************************************************************************

TYPE:
Template class.

PURPOSE:
Heap memory management.

FUNCTION:
Keeps track of references to a dynamically allocated memory area.
Deletes memory when there are no more references.

INTERFACES:
See 'INTERFACES:' in the module declaration below.

RESOURCES:
None.

REFERENCES:
'Extending the Reference-Counting Pattern' by Vl. Batov, C/C++ Users Journal,
September 1998.

PROCESSING:

DATA:
See 'DATA:' in the class header below.

LOGIC:
-

 *******************************************************************************/

#ifdef PAL_WIN32_RUN
#pragma warning(disable: 4284)
#endif

#include "PAL_platformheader.h"
#include "ErrorHandling.h"




namespace Util
{
    template<class T> class CSmartPtr
    {
        private:

            // Holds address of heap memory and maintains counter of references to it.
            class CCounted
            {
                private:

                    // DATA:

                    T*          m_Ptr;                  // Start address of heap memory.
                    LONG        m_NumReferences;        // Number of references to this heap memory.

                public:

                    // INTERFACES:

                    // Description:     Stores address of heap memory,
                    //                  initializes number of references.
                    // Returns:         Nothing.
                    CCounted
                        (
                         T* i_Ptr
                        )
                        {
                            m_NumReferences = 0;
                            m_Ptr = i_Ptr;
                            PAL_absclass::PAL_InterlockedExchange(&m_NumReferences, 0);
                        }

                    // Description:     Increments number of references.
                    // Returns:         Nothing.
                    void Use
                        (
                        )
                        {
                            PAL_absclass::PAL_InterlockedIncrement(&m_NumReferences);
                        }

                    // Description:     Decrements number of references,
                    //                          deletes itself when last reference dismisses.
                    // Returns:         Nothing.
                    void Dismiss
                        (
                        )
                        {
                            if (PAL_absclass::PAL_InterlockedDecrement(&m_NumReferences) == 0)
                            {
#ifdef __linux__
                                // this eliminates memory leakage in Linux when 
                                // deleting pointer to array, but throws exception
                                // in Win32 build
                                if ( m_Ptr != NULL)
                                    delete m_Ptr;
#else
                                // this avoids exception in win32
                                // not known whether memory leak occurs
                                if ( m_Ptr != NULL)
	                                delete m_Ptr;
#endif
                                m_Ptr = NULL;
                                delete this;
                            }
                        }

                    // Description:     Returns heap memory address and set sets own copy
                    //                          of the address to NULL to prevent deletion.
                    // Returns:         Heap memory address.
                    T* Release
                        (
                        )
                        {
                            // Return the heap memory address.
                            T* t = m_Ptr;
                            m_Ptr = NULL;
                            return t;
                        }

                    // Description:     Operators to access the heap memory.
                    // Returns:         Address or value of dynamic object.
                    operator T&() const { return *m_Ptr; }
                    operator T*() const { return  m_Ptr; }
                    T* operator-> () const { return  m_Ptr; }

            };

            // DATA:

            CCounted*   m_Counted;              // Start address of heap memory and reference counter.

        public:

            // INTERFACES:

            // Description:     Constructor.
            // Returns:         Nothing.
            explicit CSmartPtr
                (
                 T* i_Ptr = NULL        // Start address of heap memory.
                )
                {
                    m_Counted = new CCounted(i_Ptr);
                    m_Counted->Use();
                }

            // Description:     Copy constructor.
            // Returns:         Nothing.
            CSmartPtr
                (
                 const CSmartPtr<T>& i_Src      // Reference CSmartPtr.
                )
                {
                    m_Counted = i_Src.m_Counted;
                    m_Counted->Use();
                }

            // Description:     Destructor.
            // Returns:         Nothing.
            ~CSmartPtr
                (
                )
                {
                    m_Counted->Dismiss();
                }

            // Description:     Assignment operator.
            // Returns:         Reference to itself.
            CSmartPtr<T>& operator=
                (
                 T* i_Ptr       // Start address of heap memory.
                )
                {
                    m_Counted->Dismiss();
                    m_Counted = new CCounted(i_Ptr);
                    m_Counted->Use();
                    return *this;
                }

            // Description:     Assignment operator.
            // Returns:         Reference to itself.
            CSmartPtr<T>& operator=
                (
                 const CSmartPtr<T>& i_Src      // Reference CSmartPtr.
                )
                {
                    m_Counted->Dismiss();
                    m_Counted = i_Src.m_Counted;
                    m_Counted->Use();
                    return *this;
                }

            // Description:     Operators and functions to access the heap memory.
            // Returns:         Address or value of dynamic object.
            operator T&() const { return m_Counted->operator T&();      }
            operator T*() const { return m_Counted->operator T*();      }
            T* operator-> () const      { return m_Counted->operator->();       }
            T* Get()         const      { return m_Counted->operator->();       }
            bool null()      const  { return m_Counted->operator->() == 0; }
            bool notNull()   const  { return m_Counted->operator->() != 0; }

            // Only for easier conversion of former auto_ptr's.
            T* get()         const      { return m_Counted->operator->();       }

            // Description:     Returns heap memory address and set sets own copy
            //                          of the address to NULL to prevent deletion.
            // Returns:         Heap memory address.
            T* Release
                (
                )
                {
                    return m_Counted->Release();
                }

            // Description:     Casts a Util::CSmartPtr<F> object to a Util:CSmartPtr<T> object.
            //                          Note that this will actually release the original and set it to NULL.
            // Returns:         Util::CSmartPtr<T>.
            //                          The address will be NULL if
            //                          - the cast operation is not possible because the classes are not
            //                            suitably related, or
            //                          - the original CSmartPtr contains a NULL pointer.
            template<class F> inline static Util::CSmartPtr<T> Cast
                (
                 Util::CSmartPtr<F> i_From
                )
                {
                    T* to = NULL;
                    if (dynamic_cast<T*>(i_From.Get()))
                        to = dynamic_cast<T*>(i_From.Release());
                    return Util::CSmartPtr<T>(to);
                }

    };




    // Description:     Casts a Util::CSmartPtr<CFrom> object to a Util:CSmartPtr<CTo> object.
    //                          Note that this will actually release the original and set it to NULL.
    // Returns:         Util::CSmartPtr<CTo>.
    //                          The address will be NULL if
    //                          - the cast operation is not possible because the classes are not
    //                            suitably related, or
    //                          - the original CSmartPtr contains a NULL pointer.
    //
    // NOTE:            Function exists only for backward compatibility.
    //                          Rather use the CSmartPtr<CTo>::Cast() function directly!
    //

    template<class CTo, class CFrom> inline Util::CSmartPtr<CTo> SmartPtrCast
        (
         Util::CSmartPtr<CFrom> i_From
        )
        {
            try
            {
                return Util::CSmartPtr<CTo>::Cast(i_From);
            }
            catch (...)
            {
		        Debug::trace("CSmartPtr.error", "SmartPtrCast():  catch (...)");
                LogCatchAndThrow;
            }
        }


    // Description
    //          is the CSmartPtr that we have null
    // Arguments
    //                  CSmartPtr<T>& i_T
    // Returns
    //          inline bool
    // CJC May 11/2000 NOTE: This function was created to replace AutoPtrNotNull since
    // the use of auto_ptrs is being removed
    template<class T>
        inline bool SmartPtrNotNull(Util::CSmartPtr<T>& i_T)
        {
            return i_T.Get() != NULL;
        }


} // end namespace


#endif
