/*
File:   game_inventory.cpp
Author: Taylor Robbins
Date:   09\11\2023
Description: 
	** Inventories are things that have some number of slots to hold items.
	** They can be chests, machines, or the player inventory
*/

//Returns number of items that couldn't be placed
u8 TryAddItemStackToInventory(Inventory_t* inventory, ItemStack_t stack)
{
	u8 countLeft = stack.count;
	
	while (countLeft > 0)
	{
		InvSlot_t* targetSlot = nullptr;
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			if (slot->stack.count > 0 && slot->stack.id == stack.id)
			{
				u8 spaceLeft = STACK_MAX - slot->stack.count;
				if (spaceLeft > 0)
				{
					targetSlot = slot;
					break;
				}
			}
			if (targetSlot == nullptr && slot->stack.count == 0)
			{
				targetSlot = slot;
			}
		}
		
		if (targetSlot != nullptr)
		{
			u8 spaceLeft = STACK_MAX - targetSlot->stack.count;
			u8 amountToAdd = (u8)MinU32(spaceLeft, countLeft);
			targetSlot->stack.id = stack.id;
			targetSlot->stack.count += amountToAdd;
			PrintLine_D("Adding %u %s to slot[%llu] (%d, %d)", amountToAdd, GetItemIdStr(stack.id), targetSlot->index, targetSlot->gridPos.x, targetSlot->gridPos.y);
			countLeft -= amountToAdd;
		}
		else { break; }
	}
	
	return countLeft;
}

void FreeInventory(Inventory_t* inventory)
{
	NotNull(inventory);
	if (inventory->allocArena != nullptr)
	{
		FreeMem(inventory->allocArena, inventory->slots, sizeof(InvSlot_t) * inventory->numSlots);
	}
	ClearPointer(inventory);
}

void InitInventory(Inventory_t* inventory, MemArena_t* memArena, InvType_t type)
{
	NotNull2(inventory, memArena);
	ClearPointer(inventory);
	inventory->allocArena = memArena;
	inventory->slots = GetInvSlotsForInvType(type, inventory->allocArena, &inventory->numSlots);
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		slot->stack.id = ItemId_None;
		if (GetRandR32(&pig->random) < 0.25f) { slot->stack.id = (ItemId_t)(GetRandU32(&pig->random, ItemId_Peppermint, ItemId_NumIds)); } //TODO: Remove me!
		slot->stack.count = 1;
	}
}

void OnOpenInventory(Inventory_t* inventory, bool scrollView)
{
	NotNull2(inventory, inventory->allocArena);
	inventory->inScrollView = scrollView;
	inventory->showCrankHint = true;
}

void UpdateInventory(Inventory_t* inventory)
{
	NotNull2(inventory, inventory->allocArena);
	
	
	if (inventory->inScrollView)
	{
		v2i firstSlotSize = Vec2i_Zero;
		v2i lastSlotSize = Vec2i_Zero;
		inventory->contentRec = Reci_Zero;
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			
			TrackMax(sIndex == 0, inventory->contentRec.width, slot->mainRec.width);
			
			if (sIndex > 0) { inventory->contentRec.height += INV_SLOT_MARGIN; }
			inventory->contentRec.height += slot->mainRec.height;
			
			if (sIndex == 0) { firstSlotSize = slot->mainRec.size; }
			lastSlotSize = slot->mainRec.size;
		}
		if (inventory->numSlots > 0)
		{
			inventory->contentRec.height += INV_SLOT_LARGE_SIZE - INV_SLOT_SIZE;
		}
		inventory->contentRec.topLeft = NewVec2i(INV_PADDING, INV_PADDING);
		
		inventory->mainRec.width = inventory->contentRec.width + 2*INV_PADDING;
		inventory->mainRec.height = ScreenSize.height;
		inventory->mainRec.x = ScreenSize.width - inventory->mainRec.width;
		inventory->mainRec.y = 0;
		
		inventory->scrollMin = (r32)(INV_SLOT_LARGE_SIZE/2);
		inventory->scrollMax = (r32)(inventory->contentRec.height - (INV_SLOT_LARGE_SIZE/2));
		
		// +==============================+
		// |   Crank Scrolls Inventory    |
		// +==============================+
		if (CrankMoved())
		{
			HandleCrankDelta();
			inventory->scroll += input->crankDelta;
			inventory->showCrankHint = false;
		}
		
		inventory->scroll = ClampR32(inventory->scroll, inventory->scrollMin, inventory->scrollMax);
		
		// +==================================+
		// | Update Scrolling selectionIndex  |
		// +==================================+
		v2i slotOffset = Vec2i_Zero;
		inventory->selectionIndex = -1;
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			if (inventory->selectionIndex < 0 && RoundR32i(inventory->scroll) <= slotOffset.y + RoundR32i(INV_SLOT_LARGE_SIZE*0.75f))
			{
				inventory->selectionIndex = (i64)sIndex;
				slot->isSelected = true;
			}
			else { slot->isSelected = false; }
			if (slot->isSelected) { UpdateAnimationUp(&slot->selectedAnimProgress, INV_SLOT_GROW_BIG_ANIM_TIME); }
			else { UpdateAnimationDown(&slot->selectedAnimProgress, INV_SLOT_GROW_BIG_ANIM_TIME); }
			slotOffset.y += slot->mainRec.height + INV_SLOT_MARGIN;
		}
	}
	else
	{
		inventory->contentRec = Reci_Zero;
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			inventory->contentRec = ReciExpandToVec2i(inventory->contentRec, slot->mainRec.topLeft);
			inventory->contentRec = ReciExpandToVec2i(inventory->contentRec, slot->mainRec.topLeft + slot->mainRec.size);
		}
		inventory->contentRec.topLeft += NewVec2i(INV_PADDING, INV_PADDING);
		inventory->mainRec.size = inventory->contentRec.size + NewVec2i(2*INV_PADDING, 2*INV_PADDING);
		inventory->mainRec.topLeft = NewVec2i(
			ScreenSize.width / 2 - inventory->mainRec.width/2,
			ScreenSize.height / 2 - inventory->mainRec.height/2
		);
	}
}

void RenderInventorySlot(InvSlot_t* slot, reci slotRec)
{
	PdDrawRec(slotRec, kColorWhite);
	PdDrawRecOutline(slotRec, 2, kColorBlack);
	
	if (slot->stack.count > 0)
	{
		v2i itemFrame = GetItemIdFrame(slot->stack.id);
		if (itemFrame != NewVec2i(-1, -1))
		{
			reci itemRec = NewReci(
				slotRec.x + slotRec.width/2 - INV_ITEM_SIZE/2,
				slotRec.y + slotRec.height/2 - INV_ITEM_SIZE/2,
				INV_ITEM_SIZE,
				INV_ITEM_SIZE
			);
			PdDrawSheetFrame(game->entitiesSheet, itemFrame, itemRec);
		}
	}
}

void RenderInventoryUi(Inventory_t* inventory)
{
	NotNull2(inventory, inventory->allocArena);
	
	if (inventory->inScrollView)
	{
		i32 largeSlotDiff = (INV_SLOT_LARGE_SIZE - INV_SLOT_SIZE);
		
		PdDrawRec(inventory->mainRec, kColorBlack);
		
		v2i slotBaseOffset = NewVec2i(
			inventory->mainRec.x + inventory->contentRec.x,
			inventory->mainRec.y + inventory->mainRec.height/2 - RoundR32i(inventory->scroll)
		);
		v2i slotOffset = Vec2i_Zero;
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			reci slotRec = NewReci(slotBaseOffset + slotOffset, slot->mainRec.size);
			i32 animSlotSize = RoundR32i(LerpR32((r32)slotRec.width, (r32)INV_SLOT_LARGE_SIZE, slot->selectedAnimProgress));
			i32 slotSizeDiff = animSlotSize - slotRec.width;
			slotRec.x -= slotSizeDiff;
			// if (slot->isSelected) { slotRec.y += (largeSlotDiff - slotSizeDiff) / 2; }
			slotRec.width += slotSizeDiff;
			slotRec.height += slotSizeDiff;
			RenderInventorySlot(slot, slotRec);
			slotOffset.y += slotRec.height + INV_SLOT_MARGIN;
		}
		
		if (pig->debugEnabled)
		{
			LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
			reci contentRec = NewReci(
				inventory->mainRec.x + inventory->contentRec.x,
				inventory->mainRec.y + inventory->mainRec.height/2 - RoundR32i(inventory->scroll),
				inventory->contentRec.size
			);;
			PdDrawRec(ReciInflate(contentRec, RoundR32i(Oscillate(0, 5, 1000)), 0), kColorBlack);
			PdSetDrawMode(oldDrawMode);
		}
		
		if (inventory->showCrankHint)
		{
			const r32 crankHintScale = 1.0f;
			v2i playdateCrankHintSize = PlaydateCrankHintBubble(false, true, crankHintScale, Vec2i_Zero).size;
			v2i playdateCrankHintTarget = NewVec2i(inventory->mainRec.x, inventory->mainRec.y + inventory->mainRec.height - 5 - playdateCrankHintSize.height/2);
			PlaydateCrankHintBubble(true, true, crankHintScale, playdateCrankHintTarget);
		}
	}
	else 
	{
		PdDrawRec(inventory->mainRec, kColorWhite);
		PdDrawRecOutline(inventory->mainRec, INV_BORDER_THICKNESS, kColorBlack);
		
		reci contentRec = inventory->contentRec + inventory->mainRec.topLeft;
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			reci slotRec = slot->mainRec + contentRec.topLeft;
			RenderInventorySlot(slot, slotRec);
		}
	}
}
