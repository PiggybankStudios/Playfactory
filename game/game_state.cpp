/*
File:   game.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Holds the game AppState
*/

GameState_t* game = nullptr;

#include "game_inv_types.cpp"
#include "game_inventory.cpp"
#include "game_view.cpp"
#include "game_world.cpp"
#include "game_player.cpp"

// +--------------------------------------------------------------+
// |                            Start                             |
// +--------------------------------------------------------------+
void StartAppState_Game(bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	if (initialize)
	{
		game->mainFont = LoadFont(NewStr(MAIN_FONT_PATH));
		Assert(game->mainFont.isValid);
		
		game->pigTexture = LoadTexture(NewStr("Resources/Sprites/pig64"));
		Assert(game->pigTexture.isValid);
		
		game->testSound = LoadSound(NewStr("Resources/Sounds/test"));
		Assert(game->testSound.isValid);
		
		game->kennySheet = LoadSpriteSheet(NewStr("Resources/Sheets/kenny"), 49);
		Assert(game->kennySheet.isValid);
		game->playerSheet = LoadSpriteSheet(NewStr("Resources/Sheets/player"), 6);
		Assert(game->playerSheet.isValid);
		game->entitiesSheet = LoadSpriteSheet(NewStr("Resources/Sheets/entities"), 8);
		Assert(game->entitiesSheet.isValid);
		
		InitWorld(&game->world, mainHeap, DEFAULT_WORLD_SIZE, DEFAULT_WORLD_SEED);
		InitPlayer(&game->player, mainHeap, ToVec2(game->world.pixelSize) / 2.0f);
		InitGameView(&game->view, game->player.position, game->world.size * TILE_SIZE);
		
		game->initialized = true;
	}
}

// +--------------------------------------------------------------+
// |                             Stop                             |
// +--------------------------------------------------------------+
void StopAppState_Game(bool deinitialize, AppState_t nextState)
{
	if (deinitialize)
	{
		FreePlayer(&game->player);
		FreeWorld(&game->world);
		ClearPointer(game);
	}
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void UpdateAppState_Game()
{
	MemArena_t* scratch = GetScratchArena();
	
	// +==============================+
	// |            Btn_B             |
	// +==============================+
	if (BtnReleased(Btn_B))
	{
		HandleBtn(Btn_B);
		if (game->openInventory == nullptr && game->openScrollInventory == nullptr)
		{
			game->openInventory = &game->player.scienceInventory; OnOpenInventory(game->openInventory, false);
			game->openScrollInventory = &game->player.inventory; OnOpenInventory(game->openScrollInventory, true);
		}
		else
		{
			game->openInventory = nullptr;
			game->openScrollInventory = nullptr;
		}
	}
	
	// +==============================+
	// |   Update Open Inventories    |
	// +==============================+
	if (game->openInventory != nullptr) { UpdateInventory(game->openInventory); }
	if (game->openScrollInventory != nullptr) { UpdateInventory(game->openScrollInventory); }
	
	UpdatePlayer(&game->player, &game->world);
	UpdateGameView(&game->view, game->player.position, ToVec2(game->player.inputDir), game->world.size * TILE_SIZE);
	UpdateWorld(&game->world);
	
	// +==============================+
	// |            Btn_A             |
	// +==============================+
	if (BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		//TODO: Implement me!
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Render                            |
// +--------------------------------------------------------------+
void RenderAppState_Game(bool isOnTop)
{
	MemArena_t* scratch = GetScratchArena();
	
	pd->graphics->clear(kColorWhite);
	PdSetDrawMode(kDrawModeCopy);
	
	// +==============================+
	// |    Render Items in World     |
	// +==============================+
	{
		PdSetRenderOffset(-game->view.worldReci.topLeft);
		
		RenderWorld(&game->world);
		RenderPlayer(&game->player);
		
		PdSetRenderOffset(Vec2i_Zero);
	}
	
	// +==============================+
	// |      Render Inventories      |
	// +==============================+
	if (game->openInventory != nullptr) { RenderInventoryUi(game->openInventory); }
	if (game->openScrollInventory != nullptr) { RenderInventoryUi(game->openScrollInventory); }
	
	// +==============================+
	// |         Debug Render         |
	// +==============================+
	if (pig->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		
		v2i textPos = NewVec2i(1, 1);
		if (pig->perfGraph.enabled) { textPos.y += pig->perfGraph.mainRec.y + pig->perfGraph.mainRec.height + 1; }
		PdBindFont(&pig->debugFont);
		i32 stepY = pig->debugFont.lineHeight + 1;
		
		PdDrawTextPrint(textPos, "ProgramTime: %u (%u)", ProgramTime, input->realProgramTime);
		textPos.y += stepY;
		
		u64 numSoundInstances = 0;
		for (u64 iIndex = 0; iIndex < MAX_SOUND_INSTANCES; iIndex++) { if (pig->soundPool.instances[iIndex].isPlaying) { numSoundInstances++; } }
		PdDrawTextPrint(textPos, "%llu sound instance%s", numSoundInstances, Plural(numSoundInstances, "s"));
		textPos.y += stepY;
		
		PdDrawTextPrint(textPos, "main: %llu chars Height:%d %s", game->mainFont.numChars, game->mainFont.lineHeight, GetFontCapsStr(game->mainFont));
		textPos.y += stepY;
		
		#if 0
		for (u8 rangeIndex = 0; rangeIndex < FontRange_NumRanges; rangeIndex++)
		{
			FontRange_t range = FontRangeByIndex(rangeIndex);
			if (FontHasRange(game->mainFont, range, true))
			{
				u8 numCharsInRange = GetNumCharsInFontRange(range);
				for (u8 charIndex = 0; charIndex < numCharsInRange; charIndex++)
				{
					u32 codepoint = GetFontRangeChar(range, charIndex);
					LCDFontPage* fontPage = pd->graphics->getFontPage(game->mainFont.font, codepoint);
					if (fontPage != nullptr)
					{
						LCDBitmap* glyphBitmap = nullptr;
						i32 glyphAdvance = 0;
						LCDFontGlyph* fontGlyph = pd->graphics->getPageGlyph(fontPage, codepoint, &glyphBitmap, &glyphAdvance);
						if (fontGlyph != nullptr && glyphBitmap != nullptr && glyphAdvance > 0)
						{
							v2i glyphBitmapSize = GetBitmapSize(glyphBitmap);
							if (textPos.x + glyphAdvance > ScreenSize.width)
							{
								textPos.x = 1;
								textPos.y += stepY;
							}
							PdDrawTexturedRec(glyphBitmap, glyphBitmapSize, NewReci(textPos, glyphBitmapSize));
							textPos.x += glyphAdvance;
						}
					}
				}
				textPos.x = 1;
				textPos.y += stepY;
			}
		}
		#endif
		
		PdSetDrawMode(oldDrawMode);
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                           Register                           |
// +--------------------------------------------------------------+
void RegisterAppState_Game()
{
	game = (GameState_t*)RegisterAppState(
		AppState_Game,
		sizeof(GameState_t),
		StartAppState_Game,
		StopAppState_Game,
		UpdateAppState_Game,
		RenderAppState_Game
	);
}
