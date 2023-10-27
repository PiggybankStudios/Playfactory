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
	ItemId_Sugar,
	ItemId_Dough,
	ItemId_Fruit,
	ItemId_Mint,
	ItemId_PowderSuger,
	ItemId_Gum,
	ItemId_Apple,
	ItemId_Jam,
	ItemId_Cake,
	ItemId_CandyCane,
	ItemId_Donut,
	ItemId_Cookie,
	ItemId_Ice,
	ItemId_NumIds,
};
const char* GetItemIdStr(ItemId_t enumValue)
{
	switch (enumValue)
	{
		case ItemId_None:        return "None";
		case ItemId_Sugar:       return "Sugar";
		case ItemId_Dough:       return "Dough";
		case ItemId_Fruit:       return "Fruit";
		case ItemId_Mint:        return "Mint";
		case ItemId_PowderSuger: return "PowderSuger";
		case ItemId_Gum:         return "Gum";
		case ItemId_Apple:       return "Apple";
		case ItemId_Jam:         return "Jam";
		case ItemId_Cake:        return "Cake";
		case ItemId_CandyCane:   return "CandyCane";
		case ItemId_Donut:       return "Donut";
		case ItemId_Cookie:      return "Cookie";
		case ItemId_Ice:         return "Ice";
		default: return "Unknown";
	}
}
const char* GetItemIdDisplayStr(ItemId_t enumValue)
{
	switch (enumValue)
	{
		case ItemId_None:        return "None";
		case ItemId_Sugar:       return "Sugar";
		case ItemId_Dough:       return "Dough";
		case ItemId_Fruit:       return "Fruit";
		case ItemId_Mint:        return "Mint";
		case ItemId_PowderSuger: return "Powder Suger";
		case ItemId_Gum:         return "Gum";
		case ItemId_Apple:       return "Apple";
		case ItemId_Jam:         return "Jam";
		case ItemId_Cake:        return "Cake";
		case ItemId_CandyCane:   return "Candy Cane";
		case ItemId_Donut:       return "Donut";
		case ItemId_Cookie:      return "Cookie";
		case ItemId_Ice:         return "Ice";
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
		case ItemId_Sugar:       return NewVec2i(0, 6);
		case ItemId_Dough:       return NewVec2i(1, 6);
		case ItemId_Fruit:       return NewVec2i(2, 6);
		case ItemId_Mint:        return NewVec2i(3, 6);
		case ItemId_PowderSuger: return NewVec2i(4, 6);
		case ItemId_Gum:         return NewVec2i(5, 6);
		case ItemId_Apple:       return NewVec2i(6, 6);
		case ItemId_Jam:         return NewVec2i(7, 6);
		case ItemId_Cake:        return NewVec2i(4, 7);
		case ItemId_CandyCane:   return NewVec2i(5, 7);
		case ItemId_Donut:       return NewVec2i(6, 7);
		case ItemId_Cookie:      return NewVec2i(7, 7);
		case ItemId_Ice:         return NewVec2i(7, 5);
		default: return NewVec2i(-1, -1);
	}
}

#endif //  _GAME_ITEM_IDS_H
