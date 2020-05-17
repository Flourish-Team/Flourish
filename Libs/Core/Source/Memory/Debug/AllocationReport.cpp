#include "Memory/Debug/AllocationReport.h"
#include "Memory/Memory.h"
#include "Utils/CharArrayUtils.h"

namespace Flourish::Memory
{
	AllocationReport::AllocationReport(FL_SharedPtrArray<AllocationReportItem>& reportItems, size_t numReportItems, 
		AllocationStats stats, const char* allocatorName)
		: _stats { stats }
		, _numReportItems { numReportItems }
		, _reportItems { std::move(reportItems) }
	{
		strcpy_s(_allocatorNameBuffer, allocatorName);
	}


	const AllocationReport::AllocationReportItem& AllocationReport::GetAllocationReportItem(const uint32_t index) const
	{
		FL_ASSERT(index >= 0 && index < _numReportItems)
		return _reportItems[index];
	}

	bool AllocationReport::DumpCurrentAllocationsStatsReportToString(char* statsReportOut, int statsReportOutLength)
	{
		FL_ASSERT(statsReportOutLength > 0);

		statsReportOut[0] = '\0';

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, 
			"Allocation Report for allocator '%s'\n\n", _allocatorNameBuffer))
		{
			return false;
		}

		if(_numReportItems == 0)
		{
			if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "No Open Allocations\n"))
			{
				return false;
			}
		}
		else
		{
			if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "Current Open Allocations (Alloc Size / Metadata Size / Tracking Data Size):\n\n"))
			{
				return false;
			}

			for(size_t i = 0; i < _numReportItems; ++i)
			{
				const auto reportItem = _reportItems[i];

				if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, 
					"- %d / %d / %d allocated at [%08x] from %s(%i): %s\n", reportItem.AllocSizeData.AllocSize,
					reportItem.AllocSizeData.MetadataSize, reportItem.AllocSizeData.TrackingDataSize, reportItem.AllocMemoryAddress,
					reportItem.FileName, reportItem.LineNumber, reportItem.FunctionName))
				{
					return false;
				}
			}
		}

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "\nOther Stats:\n\n"))
		{
			return false;
		}

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "- Current Bytes Allocated (Alloc Size / Metadata Size / Tracking Data Size): %d / %d / %d", _stats.CurrentTotalAllocSizeData))
		{
			return false;
		}

		if(!Utils::CharArrayUtils::sprintfAppend(statsReportOut, statsReportOutLength, "- Allocated Bytes High Watermark (Alloc + Metadata): %d", _stats.BytesAllocatedHighWatermark))
		{
			return false;
		}

		return true;
	}
}
