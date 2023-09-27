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
			else if (targetSlot == nullptr && slot->stack.count == 0)
			{
				targetSlot = slot;
			}
			else
			{
				// PrintLine_D("Ignoring slot[%llu] x%u %s", sIndex, slot->stack.count, GetItemIdStr(slot->stack.id));
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

InvSlot_t* GetInvSlotAtGridPos(Inventory_t* inventory, v2i gridPos)
{
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		if (slot->gridPos == gridPos) { return slot; }
	}
	return nullptr;
}
InvSlot_t* GetInvSlotMaxOrMinInRow(Inventory_t* inventory, i32 row, bool findMax)
{
	InvSlot_t* result = nullptr;
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		if (slot->gridPos.y == row)
		{
			if (result == nullptr) { result = slot; }
			else if (slot->gridPos.x > result->gridPos.x && findMax) { result = slot; }
			else if (slot->gridPos.x < result->gridPos.x && !findMax) { result = slot; }
		}
	}
	return result;
}
InvSlot_t* GetInvSlotMaxOrMinInCol(Inventory_t* inventory, i32 column, bool findMax)
{
	InvSlot_t* result = nullptr;
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		if (slot->gridPos.x == column)
		{
			if (result == nullptr) { result = slot; }
			else if (slot->gridPos.y > result->gridPos.y && findMax) { result = slot; }
			else if (slot->gridPos.y < result->gridPos.y && !findMax) { result = slot; }
		}
	}
	return result;
}

InvSlot_t* InvMoveSelection(Inventory_t* inventory, InvSlot_t* selectedSlot, Dir2_t direction)
{
	Assert(direction != Dir2_None);
	InvSlot_t* newSelectedSlot = nullptr;
	
	if (selectedSlot == nullptr)
	{
		selectedSlot = (inventory->numSlots > 0) ? &inventory->slots[0] : nullptr;
	}
	else
	{
		newSelectedSlot = GetInvSlotAtGridPos(inventory, selectedSlot->gridPos + ToVec2i(direction));
		if (newSelectedSlot == nullptr)
		{
			if (direction == Dir2_Left)       { newSelectedSlot = GetInvSlotMaxOrMinInRow(inventory, selectedSlot->gridPos.y, true);  }
			else if (direction == Dir2_Right) { newSelectedSlot = GetInvSlotMaxOrMinInRow(inventory, selectedSlot->gridPos.y, false); }
			else if (direction == Dir2_Up)    { newSelectedSlot = GetInvSlotMaxOrMinInCol(inventory, selectedSlot->gridPos.x, true);  }
			else if (direction == Dir2_Down)  { newSelectedSlot = GetInvSlotMaxOrMinInCol(inventory, selectedSlot->gridPos.x, false); }
		}
	}
	
	if (newSelectedSlot != nullptr && newSelectedSlot != selectedSlot)
	{
		selectedSlot = newSelectedSlot;
		inventory->selectionIndex = (i64)newSelectedSlot->index;
	}
	
	return selectedSlot;
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
		slot->stack.count = 0;
		// if (GetRandR32(&pig->random) < 0.25f)//TODO: Remove me!
		// {
		// 	slot->stack.id = (ItemId_t)(GetRandU32(&pig->random, ItemId_Peppermint, ItemId_NumIds));
		// 	slot->stack.count = 1;
		// }
	}
}

void OnOpenInventory(Inventory_t* inventory, bool scrollView)
{
	NotNull2(inventory, inventory->allocArena);
	inventory->inScrollView = scrollView;
	inventory->showCrankHint = true;
}

void UpdateInventory(Inventory_t* inventory, Inventory_t* otherInventory)
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
		
		// +==============================+
		// |      Btn_A Swaps Stacks      |
		// +==============================+
		if (BtnPressed(Btn_A) && otherInventory != nullptr && !otherInventory->inScrollView)
		{
			if (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots)
			{
				InvSlot_t* ourSelectedSlot = &inventory->slots[inventory->selectionIndex];
				if (otherInventory->selectionIndex >= 0 && (u64)otherInventory->selectionIndex < otherInventory->numSlots)
				{
					InvSlot_t* otherSelectedSlot = &otherInventory->slots[otherInventory->selectionIndex];
					if (ourSelectedSlot->type == InvSlotType_Default && otherSelectedSlot->type == InvSlotType_Default)
					{
						HandleBtnExtended(Btn_A);
						ItemStack_t tempStack;
						MyMemCopy(&tempStack, &otherSelectedSlot->stack, sizeof(ItemStack_t));
						MyMemCopy(&otherSelectedSlot->stack, &ourSelectedSlot->stack, sizeof(ItemStack_t));
						MyMemCopy(&ourSelectedSlot->stack, &tempStack, sizeof(ItemStack_t));
					}
				}
			}
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
		
		InvSlot_t* selectedSlot = nullptr;
		if (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots) { selectedSlot = &inventory->slots[inventory->selectionIndex]; }
		
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			slot->isSelected = (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex == sIndex);
		}
		
		if (BtnPressed(Btn_Left))  { HandleBtnExtended(Btn_Left);  selectedSlot = InvMoveSelection(inventory, selectedSlot, Dir2_Left);  }
		if (BtnPressed(Btn_Right)) { HandleBtnExtended(Btn_Right); selectedSlot = InvMoveSelection(inventory, selectedSlot, Dir2_Right); }
		if (BtnPressed(Btn_Up))    { HandleBtnExtended(Btn_Up);    selectedSlot = InvMoveSelection(inventory, selectedSlot, Dir2_Up);    }
		if (BtnPressed(Btn_Down))  { HandleBtnExtended(Btn_Down);  selectedSlot = InvMoveSelection(inventory, selectedSlot, Dir2_Down);  }
	}
}

void RenderInventorySlot(InvSlot_t* slot, reci slotRec, bool inScrollView)
{
	MemArena_t* scratch = GetScratchArena();
	
	PdDrawRec(slotRec, kColorWhite);
	PdDrawRecOutline(slotRec, 2, kColorBlack);
	
	if (slot->isSelected && !inScrollView)
	{
		r32 selectionRotation = Animate(0.0f, 1.0f, 2000);
		PdDrawRecOutlineArc(slotRec, 5, selectionRotation + 0.00f, selectionRotation + 0.00f + 0.125f, kColorBlack);
		PdDrawRecOutlineArc(slotRec, 5, selectionRotation + 0.25f, selectionRotation + 0.25f + 0.125f, kColorBlack);
		PdDrawRecOutlineArc(slotRec, 5, selectionRotation + 0.50f, selectionRotation + 0.50f + 0.125f, kColorBlack);
		PdDrawRecOutlineArc(slotRec, 5, selectionRotation + 0.75f, selectionRotation + 0.75f + 0.125f, kColorBlack);
	}
	
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
		
		MyStr_t countStr = PrintInArenaStr(scratch, "%u", slot->stack.count);
		v2i countStrSize = MeasureText(game->itemCountFont.font, countStr);
		v2i countStrPos = slotRec.topLeft + slotRec.size - NewVec2i(5, 2) - countStrSize;
		PdBindFont(&game->itemCountFont);
		// LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		PdDrawText(countStr, countStrPos);
		// PdSetDrawMode(oldDrawMode);
	}
	
	FreeScratchArena(scratch);
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
			RenderInventorySlot(slot, slotRec, inventory->inScrollView);
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
		
		if (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots)
		{
			InvSlot_t* selectedSlot = &inventory->slots[inventory->selectionIndex];
			if (selectedSlot->stack.count > 0 && selectedSlot->stack.id != ItemId_None)
			{
				MyStr_t itemName = NewStr(GetItemIdStr(selectedSlot->stack.id));
				v2i itemNameSize = MeasureText(game->mainFont.font, itemName);
				v2i itemNamePos = NewVec2i(inventory->mainRec.x - 2 - itemNameSize.width, 2);
				PdBindFont(&game->mainFont);
				PdDrawText(itemName, itemNamePos);
			}
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
			RenderInventorySlot(slot, slotRec, inventory->inScrollView);
		}
	}
}
