/*
File:   game_tile_types.h
Author: Taylor Robbins
Date:   09\10\2023
*/

#ifndef _GAME_TILE_TYPES_H
#define _GAME_TILE_TYPES_H

enum TileType_t
{
	TileType_None = 0,
	TileType_Grass,
	TileType_Peppermint,
	TileType_Strawberry,
	TileType_Cherry,
	TileType_Caramel,
	TileType_NumTiles,
};
const char* GetTileTypeStr(TileType_t enumValue)
{
	switch (enumValue)
	{
		case TileType_None:       return "None";
		case TileType_Grass:      return "Grass";
		case TileType_Peppermint: return "Peppermint";
		case TileType_Strawberry: return "Strawberry";
		case TileType_Cherry:     return "Cherry";
		case TileType_Caramel:    return "Caramel";
		default: return "Unknown";
	}
}

v2i GetTileTypeFrame(TileType_t type, bool inWorld, v2i tilePos)
{
	bool oddTile = ((tilePos.x + tilePos.y) % 2 != 0);
	switch (type)
	{
		case TileType_Grass:        return NewVec2i(3, 0);
		case TileType_Peppermint:   return inWorld ? (oddTile ? NewVec2i(1, 0) : NewVec2i(2, 0)) : NewVec2i(0, 0);
		case TileType_Strawberry:   return inWorld ? (oddTile ? NewVec2i(1, 1) : NewVec2i(2, 1)) : NewVec2i(0, 1);
		case TileType_Cherry:       return inWorld ? (oddTile ? NewVec2i(1, 2) : NewVec2i(2, 2)) : NewVec2i(0, 2);
		case TileType_Caramel:      return inWorld ? (oddTile ? NewVec2i(1, 3) : NewVec2i(2, 3)) : NewVec2i(0, 3);
		default: return NewVec2i(-1, -1);
	}
}
bool IsTileTypeSolid(TileType_t type)
{
	switch (type)
	{
		case TileType_Peppermint: return true;
		case TileType_Strawberry: return true;
		case TileType_Cherry:     return true;
		case TileType_Caramel:    return true;
		default: return false;
	}
}

ItemStack_t GetTileTypeDrop(TileType_t type)
{
	switch (type)
	{
		case TileType_Peppermint: return NewItemStack(ItemId_Peppermint, 1);
		case TileType_Strawberry: return NewItemStack(ItemId_Strawberry, 1);
		case TileType_Cherry:     return NewItemStack(ItemId_Cherry,     1);
		case TileType_Caramel:    return NewItemStack(ItemId_Caramel,    1);
		default: return NewItemStack(ItemId_None, 0);
	}
}

#endif //  _GAME_TILE_TYPES_H
