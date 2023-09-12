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
	TileType_Log,
	TileType_NumTiles,
};
const char* GetTileTypeStr(TileType_t enumValue)
{
	switch (enumValue)
	{
		case TileType_None:  return "None";
		case TileType_Grass: return "Grass";
		case TileType_Log:   return "Log";
		default: return "Unknown";
	}
}

v2i GetTileTypeFrame(TileType_t type)
{
	switch (type)
	{
		case TileType_Grass: return NewVec2i(1, 0);
		case TileType_Log:   return NewVec2i(18, 6);
		default: return NewVec2i(-1, -1);
	}
}
bool IsTileTypeSolid(TileType_t type)
{
	switch (type)
	{
		case TileType_Log:   return true;
		default: return false;
	}
}

#endif //  _GAME_TILE_TYPES_H
