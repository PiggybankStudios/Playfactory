/*
File:   game_item_defs.h
Author: Taylor Robbins
Date:   10\27\2023
*/

#ifndef _GAME_ITEM_DEFS_H
#define _GAME_ITEM_DEFS_H

enum ItemFlags_t
{
	ItemFlags_None        = 0x00,
	ItemFlags_Tile        = 0x01,
	ItemFlags_Solid       = 0x02,
	ItemFlags_Meta        = 0x04,
	ItemFlags_Underground = 0x08,
	ItemFlags_Surface     = 0x10,
	ItemFlags_Sky         = 0x20,
	ItemFlags_Decor       = 0x40,
	// ItemFlags_Unused = 0x80,
	ItemFlags_NumFlags = 0x80,
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
	u8 flags; //ItemFlags_t
	MyStr_t idStr;
	MyStr_t displayName;
	MyStr_t displayNamePlural;
	v2i frame;
	ItemStack_t dropStack;
};

struct ItemBook_t
{
	MemArena_t* allocArena;
	u16 nextId;
	VarArray_t items; //ItemDef_t
};

#endif //  _GAME_ITEM_DEFS_H
