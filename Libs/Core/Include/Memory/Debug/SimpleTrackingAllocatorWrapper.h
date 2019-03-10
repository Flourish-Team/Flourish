#pragma once

#include "Memory/IAllocator.h"
#include "Macro/MacroUtils.h"

namespace Flourish::Memory
{
	// A Wrapper around a IAllocator that can be used to track memory allocations for both leaks and overwrites. 
	class SimpleTrackingAllocatorWrapper : public IAllocator
	{
	public:
		static const unsigned int FRONT_GUARD = 0x02468ACE;
		static const unsigned int BACK_GUARD =  0x13579BDF;

		SimpleTrackingAllocatorWrapper(IAllocator& allocatorToWrap);
		virtual ~SimpleTrackingAllocatorWrapper();

		DISALLOW_COPY_AND_MOVE(SimpleTrackingAllocatorWrapper);

		//Total bytes allocated by the base allocator across its lifetime (not including extra size used by this wrapper)
		long GetAllocatorTotalLifeTimeMemoryUsage() const { return mAllocatorTotalLifeTimeMemoryUsage; }

		//Highest total bytes allocated at once by the base allocator across its lifetime (not including extra size used by this wrapper)
		long GetAllocatorHighWatermarkMemoryUsage() const {	return mAllocatorHighWatermarkMemoryUsage; }

		// Allocates some raw memory of size with alignment. 
		// (Helper Macros exist in Memory.h for easier use)
		void* Alloc(size_t size, const Debug::SourceInfo& sourceInfo) override;

		// Returns memory allocated by this allocator
		// (Helper Macros exist in Memory.h for easier use)
		void Free(void* ptr) override;

		//Given a pointer, calculates the size of the allocation.
		size_t GetAllocationSize(void* ptr) override;

		//Given a pointer, calculates the size of any internal metadata that was needed for this allocation
		size_t GetMetaDataAllocationSize(void* ptr) override;

		//Call this when you expect the allocator to have freed everything to check for leaks. Returns true if
		//the allocator still has open allocations. leakReportOut should be a valid buffer to save the leak report too, ot can be null to skip the report
		//(Is automaticly called during the dtor and dumped via debugprintf if leaks found)
		bool CheckForMemoryLeaks(char* leakReportOut, int leakReportOutBufferLength, bool& reportTruncatedOut);

		//Fills statsReportOut with a report of all open allocations and misc stats about the allocator. Returns false 
		//if the report was truncated due to the buffer been too small
		bool DumpCurrentAllocationsStatsReport(char* statsReportOut, int statsReportOutLength);

		const char* GetAllocatorName() override;

	private:
		//Simple Linked list for tracking allocations. 
		struct TrackingDataLinkedListItem
		{
			TrackingDataLinkedListItem* prev;
			TrackingDataLinkedListItem* next;
		};

		//Stored in front of the allocation to keep the tracking information
		struct TrackingData
		{
			Debug::SourceInfo sourceInfo;
			TrackingDataLinkedListItem listItem;
		};

		//Init the tracking list so it is ready for use
		void InitTrackingList();

		//Add a new entry to the back of the list
		void AddEntryToTrackingList(TrackingDataLinkedListItem* newItem);

		//Removed an entry from the list
		void RemoveEntryFromTrackingList(TrackingDataLinkedListItem* itemToRemove);

		//Returns true if no items are in the list
		bool IsTrackingListEmpty();

		//Gets the tracking data that this list item belongs too
		TrackingData* GetTrackingDataFromList(TrackingDataLinkedListItem* listItem);

		//Gets a pointer various data stored in the allocation
		TrackingData* GetTrackingDataFromUserPtr(void* ptr);
		int GetFrontGuardFromUserPtr(void* ptr);
		int GetBackGuardFromUserPtr(void* ptr);

		//Main Item representing tracking list:
		// - next points to first entry in list
		// - prev points to last entry in list.
		// - first entry in list prev will point to this
		// - last entry in list next will point to this
		// if this prev = next = mTrackingList, list is empty
		TrackingDataLinkedListItem mTrackingList;

		//Base allocator we are wrapping
		IAllocator& mBaseAllocator;

		//Total bytes allocated by the base allocator across its lifetime (not including extra size used by this wrapper)
		long mAllocatorTotalLifeTimeMemoryUsage;

		//Highest total bytes allocated at once by the base allocator across its lifetime (not including extra size used by this wrapper)
		long mAllocatorHighWatermarkMemoryUsage;

		//Buffer to sprintf name with
		static const size_t NAME_BUFFER_SIZE = 256;
		char mNameBuffer[NAME_BUFFER_SIZE];;
	};
}