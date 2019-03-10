#pragma once
#include "Macro/MacroUtils.h"

//Simple Linked list for tracking allocations. 
class MemoryTrackingLinkedList
{
public:
	struct MemoryTrackingLinkedListItem
	{
		MemoryTrackingLinkedListItem* prev;
		MemoryTrackingLinkedListItem* next;
	};

	MemoryTrackingLinkedList();

	DISALLOW_COPY_AND_MOVE(MemoryTrackingLinkedList);

private:
	
};