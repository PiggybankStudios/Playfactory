/*
File:   game.h
Author: Taylor Robbins
Date:   09\08\2023
*/

#ifndef _GAME_H
#define _GAME_H

#include "game_defines.h"
#include "game_particles.h"
#include "game_inv_types.h"
#include "game_item_defs.h"
#include "game_recipes.h"
#include "game_inventory.h"
#include "game_view.h"
#include "game_world.h"
#include "game_player.h"

struct GameState_t
{
	bool initialized;
	
	Font_t mainFont;
	Font_t buttonFont;
	Font_t itemCountFont;
	Texture_t pigTexture;
	Sound_t testSound;
	SpriteSheet_t kennySheet;
	SpriteSheet_t playerSheet;
	SpriteSheet_t playerMiningSheet;
	SpriteSheet_t entitiesSheet;
	
	GameView_t view;
	Player_t player;
	World_t world;
	ParticleSystem_t parts;
	Inventory_t storeInventory;
	
	Inventory_t* openInventory;
	Inventory_t* openScrollInventory;
};

#endif //  _GAME_H
