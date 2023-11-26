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

void SetGeneratedWorldTile(World_t* world, WorldTile_t* tilePntr, u16 itemId)
{
	NotNull2(world, tilePntr);
	tilePntr->generatedId = itemId;
	tilePntr->isSolid =
		IsItemSolid(&gl->itemBook, tilePntr->generatedId) ||
		IsItemSolid(&gl->itemBook, tilePntr->groundId) ||
		IsItemSolid(&gl->itemBook, tilePntr->placedId);
}
void SetGroundWorldTile(World_t* world, WorldTile_t* tilePntr, u16 itemId)
{
	NotNull2(world, tilePntr);
	tilePntr->groundId = itemId;
	tilePntr->isSolid =
		IsItemSolid(&gl->itemBook, tilePntr->generatedId) ||
		IsItemSolid(&gl->itemBook, tilePntr->groundId) ||
		IsItemSolid(&gl->itemBook, tilePntr->placedId);
}
void SetPlacedWorldTile(World_t* world, WorldTile_t* tilePntr, u16 itemId)
{
	NotNull2(world, tilePntr);
	tilePntr->placedId = itemId;
	tilePntr->isSolid =
		IsItemSolid(&gl->itemBook, tilePntr->generatedId) ||
		IsItemSolid(&gl->itemBook, tilePntr->groundId) ||
		IsItemSolid(&gl->itemBook, tilePntr->placedId);
}

void SetGeneratedWorldTileAt(World_t* world, v2i tilePos, u16 itemId)
{
	NotNull(world);
	WorldTile_t* tile = GetWorldTileAt(world, tilePos);
	AssertMsg(tile != nullptr, "Tried to set tile outside the bounds of the world!");
	Assert(tile->pos == tilePos);
	SetGeneratedWorldTile(world, tile, itemId);
}
void SetGroundWorldTileAt(World_t* world, v2i tilePos, u16 itemId)
{
	NotNull(world);
	WorldTile_t* tile = GetWorldTileAt(world, tilePos);
	AssertMsg(tile != nullptr, "Tried to set tile outside the bounds of the world!");
	Assert(tile->pos == tilePos);
	SetGroundWorldTile(world, tile, itemId);
}
void SetPlacedWorldTileAt(World_t* world, v2i tilePos, u16 itemId)
{
	NotNull(world);
	WorldTile_t* tile = GetWorldTileAt(world, tilePos);
	AssertMsg(tile != nullptr, "Tried to set tile outside the bounds of the world!");
	Assert(tile->pos == tilePos);
	SetPlacedWorldTile(world, tile, itemId);
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
	world->pixelSize = Vec2iMultiply(size, Vec2iFill(TILE_SIZE));
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
			tile->pos = tilePos;
			
			u16 generatedItemId = ITEM_ID_NONE;
			if (GetRandR32(&world->genRand) < WORLD_GEN_CANDY_DENSITY)
			{
				ItemDef_t* spawnItemDef = GetRandomItemWithFlag(&gl->itemBook, ItemFlags_Surface|ItemFlags_Tile, ItemFlags_Decor);
				if (spawnItemDef != nullptr) { generatedItemId = spawnItemDef->runtimeId; }
			}
			else if (GetRandR32(&world->genRand) < WORLD_GEN_DECOR_DENSITY)
			{
				ItemDef_t* spawnItemDef = GetRandomItemWithFlag(&gl->itemBook, ItemFlags_Surface|ItemFlags_Tile|ItemFlags_Decor);
				if (spawnItemDef != nullptr) { generatedItemId = spawnItemDef->runtimeId; }
			}
			SetGeneratedWorldTile(world, tile, generatedItemId);
			SetGroundWorldTile(world, tile, ITEM_ID_NONE);
			SetPlacedWorldTile(world, tile, ITEM_ID_NONE);
		}
	}
	
	v2i worldCenter = NewVec2i(world->size.width/2, world->size.height/2);
	v2i storePos = worldCenter - Vec2i_One;
	SetPlacedWorldTileAt(world, storePos + NewVec2i(0, 0), LookupRuntimeId(&gl->itemBook, NewStr("StoreTL")));
	SetPlacedWorldTileAt(world, storePos + NewVec2i(1, 0), LookupRuntimeId(&gl->itemBook, NewStr("StoreTR")));
	SetPlacedWorldTileAt(world, storePos + NewVec2i(0, 1), LookupRuntimeId(&gl->itemBook, NewStr("StoreBL")));
	SetPlacedWorldTileAt(world, storePos + NewVec2i(1, 1), LookupRuntimeId(&gl->itemBook, NewStr("StoreBR")));
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
			
			v2i groundTileFrame = GetItemFrame(&gl->itemBook, tile->groundId);
			if (groundTileFrame != INVALID_FRAME)
			{
				PdDrawSheetFrame(game->entitiesSheet, groundTileFrame, tileRec);
			}
			v2i generatedTileFrame = GetItemFrame(&gl->itemBook, tile->generatedId);
			if (generatedTileFrame != INVALID_FRAME)
			{
				PdDrawSheetFrame(game->entitiesSheet, generatedTileFrame, tileRec);
			}
			v2i placedTileFrame = GetItemFrame(&gl->itemBook, tile->placedId);
			if (placedTileFrame != INVALID_FRAME)
			{
				PdDrawSheetFrame(game->entitiesSheet, placedTileFrame, tileRec);
			}
			
			bool isInventoryOpen = (game->openInventory != nullptr || game->openScrollInventory != nullptr);
			if (player->targetTilePos == tilePos && !player->isMining && !isInventoryOpen)
			{
				ItemStack_t dropStack = GetItemDrop(&gl->itemBook, tile->generatedId);
				InvType_t invType = GetItemInvType(&gl->itemBook, tile->placedId);
				
				if (dropStack.count > 0)
				{
					PdDrawRecOutline(tileRec, RoundR32i(Oscillate(1, 3, 1000)), true);
				}
				else if (invType != InvType_None)
				{
					if (invType == InvType_Store)
					{
						v2i storeOrigin = tilePos;
						
						WorldTile_t* upLeftTile = GetWorldTileAt(world, tilePos + NewVec2i(-1, -1));
						WorldTile_t* leftTile = GetWorldTileAt(world, tilePos + NewVec2i(-1, 0));
						WorldTile_t* upTile = GetWorldTileAt(world, tilePos + NewVec2i(0, -1));
						if (upLeftTile != nullptr && GetItemInvType(&gl->itemBook, upLeftTile->placedId) == invType) { storeOrigin += NewVec2i(-1, -1); }
						else if (leftTile != nullptr && GetItemInvType(&gl->itemBook, leftTile->placedId) == invType) { storeOrigin += NewVec2i(-1, 0); }
						else if (upTile != nullptr && GetItemInvType(&gl->itemBook, upTile->placedId) == invType) { storeOrigin += NewVec2i(0, -1); }
						
						reci storeRec = NewReci(
							Vec2iMultiply(storeOrigin, Vec2iFill(TILE_SIZE)),
							TILE_SIZE*2, TILE_SIZE*2
						);
						PdDrawRecOutline(storeRec, RoundR32i(Oscillate(1, 3, 1000)), true);
					}
					else
					{
						PdDrawRecOutline(tileRec, RoundR32i(Oscillate(1, 3, 1000)), true);
					}
				}
			}
		}
	}
	
	reci worldRec = NewReci(0, 0, world->size * TILE_SIZE);
	PdDrawRecOutline(ReciDeflate(worldRec, 1, 1), 2);
	PdDrawRecOutline(ReciDeflate(worldRec, 4, 4), 1);
}
