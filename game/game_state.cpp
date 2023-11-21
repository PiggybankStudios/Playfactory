/*
File:   game.cpp
Author: Taylor Robbins
Date:   09\08\2023
Description: 
	** Holds the game AppState
*/

GameState_t* game = nullptr;

#include "game_inv_types.cpp"
#include "game_item_defs.cpp"
#include "game_item_defs_serialization.cpp"
#include "game_recipes.cpp"
#include "game_recipes_serialization.cpp"
#include "game_particles.cpp"
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
		game->buttonFont = LoadFont(NewStr(BUTTON_FONT_PATH));
		Assert(game->buttonFont.isValid);
		game->itemCountFont = LoadFont(NewStr(ITEM_COUNT_FONT_PATH));
		Assert(game->itemCountFont.isValid);
		game->beanCountFont = LoadFont(NewStr(BEAN_COUNT_FONT_PATH));
		Assert(game->beanCountFont.isValid);
		game->itemNameFont = LoadFont(NewStr(ITEM_NAME_FONT_PATH));
		Assert(game->itemNameFont.isValid);
		
		game->pigTexture = LoadTexture(NewStr("Resources/Sprites/pig64"));
		Assert(game->pigTexture.isValid);
		game->beanTexture = LoadTexture(NewStr("Resources/Sprites/bean"));
		Assert(game->beanTexture.isValid);
		game->beanMarketSignTexture = LoadTexture(NewStr("Resources/Sprites/bean_market_sign"));
		Assert(game->beanMarketSignTexture.isValid);
		game->scienceIconTexture = LoadTexture(NewStr("Resources/Sprites/science_icon"));
		Assert(game->scienceIconTexture.isValid);
		
		game->testSound = LoadSound(NewStr("Resources/Sounds/test"));
		Assert(game->testSound.isValid);
		
		game->kennySheet = LoadSpriteSheet(NewStr("Resources/Sheets/kenny"), 49);
		Assert(game->kennySheet.isValid);
		game->playerSheet = LoadSpriteSheet(NewStr("Resources/Sheets/player"), 6);
		Assert(game->playerSheet.isValid);
		game->playerMiningSheet = LoadSpriteSheet(NewStr("Resources/Sheets/player_mining"), 8);
		Assert(game->playerMiningSheet.isValid);
		game->entitiesSheet = LoadSpriteSheet(NewStr("Resources/Sheets/entities"), 8);
		Assert(game->entitiesSheet.isValid);
		game->uiIconsSheet = LoadSpriteSheet(NewStr("Resources/Sheets/ui_icons"), 8);
		Assert(game->uiIconsSheet.isValid);
		
		InitParticleSystem(&game->parts, mainHeap, GAME_MAX_NUM_PARTICLES);
		InitWorld(&game->world, mainHeap, DEFAULT_WORLD_SIZE, DEFAULT_WORLD_SEED);
		InitPlayer(&game->player, mainHeap, ToVec2(game->world.pixelSize) / 2.0f);
		InitGameView(&game->view, game->player.position, game->world.size * TILE_SIZE);
		InitInventory(&game->storeInventory, mainHeap, InvType_Store);
		
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
		FreeParticleSystem(&game->parts);
		FreeInventory(&game->storeInventory);
		ClearPointer(game);
	}
}

// +--------------------------------------------------------------+
// |                            Layout                            |
// +--------------------------------------------------------------+
void GameUiLayout()
{
	game->topBarRec.topLeft = Vec2i_Zero;
	game->topBarRec.height = game->beanCountFont.lineHeight + 2;
	game->topBarRec.width = ScreenSize.width;
	
	game->beanRec.size = game->beanTexture.size;
	game->beanRec.x = 2;
	game->beanRec.y = game->topBarRec.y + game->topBarRec.height/2 - game->beanRec.height/2;
	
	game->beanCountTextPos.x = game->beanRec.x + game->beanRec.width + 2;
	game->beanCountTextPos.y = game->topBarRec.y + game->topBarRec.height/2 - game->beanCountFont.lineHeight/2;
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void UpdateAppState_Game()
{
	MemArena_t* scratch = GetScratchArena();
	
	UpdateParticleSystem(&game->parts);
	
	// +==============================+
	// | Btn_B Opens/Closes Inventory |
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
			if (game->openInventory != nullptr) { OnCloseInventory(game->openInventory, game->openScrollInventory); }
			if (game->openScrollInventory != nullptr) { OnCloseInventory(game->openScrollInventory, game->openInventory); }
			game->openInventory = nullptr;
			game->openScrollInventory = nullptr;
		}
	}
	
	// +==============================+
	// |   Update Open Inventories    |
	// +==============================+
	if (game->openInventory != nullptr) { UpdateInventory(game->openInventory, game->openScrollInventory); }
	if (game->openScrollInventory != nullptr) { UpdateInventory(game->openScrollInventory, game->openInventory); }
	
	UpdatePlayer(&game->player, &game->world);
	UpdateGameView(&game->view, game->player.position, ToVec2(game->player.inputDir), game->world.size * TILE_SIZE);
	UpdateWorld(&game->world);
	
	// +==============================+
	// |   Debug Crank Gives Beans    |
	// +==============================+
	if (pig->debugEnabled && CrankMovedRaw() && game->openInventory == nullptr && game->openScrollInventory == nullptr && !game->player.isMining)
	{
		game->player.beanCount += RoundR32i(input->crankDelta * 1);
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Render                            |
// +--------------------------------------------------------------+
void RenderAppState_Game(bool isOnTop)
{
	MemArena_t* scratch = GetScratchArena();
	GameUiLayout();
	
	pd->graphics->clear(kColorWhite);
	PdSetDrawMode(kDrawModeCopy);
	
	// +==============================+
	// |    Render Items in World     |
	// +==============================+
	{
		PdSetRenderOffset(-game->view.worldReci.topLeft);
		
		RenderParticleSystem(&game->parts, PartLayer_Low);
		RenderWorld(&game->world, &game->player);
		RenderPlayer(&game->player);
		RenderParticleSystem(&game->parts, PartLayer_High);
		
		PdSetRenderOffset(Vec2i_Zero);
	}
	
	RenderParticleSystem(&game->parts, PartLayer_LowUi);
	
	// +==============================+
	// |        Render Topbar         |
	// +==============================+
	// PdDrawRec(game->topBarRec, kColorBlack);
	PdDrawTexturedRec(game->beanTexture, game->beanRec);
	PdBindFont(&game->beanCountFont);
	PdDrawTextPrint(game->beanCountTextPos, "%s", FormatNumberWithCommasNt(game->player.beanCount));
	
	// +==============================+
	// | Render Crank Hint for Mining |
	// +==============================+
	if (game->player.isMining)
	{
		reci crankHintRec = PlaydateCrankHintBubble(false, true, 1.0f, Vec2i_Zero);
		v2i crankTargetPos = NewVec2i(ScreenSize.width, ScreenSize.height - 8 - crankHintRec.height/2);
		PlaydateCrankHintBubble(true, true, 1.0f, crankTargetPos);
	}
	
	// +==============================+
	// |      Render Inventories      |
	// +==============================+
	if (game->openInventory != nullptr) { RenderInventoryUi(game->openInventory, game->openScrollInventory); }
	if (game->openScrollInventory != nullptr) { RenderInventoryUi(game->openScrollInventory, game->openInventory); }
	
	RenderParticleSystem(&game->parts, PartLayer_HighUi);
	
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
		
		PdDrawTextPrint(textPos, "Target: (%g, %g) (%d, %d)", game->player.targetPos.x, game->player.targetPos.y, game->player.targetTilePos.x, game->player.targetTilePos.y);
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
