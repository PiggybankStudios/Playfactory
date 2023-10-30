/*
File:   game_inv_types.cpp
Author: Taylor Robbins
Date:   09\11\2023
Description: 
	** Inventories have a variable number of slots arranged in some particular way
	** and grouped to make it easy for the player to see what the purpose of the slots are.
	** Each type of inventory gets to decide how these slots are arranged and grouped.
*/

InvSlot_t* _TwoPassAddInvSlot(InvSlot_t* slots, u64 numSlots, u64* slotIndex, InvSlotType_t type, u64 groupId, reci gridRec, v2i position, Texture_t* texturePntr)
{
	InvSlot_t* result = nullptr;
	if (slots != nullptr)
	{
		Assert(*slotIndex < numSlots);
		result = &slots[*slotIndex];
		ClearPointer(result);
		result->index = *slotIndex;
		result->type = type;
		result->groupId = groupId;
		result->gridRec = gridRec;
		result->stack = NewItemStack(ITEM_ID_NONE, 0);
		result->texturePntr = texturePntr;
		result->mainRec.topLeft = position;
		result->mainRec.size = (texturePntr != nullptr) ? texturePntr->size : Vec2iFill(INV_SLOT_SIZE);
		if (type == InvSlotType_Button)
		{
			result->mainRec.height = game->mainFont.lineHeight + 2*2;
		}
		else if (type == InvSlotType_ToolBtn)
		{
			result->mainRec.size = Vec2iFill(INV_TOOL_BTN_SIZE);
		}
	}
	*slotIndex = (*slotIndex) + 1;
	return result;
}

InvSlot_t* GetInvSlotsForInvType(InvType_t type, MemArena_t* memArena, u64* numSlotsOut)
{
	#define AddInvSlot(type, groupId, gridRec, position) _TwoPassAddInvSlot(slots, numSlots, &slotIndex, (type), (groupId), (gridRec), (position), nullptr)
	#define AddInvSlotSimple(groupId, gridPos) _TwoPassAddInvSlot(slots, numSlots, &slotIndex, InvSlotType_Default, (groupId), NewReci((gridPos), 1, 1), Vec2iMultiply((gridPos), Vec2iFill(INV_SLOT_SIZE + INV_SLOT_MARGIN)), nullptr)
	#define AddInvSlotDecor(groupId, position, texturePntr) _TwoPassAddInvSlot(slots, numSlots, &slotIndex, InvSlotType_Decor, (groupId), Reci_Zero, (position), (texturePntr))
	
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
						AddInvSlotSimple(0, NewVec2i(xPos, yPos));
					}
				}
				
				TwoPassAddSlotLoopEnd();
			}
		} break;
		case InvType_PlayerScience:
		{
			TwoPassAddSlotLoopStart()
			{
				slotIndex = 0;
				
				v2i iconSize = game->scienceIconTexture.size;
				i32 rightSlotX = INV_SLOT_SIZE + 2*INV_SLOT_MARGIN + iconSize.width;
				
				AddInvSlot(InvSlotType_Default, 0, NewReci(0, 0, 1, 1), NewVec2i(0, iconSize.height/2 - INV_SLOT_SIZE/2 - 10));
				AddInvSlot(InvSlotType_Default, 0, NewReci(1, 0, 1, 1), NewVec2i(rightSlotX, iconSize.height/2 - INV_SLOT_SIZE/2 - 10));
				InvSlot_t* buttonSlot = AddInvSlot(InvSlotType_Button, 1, NewReci(0, 1, 2, 1), NewVec2i(INV_SLOT_SIZE + INV_SLOT_MARGIN + iconSize.width/2 - INV_SLOT_SIZE/2 - 8, iconSize.height + INV_SLOT_MARGIN));
				if (buttonSlot != nullptr)
				{
					buttonSlot->button = InvButton_Combine;
					buttonSlot->mainRec.width += 15;
				}
				
				AddInvSlotDecor(0, NewVec2i(INV_SLOT_SIZE + INV_SLOT_MARGIN, 0), &game->scienceIconTexture);
				
				TwoPassAddSlotLoopEnd();
			}
		} break;
		case InvType_SmallBox:
		{
			TwoPassAddSlotLoopStart()
			{
				for (i32 yPos = 0; yPos < SMALL_BOX_INV_SIZE.height; yPos++)
				{
					for (i32 xPos = 0; xPos < SMALL_BOX_INV_SIZE.width; xPos++)
					{
						AddInvSlotSimple(0, NewVec2i(xPos, yPos));
					}
				}
				TwoPassAddSlotLoopEnd();
			}
		} break;
		case InvType_Store:
		{
			TwoPassAddSlotLoopStart()
			{
				slotIndex = 0;
				const u64 groupCounter = 0; //Bean Counter
				const u64 groupMarket = 1; //Bean Market
				const u64 groupTech = 2; //Bean Tech
				const u64 groupBank = 3; //Bean Bank
				
				v2i signSize = game->beanMarketSignTexture.size;
				
				AddInvSlot(InvSlotType_Sell, groupCounter, NewReci(0, 0, 1, 1000), NewVec2i(signSize.width/2 - INV_SLOT_SIZE/2, 5 + signSize.height));
				AddInvSlotDecor(groupCounter, NewVec2i(0, 0), &game->beanMarketSignTexture);
				
				TwoPassAddSlotLoopEnd();
			}
		} break;
		default: DebugAssert(false); SetOptionalOutPntr(numSlotsOut, 0); return nullptr;
	}
	
	return slots;
}
