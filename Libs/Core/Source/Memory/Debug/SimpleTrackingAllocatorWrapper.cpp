#include "Memory/Debug/SimpleTrackingAllocatorWrapper.h"
#include "Memory/AddressUtils.h"
#include "Debug/Assert.h"
#include "Utils/CharArrayUtils.h"

namespace Flourish::Memory
{
	SimpleTrackingAllocatorWrapper::SimpleTrackingAllocatorWrapper(IAllocator& allocatorToWrap)
		: mBaseAllocator(allocatorToWrap)
	{
		InitTrackingList();
	}

	SimpleTrackingAllocatorWrapper::~SimpleTrackingAllocatorWrapper()
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

	void* SimpleTrackingAllocatorWrapper::Alloc(size_t size, const Debug::SourceInfo& sourceInfo)
	{
		size_t spaceNeeded = size + sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD);
		void* baseMemPtr = mBaseAllocator.Alloc(spaceNeeded, sourceInfo);

		TrackingData* headerData = static_cast<TrackingData*>(baseMemPtr);

		headerData->sourceInfo = sourceInfo;

		AllocatorTotalLifeTimeMemoryUsage += mBaseAllocator.GetAllocationSize(baseMemPtr) + mBaseAllocator.GetMetaDataAllocationSize(baseMemPtr);

		if(AllocatorTotalLifeTimeMemoryUsage > AllocatorHighWatermarkMemoryUsage)
		{
			AllocatorHighWatermarkMemoryUsage = AllocatorTotalLifeTimeMemoryUsage;
		}

		AddEntryToTrackingList(&headerData->listItem);

		unsigned int* frontGuard = static_cast<unsigned int*>(AddressUtils::AddressAddOffset(baseMemPtr, sizeof(TrackingData)));
		(*frontGuard) = FRONT_GUARD;

		unsigned int* backGuard = static_cast<unsigned int*>(AddressUtils::AddressAddOffset(baseMemPtr, sizeof(TrackingData) + sizeof(FRONT_GUARD) + size));
		(*backGuard) = BACK_GUARD;

		return AddressUtils::AddressAddOffset(baseMemPtr, sizeof(TrackingData) + sizeof(FRONT_GUARD));
	}

	void SimpleTrackingAllocatorWrapper::Free(void* ptr)
	{
		TrackingData* header = GetTrackingDataFromUserPtr(ptr);

		RemoveEntryFromTrackingList(&header->listItem);

		FL_ASSERT_MSG(GetFrontGuardFromUserPtr(ptr) == FRONT_GUARD, "Memory Curruption! Allocation Front Guard has been overwriten. Allocation Source: %s(%d): %s", 
			header->sourceInfo.FileName, header->sourceInfo.Line, header->sourceInfo.FunctionName);

		FL_ASSERT_MSG(GetBackGuardFromUserPtr(ptr) == BACK_GUARD, "Memory Curruption! Allocation Back Guard has been overwriten. Allocation Source: %s(%d): %s", 
			header->sourceInfo.FileName, header->sourceInfo.Line, header->sourceInfo.FunctionName);

		mBaseAllocator.Free(header);
	}

	size_t SimpleTrackingAllocatorWrapper::GetAllocationSize(void* ptr)
	{
		return mBaseAllocator.GetAllocationSize(GetTrackingDataFromUserPtr(ptr));
	}

	size_t SimpleTrackingAllocatorWrapper::GetMetaDataAllocationSize(void* ptr)
	{
		return mBaseAllocator.GetMetaDataAllocationSize(GetTrackingDataFromUserPtr(ptr)) + sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD);
	}

	bool SimpleTrackingAllocatorWrapper::CheckForMemoryLeaks(char* leakReportOut, int leakReportOutBufferLength, bool& reportTruncatedOut)
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

		if(Utils::CharArrayUtils::sprintfAppend(leakReportOut, leakReportOutBufferLength, "Leaks Detected in allocator '%s'\n", mBaseAllocator.GetAllocatorName()))
		{
			if(Utils::CharArrayUtils::sprintfAppend(leakReportOut, leakReportOutBufferLength, "Leaked Allocations:\n(Note %d metadata bytes currently added by tracking to each allocation)\n\n",
				sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD)))
			{
				TrackingDataLinkedListItem* entry = mTrackingList.next;
			    while( entry != &mTrackingList ) 
				{
					TrackingData* data = GetTrackingDataFromList(entry);

					if(!Utils::CharArrayUtils::sprintfAppend(leakReportOut, leakReportOutBufferLength, "- %d bytes [+ %d metadata bytes] allocated at [%08x] from %s(%i): %s\n", GetAllocationSize(data),
						 GetMetaDataAllocationSize(data), reinterpret_cast<unsigned int>(data), data->sourceInfo.FileName, data->sourceInfo.Line, data->sourceInfo.FunctionName))
					{
						reportTruncatedOut = true;
						break;
					}
		        
					entry = entry->next;
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

	bool SimpleTrackingAllocatorWrapper::DumpCurrentAllocationsStatsReport(char* statsReportOut, int statsReportOutLength)
	{
		FL_ASSERT(statsReportOutLength > 0);

		statsReportOut[0] = '\0';

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "Allocation Stats for allocator '%s'\n(Note %d metadata bytes currently added by tracking to each allocation)\n\n", 
			mBaseAllocator.GetAllocatorName(), sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD)))
		{
			return false;
		}

		size_t totalBytesCurrentAllocated = 0;

		if(IsTrackingListEmpty())
		{
			if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "No Open Allocations\n"))
			{
				return false;
			}
		}
		else
		{
			if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "Current Open Allocations:\n\n"))
			{
				return false;
			}

			TrackingDataLinkedListItem* entry = mTrackingList.next;
		    while( entry != &mTrackingList ) 
			{
				TrackingData* data = GetTrackingDataFromList(entry);

				size_t allocSize = GetAllocationSize(data);
				size_t metadataSize = GetMetaDataAllocationSize(data);

				totalBytesCurrentAllocated += allocSize + metadataSize;

				if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "- %d bytes [+ %d metadata bytes] allocated at [%08x] from %s(%i): %s\n", allocSize,
					metadataSize, reinterpret_cast<unsigned int>(data), data->sourceInfo.FileName, data->sourceInfo.Line, data->sourceInfo.FunctionName))
				{
					return false;
				}
	        
				entry = entry->next;
			}
		}

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "\nOther Stats:\n"))
		{
			return false;
		}

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "- Current Bytes Allocated: %d", totalBytesCurrentAllocated))
		{
			return false;
		}

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "- Total Bytes Allocated: %d", AllocatorTotalLifeTimeMemoryUsage))
		{
			return false;
		}

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "- Allocated Bytes High Watermark: %d", AllocatorHighWatermarkMemoryUsage))
		{
			return false;
		}

		return true;
	}

	void SimpleTrackingAllocatorWrapper::InitTrackingList()
	{
		mTrackingList.next = &mTrackingList;
		mTrackingList.prev = &mTrackingList;
	}

	void SimpleTrackingAllocatorWrapper::AddEntryToTrackingList(TrackingDataLinkedListItem* newItem)
	{
		FL_ASSERT(newItem != nullptr);
		FL_ASSERT(newItem->next != nullptr);
		FL_ASSERT(newItem->prev != nullptr);

		newItem->next = &mTrackingList;
		newItem->prev = mTrackingList.prev;

		mTrackingList.prev->next = newItem;
		mTrackingList.prev = newItem;
	}

	void SimpleTrackingAllocatorWrapper::RemoveEntryFromTrackingList(TrackingDataLinkedListItem* itemToRemove)
	{
		FL_ASSERT(itemToRemove != nullptr);
		FL_ASSERT(itemToRemove->next != nullptr);
		FL_ASSERT(itemToRemove->prev != nullptr);

		itemToRemove->prev->next = itemToRemove->next;
		itemToRemove->next->prev = itemToRemove->prev;
	}

	bool SimpleTrackingAllocatorWrapper::IsTrackingListEmpty()
	{
		return mTrackingList.next == &mTrackingList && mTrackingList.prev == &mTrackingList;
	}

	SimpleTrackingAllocatorWrapper::TrackingData* SimpleTrackingAllocatorWrapper::GetTrackingDataFromList(
		TrackingDataLinkedListItem* listItem)
	{
		auto memberOffset = reinterpret_cast<unsigned long>(&static_cast<TrackingData*>(nullptr)->listItem);
		return reinterpret_cast<TrackingData*>(reinterpret_cast<char *>(listItem) - memberOffset);
	}

	SimpleTrackingAllocatorWrapper::TrackingData* SimpleTrackingAllocatorWrapper::GetTrackingDataFromUserPtr(void* ptr)
	{
		return reinterpret_cast<TrackingData*>(AddressUtils::AddressSubOffset(ptr, sizeof(TrackingData) + sizeof(FRONT_GUARD)));
	}

	int SimpleTrackingAllocatorWrapper::GetFrontGuardFromUserPtr(void* ptr)
	{
		return *reinterpret_cast<int*>(AddressUtils::AddressSubOffset(ptr, sizeof(FRONT_GUARD)));
	}

	int SimpleTrackingAllocatorWrapper::GetBackGuardFromUserPtr(void* ptr)
	{
		size_t offset = GetAllocationSize(ptr) - (sizeof(TrackingData) + sizeof(FRONT_GUARD) + sizeof(BACK_GUARD));
		return *reinterpret_cast<int*>(AddressUtils::AddressAddOffset(ptr, offset));
	}
}
