#ifndef EVOLWRAPPERPACKAGEHISTORY_H
#define EVOLWRAPPERPACKAGEHISTORY_H

#include <string>
#include <vector>

#include "EvolMessage.h"
#include "WrapperEvent.h"
#include "Wrapper.h"

class HobThread;

class EvolWrapperPackageHistory : public EvolMessage
{
	public:
		static const unsigned int _MaxNameLength_ = 10;

		// --- Types and subtypes ---
		struct HistoryItem
		{
			HistoryItem(UINT16 packId = 0, UINT16 width = 0, UINT16 length = 0, UINT16 height = 0, UINT16 lip = 0, WrFilmWidthSelection filmSelection = NoFilmWidthSelected, bool bestFit = false, bool modifiedStretch = false)
					: packId_(packId), width_(width), length_(length), height_(height), lip_(lip), filmSelection_(filmSelection), usedWrapBoxBestFit_(bestFit), modifiedNetStretch_(modifiedStretch) {}
			HistoryItem(const HistoryItem& item)
					: packId_(item.packId_), width_(item.width_), length_(item.length_), height_(item.height_), lip_(item.lip_), filmSelection_(item.filmSelection_), usedWrapBoxBestFit_(item.usedWrapBoxBestFit_), modifiedNetStretch_(item.modifiedNetStretch_) {}
			~HistoryItem() {}

			HistoryItem& operator=(const HistoryItem& rhs)
					{ packId_ = rhs.packId_; width_ = rhs.width_; length_ = rhs.length_;
					  height_ = rhs.height_; lip_ = rhs.lip_; filmSelection_ = rhs.filmSelection_;
					  trayName_ = rhs.trayName_;
					  usedWrapBoxBestFit_ = rhs.usedWrapBoxBestFit_; modifiedNetStretch_ = rhs.modifiedNetStretch_;
					  return *this; }

			const std::string & getTrayName() const { return trayName_; }

			UINT16					packId_;
			UINT16					width_;
			UINT16					length_;
			UINT16					height_;
			UINT16					lip_;
			WrFilmWidthSelection 	filmSelection_;
			bool					usedWrapBoxBestFit_;		//	Used a best fit wrap box to wrap the pkg.
			bool					modifiedNetStretch_;		//	Modified % net stretch to wrap the pkg.
			std::string				trayName_;
		};
		class HistoryIterator
		{
			friend class EvolWrapperPackageHistory;

			public:
				HistoryIterator(const HistoryIterator& rhs)
						: container_(rhs.container_), index_(rhs.index_) {}
				~HistoryIterator() {}
					
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
				EvolWrapperPackageHistory* container_;
				mutable int index_;

				HistoryIterator(EvolWrapperPackageHistory* container, int index)
						: container_(container), index_(index) {}

				// Prevent default constructor
				HistoryIterator();
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
		EvolWrapperPackageHistory(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperPackageHistory(const char *buffer);
		virtual ~EvolWrapperPackageHistory();
		 

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
		const_iterator end() const { return const_iterator(const_cast<EvolWrapperPackageHistory*>(this), -1); }

		void addPackage(const HistoryItem& item);
		void addPackage(UINT16 packId, UINT16 width, UINT16 length, UINT16 height, UINT16 lip, WrFilmWidthSelection filmSelection, std::string &trayName, bool bestFit, bool modifiedStretch);

		const char* serialize() const;

		void show();	


	private:
		static const int _maxElements_ = 10;

		int totalElements_;
		int firstElement_;
		historyContainer_type history_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* EVOLWRAPPERPACKAGEHISTORY_H */
