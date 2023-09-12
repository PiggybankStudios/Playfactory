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
		//TODO: Implement me!
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
}

void UpdatePlayer(Player_t* player, World_t* world)
{
	NotNull4(player, player->allocArena, world, world->allocArena);
	
	u8 inputDirFlags = Dir2_None;
	if (BtnDown(Btn_Right)) { HandleBtn(Btn_Right); inputDirFlags |= Dir2_Right; }
	if (BtnDown(Btn_Left))  { HandleBtn(Btn_Left);  inputDirFlags |= Dir2_Left;  }
	if (BtnDown(Btn_Up))    { HandleBtn(Btn_Up);    inputDirFlags |= Dir2_Up;    }
	if (BtnDown(Btn_Down))  { HandleBtn(Btn_Down);  inputDirFlags |= Dir2_Down;  }
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
}

void RenderPlayer(Player_t* player)
{
	NotNull2(player, player->allocArena);
	
	bool drawPlayer = true;
	v2i playerFrame = NewVec2i(0, 0);
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
	if (player->inputDir != Dir2Ex_None)
	{
		playerFrame.x += (i32)AnimateU64(0, 4, 1000);
	}
	
	reci playerRec;
	playerRec.size = PLAYER_SIZE;
	playerRec.x = RoundR32i(player->position.x - (r32)playerRec.width / 2.0f);
	playerRec.y = RoundR32i(player->position.y - (r32)playerRec.height / 2.0f);
	PdDrawSheetFrame(game->playerSheet, playerFrame, playerRec);
	
	if (pig->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		PdDrawRec(NewReci(Vec2Roundi(player->colRec.topLeft), Vec2Roundi(player->colRec.size)), kColorBlack);
		PdSetDrawMode(oldDrawMode);
	}
}
