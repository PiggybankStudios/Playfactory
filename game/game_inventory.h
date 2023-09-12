/*
File:   game_inventory.h
Author: Taylor Robbins
Date:   09\11\2023
*/

#ifndef _GAME_INVENTORY_H
#define _GAME_INVENTORY_H

#define INV_SLOT_SIZE    NewVec2i(26, 26)
#define INV_SLOT_ADVANCE (INV_SLOT_SIZE + NewVec2i(2, 2))
#define INV_ITEM_SIZE    NewVec2i(24, 24)

struct InvSlot_t
{
	u64 index;
	u64 groupId;
	v2i gridPos;
	ItemStack_t stack;
	reci mainRec;
};

struct Inventory_t
{
	MemArena_t* allocArena;
	u64 numSlots;
	InvSlot_t* slots;
	reci contentRec;
	reci mainRec;
};

#endif //  _GAME_INVENTORY_H
