#ifndef SYNCHRONIZATIONRECORD_H
#define SYNCHRONIZATIONRECORD_H



#include "HfcObject.h"


#include "Scale.h"


#include "tags/Tags.h"



namespace hfc
{


	class SynchronizationRecord : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				CommandInvalid,

				CommandRegisterSecondaryScale,
				CommandNotifyLocalBatches,

				CommandLast
			} SynchronizationCommand;


			/* --- Constructors and destructor --- */


			/* --- Getters and setters --- */
			SynchronizationCommand getCommand() const { return command_; }
			void setCommand(SynchronizationCommand command) { command_ = command; }


			/* --- Instance function members --- */
			const char * serialize(int format) const;


			/* --- Class function members --- */
			static void initFieldList();

			static const std::string& getCommandName(SynchronizationCommand command);


		protected:
			/* --- Constructors and destructor --- */
			SynchronizationRecord(SynchronizationCommand command);
			SynchronizationRecord(const char * data, int len, int formatType);
			~SynchronizationRecord();


		private:
			/* --- Data members --- */
			SynchronizationCommand command_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
			static FieldOffsetsList _fieldList_;

			static const std::string _commandNotifyLocalBatchesName_;
			static const std::string _commandUnknownName_;


			/* --- Constructors --- */


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* SYNCHRONIZATIONRECORD_H */
