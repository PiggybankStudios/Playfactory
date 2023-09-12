/*
File:   game_inv_types.cpp
Author: Taylor Robbins
Date:   09\11\2023
Description: 
	** Inventories have a variable number of slots arranged in some particular way
	** and grouped to make it easy for the player to see what the purpose of the slots are.
	** Each type of inventory gets to decide how these slots are arranged and grouped.
*/

InvSlot_t* _TwoPassAddInvSlot(InvSlot_t* slots, u64 numSlots, u64* slotIndex, u64 groupId, v2i gridPos, v2i position)
{
	InvSlot_t* result = nullptr;
	if (slots != nullptr)
	{
		Assert(*slotIndex < numSlots);
		result = &slots[*slotIndex];
		ClearPointer(result);
		result->index = *slotIndex;
		result->groupId = groupId;
		result->gridPos = gridPos;
		result->stack = NewItemStack(TileType_None, 0);
		result->mainRec.topLeft = position;
		result->mainRec.size = INV_SLOT_SIZE;
	}
	*slotIndex = (*slotIndex) + 1;
	return result;
}

InvSlot_t* GetInvSlotsForInvType(InvType_t type, MemArena_t* memArena, u64* numSlotsOut)
{
	#define AddInvSlot(groupId, gridPos, position) _TwoPassAddInvSlot(slots, numSlots, &slotIndex, (groupId), (gridPos), (position))
	
	#define TwoPassAddSlotLoopStart() for (pass = 0; pass < 2; pass++)
	#define TwoPassAddSlotLoopEnd() do                                    \
	{                                                                     \
		if (pass == 0)                                                    \
		{                                                                 \
			numSlots = slotIndex;                                         \
			SetOptionalOutPntr(numSlotsOut, numSlots);                    \
			if (memArena == nullptr || numSlots == 0) { return nullptr; } \
			slots = AllocArray(memArena, InvSlot_t, numSlots);            \
			NotNull(slots);                                               \
		}                                                                 \
		else { Assert(slotIndex == numSlots); }                           \
	} while(0)
	
	InvSlot_t* slots = nullptr;
	u64 numSlots = 0;
	u64 slotIndex = 0;
	u8 pass = 0;
	switch (type)
	{
		case InvType_PlayerInventory:
		{
			TwoPassAddSlotLoopStart()
			{
				slotIndex = 0;
				for (i32 yPos = 0; yPos < PLAYER_INV_SIZE.height; yPos++)
				{
					for (i32 xPos = 0; xPos < PLAYER_INV_SIZE.width; xPos++)
					{
						AddInvSlot(0, NewVec2i(xPos, yPos), NewVec2i(xPos * INV_SLOT_ADVANCE.width, yPos * INV_SLOT_ADVANCE.height));
					}
				}
				
				TwoPassAddSlotLoopEnd();
			}
		} break;
		case InvType_SmallBox:
		{
			TwoPassAddSlotLoopStart()
			{
				
				TwoPassAddSlotLoopEnd();
			}
		} break;
		default: DebugAssert(false); SetOptionalOutPntr(numSlotsOut, 0); return nullptr;
	}
	
	return slots;
}
