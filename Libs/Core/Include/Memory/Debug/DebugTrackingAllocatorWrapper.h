#pragma once

#include "Memory/IAllocator.h"
#include "Macro/MacroUtils.h"
#include "AllocationReport.h"

namespace Flourish::Memory
{
	// A Wrapper around a IAllocator that can be used to track memory allocations for both leaks and overwrites. 
	class DebugTrackingAllocatorWrapper : public IAllocator
	{
	public:
		static const uint32_t FRONT_GUARD = 0x02468ACE;
		static const uint32_t BACK_GUARD =  0x13579BDF;
		static const uint32_t UNINITIALIZED_MEMORY_PATTERN = 0xABADBEEF;

		DebugTrackingAllocatorWrapper(IAllocator& allocatorToWrap);
		virtual ~DebugTrackingAllocatorWrapper();

		DISALLOW_COPY_AND_MOVE(DebugTrackingAllocatorWrapper);

		//Total number of allocations currently open
		int64_t GetAllocatorNumAllocations() const { return _allocatorNumAllocations; }

		//Total bytes currently allocated by the base allocator (not including extra memory used by this wrapper for tracking)
		int64_t GetAllocatorVurrentMemoryUsage() const { return _allocatorCurrentMemoryUsage; }

		//Highest total bytes allocated at once by the base allocator across its lifetime (not including extra memory used by this wrapper for tracking)
		int64_t GetAllocatorHighWatermarkMemoryUsage() const {	return _allocatorHighWatermarkMemoryUsage; }

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

		static size_t GetTrackingDataOverheadPerAllocation() { return sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD); }

		//Call this when you expect the allocator to have freed everything to check for leaks. Returns true if
		//the allocator still has open allocations. leakReportOut should be a valid buffer to save the leak report too, or can be null to skip the report
		//(Is automatic called during the dtor and dumped via debugprintf if leaks found)
		bool CheckForMemoryLeaks(char* leakReportOut, int leakReportOutBufferLength, bool& reportTruncatedOut);

		//Returns an AllocationReport object representing data about all current allocations from the underlying allocator.
		//This version will use the underlying allocator to allocate memory needed to store the report
		//(Note: the data allocated for the report will not be shown in the report itself)
		AllocationReport GetCurrentAllocationsReport();

		//Returns an AllocationReport object representing data about all current allocations from the underlying allocator.
		//This overload allows the user to use a different allocator then the one that is wrapped for allocating the report data
		AllocationReport GetCurrentAllocationsReport(IAllocator& reportDataAllocator);

	private:
		//Simple Linked list for tracking allocations. 
		struct TrackingDataLinkedListItem
		{
			TrackingDataLinkedListItem* Prev;
			TrackingDataLinkedListItem* Next;
		};

		//Stored in front of the allocation to keep the tracking information
		struct TrackingData
		{
			Debug::SourceInfo SourceInfo;
			TrackingDataLinkedListItem ListItem;
		};

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
		void* GetUserPtrFromTrackingDataPtr(TrackingData* ptr);
		int GetFrontGuardFromUserPtr(void* ptr);
		int GetBackGuardFromUserPtr(void* ptr);

		//fills an area of memory with UNINITIALIZED_MEMORY_PATTERN to help debug uninitialized memory issues.
		static void FillMemoryWithUninitializedPattern(void* ptr, size_t size);

		//Main Item representing tracking list:
		// - next points to first entry in list
		// - prev points to last entry in list.
		// - first entry in list prev will point to this
		// - last entry in list next will point to this
		// if this prev = next = mTrackingList, list is empty
		TrackingDataLinkedListItem _trackingList { };

		//Base allocator we are wrapping
		IAllocator& _baseAllocator;

		//Total number of allocations currently open
		size_t _allocatorNumAllocations { 0 };

		//Total bytes currently allocated by the base allocator (not including extra memory used by this wrapper for tracking)
		size_t _allocatorCurrentMemoryUsage { 0 };

		//Highest total bytes allocated at once by the base allocator across its lifetime (not including extra memory used by this wrapper for tracking)
		size_t _allocatorHighWatermarkMemoryUsage { 0 };
	};
}
