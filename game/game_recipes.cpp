/*
File:   game_recipes.cpp
Author: Taylor Robbins
Date:   10\25\2023
Description: 
	** Holds functions to help us construct and interact with Recipe_t structures
	** As well as fill and query a RecipeBook_t that defines all the recipes that
	** are possible in the game
*/

Recipe_t NewRecipe(ItemId_t item1, ItemId_t item2, ItemId_t output)
{
	Recipe_t result = {};
	result.item1 = item1;
	result.item2 = item2;
	result.output = output;
	return result;
}

bool IsInitialized(RecipeBook_t* book)
{
	NotNull(book);
	return (book->allocArena != nullptr);
}

void FreeRecipeBook(RecipeBook_t* book)
{
	NotNull(book);
	if (book->allocArena != nullptr)
	{
		FreeVarArray(&book->recipes);
	}
	ClearPointer(book);
}

void InitRecipeBook(RecipeBook_t* book, MemArena_t* memArena, u64 numRecipesExpected = 0)
{
	NotNull2(book, memArena);
	ClearPointer(book);
	book->allocArena = memArena;
	book->nextId = 1;
	CreateVarArray(&book->recipes, memArena, sizeof(Recipe_t), numRecipesExpected);
}

Recipe_t* AddRecipeToBook(RecipeBook_t* book, Recipe_t recipe)
{
	NotNull(book);
	Recipe_t* result = VarArrayAdd(&book->recipes, Recipe_t);
	NotNull(result);
	MyMemCopy(result, &recipe, sizeof(Recipe_t));
	result->id = book->nextId;
	book->nextId++;
	// PrintLine_D("Added recipe[%llu]: %s + %s = %s", result->id, GetItemIdStr(result->item1), GetItemIdStr(result->item2), GetItemIdStr(result->output));
	return result;
}

Recipe_t* FindRecipeInBook(RecipeBook_t* book, ItemId_t item1, ItemId_t item2, bool respectOrder = false)
{
	NotNull(book);
	VarArrayLoop(&book->recipes, rIndex)
	{
		VarArrayLoopGet(Recipe_t, recipe, &book->recipes, rIndex);
		if (recipe->item1 == item1 && recipe->item2 == item2) { return recipe; }
		else if (!respectOrder && recipe->item1 == item2 && recipe->item2 == item1) { return recipe; }
	}
	return nullptr;
}
Recipe_t* FindRecipeInBookById(RecipeBook_t* book, u64 recipeId)
{
	NotNull(book);
	VarArrayLoop(&book->recipes, rIndex)
	{
		VarArrayLoopGet(Recipe_t, recipe, &book->recipes, rIndex);
		if (recipe->id == recipeId) { return recipe; }
	}
	return nullptr;
}
