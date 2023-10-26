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
	if (!TryLoadAllRecipes(&gl->recipeBook, mainHeap))
	{
		PrintLine_E("Failed to load recipes!");
		DebugAssertMsg(false, "Failed to load recipes!");
		//TODO: Should we do some sort of initialization failure?
	}
	
	#if 1
	PrintLine_D("There %s %llu recipe%s in the game", PluralEx(gl->recipeBook.recipes.length, "is", "are"), gl->recipeBook.recipes.length, Plural(gl->recipeBook.recipes.length, "s"));
	VarArrayLoop(&gl->recipeBook.recipes, rIndex)
	{
		VarArrayLoopGet(Recipe_t, recipe, &gl->recipeBook.recipes, rIndex);
		PrintLine_D("Recipe[%llu]: %s + %s = %s", rIndex, GetItemIdStr(recipe->item1), GetItemIdStr(recipe->item2), GetItemIdStr(recipe->output));
	}
	#endif
	
	gl->titleFont = LoadFont(NewStr("Resources/Fonts/SpaceContract"));
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
