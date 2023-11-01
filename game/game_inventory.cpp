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
			PrintLine_D("Adding %u %s to slot[%llu] (%d, %d)", amountToAdd, GetItemDisplayNameNt(&gl->itemBook, stack.id), targetSlot->index, targetSlot->gridRec.x, targetSlot->gridRec.y);
			countLeft -= amountToAdd;
		}
		else { break; }
	}
	
	return countLeft;
}

InvSlot_t* GetInvSlotAtGridPos(Inventory_t* inventory, u64 group, v2i gridPos)
{
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		if (slot->group == group && IsInsideReci(slot->gridRec, gridPos) && slot->type != InvSlotType_Decor) { return slot; }
	}
	return nullptr;
}
InvSlot_t* GetInvSlotMaxOrMinInRow(Inventory_t* inventory, u64 group, i32 row, bool findMax)
{
	InvSlot_t* result = nullptr;
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		if (slot->group == group && slot->type != InvSlotType_GroupChange && row >= slot->gridRec.y && row < slot->gridRec.y + slot->gridRec.height)
		{
			if (result == nullptr) { result = slot; }
			else if (slot->gridRec.x + slot->gridRec.width > result->gridRec.x + result->gridRec.width && findMax) { result = slot; }
			else if (slot->gridRec.x < result->gridRec.x && !findMax) { result = slot; }
		}
	}
	return result;
}
InvSlot_t* GetInvSlotMaxOrMinInCol(Inventory_t* inventory, u64 group, i32 column, bool findMax)
{
	InvSlot_t* result = nullptr;
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		if (slot->group == group && slot->type != InvSlotType_GroupChange && column >= slot->gridRec.x && column < slot->gridRec.x + slot->gridRec.width)
		{
			if (result == nullptr) { result = slot; }
			else if (slot->gridRec.y + slot->gridRec.height > result->gridRec.y + result->gridRec.height && findMax) { result = slot; }
			else if (slot->gridRec.y < result->gridRec.y && !findMax) { result = slot; }
		}
	}
	return result;
}
InvSlot_t* FindDefaultSlotInInventory(Inventory_t* inventory, u64 group = MAX_NUM_INV_GROUPS)
{
	// We want to find the slot that is not Decor or GroupChange, is in the first most group, and then is the top-left-most slot (prioritized in that order)
	InvSlot_t* result = nullptr;
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		if (slot->type != InvSlotType_Decor && slot->type != InvSlotType_GroupChange &&
			(group == MAX_NUM_INV_GROUPS || slot->group == group))
		{
			if (result == nullptr) { result = slot; }
			else if (slot->group < result->group) { result = slot; }
			else if (slot->group == result->group && slot->gridRec.x + slot->gridRec.y < result->gridRec.x + result->gridRec.y) { result = slot; }
		}
	}
	return result;
}

InvSlot_t* InvMoveSelection(Inventory_t* inventory, Dir2_t direction)
{
	Assert(direction != Dir2_None);
	InvSlot_t* selectedSlot = (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots) ? &inventory->slots[inventory->selectionIndex] : nullptr;
	InvSlot_t* newSelectedSlot = nullptr;
	
	if (selectedSlot == nullptr)
	{
		newSelectedSlot = FindDefaultSlotInInventory(inventory);
	}
	else
	{
		newSelectedSlot = GetInvSlotAtGridPos(inventory, selectedSlot->group, inventory->selectionGridPos + ToVec2i(direction));
		if (newSelectedSlot == nullptr)
		{
			if (direction == Dir2_Left)       { newSelectedSlot = GetInvSlotMaxOrMinInRow(inventory, selectedSlot->group, inventory->selectionGridPos.y, true);  }
			else if (direction == Dir2_Right) { newSelectedSlot = GetInvSlotMaxOrMinInRow(inventory, selectedSlot->group, inventory->selectionGridPos.y, false); }
			else if (direction == Dir2_Up)    { newSelectedSlot = GetInvSlotMaxOrMinInCol(inventory, selectedSlot->group, inventory->selectionGridPos.x, true);  }
			else if (direction == Dir2_Down)  { newSelectedSlot = GetInvSlotMaxOrMinInCol(inventory, selectedSlot->group, inventory->selectionGridPos.x, false); }
		}
	}
	
	if (newSelectedSlot != nullptr && newSelectedSlot->type == InvSlotType_GroupChange)
	{
		inventory->groups[selectedSlot->group].prevSelectionIndex = inventory->selectionIndex;
		Assert(newSelectedSlot->newGroup < MAX_NUM_INV_GROUPS);
		i64 prevSelectionInNewGroup = inventory->groups[newSelectedSlot->newGroup].prevSelectionIndex;
		if (prevSelectionInNewGroup >= 0 && (u64)prevSelectionInNewGroup < inventory->numSlots)
		{
			newSelectedSlot = &inventory->slots[prevSelectionInNewGroup];
		}
		else
		{
			newSelectedSlot = FindDefaultSlotInInventory(inventory, newSelectedSlot->newGroup);
		}
	}
	
	if (newSelectedSlot != nullptr && newSelectedSlot != selectedSlot)
	{
		bool didHaveSelection = (selectedSlot != nullptr);
		selectedSlot = newSelectedSlot;
		inventory->selectionIndex = (i64)newSelectedSlot->index;
		if (didHaveSelection)
		{
			// We're going to move 1 spot in the logical grid, but then clamp into the rectangle that the selected slot occupies in the logical grid.
			// This preserves our logical location when returning from buttons that have a large logical rec area to a space
			// where there are a bunch of buttons with individual positions in the logical grid
			inventory->selectionGridPos += ToVec2i(direction);
			inventory->selectionGridPos.x = ClampI32(inventory->selectionGridPos.x, newSelectedSlot->gridRec.x, newSelectedSlot->gridRec.x + newSelectedSlot->gridRec.width-1);
			inventory->selectionGridPos.y = ClampI32(inventory->selectionGridPos.y, newSelectedSlot->gridRec.y, newSelectedSlot->gridRec.y + newSelectedSlot->gridRec.height-1);
		}
		else
		{
			inventory->selectionGridPos = newSelectedSlot->gridRec.topLeft;
		}
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
	inventory->type = type;
	inventory->slots = GetInvSlotsForInvType(type, inventory->allocArena, &inventory->numSlots);
	inventory->selectionIndex = -1;
	inventory->numGroups = 0;
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		slot->stack.id = ITEM_ID_NONE;
		slot->stack.count = 0;
	}
	
	u64 numSlotsPerGroup[MAX_NUM_INV_GROUPS];
	v2i minSlotPos[MAX_NUM_INV_GROUPS];
	ClearArray(numSlotsPerGroup);
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		Assert(slot->group < MAX_NUM_INV_GROUPS);
		if (slot->group >= inventory->numGroups) { inventory->numGroups = slot->group+1; }
		TrackMin((numSlotsPerGroup[slot->group] == 0), minSlotPos[slot->group].x, slot->mainRec.x);
		TrackMin((numSlotsPerGroup[slot->group] == 0), minSlotPos[slot->group].y, slot->mainRec.y);
		numSlotsPerGroup[slot->group]++;
	}
	
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		slot->mainRec.topLeft -= minSlotPos[slot->group];
	}
	
	for (u64 gIndex = 0; gIndex < inventory->numGroups; gIndex++)
	{
		inventory->groups[gIndex].prevSelectionIndex = -1;
	}
	
	InvMoveSelection(inventory, Dir2_Right);
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
		InvGroup_t* mainGroup = &inventory->groups[0];
		mainGroup->contentRec = Reci_Zero;
		u64 foundIndex = 0;
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			if (slot->group == 0)
			{
				TrackMax(foundIndex == 0, mainGroup->contentRec.width, slot->mainRec.width);
				
				if (foundIndex > 0) { mainGroup->contentRec.height += INV_SLOT_MARGIN; }
				mainGroup->contentRec.height += slot->mainRec.height;
				
				if (foundIndex == 0) { firstSlotSize = slot->mainRec.size; }
				lastSlotSize = slot->mainRec.size;
				
				foundIndex++;
			}
		}
		if (inventory->numSlots > 0)
		{
			mainGroup->contentRec.height += INV_SLOT_LARGE_SIZE - INV_SLOT_SIZE;
		}
		mainGroup->contentRec.topLeft = NewVec2i(INV_PADDING, INV_PADDING);
		
		mainGroup->mainRec.width = mainGroup->contentRec.width + 2*INV_PADDING;
		mainGroup->mainRec.height = ScreenSize.height;
		mainGroup->mainRec.x = ScreenSize.width - mainGroup->mainRec.width;
		mainGroup->mainRec.y = 0;
		
		inventory->scrollMin = (r32)(INV_SLOT_LARGE_SIZE/2);
		inventory->scrollMax = (r32)(mainGroup->contentRec.height - (INV_SLOT_LARGE_SIZE/2));
		
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
		// +==============================+
		// |      Btn_A Sells Items       |
		// +==============================+
		if (BtnPressedRaw(Btn_A) && otherInventory != nullptr)
		{
			if (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots &&
				otherInventory->selectionIndex >= 0 && (u64)otherInventory->selectionIndex < otherInventory->numSlots)
			{
				InvSlot_t* selectedSlot = &inventory->slots[inventory->selectionIndex];
				InvSlot_t* otherSelectedSlot = &otherInventory->slots[otherInventory->selectionIndex];
				if (selectedSlot->type == InvSlotType_Default && otherSelectedSlot->type == InvSlotType_Sell)
				{
					HandleBtnExtended(Btn_A);
					if (selectedSlot->stack.count > 0 && selectedSlot->stack.id != ITEM_ID_NONE)
					{
						u8 itemValue = GetItemValue(&gl->itemBook, selectedSlot->stack.id);
						game->player.beanCount += itemValue;
						selectedSlot->stack.count--;
						if (selectedSlot->stack.count == 0) { selectedSlot->stack.id = ITEM_ID_NONE; }
					}
				}
			}
		}
		// +==============================+
		// |       Btn_A Buys Items       |
		// +==============================+
		if (BtnPressedRaw(Btn_A) && inventory->type == InvType_PlayerInventory && otherInventory != nullptr)
		{
			if (otherInventory->selectionIndex >= 0 && (u64)otherInventory->selectionIndex < otherInventory->numSlots)
			{
				InvSlot_t* otherSelectedSlot = &otherInventory->slots[otherInventory->selectionIndex];
				if (otherSelectedSlot->type == InvSlotType_Buy)
				{
					HandleBtnExtended(Btn_A);
					u16 buyItemId = otherSelectedSlot->buyItemId;
					u8 buyItemValue = GetItemValue(&gl->itemBook, buyItemId);
					if (game->player.beanCount >= buyItemValue)
					{
						u64 numUnadded = TryAddItemStackToInventory(inventory, NewItemStack(buyItemId, 1));
						if (numUnadded == 0)
						{
							game->player.beanCount -= buyItemValue;
						}
						else
						{
							//TODO: Add a error/notification of some kind?
						}
					}
				}
			}
		}
	}
	else
	{
		for (u64 gIndex = 0; gIndex < inventory->numGroups; gIndex++)
		{
			InvGroup_t* group = &inventory->groups[gIndex];
			group->contentRec = Reci_Zero;
			for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
			{
				InvSlot_t* slot = &inventory->slots[sIndex];
				if (slot->group == gIndex && slot->type != InvSlotType_GroupChange)
				{
					group->contentRec = ReciExpandToVec2i(group->contentRec, slot->mainRec.topLeft);
					group->contentRec = ReciExpandToVec2i(group->contentRec, slot->mainRec.topLeft + slot->mainRec.size);
				}
			}
			group->contentRec.topLeft += NewVec2i(INV_PADDING, INV_PADDING);
			group->mainRec.size = group->contentRec.size + NewVec2i(2*INV_PADDING, 2*INV_PADDING);
			group->mainRec.topLeft = NewVec2i(
				ScreenSize.width / 2 - group->mainRec.width/2,
				ScreenSize.height / 2 - group->mainRec.height/2
			);
		}
		
		InvSlot_t* selectedSlot = nullptr;
		if (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots) { selectedSlot = &inventory->slots[inventory->selectionIndex]; }
		
		for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
		{
			InvSlot_t* slot = &inventory->slots[sIndex];
			slot->isSelected = (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex == sIndex);
		}
		
		if (BtnPressed(Btn_Left))  { HandleBtnExtended(Btn_Left);  selectedSlot = InvMoveSelection(inventory, Dir2_Left);  }
		if (BtnPressed(Btn_Right)) { HandleBtnExtended(Btn_Right); selectedSlot = InvMoveSelection(inventory, Dir2_Right); }
		if (BtnPressed(Btn_Up))    { HandleBtnExtended(Btn_Up);    selectedSlot = InvMoveSelection(inventory, Dir2_Up);    }
		if (BtnPressed(Btn_Down))  { HandleBtnExtended(Btn_Down);  selectedSlot = InvMoveSelection(inventory, Dir2_Down);  }
		
		// +==============================+
		// | Update Group Slide Animation |
		// +==============================+
		inventory->currentGroup = (selectedSlot != nullptr) ? selectedSlot->group : 0;
		r32 targetSlidePos = (r32)inventory->currentGroup;
		if (inventory->groupSlidePos < targetSlidePos)
		{
			
			inventory->groupSlidePos = MinR32(inventory->groupSlidePos + TIME_SCALED_ANIM(200, TimeScale), targetSlidePos);
		}
		else if (inventory->groupSlidePos > targetSlidePos)
		{
			inventory->groupSlidePos = MaxR32(inventory->groupSlidePos - TIME_SCALED_ANIM(200, TimeScale), targetSlidePos);
		}
		
		// +==============================+
		// |    Btn_A Presses Buttons     |
		// +==============================+
		if (BtnPressedRaw(Btn_A))
		{
			if (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots)
			{
				InvSlot_t* selectedSlot = &inventory->slots[inventory->selectionIndex];
				if (selectedSlot->type == InvSlotType_Button)
				{
					HandleBtnExtended(Btn_A);
					
					switch (selectedSlot->button)
					{
						// +==============================+
						// |   Press InvButton_Combine    |
						// +==============================+
						case InvButton_Combine:
						{
							InvSlot_t* slot1 = GetInvSlotAtGridPos(inventory, selectedSlot->group, NewVec2i(0, 0));
							InvSlot_t* slot2 = GetInvSlotAtGridPos(inventory, selectedSlot->group, NewVec2i(1, 0));
							if (slot1 != nullptr && slot2 != nullptr && slot1->stack.count > 0 && slot2->stack.count > 0)
							{
								bool successfulRecipe = false;
								Recipe_t* recipe = FindRecipeInBook(&gl->recipeBook, slot1->stack.id, slot2->stack.id);
								if (recipe != nullptr)
								{
									//otherInventory should be player's inventory
									if (otherInventory != nullptr)
									{
										u8 numUnadded = TryAddItemStackToInventory(otherInventory, NewItemStack(recipe->outputId, 1));
										if (numUnadded == 0)
										{
											successfulRecipe = true;
											InvGroup_t* group = &inventory->groups[slot1->group];
											Particle_t* newPart = TrySpawnParticle(
												&game->parts,
												PartLayer_HighUi,
												ToVec2(group->mainRec.topLeft + selectedSlot->mainRec.topLeft) + ToVec2(selectedSlot->mainRec.size) / 2,
												NewVec2(0, -1), //velocity
												&game->entitiesSheet,
												GetItemFrame(&gl->itemBook, recipe->outputId),
												1000 //lifespan
											);
											// PrintLine_D("Spawned particle (%d, %d) at (%g, %g)", newPart->frame.x, newPart->frame.y, newPart->position.x, newPart->position.y);
											UNUSED(newPart);
										}
										
									}
								}
								
								UNUSED(successfulRecipe);
								//Whether a recipe exists or not, we should consume the inputs and pay the cost
								//TODO: Make the player pay some money to try the combination
								//TODO: Play a sound effect based on success/failure
								
								slot1->stack.count--;
								if (slot1->stack.count == 0) { slot1->stack.id = ITEM_ID_NONE; }
								slot2->stack.count--;
								if (slot2->stack.count == 0) { slot2->stack.id = ITEM_ID_NONE; }
							}
						} break;
						
						default: PrintLine_E("Unhandled InvButton_t enum value: 0x%X %u", selectedSlot->button, selectedSlot->button); break;
					}
				}
			}
		}
	}
}

void RenderInventorySlot(InvSlot_t* slot, reci slotRec, bool inScrollView)
{
	MemArena_t* scratch = GetScratchArena();
	
	if (slot->type != InvSlotType_Decor && slot->type != InvSlotType_GroupChange)
	{
		PdDrawRec(slotRec, kColorWhite);
		PdDrawRecOutline(slotRec, 2, kColorBlack);
	}
	
	// +==============================+
	// |  Render InvSlotType_Default  |
	// +==============================+
	if (slot->type == InvSlotType_Default)
	{
		if (slot->stack.count > 0)
		{
			v2i itemFrame = GetItemFrame(&gl->itemBook, slot->stack.id);
			if (itemFrame != INVALID_FRAME)
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
	}
	// +==============================+
	// |  Render InvSlotType_Button   |
	// +==============================+
	else if (slot->type == InvSlotType_Button)
	{
		PdBindFont(&game->buttonFont);
		MyStr_t displayStr = NewStr(GetInvButtonDisplayStr(slot->button));
		v2i displayStrSize = MeasureText(game->buttonFont.font, displayStr);
		v2i textPos = NewVec2i(
			slotRec.x + slotRec.width/2 - displayStrSize.width/2,
			slotRec.y + slotRec.height/2 - displayStrSize.height/2
		);
		PdDrawText(displayStr, textPos);
	}
	// +==============================+
	// |   Render InvSlotType_Decor   |
	// +==============================+
	else if (slot->type == InvSlotType_Decor)
	{
		if (slot->texturePntr != nullptr && slot->texturePntr->isValid)
		{
			PdDrawTexturedRec(*slot->texturePntr, slotRec);
		}
		else
		{
			PdDrawRecOutline(slotRec, 2, false, kColorBlack);
		}
	}
	// +==============================+
	// |    Render InvSlotType_Buy    |
	// +==============================+
	else if (slot->type == InvSlotType_Buy)
	{
		v2i itemFrame = GetItemFrame(&gl->itemBook, slot->buyItemId);
		if (itemFrame != INVALID_FRAME)
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
	
	if (slot->isSelected && !inScrollView)
	{
		r32 selectionRotation = Animate(0.0f, 1.0f, 2000);
		PdDrawRecOutlineArc(slotRec, 5, selectionRotation + 0.00f, selectionRotation + 0.00f + 0.125f, kColorBlack);
		PdDrawRecOutlineArc(slotRec, 5, selectionRotation + 0.25f, selectionRotation + 0.25f + 0.125f, kColorBlack);
		PdDrawRecOutlineArc(slotRec, 5, selectionRotation + 0.50f, selectionRotation + 0.50f + 0.125f, kColorBlack);
		PdDrawRecOutlineArc(slotRec, 5, selectionRotation + 0.75f, selectionRotation + 0.75f + 0.125f, kColorBlack);
	}
	
	FreeScratchArena(scratch);
}

void RenderInventoryUi(Inventory_t* inventory, Inventory_t* otherInventory)
{
	NotNull2(inventory, inventory->allocArena);
	MemArena_t* scratch = GetScratchArena();
	bool isInShop = (inventory->type == InvType_Store || (otherInventory != nullptr && otherInventory->type == InvType_Store));
	
	if (inventory->inScrollView)
	{
		i32 largeSlotDiff = (INV_SLOT_LARGE_SIZE - INV_SLOT_SIZE);
		InvGroup_t* mainGroup = &inventory->groups[0];
		
		PdDrawRec(mainGroup->mainRec, kColorBlack);
		
		v2i slotBaseOffset = NewVec2i(
			mainGroup->mainRec.x + mainGroup->contentRec.x,
			mainGroup->mainRec.y + mainGroup->mainRec.height/2 - RoundR32i(inventory->scroll)
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
			
			if (pig->debugEnabled && inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex == sIndex)
			{
				PdBindFont(&pig->debugFont);
				PdDrawTextPrint(slotRec.topLeft + NewVec2i(3, 3), "(%d, %d)", inventory->selectionGridPos.x, inventory->selectionGridPos.y);
			}
			
			if (isInShop && slot->stack.count > 0)
			{
				u8 itemValue = GetItemValue(&gl->itemBook, slot->stack.id);
				if (itemValue > 0)
				{
					PdBindFont(&game->beanCountFont);
					MyStr_t priceStr = FormatNumberWithCommas(itemValue, scratch);
					v2i priceSize = MeasureText(game->beanCountFont.font, priceStr);
					v2i pricePos = NewVec2i(slotRec.x - 2 - priceSize.width, slotRec.y + slotRec.height/2 - priceSize.height/2);
					PdDrawText(priceStr, pricePos);
					v2i beanSize = game->beanTexture.size;
					reci beanRec = NewReci(pricePos.x - beanSize.width, pricePos.y + priceSize.height/2 - beanSize.height/2, beanSize);
					PdDrawTexturedRec(game->beanTexture, beanRec);
				}
			}
			
			slotOffset.y += slotRec.height + INV_SLOT_MARGIN;
		}
		
		if (pig->debugEnabled)
		{
			LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
			reci contentRec = NewReci(
				mainGroup->mainRec.x + mainGroup->contentRec.x,
				mainGroup->mainRec.y + mainGroup->mainRec.height/2 - RoundR32i(inventory->scroll),
				mainGroup->contentRec.size
			);;
			PdDrawRec(ReciInflate(contentRec, RoundR32i(Oscillate(0, 5, 1000)), 0), kColorBlack);
			PdSetDrawMode(oldDrawMode);
		}
		
		if (inventory->showCrankHint)
		{
			const r32 crankHintScale = 1.0f;
			v2i playdateCrankHintSize = PlaydateCrankHintBubble(false, true, crankHintScale, Vec2i_Zero).size;
			v2i playdateCrankHintTarget = NewVec2i(mainGroup->mainRec.x, mainGroup->mainRec.y + mainGroup->mainRec.height - 5 - playdateCrankHintSize.height/2);
			PlaydateCrankHintBubble(true, true, crankHintScale, playdateCrankHintTarget);
		}
		
		if (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots)
		{
			InvSlot_t* selectedSlot = &inventory->slots[inventory->selectionIndex];
			if (selectedSlot->stack.count > 0 && selectedSlot->stack.id != ITEM_ID_NONE)
			{
				MyStr_t itemName = GetItemDisplayName(&gl->itemBook, selectedSlot->stack.id, (selectedSlot->stack.count > 1));
				v2i itemNameSize = MeasureText(game->mainFont.font, itemName);
				v2i itemNamePos = NewVec2i(mainGroup->mainRec.x - 2 - itemNameSize.width, 2);
				PdBindFont(&game->mainFont);
				PdDrawText(itemName, itemNamePos);
			}
		}
	}
	else 
	{
		// +==============================+
		// |   Render Inv Groups/Slots    |
		// +==============================+
		for (u64 gIndex = 0; gIndex < inventory->numGroups; gIndex++)
		{
			InvGroup_t* group = &inventory->groups[gIndex];
			if (inventory->groupSlidePos > (r32)gIndex - 1.0f && inventory->groupSlidePos < (r32)gIndex + 1.0f)
			{
				reci mainRec = group->mainRec;
				r32 animRight = SubAnimAmountR32(inventory->groupSlidePos, (r32)gIndex, (r32)gIndex - 1.0f);
				r32 animLeft = SubAnimAmountR32(inventory->groupSlidePos, (r32)gIndex, (r32)gIndex + 1.0f);
				mainRec.x += RoundR32i((ScreenSize.width - mainRec.x) * EaseQuadraticOut(animRight));
				mainRec.x -= RoundR32i((mainRec.x + mainRec.width) * EaseQuadraticOut(animLeft));
				
				PdDrawRec(mainRec, kColorWhite);
				PdDrawRecOutline(mainRec, INV_BORDER_THICKNESS, kColorBlack);
				
				// +==============================+
				// |   Render Buying Item Name    |
				// +==============================+
				if (inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex < inventory->numSlots)
				{
					InvSlot_t* selectedSlot = &inventory->slots[inventory->selectionIndex];
					if (selectedSlot->type == InvSlotType_Buy && selectedSlot->group == gIndex)
					{
						MyStr_t displayName = GetItemDisplayName(&gl->itemBook, selectedSlot->buyItemId, false);
						v2i displayNameSize = MeasureText(game->mainFont.font, displayName);
						MyStr_t priceStr = PrintInArenaStr(scratch, "%llu", GetItemValue(&gl->itemBook, selectedSlot->buyItemId));
						v2i priceStrSize = MeasureText(game->beanCountFont.font, priceStr);
						v2i beanSize = game->beanTexture.size;
						i32 totalWidth = displayNameSize.width + beanSize.width + priceStrSize.width;
						i32 leftSide = mainRec.x + mainRec.width/2 - totalWidth/2;
						v2i displayNamePos = NewVec2i(
							leftSide + 0,
							mainRec.y - displayNameSize.height
						);
						reci beanRec = NewReci(
							leftSide + displayNameSize.width + ITEM_NAME_PRICE_SPACING,
							mainRec.y - beanSize.height,
							beanSize
						);
						v2i priceStrPos = NewVec2i(
							leftSide + displayNameSize.width + ITEM_NAME_PRICE_SPACING + beanSize.width,
							mainRec.y - displayNameSize.height
						);
						PdDrawTexturedRec(game->beanTexture, beanRec);
						PdBindFont(&game->mainFont);
						PdDrawText(displayName, displayNamePos);
						PdBindFont(&game->beanCountFont);
						PdDrawText(priceStr, priceStrPos);
					}
				}
				
				reci contentRec = group->contentRec + mainRec.topLeft;
				for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
				{
					InvSlot_t* slot = &inventory->slots[sIndex];
					if (slot->group == gIndex)
					{
						reci slotRec = slot->mainRec + contentRec.topLeft;
						RenderInventorySlot(slot, slotRec, inventory->inScrollView);
						if (pig->debugEnabled && inventory->selectionIndex >= 0 && (u64)inventory->selectionIndex == sIndex)
						{
							PdBindFont(&pig->debugFont);
							PdDrawTextPrint(slotRec.topLeft + NewVec2i(4, 4), "(%d, %d)", inventory->selectionGridPos.x, inventory->selectionGridPos.y);
						}
					}
				}
			}
		}
	}
	
	FreeScratchArena(scratch);
}
