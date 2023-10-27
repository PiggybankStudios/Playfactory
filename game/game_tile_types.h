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
	TileType_Sugar,
	TileType_Dough,
	TileType_Fruit,
	TileType_Mint,
	TileType_NumTiles,
};
const char* GetTileTypeStr(TileType_t enumValue)
{
	switch (enumValue)
	{
		case TileType_None:  return "None";
		case TileType_Grass: return "Grass";
		case TileType_Sugar: return "Sugar";
		case TileType_Dough: return "Dough";
		case TileType_Fruit: return "Fruit";
		case TileType_Mint:  return "Mint";
		default: return "Unknown";
	}
}

v2i GetTileTypeFrame(TileType_t type, v2i tilePos)
{
	bool oddTile = ((tilePos.x + tilePos.y) % 2 != 0);
	switch (type)
	{
		case TileType_Grass:        return NewVec2i(3, 0);
		case TileType_Sugar:        return NewVec2i(0, 7);
		case TileType_Dough:        return NewVec2i(1, 7);
		case TileType_Fruit:        return NewVec2i(2, 7);
		case TileType_Mint:         return NewVec2i(3, 7);
		default: return NewVec2i(-1, -1);
	}
}
bool IsTileTypeSolid(TileType_t type)
{
	switch (type)
	{
		case TileType_Sugar: return false;
		case TileType_Dough: return true;
		case TileType_Fruit: return true;
		case TileType_Mint:  return true;
		default: return false;
	}
}

ItemStack_t GetTileTypeDrop(TileType_t type)
{
	switch (type)
	{
		case TileType_Sugar: return NewItemStack(ItemId_Sugar, 2);
		case TileType_Dough: return NewItemStack(ItemId_Dough, 1);
		case TileType_Fruit: return NewItemStack(ItemId_Fruit, 1);
		case TileType_Mint:  return NewItemStack(ItemId_Mint,  1);
		default: return NewItemStack(ItemId_None, 0);
	}
}

#endif //  _GAME_TILE_TYPES_H
