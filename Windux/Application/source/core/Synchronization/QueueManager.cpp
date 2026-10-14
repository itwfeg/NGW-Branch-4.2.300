#include "QueueManager.h"


#include "TimerManager.h"


#include "Messages/ProcessedMessage.h"
#include "Messages/ExecuteCommandMessage.h"


#include "Debug.h"

#include "util/e_string/utilstring.h"



namespace synchronization
{


	const std::string QueueManager::communicationQueueName   ("CommQueue");
	const std::string QueueManager::databaseQueueName        ("DBQueue");
	const std::string QueueManager::invalidQueueName         ("InvalidQueue");


	void QueueManager::queueCommand(Util::CSmartPtr<Command> command)
	{
		/* Push the command at the end of the corresponding queue, assigning it
		the queue sequence number (after incrementing it). */
		command->setQueue(type_);
		if ( command->getSequenceNumber() == Command::_invalidSequenceNumber_ )
			command->setSequenceNumber(getNewSequenceNumber());

		addCommandToQueue(command);
	}


	void QueueManager::addCommandToQueue(Util::CSmartPtr<Command> command)
	{
		queue_.push_back(command);

#ifndef EVOL_NDEBUG
		Debug::trace("QueueManager", std::string(type_ == CommunicationQueue ? "CommQueue" : "DBQueue") +
				" just added a new command of type " + command->getTypeName() + " with sequence number " +
				util_string::inttostring(command->getSequenceNumber()) + " (there are currently " +
				util_string::inttostring(size()) + " command(s) in queue)");
#endif
	}


	void QueueManager::sendCommandForExecution(Util::CSmartPtr<Command> command)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("QueueManager", std::string(type_ == CommunicationQueue ? "CommQueue" : "DBQueue") +
				" is going to send command of type " + command->getTypeName() + " with sequence number " +
				util_string::inttostring(command->getSequenceNumber()) + " to be executed.");
#endif

		ExecuteCommandMessage message(command);
		msgManager_->dispatchEvent(&message, synchronizer_->getMessageReceiver());
	}


	void QueueManager::processRetryingCommand(Command* command)
	{
		bool atBegining = true;
		QueueIterator iter = queue_.begin();
		QueueIterator previous = queue_.end();
		while ( iter != queue_.end() )
		{
			/* (uc5.56) For each command not marked as Executing, starting at the front of
			the queue until the command to retry, if the command is targetting the same scale,
			remove it from the queue and queue it at the end of the queue. */
			if ( !(*iter)->isExecutingState() )
			{
				if ( (*iter)->getSequenceNumber() != command->getSequenceNumber() )
				{
					if ( (*iter)->getTargetIdentifier() == command->getTargetIdentifier() )
					{
						Util::CSmartPtr<Command> sameTargetCommand = *iter;
						queue_.erase(iter);

#ifndef EVOL_NDEBUG
						Debug::trace("QueueManager", getTypeName() + " just removed (1) a command of type " +
								sameTargetCommand->getTypeName() + " with sequence number " +
								util_string::inttostring(sameTargetCommand->getSequenceNumber()) +
								" (there are currently " + util_string::inttostring(size()) + " command(s) in queue)");
#endif

						addCommandToQueue(sameTargetCommand);
						if ( !atBegining )
						{
							iter = previous;
							++iter;
						}
						else
							iter = queue_.begin();
					}
					else
					{
						atBegining = false;
						previous = iter;
						++iter;
					}
				}
				else
					break;
			}
			else
			{
				atBegining = false;
				previous = iter;
				++iter;
			}
		}


		if ( iter != queue_.end() )
		{
			/* (uc5.56) Remove from the queue the command to retry and put it at the front
			of the queue (after any command marked as Executing). */
			Util::CSmartPtr<Command> foundCommand = *iter;
			queue_.erase(iter);

#ifndef EVOL_NDEBUG
			Debug::trace("QueueManager", getTypeName() + " just removed (2) a command of type " +
				foundCommand->getTypeName() + " with sequence number " + util_string::inttostring(foundCommand->getSequenceNumber()) +
				" (there are currently " + util_string::inttostring(size()) + " command(s) in queue)");
#endif

			iter = queue_.begin();
			while ( iter != queue_.end() )
			{
				if ( !(*iter)->isExecutingState() )
					break;

				++iter;
			}

			queue_.insert(iter, foundCommand);

#ifndef EVOL_NDEBUG
			Debug::trace("QueueManager", std::string(type_ == CommunicationQueue ? "CommQueue" : "DBQueue") +
					" just added a retry command of type " + command->getTypeName() + " with sequence number " +
					util_string::inttostring(command->getSequenceNumber()) + " (there are currently " +
					util_string::inttostring(size()) + " command(s) in queue)");
#endif
		}
	}


	QueueManager::QueueIterator QueueManager::removeCommand(QueueIterator iter)
	{
		if ( iter != end() )
		{
			Util::CSmartPtr<Command> command(*iter);
			iter = queue_.erase(iter);

#ifndef EVOL_NDEBUG
			Debug::trace("QueueManager", getTypeName() + " just removed (3) a command of type " + command->getTypeName() +
					" with sequence number " + util_string::inttostring(command->getSequenceNumber()) +
					" (there are currently " + util_string::inttostring(size()) + " command(s) in queue)");
#endif

			return iter;
		}

		return end();
	}


	Util::CSmartPtr<Command> QueueManager::removeCommand(Util::CSmartPtr<Command> command)
	{
		Util::CSmartPtr<Command> found;

		QueueIterator iter = queue_.begin();
		while ( iter != queue_.end() )
		{
			if ( (*iter)->getSequenceNumber() == command->getSequenceNumber() )
			{
				found = *iter;
				queue_.erase(iter);

#ifndef EVOL_NDEBUG
				Debug::trace("QueueManager", getTypeName() +
						" just removed (4) a command of type " + found->getTypeName() + " with sequence number " +
						util_string::inttostring(found->getSequenceNumber()) + " (there are currently " +
						util_string::inttostring(size()) + " command(s) in queue)");
#endif

				break;
			}

			++iter;
		}

		return found;
	}


	Util::CSmartPtr<Command> QueueManager::findRetryingCommand(int targetId) const
	{
		QueueConstIterator iter = queue_.begin();
		while ( iter != queue_.end() )
		{
			if ( (*iter)->getTargetIdentifier() == targetId && (*iter)->isRetryingState() )
					return *iter;


			++iter;
		}

		return Util::CSmartPtr<Command>(0);
	}


	void QueueManager::purgeSameTargetCommands(int targetId, int status)
	{
		Util::CSmartPtr<Command> command;

		QueueIterator iter = queue_.begin();
		while ( iter != queue_.end() )
		{
			/* (uc5.56) For each command in the "communication" queue that targets the same scale,
			perform the following steps: */
			if ( (*iter)->getTargetIdentifier() == targetId )
			{
				/* (uc5.56) Remove the command from the queue. */
				command = *iter;
				iter = queue_.erase(iter);


				/* (uc5.56) If the command contains a completion message, then mark it as "Scale
				off-line failure" and post it. */
				Util::CSmartPtr<ProcessedMessage> completionMessage = command->getCompletionMessage();
				if ( completionMessage.notNull() )
				{
					completionMessage->setStatus(status);
					msgManager_->dispatchEvent(completionMessage.get(), completionMessage->getSourceObject());
				}


				/* (uc5.56) Drop the command. */
				if ( command->isRetryingState() && !command->isRetryingReadyToExecute() )
					TimerManager::getInstance()->removeTimer(command->getRetryTimerId());


#ifndef EVOL_NDEBUG
				Debug::trace("QueueManager", std::string(type_ == CommunicationQueue ? "CommQueue" : "DBQueue") +
						" just purged a command of type " + command->getTypeName() + " with sequence number " +
						util_string::inttostring(command->getSequenceNumber()) + " (there are currently " +
						util_string::inttostring(size()) + " command(s) in queue)");
#endif
			}
			else
				++iter;
		}
	}


	void QueueManager::purgeNonExecutingCommands()
	{
		QueueIterator iter = queue_.begin();
		while ( iter != queue_.end() )
		{
			if ( !(*iter)->isExecutingState() )
			{
#ifndef EVOL_NDEBUG
				Debug::trace("QueueManager", std::string(type_ == CommunicationQueue ? "CommQueue" : "DBQueue") +
						".purgeNonExecutingCommands: going to purge command of type " + (*iter)->getTypeName() +
						" with sequence number " + util_string::inttostring((*iter)->getSequenceNumber()) +
						" (there are currently " + util_string::inttostring(size()) + " command(s) in queue)");
#endif

				if ( (*iter)->isRetryingState() && !(*iter)->isRetryingReadyToExecute() )
					TimerManager::getInstance()->removeTimer((*iter)->getRetryTimerId());

				iter = queue_.erase(iter);
			}
			else
				++iter;
		}


#ifndef EVOL_NDEBUG
		Debug::trace("QueueManager", std::string(type_ == CommunicationQueue ? "CommQueue" : "DBQueue") +
				".purgeNonExecutingCommands: finished leaving only " + util_string::inttostring(size()) + " command(s) in queue");
#endif
	}


	void QueueManager::stopSynchronizer()
	{
		if ( isSynchronizerProcessing() )
			synchronizer_->usecaseStoppingSynchronizer();
	}


	void QueueManager::waitUntilSynchronizerStops()
	{
		synchronizer_->waitUntilRunThreadEnds();
	}


	void QueueManager::resumeProcessing()
	{
		if ( isSynchronizerProcessing() )
			synchronizer_->usecaseResumingProcessing();
	}


	void QueueManager::pauseProcessing(int reason)
	{
		if ( isSynchronizerProcessing() )
			synchronizer_->usecasePausingProcessing(reason);
	}


	const std::string& QueueManager::getTypeName(QueueType type)
	{
		switch ( type )
		{
			case CommunicationQueue: return communicationQueueName;
			case DatabaseQueue:      return databaseQueueName;
			default:                 return invalidQueueName;
		}
	}


}
