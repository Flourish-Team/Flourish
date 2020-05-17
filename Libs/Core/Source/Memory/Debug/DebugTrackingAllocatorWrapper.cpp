#include "Memory/Debug/DebugTrackingAllocatorWrapper.h"
#include "Memory/AddressUtils.h"
#include "Debug/Assert.h"
#include "Utils/CharArrayUtils.h"

namespace Flourish::Memory
{
	DebugTrackingAllocatorWrapper::DebugTrackingAllocatorWrapper(IAllocator& allocatorToWrap)
		: IAllocator(allocatorToWrap.GetAllocatorName())
		, _baseAllocator(allocatorToWrap)
	{
		strcat_s(_allocatorName, " [Wrapped by DebugTrackingAllocatorWrapper]");

		//Init the tracking list so it is ready for use
		_trackingList.Next = &_trackingList;
		_trackingList.Prev = &_trackingList;
	}

	DebugTrackingAllocatorWrapper::~DebugTrackingAllocatorWrapper()
	{
		bool truncated;
		if(CheckForMemoryLeaks(nullptr, 0, truncated))
		{
			const size_t reportBufferSize = 4096; 
			char reportBuffer[reportBufferSize];

			CheckForMemoryLeaks(reportBuffer, reportBufferSize, truncated);
			Debug::DebugPrintf("%s%s", reportBuffer, truncated ? "\n\n(Leak Report Truncated)" : "");
		}
	}

	void* DebugTrackingAllocatorWrapper::Alloc(size_t size, const Debug::SourceInfo& sourceInfo)
	{
		size_t spaceNeeded = size + sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD);
		void* baseMemPtr = _baseAllocator.Alloc(spaceNeeded, sourceInfo);

		TrackingData* headerData = static_cast<TrackingData*>(baseMemPtr);

		headerData->SourceInfo = sourceInfo;

		size_t allocSize = _baseAllocator.GetAllocationSize(baseMemPtr) + _baseAllocator.GetMetaDataAllocationSize(baseMemPtr);

		++_allocatorNumAllocations;
		_allocatorCurrentMemoryUsage += allocSize;

		if(_allocatorCurrentMemoryUsage > _allocatorHighWatermarkMemoryUsage)
		{
			_allocatorHighWatermarkMemoryUsage = _allocatorCurrentMemoryUsage;
		}

		AddEntryToTrackingList(&headerData->ListItem);

		uint32_t* frontGuard = static_cast<uint32_t*>(AddressUtils::AddressAddOffset(baseMemPtr, sizeof(TrackingData)));
		(*frontGuard) = FRONT_GUARD;

		uint32_t* backGuard = static_cast<uint32_t*>(AddressUtils::AddressAddOffset(baseMemPtr, sizeof(TrackingData) + sizeof(FRONT_GUARD) + size));
		(*backGuard) = BACK_GUARD;

		void* userAddress = AddressUtils::AddressAddOffset(baseMemPtr, sizeof(TrackingData) + sizeof(FRONT_GUARD));

		FillMemoryWithUninitializedPattern(userAddress, size);

		return userAddress;
	}

	void DebugTrackingAllocatorWrapper::Free(void* ptr)
	{
		TrackingData* header = GetTrackingDataFromUserPtr(ptr);

		const size_t allocSize = GetAllocationSize(ptr);
		const size_t metadataSize = GetMetaDataAllocationSize(ptr) - GetTrackingDataOverheadPerAllocation();

		--_allocatorNumAllocations;
		_allocatorCurrentMemoryUsage -= (allocSize + metadataSize);

		RemoveEntryFromTrackingList(&header->ListItem);

		FL_ASSERT_MSG(GetFrontGuardFromUserPtr(ptr) == FRONT_GUARD, "Memory Curruption! Allocation Front Guard has been overwriten. Allocation Source: %s(%d): %s", 
			header->SourceInfo.FileName, header->SourceInfo.Line, header->SourceInfo.FunctionName);

		FL_ASSERT_MSG(GetBackGuardFromUserPtr(ptr) == BACK_GUARD, "Memory Curruption! Allocation Back Guard has been overwriten. Allocation Source: %s(%d): %s", 
			header->SourceInfo.FileName, header->SourceInfo.Line, header->SourceInfo.FunctionName);

		_baseAllocator.Free(header);
	}

	size_t DebugTrackingAllocatorWrapper::GetAllocationSize(void* ptr)
	{
		return _baseAllocator.GetAllocationSize(GetTrackingDataFromUserPtr(ptr));
	}

	size_t DebugTrackingAllocatorWrapper::GetMetaDataAllocationSize(void* ptr)
	{
		return _baseAllocator.GetMetaDataAllocationSize(GetTrackingDataFromUserPtr(ptr)) + GetTrackingDataOverheadPerAllocation();
	}

	bool DebugTrackingAllocatorWrapper::CheckForMemoryLeaks(char* leakReportOut, int leakReportOutBufferLength, bool& reportTruncatedOut)
	{
		if(IsTrackingListEmpty())
		{
			return false;
		}

		if(leakReportOut == nullptr)
		{
			return true;
		}

		FL_ASSERT(leakReportOutBufferLength > 0);
		leakReportOut[0] = '\0';

		reportTruncatedOut = false;

		if(Utils::CharArrayUtils::sprintfAppend(leakReportOut, leakReportOutBufferLength, "Leaks Detected in allocator '%s'\n", _baseAllocator.GetAllocatorName()))
		{
			if(Utils::CharArrayUtils::sprintfAppend(leakReportOut, leakReportOutBufferLength, "Leaked Allocations:\n(Note %d bytes (Not shown in stats) currently added by tracking to each allocation)\n\n",
				sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD)))
			{
				TrackingDataLinkedListItem* entry = _trackingList.Next;
			    while( entry != &_trackingList ) 
				{
					TrackingData* data = GetTrackingDataFromList(entry);
					void* userPtr = GetUserPtrFromTrackingDataPtr(data);

					if(!Utils::CharArrayUtils::sprintfAppend(leakReportOut, leakReportOutBufferLength, "- 0x%08X at %s(%i): %s (%d bytes [+%d metadata bytes])\n", 
						userPtr, data->SourceInfo.FileName, data->SourceInfo.Line, data->SourceInfo.FunctionName, 
						_baseAllocator.GetAllocationSize(data), _baseAllocator.GetMetaDataAllocationSize(data)))
					{
						reportTruncatedOut = true;
						break;
					}
		        
					entry = entry->Next;
				}
			}
			else
			{
				reportTruncatedOut = true;
			}
		}
		else
		{
			reportTruncatedOut = true;
		}

		return true;
	}

	AllocationReport DebugTrackingAllocatorWrapper::GetCurrentAllocationsReport()
	{
		return GetCurrentAllocationsReport(_baseAllocator);
	}

	AllocationReport DebugTrackingAllocatorWrapper::GetCurrentAllocationsReport(IAllocator& reportDataAllocator)
	{
		size_t numAllocations = 0;

		TrackingDataLinkedListItem* entry = _trackingList.Next;
	    while( entry != &_trackingList ) 
		{
		    numAllocations++;	        
			entry = entry->Next;
		}
		
		
		AllocationReport::AllocationsSizeData totalAllocSizeData { 0, 0, 0 };
		auto reportItems = FL_NEW_SHARED_ARRAY(reportDataAllocator, AllocationReport::AllocationReportItem, numAllocations);

		const size_t trackingOverheadPerAllocation = GetTrackingDataOverheadPerAllocation();

		entry = _trackingList.Next;
	    for(size_t index = 0; index < numAllocations; ++index)
		{
			TrackingData* data = GetTrackingDataFromList(entry);
		    void* userPtr = GetUserPtrFromTrackingDataPtr(data);

			const size_t allocSize = GetAllocationSize(userPtr);
			const size_t metadataSize = GetMetaDataAllocationSize(userPtr) - trackingOverheadPerAllocation;

		    totalAllocSizeData.AllocSize += allocSize;
		    totalAllocSizeData.MetadataSize += metadataSize;
		    totalAllocSizeData.TrackingDataSize += trackingOverheadPerAllocation;

		    reportItems[index].AllocSizeData = {allocSize, metadataSize, trackingOverheadPerAllocation};
			reportItems[index].AllocMemoryAddress = reinterpret_cast<intptr_t>(userPtr);
			strcpy_s(reportItems[index].FileName, data->SourceInfo.FileName);
			reportItems[index].LineNumber = data->SourceInfo.Line;
		    strcpy_s(reportItems[index].FunctionName, data->SourceInfo.FunctionName);		    
			entry = entry->Next;
		}

		FL_ASSERT(totalAllocSizeData.AllocSize + totalAllocSizeData.MetadataSize == _allocatorCurrentMemoryUsage);
		FL_ASSERT(numAllocations == _allocatorNumAllocations);

		AllocationReport::AllocationStats allocationStats;

		allocationStats.CurrentTotalAllocSizeData = totalAllocSizeData;
		allocationStats.BytesAllocatedHighWatermark = _allocatorHighWatermarkMemoryUsage;

		return AllocationReport(reportItems, numAllocations, allocationStats, _baseAllocator.GetAllocatorName());
	}

	void DebugTrackingAllocatorWrapper::AddEntryToTrackingList(TrackingDataLinkedListItem* newItem)
	{
		FL_ASSERT(newItem != nullptr);

		newItem->Next = &_trackingList;
		newItem->Prev = _trackingList.Prev;

		_trackingList.Prev->Next = newItem;
		_trackingList.Prev = newItem;
	}

	void DebugTrackingAllocatorWrapper::RemoveEntryFromTrackingList(TrackingDataLinkedListItem* itemToRemove)
	{
		FL_ASSERT(itemToRemove != nullptr);
		FL_ASSERT(itemToRemove->Next != nullptr);
		FL_ASSERT(itemToRemove->Prev != nullptr);

		itemToRemove->Prev->Next = itemToRemove->Next;
		itemToRemove->Next->Prev = itemToRemove->Prev;
	}

	bool DebugTrackingAllocatorWrapper::IsTrackingListEmpty()
	{
		return _trackingList.Next == &_trackingList && _trackingList.Prev == &_trackingList;
	}

	DebugTrackingAllocatorWrapper::TrackingData* DebugTrackingAllocatorWrapper::GetTrackingDataFromList(
		TrackingDataLinkedListItem* listItem)
	{
		const auto memberOffset = reinterpret_cast<unsigned long>(&static_cast<TrackingData*>(nullptr)->ListItem);
		return reinterpret_cast<TrackingData*>(reinterpret_cast<char *>(listItem) - memberOffset);
	}

	DebugTrackingAllocatorWrapper::TrackingData* DebugTrackingAllocatorWrapper::GetTrackingDataFromUserPtr(void* ptr)
	{
		return reinterpret_cast<TrackingData*>(AddressUtils::AddressSubOffset(ptr, sizeof(TrackingData) + sizeof(FRONT_GUARD)));
	}

	void* DebugTrackingAllocatorWrapper::GetUserPtrFromTrackingDataPtr(TrackingData* ptr)
	{
		return reinterpret_cast<void*>(AddressUtils::AddressAddOffset(ptr, sizeof(TrackingData) + sizeof(FRONT_GUARD)));
	}

	int DebugTrackingAllocatorWrapper::GetFrontGuardFromUserPtr(void* ptr)
	{
		return *reinterpret_cast<int32_t*>(AddressUtils::AddressSubOffset(ptr, sizeof(FRONT_GUARD)));
	}

	int DebugTrackingAllocatorWrapper::GetBackGuardFromUserPtr(void* ptr)
	{
		size_t offset = GetAllocationSize(ptr) - (sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD));
		return *reinterpret_cast<int32_t*>(AddressUtils::AddressAddOffset(ptr, offset));
	}

	void DebugTrackingAllocatorWrapper::FillMemoryWithUninitializedPattern(void* ptr, size_t size)
	{
		int patternSize = sizeof(UNINITIALIZED_MEMORY_PATTERN);

		if(size < 4)
		{
			memmove(ptr, &UNINITIALIZED_MEMORY_PATTERN, size);
			return;
		}

		memmove(ptr, &UNINITIALIZED_MEMORY_PATTERN, patternSize);
		uint8_t* start = static_cast<uint8_t*>(ptr);
		uint8_t* current = static_cast<uint8_t*>(ptr) + patternSize;
		uint8_t* end = start + size;

		while(current + patternSize < end) {
		    memmove(current, start, patternSize);
		    current += patternSize;
		    patternSize *= 2;
		}
		// fill the rest
		memmove(current, start, static_cast<int32_t>(end - current));
	}
}
