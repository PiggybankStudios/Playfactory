/*
File:   game_inventory.cpp
Author: Taylor Robbins
Date:   09\11\2023
Description: 
	** Inventories are things that have some number of slots to hold items.
	** They can be chests, machines, or the player inventory
*/

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
		slot->stack.type = (TileType_t)(GetRandU32(&pig->random, TileType_Peppermint, TileType_NumTiles)); //TODO: Remove me!
		slot->stack.count = 1;
	}
}

void RenderInventoryUi(Inventory_t* inventory)
{
	NotNull2(inventory, inventory->allocArena);
	
	inventory->contentRec = Reci_Zero;
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		inventory->contentRec = ReciExpandToVec2i(inventory->contentRec, slot->mainRec.topLeft);
		inventory->contentRec = ReciExpandToVec2i(inventory->contentRec, slot->mainRec.topLeft + slot->mainRec.size);
	}
	inventory->contentRec.topLeft += NewVec2i(2, 2);
	inventory->mainRec.size = inventory->contentRec.size + NewVec2i(2*2, 2*2);
	inventory->mainRec.topLeft = NewVec2i(
		ScreenSize.width / 2 - inventory->mainRec.width/2,
		ScreenSize.height / 2 - inventory->mainRec.height/2
	);
	
	PdDrawRec(inventory->mainRec, kColorWhite);
	PdDrawRecOutline(inventory->mainRec, 2, kColorBlack);
	
	reci contentRec = inventory->contentRec + inventory->mainRec.topLeft;
	for (u64 sIndex = 0; sIndex < inventory->numSlots; sIndex++)
	{
		InvSlot_t* slot = &inventory->slots[sIndex];
		reci slotRec = slot->mainRec + contentRec.topLeft;
		
		PdDrawRec(slotRec, kColorWhite);
		PdDrawRecOutline(slotRec, 2, kColorBlack);
		
		if (slot->stack.count > 0)
		{
			v2i itemFrame = GetTileTypeFrame(slot->stack.type, false, slot->gridPos);
			if (itemFrame != NewVec2i(-1, -1))
			{
				reci itemRec = NewReci(
					slotRec.x + slotRec.width/2 - INV_ITEM_SIZE.width/2,
					slotRec.y + slotRec.height/2 - INV_ITEM_SIZE.height/2,
					INV_ITEM_SIZE
				);
				PdDrawSheetFrame(game->entitiesSheet, itemFrame, itemRec);
			}
		}
	}
}
