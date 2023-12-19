/*
File:   game_main.cpp
Author: Taylor Robbins
Date:   09\10\2023
Description: 
	** Includes every other app state .cpp file and also contains functions like InitGame
	** which are called by pig_main.cpp. The app state .h files are included by game_main.h
*/

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
GameGlobals_t* gl = nullptr;

// +--------------------------------------------------------------+
// |                         Source Files                         |
// +--------------------------------------------------------------+
#include "main_menu.cpp"
#include "game_state.cpp"

// +--------------------------------------------------------------+
// |                        Main Functions                        |
// +--------------------------------------------------------------+
AppState_t InitGame()
{
	WriteLine_O("+==============================+");
	PrintLine_O("|     %s v%u.%u(%0u)     |", PROJECT_NAME, GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_BUILD);
	WriteLine_O("+==============================+");
	
	RegisterAppState_MainMenu();
	RegisterAppState_Game();
	
	gl = AllocStruct(fixedHeap, GameGlobals_t);
	ClearPointer(gl);
	gl->initialized = true;
	
	// +==================================+
	// | Load Item Definitions at Startup |
	// +==================================+
	//TODO: Turn me back on!
	#if 1
	if (TryLoadAllItemDefs(&gl->itemBook, mainHeap))
	{
		#if 1
		PrintLine_D("There %s %llu item%s in the game", PluralEx(gl->itemBook.items.length, "is", "are"), gl->itemBook.items.length, Plural(gl->itemBook.items.length, "s"));
		VarArrayLoop(&gl->itemBook.items, iIndex)
		{
			VarArrayLoopGet(ItemDef_t, item, &gl->itemBook.items, iIndex);
			PrintLine_D("ItemDef[%llu]: %u %.*s %04X", iIndex, item->runtimeId, StrPrint(item->idStr), item->flags);
		}
		#endif
	}
	else
	{
		PrintLine_E("Failed to load items!");
		DebugAssertMsg(false, "Failed to load items!");
		//TODO: Should we do some sort of initialization failure?
	}
	#endif
	
	// +====================================+
	// | Load Recipe Definitions at Startup |
	// +====================================+
	//TODO: Turn me back on!
	#if 1
	if (TryLoadAllRecipes(&gl->itemBook, &gl->recipeBook, mainHeap))
	{
		#if 1
		PrintLine_D("There %s %llu recipe%s in the game", PluralEx(gl->recipeBook.recipes.length, "is", "are"), gl->recipeBook.recipes.length, Plural(gl->recipeBook.recipes.length, "s"));
		VarArrayLoop(&gl->recipeBook.recipes, rIndex)
		{
			VarArrayLoopGet(Recipe_t, recipe, &gl->recipeBook.recipes, rIndex);
			PrintLine_D("Recipe[%llu]: %s + %s = %s", rIndex, GetItemDisplayNameNt(&gl->itemBook, recipe->itemId1), GetItemDisplayNameNt(&gl->itemBook, recipe->itemId2), GetItemDisplayNameNt(&gl->itemBook, recipe->outputId));
		}
		#endif
	}
	else
	{
		PrintLine_E("Failed to load recipes!");
		DebugAssertMsg(false, "Failed to load recipes!");
		//TODO: Should we do some sort of initialization failure?
	}
	#endif
	
	gl->titleFont = LoadFont(NewStr(TITLE_FONT_PATH));
	Assert(gl->titleFont.isValid);
	
	return FIRST_APP_STATE;
}

void PreUpdateGame()
{
	
}
void PostUpdateGame()
{
	
}

void PreRenderGame()
{
	
}
void PostRenderGame()
{
	
}
