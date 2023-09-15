/*
File:   game_particles.h
Author: Taylor Robbins
Date:   09\15\2023
*/

#ifndef _GAME_PARTICLES_H
#define _GAME_PARTICLES_H

enum PartFlag_t
{
	PartFlag_None       = 0x00,
	
	PartFlag_Alive      = 0x01,
	PartFlag_ShowNumber = 0x02,
	
	PartFlag_All        = 0x03,
	PartFlag_NumFlags   = 2,
};
const char* GetPartFlagStr(PartFlag_t enumValue)
{
	switch (enumValue)
	{
		case PartFlag_None:       return "None";
		case PartFlag_Alive:      return "Alive";
		case PartFlag_ShowNumber: return "ShowNumber";
		case PartFlag_All:        return "All";
		default:                  return "Unknown";
	}
}

enum PartLayer_t
{
	PartLayer_None      = 0x00,
	
	PartLayer_Low       = 0x01,
	PartLayer_High      = 0x02,
	PartLayer_LowUi     = 0x04,
	PartLayer_HighUi    = 0x08,
	
	PartLayer_All       = 0x0F,
	PartLayer_NumLayers = 4,
};
const char* GetPartLayerStr(PartLayer_t enumValue)
{
	switch (enumValue)
	{
		case PartLayer_None:   return "None";
		case PartLayer_Low:    return "Low";
		case PartLayer_High:   return "High";
		case PartLayer_LowUi:  return "LowUi";
		case PartLayer_HighUi: return "HighUi";
		case PartLayer_All:    return "All";
		default: return "Unknown";
	}
}

struct Particle_t
{
	u8 flags; //PartFlag_t
	PartLayer_t layer;
	v2 velocity;
	SpriteSheet_t* sheet;
	v2i frame;
	r32 lifeSpan; //ms
	Font_t* displayNumberFont;
	i64 displayNumber;
	
	r32 age; //ms
	v2 position;
};

struct ParticleSystem_t
{
	MemArena_t* allocArena;
	u64 numPartsAlive;
	u64 maxNumParts;
	Particle_t* parts;
};

#endif //  _GAME_PARTICLES_H
