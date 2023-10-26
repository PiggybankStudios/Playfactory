/*
File:   game.h
Author: Taylor Robbins
Date:   09\08\2023
*/

#ifndef _GAME_H
#define _GAME_H

#define MAIN_FONT_PATH         "/System/Fonts/Asheville-Sans-14-Bold.pft"
#define DEFAULT_WORLD_SIZE     NewVec2i(100, 100)
#define DEFAULT_WORLD_SEED     1
#define GAME_MAX_NUM_PARTICLES 300

#include "game_particles.h"
#include "game_item_ids.h"
#include "game_recipes.h"
#include "game_tile_types.h"
#include "game_inv_types.h"
#include "game_inventory.h"
#include "game_view.h"
#include "game_world.h"
#include "game_player.h"

struct GameState_t
{
	bool initialized;
	
	Font_t mainFont;
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
	
	Inventory_t* openInventory;
	Inventory_t* openScrollInventory;
};

#endif //  _GAME_H
