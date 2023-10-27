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
	
	ItemId_Mining,
	
	ItemId_Sugar,
	ItemId_Dough,
	ItemId_Fruit,
	ItemId_Mint,
	
	ItemId_PowderSugar,
	ItemId_Gum,
	ItemId_Apple,
	ItemId_Jam,
	ItemId_CandyCane,
	ItemId_Cookie,
	ItemId_Ice,
	
	ItemId_Cake,
	ItemId_Donut,
	ItemId_CandiedFruit,
	ItemId_FruitGum,
	ItemId_ApplePie,
	
	ItemId_CottonCandy,
	ItemId_FruitCake,
	ItemId_Gumball,
	ItemId_Jello,
	
	ItemId_NumIds,
};
const char* GetItemIdStr(ItemId_t enumValue)
{
	switch (enumValue)
	{
		case ItemId_None:         return "None";
		case ItemId_Sugar:        return "Sugar";
		case ItemId_Dough:        return "Dough";
		case ItemId_Fruit:        return "Fruit";
		case ItemId_Mint:         return "Mint";
		case ItemId_PowderSugar:  return "PowderSugar";
		case ItemId_Gum:          return "Gum";
		case ItemId_Apple:        return "Apple";
		case ItemId_Jam:          return "Jam";
		case ItemId_CandyCane:    return "CandyCane";
		case ItemId_Cookie:       return "Cookie";
		case ItemId_Ice:          return "Ice";
		case ItemId_Cake:         return "Cake";
		case ItemId_Donut:        return "Donut";
		case ItemId_CandiedFruit: return "CandiedFruit";
		case ItemId_FruitGum:     return "FruitGum";
		case ItemId_ApplePie:     return "ApplePie";
		case ItemId_CottonCandy:  return "CottonCandy";
		case ItemId_FruitCake:    return "FruitCake";
		case ItemId_Gumball:      return "Gumball";
		case ItemId_Jello:        return "Jello";
		default: return "Unknown";
	}
}
const char* GetItemIdDisplayStr(ItemId_t enumValue)
{
	switch (enumValue)
	{
		case ItemId_None:         return "None";
		case ItemId_Sugar:        return "Sugar";
		case ItemId_Dough:        return "Dough";
		case ItemId_Fruit:        return "Fruit";
		case ItemId_Mint:         return "Mint";
		case ItemId_PowderSugar:  return "Powder Sugar";
		case ItemId_Gum:          return "Gum";
		case ItemId_Apple:        return "Apple";
		case ItemId_Jam:          return "Jam";
		case ItemId_CandyCane:    return "Candy Cane";
		case ItemId_Cookie:       return "Cookie";
		case ItemId_Ice:          return "Ice";
		case ItemId_Cake:         return "Cake";
		case ItemId_Donut:        return "Donut";
		case ItemId_CandiedFruit: return "Candied Fruit";
		case ItemId_FruitGum:     return "Fruit Gum";
		case ItemId_ApplePie:     return "Apple Pie";
		case ItemId_CottonCandy:  return "Cotton Candy";
		case ItemId_FruitCake:    return "Fruit Cake";
		case ItemId_Gumball:      return "Gumball";
		case ItemId_Jello:        return "Jello";
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
		case ItemId_Sugar:        return NewVec2i(0, 6);
		case ItemId_Dough:        return NewVec2i(1, 6);
		case ItemId_Fruit:        return NewVec2i(2, 6);
		case ItemId_Mint:         return NewVec2i(3, 6);
		case ItemId_PowderSugar:  return NewVec2i(4, 6);
		case ItemId_Gum:          return NewVec2i(5, 6);
		case ItemId_Apple:        return NewVec2i(6, 6);
		case ItemId_Jam:          return NewVec2i(7, 6);
		case ItemId_CandyCane:    return NewVec2i(5, 7);
		case ItemId_Cookie:       return NewVec2i(7, 7);
		case ItemId_Ice:          return NewVec2i(7, 5);
		case ItemId_Cake:         return NewVec2i(4, 7);
		case ItemId_Donut:        return NewVec2i(6, 7);
		case ItemId_CandiedFruit: return NewVec2i(1, 5); //TODO:
		case ItemId_FruitGum:     return NewVec2i(1, 5); //TODO:
		case ItemId_ApplePie:     return NewVec2i(1, 5); //TODO:
		case ItemId_CottonCandy:  return NewVec2i(1, 5); //TODO:
		case ItemId_FruitCake:    return NewVec2i(1, 5); //TODO:
		case ItemId_Gumball:      return NewVec2i(1, 5); //TODO:
		case ItemId_Jello:        return NewVec2i(1, 5); //TODO:
		default: return NewVec2i(-1, -1);
	}
}

#endif //  _GAME_ITEM_IDS_H
