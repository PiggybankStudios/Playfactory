/*
File:   game_item_ids.h
Author: Taylor Robbins
Date:   09\11\2023
*/

#ifndef _GAME_ITEM_IDS_H
#define _GAME_ITEM_IDS_H

enum ItemId_t
{
	ItemId_None = 0,
	ItemId_Peppermint,
	ItemId_Strawberry,
	ItemId_Cherry,
	ItemId_Caramel,
	ItemId_NumIds,
};
const char* GetItemIdStr(ItemId_t enumValue)
{
	switch (enumValue)
	{
		case ItemId_None:       return "None";
		case ItemId_Peppermint: return "Peppermint";
		case ItemId_Strawberry: return "Strawberry";
		case ItemId_Cherry:     return "Cherry";
		case ItemId_Caramel:    return "Caramel";
		default: return "Unknown";
	}
}

struct ItemStack_t
{
	ItemId_t id;
	u8 count;
};

ItemStack_t NewItemStack(ItemId_t id, u8 count)
{
	ItemStack_t result = {};
	result.id = id;
	result.count = count;
	return result;
}

v2i GetItemIdFrame(ItemId_t id)
{
	switch (id)
	{
		case ItemId_Peppermint:   return NewVec2i(0, 0);
		case ItemId_Strawberry:   return NewVec2i(0, 1);
		case ItemId_Cherry:       return NewVec2i(0, 2);
		case ItemId_Caramel:      return NewVec2i(0, 3);
		default: return NewVec2i(-1, -1);
	}
}

#endif //  _GAME_ITEM_IDS_H
