/*
File:   game_player.cpp
Author: Taylor Robbins
Date:   09\10\2023
Description: 
	** Holds functions related to managing the Player_t
*/

void FreePlayer(Player_t* player)
{
	NotNull(player);
	if (player->allocArena != nullptr)
	{
		FreeInventory(&player->inventory);
		FreeInventory(&player->scienceInventory);
	}
	ClearPointer(player);
}

void InitPlayer(Player_t* player, MemArena_t* memArena, v2 startPos)
{
	NotNull(player);
	ClearPointer(player);
	player->allocArena = memArena;
	player->position = startPos;
	player->velocity = Vec2_Zero;
	player->rotation = Dir2Ex_Down;
	InitInventory(&player->inventory, player->allocArena, InvType_PlayerInventory);
	InitInventory(&player->scienceInventory, player->allocArena, InvType_PlayerScience);
}

void UpdatePlayer(Player_t* player, World_t* world)
{
	NotNull4(player, player->allocArena, world, world->allocArena);
	bool isInventoryOpen = (game->openInventory != nullptr || game->openScrollInventory != nullptr);
	
	u8 inputDirFlags = Dir2_None;
	if (!isInventoryOpen && !player->isMining)
	{
		if (BtnDown(Btn_Right)) { HandleBtn(Btn_Right); inputDirFlags |= Dir2_Right; }
		if (BtnDown(Btn_Left))  { HandleBtn(Btn_Left);  inputDirFlags |= Dir2_Left;  }
		if (BtnDown(Btn_Up))    { HandleBtn(Btn_Up);    inputDirFlags |= Dir2_Up;    }
		if (BtnDown(Btn_Down))  { HandleBtn(Btn_Down);  inputDirFlags |= Dir2_Down;  }
	}
	player->inputDir = Dir2ExFromDir2Flags(inputDirFlags);
	if (player->inputDir != Dir2Ex_None)
	{
		v2 inputVec = Vec2Normalize(ToVec2(player->inputDir));
		player->velocity = inputVec * PLAYER_WALK_SPEED;
		player->rotation = player->inputDir;
		player->lookVec = Vec2Normalize(ToVec2(player->rotation));
	}
	else
	{
		if (Vec2Length(player->velocity) > 0.01f)
		{
			player->velocity = player->velocity * (1.0f - (PLAYER_FRICTION/100.0f));
		}
		else { player->velocity = Vec2_Zero; }
	}
	
	player->position += player->velocity;
	player->colRec = PLAYER_COLLISION_REC + player->position;
	
	v2 resolvedPos = ResolveWorldTileCollisions(world, player->colRec);
	v2 resolvedOffset = resolvedPos - player->colRec.topLeft;
	player->position += resolvedOffset;
	player->colRec.topLeft += resolvedOffset;
	
	player->targetPos = player->position + PLAYER_TARGET_OFFSET + (PLAYER_TARGET_DIST * Vec2Normalize(ToVec2(player->rotation)));
	player->targetTilePos = Vec2Floori(player->targetPos / TILE_SIZE);
	player->targetTile = GetWorldTileAt(world, player->targetTilePos);
	
	ItemStack_t targetDrop = {};
	if (player->targetTile != nullptr) { targetDrop = GetTileTypeDrop(player->targetTile->type); }
	
	if (targetDrop.count > 0 && targetDrop.id != ItemId_None && !isInventoryOpen)
	{
		// +==============================+
		// |    Btn_A to Start Mining     |
		// +==============================+
		if (!player->isMining && BtnPressed(Btn_A))
		{
			HandleBtnExtended(Btn_A);
			player->isMining = true;
			reci targetTileRec = NewReci(
				player->targetTilePos.x * TILE_SIZE,
				player->targetTilePos.y * TILE_SIZE,
				TILE_SIZE, TILE_SIZE
			);
			player->position = ToVec2(targetTileRec.topLeft) + NewVec2((r32)(targetTileRec.width/2), (r32)(targetTileRec.height/2)) - (ToVec2(player->rotation) * TILE_SIZE);
			player->velocity = Vec2_Zero;
			player->miningProgress = 0.25f;
		}
		
		if (player->isMining)
		{
			// +==============================+
			// |         Crank Mines          |
			// +==============================+
			if (CrankMoved())
			{
				HandleCrankDelta();
				player->miningProgress += AbsR32(input->crankDelta) * PLAYER_MINING_SPEED;
				if (player->miningProgress > 1.0f)
				{
					player->miningProgress -= 1.0f;
					//TODO: Play a sound effect
					u8 numItemsLeft = TryAddItemStackToInventory(&player->inventory, targetDrop);
					if (numItemsLeft > 0)
					{
						PrintLine_W("Couldn't find space for %u %s in the player inventory!", numItemsLeft, GetItemIdStr(targetDrop.id));
					}
				}
			}
			
			// +==============================+
			// |    Btn_B to Cancel Mining    |
			// +==============================+
			if (BtnPressed(Btn_B))
			{
				HandleBtnExtended(Btn_B);
				player->isMining = false;
			}
		}
	}
	else { player->isMining = false; }
	
}

void RenderPlayer(Player_t* player)
{
	NotNull2(player, player->allocArena);
	
	bool drawPlayer = true;
	SpriteSheet_t* sheet = &game->playerSheet;
	v2i playerFrame = NewVec2i(0, 0);
	v2i playerSize = PLAYER_SIZE;
	v2i playerOffset = Vec2i_Zero;
	switch (player->rotation)
	{
		case Dir2Ex_Right:       playerFrame = NewVec2i(0, 6); break;
		case Dir2Ex_Left:        playerFrame = NewVec2i(0, 2); break;
		case Dir2Ex_Up:          playerFrame = NewVec2i(0, 4); break;
		case Dir2Ex_Down:        playerFrame = NewVec2i(0, 0); break;
		case Dir2Ex_TopLeft:     playerFrame = NewVec2i(0, 3); break;
		case Dir2Ex_TopRight:    playerFrame = NewVec2i(0, 5); break;
		case Dir2Ex_BottomRight: playerFrame = NewVec2i(0, 7); break;
		case Dir2Ex_BottomLeft:  playerFrame = NewVec2i(0, 1); break;
	}
	if (player->isMining)
	{
		sheet = &game->playerMiningSheet;
		playerFrame.x = FloorR32i(sheet->numFramesX * player->miningProgress) % sheet->numFramesX;
		playerFrame.y = ((player->rotation == Dir2Ex_Right) ? 1 : 0);
		playerSize = sheet->frameSize * 2;
		playerOffset = NewVec2i(0, -13);
	}
	else if (player->inputDir != Dir2Ex_None)
	{
		playerFrame.x += (i32)AnimateU64(0, 4, 1000);
	}
	
	reci playerRec = NewReci(
		playerOffset.x + RoundR32i(player->position.x - (r32)playerSize.width / 2.0f),
		playerOffset.y + RoundR32i(player->position.y - (r32)playerSize.height / 2.0f),
		playerSize
	);
	PdDrawSheetFrame(*sheet, playerFrame, playerRec);
	
	if (pig->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		
		PdDrawRec(NewReci(Vec2Roundi(player->colRec.topLeft), Vec2Roundi(player->colRec.size)), kColorBlack);
		
		PdDrawRec(NewReci(Vec2Roundi(player->targetPos), 2, 2), kColorBlack);
		PdDrawRecOutline(NewReci(player->targetTilePos.x * TILE_SIZE, player->targetTilePos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE), 1);
		
		PdSetDrawMode(oldDrawMode);
	}
}
