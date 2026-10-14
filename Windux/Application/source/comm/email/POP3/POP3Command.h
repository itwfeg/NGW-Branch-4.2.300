#ifndef POP3COMMAND_H
#define POP3COMMAND_H



#include <string>
#include <vector>



namespace email
{


	class POP3Command
	{
		public:
			// --- Types and subtypes ---
			typedef enum CommandTypes
			{
				Invalid,

				Statistics,
				List,
				Retrieve,
				Delete,
				NoOperation,
				Reset,
				Quit,

				Capability,
				Authentication,
				User,
				Password,
				Digest,
				Top,
				UniqueIDList,
				SecureTransport,

				LastCommand
			} CommandType;
			typedef std::vector<std::string> parametersContainer_type;
			typedef parametersContainer_type::const_iterator parametersContainer_iterator;


			// --- Constructors and destructor ---
			POP3Command(CommandType type = Invalid);
			virtual ~POP3Command();


			// --- Getters and setters ---
			CommandType getType() const { return type_; }
			void setType(CommandType type) { if ( Invalid <= type && type < LastCommand) type_ = type; }

			int getNumberOfParameters() const { return numParameters_; }
			void setNumberOfParameters(int total) { numParameters_ = total; }

			const std::string& getParameter() const { return numParameters_ ? parameters_[0] : _noParameter_; }
			void setParameter(const std::string& param);

			const std::string& getParameter2() const { return numParameters_ >= 2 ? parameters_[1] : _noParameter_; }
			void setParameter2(const std::string& param);


			// --- Other function members ---
			const std::string& getCommandName() const;
			const std::string& serialize() const;
			bool operator ==(const POP3Command& rhs) const { return type_ == rhs.type_ && type_ != Invalid; }


		protected:


		private:
			// --- Instance data members ---
			CommandType type_;
			int numParameters_;
			parametersContainer_type parameters_;
			mutable std::string serialization_;


			// --- Class data members ---
			static const std::string _InvalidCommand_;
			static const std::string _StatisticsCommand_;
			static const std::string _ListCommand_;
			static const std::string _RetrieveCommand_;
			static const std::string _DeleteCommand_;
			static const std::string _NoOperationCommand_;
			static const std::string _ResetCommand_;
			static const std::string _QuitCommand_;
			static const std::string _CapabilityCommand_;
			static const std::string _AuthenticationCommand_;
			static const std::string _UserCommand_;
			static const std::string _PasswordCommand_;
			static const std::string _DigestCommand_;
			static const std::string _TopCommand_;
			static const std::string _UniqueIDListCommand_;
			static const std::string _SecureTransportCommand_;

			static const std::string _noParameter_;

			static const int _maxParameters_[LastCommand];
			static const int _optionalParameters_[LastCommand];


#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //POP3COMMAND_H

