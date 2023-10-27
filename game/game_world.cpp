/*
File:   game_world.cpp
Author: Taylor Robbins
Date:   09\10\2023
Description: 
	** Holds the functions that manage the world that the player walks around in,
	** and all machines, pipes, etc. are placed into
*/

WorldTile_t* GetWorldTileAt(World_t* world, v2i tilePos)
{
	NotNull2(world, world->tiles);
	if (tilePos.x < 0) { return nullptr; }
	if (tilePos.y < 0) { return nullptr; }
	if (tilePos.x >= world->size.width) { return nullptr; }
	if (tilePos.y >= world->size.height) { return nullptr; }
	return &world->tiles[tilePos.y * world->size.width + tilePos.x];
}
bool IsSolidWorldTileAt(World_t* world, v2i tilePos)
{
	NotNull2(world, world->tiles);
	if (tilePos.x < 0) { return true; }
	if (tilePos.y < 0) { return true; }
	if (tilePos.x >= world->size.width) { return true; }
	if (tilePos.y >= world->size.height) { return true; }
	return world->tiles[tilePos.y * world->size.width + tilePos.x].isSolid;
}

v2 ResolveWorldTileCollisions(World_t* world, rec colRec)
{
	rec resultRec = colRec;
	
	bool hadCollision = false;
	u64 numCollisions = 0;
	do
	{
		hadCollision = false;
		
		v2i minTilePos = NewVec2i(
			FloorR32i(resultRec.x / TILE_SIZE),
			FloorR32i(resultRec.y / TILE_SIZE)
		);
		v2i maxTilePos = NewVec2i(
			CeilR32i((resultRec.x + resultRec.width) / TILE_SIZE),
			CeilR32i((resultRec.y + resultRec.height) / TILE_SIZE)
		);
		
		for (i32 tileY = minTilePos.y; tileY < maxTilePos.y; tileY++)
		{
			for (i32 tileX = minTilePos.x; tileX < maxTilePos.x; tileX++)
			{
				v2i tilePos = NewVec2i(tileX, tileY);
				
				bool isSolid = IsSolidWorldTileAt(world, tilePos);
				if (isSolid)
				{
					u8 blockedSides = Dir2_None;
					if (IsSolidWorldTileAt(world, tilePos + NewVec2i( 1, 0))) { FlagSet(blockedSides, Dir2_Right); }
					if (IsSolidWorldTileAt(world, tilePos + NewVec2i( 0, 1))) { FlagSet(blockedSides, Dir2_Down);  }
					if (IsSolidWorldTileAt(world, tilePos + NewVec2i(-1, 0))) { FlagSet(blockedSides, Dir2_Left);  }
					if (IsSolidWorldTileAt(world, tilePos + NewVec2i( 0,-1))) { FlagSet(blockedSides, Dir2_Up);    }
					
					rec tileRec = NewRec(
						(r32)tilePos.x * TILE_SIZE,
						(r32)tilePos.y * TILE_SIZE,
						TILE_SIZE,
						TILE_SIZE
					);
					
					r32 resolveDistLeft = resultRec.x + resultRec.width - tileRec.x;
					r32 resolveDistRight = tileRec.x + tileRec.width - resultRec.x;
					r32 resolveDistUp = resultRec.y + resultRec.height - tileRec.y;
					r32 resolveDistDown = tileRec.y + tileRec.height - resultRec.y;
					if (IsFlagSet(blockedSides, Dir2_Left))  { resolveDistLeft  = INFINITY; }
					if (IsFlagSet(blockedSides, Dir2_Right)) { resolveDistRight = INFINITY; }
					if (IsFlagSet(blockedSides, Dir2_Up))    { resolveDistUp    = INFINITY; }
					if (IsFlagSet(blockedSides, Dir2_Down))  { resolveDistDown  = INFINITY; }
					
					Dir2_t resolveDir = Dir2_None;
					r32 resolveDist = 0.0f;
					
					u8 preferRight = 0;
					r32 leftRightMin = 0.0f;
					bool leftOrRight = MinNoInfinitiesR32(resolveDistLeft, resolveDistRight, &leftRightMin, &preferRight);
					u8 preferDown = 0;
					r32 upDownMin = 0.0f;
					bool upOrDown = MinNoInfinitiesR32(resolveDistUp, resolveDistDown, &upDownMin, &preferDown);
					if (!leftOrRight && !upOrDown) { continue; }
					else if (!upOrDown) { resolveDir = preferRight ? Dir2_Right : Dir2_Left; resolveDist = leftRightMin; }
					else if (!leftOrRight) { resolveDir = preferDown ? Dir2_Down : Dir2_Up; resolveDist = upDownMin; }
					else
					{
						u8 preferVertical = 0;
						MinNoInfinitiesR32(leftRightMin, upDownMin, &resolveDist, &preferVertical);
						resolveDir = preferVertical ? (preferDown ? Dir2_Down : Dir2_Up) : (preferRight ? Dir2_Right : Dir2_Left);
					}
					Assert(resolveDir != Dir2_None && !IsInfiniteR32(resolveDist) && resolveDist >= 0);
					
					resultRec.topLeft += ToVec2(resolveDir) * resolveDist;
					
					hadCollision = true;
					numCollisions++;
					break;
				}
			}
			if (hadCollision) { break; }
		}
	} while (hadCollision && numCollisions < MAX_NUM_COLLISIONS_PER_FRAME);
	
	return resultRec.topLeft;
}

void FreeWorld(World_t* world)
{
	if (world->allocArena != nullptr)
	{
		if (world->tiles != nullptr)
		{
			FreeMem(world->allocArena, world->tiles, sizeof(WorldTile_t) * world->numTiles);
		}
	}
	ClearPointer(world);
}

void InitWorld(World_t* world, MemArena_t* memArena, v2i size, u64 seed)
{
	NotNull2(world, memArena);
	ClearPointer(world);
	world->allocArena = memArena;
	world->size = size;
	world->seed = seed;
	CreateRandomSeries(&world->genRand);
	SeedRandomSeriesU64(&world->genRand, world->seed);
	
	Assert(world->size.width * world->size.height > 0);
	world->numTiles = (u64)(world->size.width * world->size.height);
	world->tiles = AllocArray(world->allocArena, WorldTile_t, world->numTiles);
	NotNull(world->tiles);
	
	for (i32 tilePosY = 0; tilePosY < world->size.height; tilePosY++)
	{
		for (i32 tilePosX = 0; tilePosX < world->size.width; tilePosX++)
		{
			v2i tilePos = NewVec2i(tilePosX, tilePosY);
			WorldTile_t* tile = GetWorldTileAt(world, tilePos);
			NotNull(tile);
			tile->type = TileType_None;
			if (GetRandR32(&world->genRand) < WORLD_GEN_CANDY_DENSITY)
			{
				tile->type = (TileType_t)(TileType_Sugar + GetRandU32(&pig->random, 0, 4));
			}
			else if (GetRandR32(&world->genRand) < WORLD_GEN_GRASS_DENSITY)
			{
				tile->type = TileType_Grass;
			}
			tile->isSolid = IsTileTypeSolid(tile->type);
			tile->pos = tilePos;
		}
	}
}

void UpdateWorld(World_t* world)
{
	NotNull2(world, world->allocArena);
	
	//TODO: Implement me!
}

void RenderWorld(World_t* world, const Player_t* player)
{
	NotNull2(world, world->allocArena);
	
	for (i32 tilePosY = 0; tilePosY < world->size.height; tilePosY++)
	{
		for (i32 tilePosX = 0; tilePosX < world->size.width; tilePosX++)
		{
			v2i tilePos = NewVec2i(tilePosX, tilePosY);
			WorldTile_t* tile = GetWorldTileAt(world, tilePos);
			reci tileRec = NewReci(tilePosX * TILE_SIZE, tilePosY * TILE_SIZE, TILE_SIZE, TILE_SIZE);
			v2i tileFrame = GetTileTypeFrame(tile->type, tilePos);
			if (tileFrame != NewVec2i(-1, -1))
			{
				PdDrawSheetFrame(game->entitiesSheet, tileFrame, tileRec);
				
				ItemStack_t dropStack = GetTileTypeDrop(tile->type);
				if (dropStack.count > 0 && player->targetTilePos == tilePos && !player->isMining)
				{
					PdDrawRecOutline(tileRec, RoundR32i(Oscillate(1, 3, 1000)), true);
				}
			}
		}
	}
	
	reci worldRec = NewReci(0, 0, world->size * TILE_SIZE);
	PdDrawRecOutline(ReciDeflate(worldRec, 1, 1), 2);
	PdDrawRecOutline(ReciDeflate(worldRec, 4, 4), 1);
}
