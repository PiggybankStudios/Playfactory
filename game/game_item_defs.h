/*
File:   game_item_defs.h
Author: Taylor Robbins
Date:   10\27\2023
*/

#ifndef _GAME_ITEM_DEFS_H
#define _GAME_ITEM_DEFS_H

enum ItemFlags_t
{
	ItemFlags_None        = 0x0000,
	ItemFlags_Tile        = 0x0001,
	ItemFlags_Solid       = 0x0002,
	ItemFlags_Meta        = 0x0004,
	ItemFlags_Underground = 0x0008,
	ItemFlags_Surface     = 0x0010,
	ItemFlags_Sky         = 0x0020,
	ItemFlags_Decor       = 0x0040,
	ItemFlags_InStore     = 0x0080,
	ItemFlags_NoResearch  = 0x0100,
	ItemFlags_Holdable    = 0x0200,
	// ItemFlags_Unused      = 0x0400,
	// ItemFlags_Unused      = 0x0800,
	// ItemFlags_Unused      = 0x1000,
	// ItemFlags_Unused      = 0x2000,
	// ItemFlags_Unused      = 0x4000,
	// ItemFlags_Unused      = 0x8000,
	ItemFlags_NumFlags = 10,
};
const char* GetItemFlagsStr(ItemFlags_t enumValue)
{
	switch (enumValue)
	{
		case ItemFlags_None:         return "None";
		case ItemFlags_Tile:         return "Tile";
		case ItemFlags_Solid:        return "Solid";
		case ItemFlags_Meta:         return "Meta";
		case ItemFlags_Underground:  return "Underground";
		case ItemFlags_Surface:      return "Surface";
		case ItemFlags_Sky:          return "Sky";
		case ItemFlags_Decor:        return "Decor";
		case ItemFlags_InStore:      return "InStore";
		case ItemFlags_NoResearch:   return "NoResearch";
		case ItemFlags_Holdable:     return "Holdable";
		default: return "Unknown";
	}
}

struct ItemStack_t
{
	u16 id; //runtimeId
	u8 count;
};

ItemStack_t NewItemStack(u16 id, u8 count)
{
	ItemStack_t result = {};
	result.id = id;
	result.count = count;
	return result;
}

struct ItemDef_t
{
	u16 runtimeId;
	u16 flags; //ItemFlags_t
	MyStr_t idStr;
	MyStr_t displayName;
	MyStr_t displayNamePlural;
	v2i frame;
	ItemStack_t dropStack;
	u8 value;
	InvType_t inventoryType;
};

struct ItemBook_t
{
	MemArena_t* allocArena;
	u16 nextId;
	VarArray_t items; //ItemDef_t
};

#endif //  _GAME_ITEM_DEFS_H
