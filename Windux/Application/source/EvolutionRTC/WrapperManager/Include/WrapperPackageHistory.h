#ifndef WRAPPERPACKAGEHISTORY_H
#define WRAPPERPACKAGEHISTORY_H



#include <string>
#include <vector>



#include "EvolMessage.h"
#include "WrapperEvent.h"
#include "Wrapper.h"


//class HobThread;



class WrapperPackageHistory : public EvolMessage
{
	public:
		// --- Types and subtypes ---
		struct HistoryItem
		{
			HistoryItem(WrPackCode packCode = UndefinedPackCode, UINT16 width = 0, UINT16 length = 0)
					: packCode_(packCode), width_(width), length_(length) {}
			HistoryItem(const HistoryItem& item)
					: packCode_(item.packCode_), width_(item.width_), length_(item.length_) {}
			~HistoryItem() {}

			HistoryItem& operator=(const HistoryItem& rhs)
					{ packCode_ = rhs.packCode_; width_ = rhs.width_; length_ = rhs.length_; return *this; }

			WrPackCode packCode_;
			UINT16 width_;
			UINT16 length_;
		};
		class HistoryIterator
		{
			friend class WrapperPackageHistory;

			public:
				HistoryIterator(const HistoryIterator& rhs);
				~HistoryIterator();
					
				HistoryIterator& operator=(const HistoryIterator& rhs);
				HistoryItem& operator*() { return container_->getElement(index_); }
				const HistoryItem& operator*() const { return container_->getElement(index_); }
				HistoryItem* operator->() { return &container_->getElement(index_); }
				const HistoryItem* operator->() const { return &container_->getElement(index_); }
				HistoryIterator& operator++();
				const HistoryIterator& operator++() const;
				HistoryIterator operator++(int);
				const HistoryIterator operator++(int) const;
				HistoryIterator& operator--();
				const HistoryIterator& operator--() const;
				HistoryIterator operator--(int);
				const HistoryIterator operator--(int) const;
				bool operator==(const HistoryIterator& rhs) const;
				bool operator!=(const HistoryIterator& rhs) const { return !(*this == rhs); }

				void show() const;

			private:
				WrapperPackageHistory* container_;
				mutable int index_;

				HistoryIterator(WrapperPackageHistory* container, int index)
						: container_(container), index_(index) {}

				// Prevent default constructor
				HistoryIterator();

#ifdef USE_ACCOUNTANT
				static int dataType;
				static std::string dataTypeName;
#endif

		};
		typedef std::vector<HistoryItem> historyContainer_type;
		typedef HistoryItem value_type;
		typedef int size_type;

		typedef HistoryIterator iterator;
		typedef const HistoryIterator const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

		typedef HistoryItem* pointer;
		typedef const HistoryItem* const_pointer;
		typedef HistoryItem& reference;
		typedef const HistoryItem& const_reference;


		// --- Constructors and destructor ---
		WrapperPackageHistory(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		WrapperPackageHistory(const char *buffer);
		virtual ~WrapperPackageHistory();
		 

		// --- Getter and Setter methods ---
		int getTotalElements() const { return totalElements_; }

		int getFirstElement() const { return firstElement_; }

		HistoryItem& getElement(int index) { return history_[index]; }
		const HistoryItem& getElement(int index) const { return history_[index]; }

		int getNextIndex(int index);
		int getPreviousIndex(int index);


		// --- Other methods ---
		iterator begin();
		const_iterator begin() const;
		iterator end() { return iterator(this, -1); }
		const_iterator end() const { return const_iterator(const_cast<WrapperPackageHistory*>(this), -1); }

		void addPackage(const HistoryItem& item);
		void addPackage(WrPackCode packCode, UINT16 width, UINT16 length);

		const char* serialize() const;

		void show();	


	private:
		static const int _maxElements_ = 10;

		int totalElements_;
		int firstElement_;
		historyContainer_type history_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif /* WRAPPERPACKAGEHISTORY_H */
