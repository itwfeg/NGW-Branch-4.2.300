#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H



#include <deque>


#include "Commands/Command.h"
#include "Synchronizer.h"

#include "CSmartPtr.h"
#include "util/thread/mutex.h"
#include "util/thread/lock.h"



namespace Hobart
{
	class CEvolMsgReceiver;
}



namespace synchronization
{


	class QueueManager
	{
	public:
		/* --- Types and subtypes --- */
		typedef enum
		{
			CommunicationQueue,
			DatabaseQueue,

			InvalidQueue
		} QueueType;

		typedef std::deque< Util::CSmartPtr<Command> > QueueContainer;
		typedef QueueContainer::iterator QueueIterator;
		typedef QueueContainer::const_iterator QueueConstIterator;


		/* --- Constructors and destructor --- */
		QueueManager(QueueType type, Hobart::CEvolMsgReceiver* syncManager, Synchronizer* sync)
			: type_(type), sequenceNumber_(Command::_invalidSequenceNumber_), msgManager_(syncManager),
			synchronizer_(sync), synchronizerProcessing_(false) {}
		virtual ~QueueManager() {}


		/* --- Getters and setters --- */
		QueueType getType() const { return type_; }
		const std::string& getTypeName() { return getTypeName(type_); }

		int getNewSequenceNumber() const
			{ util_thread::Lock< util_thread::Mutex<> > lock(seqNumberMutex_); return ++sequenceNumber_; }

		bool isSynchronizerProcessing() const { return synchronizerProcessing_; }
		void setSynchronizerProcessing(bool processing = true) { synchronizerProcessing_ = processing; }
		void clearSynchronizerProcessing() { synchronizerProcessing_ = false; }


		/* --- Instance function members --- */
		void queueCommand(Util::CSmartPtr<Command> command);
		void sendCommandForExecution(Util::CSmartPtr<Command> command);
		void processRetryingCommand(Command* command);
		QueueIterator removeCommand(QueueIterator iter);
		Util::CSmartPtr<Command> removeCommand(Util::CSmartPtr<Command> command);
		void purgeSameTargetCommands(int targetId, int status);
		void purgeNonExecutingCommands();
		void purgeCommands();

		QueueIterator begin() { util_thread::Lock< util_thread::Mutex<> > lock(queueMutex_); return queue_.begin(); }
		QueueIterator end() { util_thread::Lock< util_thread::Mutex<> > lock(queueMutex_); return queue_.end(); }

		size_t size() const
			{ util_thread::Lock< util_thread::Mutex<> > lock(queueMutex_); return queue_.size(); }
		Util::CSmartPtr<Command> getFirstCommand() const
			{ util_thread::Lock< util_thread::Mutex<> > lock(queueMutex_); return queue_.front(); }
		Util::CSmartPtr<Command> findRetryingCommand(int targetId) const;

		void stopSynchronizer();
		void waitUntilSynchronizerStops();
		void resumeProcessing();
		void pauseProcessing(int reason);


		/* --- Class function members --- */
		static const std::string& getTypeName(QueueType type);


	protected:
		/* --- Data members --- */
		QueueType type_;
		QueueContainer queue_;
		mutable util_thread::Mutex<> queueMutex_;
		mutable int sequenceNumber_;
		mutable util_thread::Mutex<> seqNumberMutex_;

		Hobart::CEvolMsgReceiver* msgManager_;
		Util::CSmartPtr<Synchronizer> synchronizer_;
		bool synchronizerProcessing_;         // Since it is boolean it does not need to be synchronized


	private:
		/* --- Class data members --- */
		static const std::string communicationQueueName;
		static const std::string databaseQueueName;
		static const std::string invalidQueueName;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		QueueManager(const QueueManager& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		QueueManager& operator = (const QueueManager& rhs);

		void addCommandToQueue(Util::CSmartPtr<Command> command);
	};


}



#endif /* QUEUEMANAGER_H */
