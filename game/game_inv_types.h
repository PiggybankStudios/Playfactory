/*
File:   game_inv_types.h
Author: Taylor Robbins
Date:   09\11\2023
*/

#ifndef _GAME_INV_TYPES_H
#define _GAME_INV_TYPES_H

#define PLAYER_INV_SIZE NewVec2i(4, 3)

enum InvType_t
{
	InvType_None = 0,
	InvType_PlayerInventory,
	InvType_SmallBox,
	InvType_NumTypes,
};
const char* GetInvTypeStr(InvType_t enumValue)
{
	switch (enumValue)
	{
		case InvType_None:            return "None";
		case InvType_PlayerInventory: return "PlayerInventory";
		case InvType_SmallBox:        return "SmallBox";
		default: return "Unknown";
	}
}

#endif //  _GAME_INV_TYPES_H
