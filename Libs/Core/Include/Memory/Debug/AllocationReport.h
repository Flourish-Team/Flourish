#pragma once
#include <cstdint>
#include "Memory/IAllocator.h"
#include <memory>
#include "Memory/Memory.h"

namespace Flourish::Memory
{
	//Represents a report of every allocation currently been tracked by a DebugTrackingAllocatorWrapper
	struct AllocationReport
	{
		//When reporting the amount of bytes used by an allocation, we split the value into 3 different parts
		// - Allocation Size: The size (in bytes) of the allocation that are returned to the user. It may be different
		// then the requested amount due to things like alignment, min alloc sizes etc
		// 
		// - Metadata Size: This is the amount of bytes that are used internally by the underlying allocator for things like
		// internal memory management as overhead.
		// 
		// - Tracking Data Size: This is the amount of bytes that is the overhead for using the DebugTrackingAllocatorWrapper.
		// This is provided separately so you can account for when tracking is disabled
		struct AllocationsSizeData
		{
			//Size of the allocation
			size_t AllocSize;

			//Size of the metadata allocated by the underlying allocator as overhead
			size_t MetadataSize;

			//Size of extra data used by the DebugTrackingAllocatorWrapper for this allocation
			size_t TrackingDataSize;
		};
		
		//Data for a single allocation
		struct AllocationReportItem
		{
			static const size_t CHAR_BUFFER_SIZE = 512;
			
			//Size of the allocation 
			AllocationsSizeData AllocSizeData { };

			//Address of the allocation returned to the user (might not be the start of the memory block due to alignment, header data etc.)
			intptr_t AllocMemoryAddress = 0;

			//Name of the file this allocation came from
			char FileName[CHAR_BUFFER_SIZE] { };

			//Line number where this allocation came from
			int32_t LineNumber = 0;

			//Name of the function where this allocation was made
			char FunctionName[CHAR_BUFFER_SIZE] { };
		};

		struct AllocationStats
		{
			//The current total size data of all currently open allocations
			AllocationsSizeData CurrentTotalAllocSizeData { };

			//The highest amount of bytes allocated by the allocator during its lifespan (Excluding any memory used by the tracking allocator for internal management)
			size_t BytesAllocatedHighWatermark = 0;
		};

		AllocationReport(FL_SharedPtrArray<AllocationReportItem> &reportItems, size_t numReportItems, 
			AllocationStats stats, const char* allocatorName);
		~AllocationReport() = default;

		AllocationReport(const AllocationReport&) = default;
		AllocationReport& operator=(const AllocationReport&) = default;

		AllocationReport(AllocationReport&& other) = default;
		AllocationReport& operator=(AllocationReport&& other) = default;
		
		size_t GetNumAllocationReportItems() const { return _numReportItems; }
		AllocationStats GetAllocatorStats() const { return _stats; }
		const AllocationReportItem& GetAllocationReportItem(const uint32_t index) const;

		bool DumpCurrentAllocationsStatsReportToString(char* statsReportOut, int statsReportOutLength);

	private:		
		char _allocatorNameBuffer[IAllocator::ALLOCATOR_NAME_BUFFER_SIZE] { };
		AllocationStats _stats;
		size_t _numReportItems;
		FL_SharedPtrArray<AllocationReportItem> _reportItems;
	};
};
