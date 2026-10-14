#ifndef GENERICHANDLE_H
#define GENERICHANDLE_H



//#pragma warning(disable: 4284)

#include "PAL_platformheader.h"
//#include <tchar.h>
//#include "ErrorHandling.h"




namespace Util
{



	class GenericHandle
	{
		public:
			/* --- Types and subtypes --- */
			class CountedHandle
			{
				public:
                #if defined(LINUX_FIX)
					CountedHandle( HANDLE handle ) { handle_ = handle; PAL_absclass::PAL_InterlockedExchange((long *)&references_, 0); }
                #else
					CountedHandle( HANDLE handle ) { handle_ = handle; InterlockedExchange(&references_, 0); }
                #endif

					HANDLE Get ( ) const { return handle_; }

                #if defined(LINUX_FIX)
					void Use ( ) { PAL_absclass::PAL_InterlockedIncrement((long *)&references_); }
					int Dismiss ( ) { return PAL_absclass::PAL_InterlockedDecrement((long *)&references_); }
                #else
					void Use ( ) { PAL_absclass::PAL_InterlockedIncrement(&references_); }
					int Dismiss ( ) { return PAL_absclass::PAL_InterlockedDecrement(&references_); }
                #endif

//					HANDLE Release ( ) { HANDLE h = handle_; handle_ = NULL; return h; }


				private:
					/* --- Instance data members --- */
					HANDLE handle_;
					volatile LONG references_;
			};


			/* --- Constructors and destructor --- */
			explicit GenericHandle ( HANDLE handle = NULL ) { CreateStorage(handle); }
			GenericHandle ( const GenericHandle& rhs ) { ReuseStorage(rhs.storage_); }
			~GenericHandle ( ) { DeleteStorage(); }


			/* --- Getters and setters --- */
			       operator HANDLE ( ) const { return storage_->Get(); }
			HANDLE Get( )              const { return storage_->Get(); }
			bool   null( )             const { return storage_->Get() == NULL; }
			bool   notNull( )          const { return storage_->Get() != NULL; }


			/* --- Instance function members --- */
			GenericHandle& operator= ( HANDLE handle )
				{ DeleteStorage(); CreateStorage(handle); return *this; }

			GenericHandle& operator= ( const GenericHandle& rhs )
				{ DeleteStorage(); ReuseStorage(rhs.storage_); return *this; }

			HANDLE Release ( ) { HANDLE h = DeleteStorage(); CreateStorage(NULL); return h; }


		private:
			/* --- Instance data members --- */
			CountedHandle* storage_;


			/* --- Instance function members --- */
			void CreateStorage(HANDLE handle) { storage_ = new CountedHandle(handle); storage_->Use(); }
			void ReuseStorage(CountedHandle* storage) { storage_ = storage; storage_->Use(); }
			HANDLE DeleteStorage()
				{ if ( storage_->Dismiss() == 0 ) { HANDLE h = storage_->Get(); delete storage_; return h; } else return NULL; }
	};



}



#endif /* GENERICHANDLE_H */
