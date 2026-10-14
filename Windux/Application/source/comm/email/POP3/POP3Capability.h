#ifndef POP3CAPABILITY_H
#define POP3CAPABILITY_H



#include <string>
#include <vector>



namespace email
{


	class POP3Capability
	{
		public:
			// --- Types and subtypes ---
			typedef enum CapabilityTypes
			{
				Invalid,

				Top,
				User,
				Authentication,
				ResponseCodes,
				LoginDelay,
				Pipelining,
				Expire,
				UniqueIDList,
				SecureTransport,
				Implementation,

				LastCapability
			} CapabilityType;


			// --- Constructors and destructor ---
			POP3Capability(CapabilityType type = Invalid);
			virtual ~POP3Capability();


			// --- Getters and setters ---
			CapabilityType getType() const { return type_; }
			void setType(CapabilityType type) { if ( Invalid <= type && type < LastCapability) type_ = type; }


			// --- Other function members ---
			const std::string& getCapabilityName() const;
			bool operator ==(const POP3Capability& rhs) const { return type_ == rhs.type_ && type_ != Invalid; }


		protected:


		private:
			// --- Instance data members ---
			CapabilityType type_;


			// --- Class data members ---
			static const std::string _InvalidCapability_;
			static const std::string _TopCapability_;
			static const std::string _UserCapability_;
			static const std::string _AuthenticationCapability_;
			static const std::string _ResponseCodesCapability_;
			static const std::string _LoginDelayCapability_;
			static const std::string _PipeliningCapability_;
			static const std::string _ExpireCapability_;
			static const std::string _UniqueIDListCapability_;
			static const std::string _SecureTransportCapability_;
			static const std::string _ImplementationCapability_;


#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


}



#endif //POP3CAPABILITY_H

