/*
File:   game_recipes.h
Author: Taylor Robbins
Date:   10\25\2023
*/

#ifndef _GAME_RECIPES_H
#define _GAME_RECIPES_H

struct Recipe_t
{
	u64 id;
	ItemId_t item1;
	ItemId_t item2;
	ItemId_t output;
};

struct RecipeBook_t
{
	MemArena_t* allocArena;
	u64 nextId;
	VarArray_t recipes; //Recipe_t
};

#endif //  _GAME_RECIPES_H
