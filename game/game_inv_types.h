/*
File:   game_inv_types.h
Author: Taylor Robbins
Date:   09\11\2023
*/

#ifndef _GAME_INV_TYPES_H
#define _GAME_INV_TYPES_H

#define PLAYER_INV_SIZE    NewVec2i(4, 3)
#define SMALL_BOX_INV_SIZE NewVec2i(2, 2)

enum InvType_t
{
	InvType_None = 0,
	InvType_PlayerInventory,
	InvType_PlayerScience,
	InvType_SmallBox,
	InvType_Store,
	InvType_NumTypes,
};
const char* GetInvTypeStr(InvType_t enumValue)
{
	switch (enumValue)
	{
		case InvType_None:            return "None";
		case InvType_PlayerInventory: return "PlayerInventory";
		case InvType_PlayerScience:   return "PlayerScience";
		case InvType_SmallBox:        return "SmallBox";
		case InvType_Store:           return "Store";
		default: return "Unknown";
	}
}

#endif //  _GAME_INV_TYPES_H
