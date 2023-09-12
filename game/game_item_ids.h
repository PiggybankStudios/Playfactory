/*
File:   game_item_ids.h
Author: Taylor Robbins
Date:   09\11\2023
*/

#ifndef _GAME_ITEM_IDS_H
#define _GAME_ITEM_IDS_H

struct ItemStack_t
{
	TileType_t type;
	u8 count;
};

ItemStack_t NewItemStack(TileType_t type, u8 count)
{
	ItemStack_t result = {};
	result.type = type;
	result.count = count;
	return result;
}

#endif //  _GAME_ITEM_IDS_H
