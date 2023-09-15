/*
File:   game_particles.cpp
Author: Taylor Robbins
Date:   09\15\2023
Description: 
	** Particles are temporary graphics that move and change over time and then disappear
*/

void FreeParticleSystem(ParticleSystem_t* system)
{
	NotNull(system);
	if (system->allocArena != nullptr)
	{
		if (system->parts != nullptr)
		{
			FreeMem(system->allocArena, system->parts, sizeof(Particle_t) * system->maxNumParts);
		}
	}
	ClearPointer(system);
}

void InitParticleSystem(ParticleSystem_t* system, MemArena_t* memArena, u64 maxNumParts)
{
	NotNull2(system, memArena);
	Assert(maxNumParts > 0);
	ClearPointer(system);
	system->allocArena = memArena;
	system->numPartsAlive = 0;
	system->maxNumParts = maxNumParts;
	system->parts = AllocArray(system->allocArena, Particle_t, system->maxNumParts);
	NotNull(system->parts);
	MyMemSet(system->parts, 0x00, sizeof(Particle_t) * system->maxNumParts);
}

void UpdateParticleSystem(ParticleSystem_t* system)
{
	NotNull2(system, system->parts);
	for (u64 pIndex = 0; pIndex < system->maxNumParts; pIndex++)
	{
		Particle_t* part = &system->parts[pIndex];
		if (IsFlagSet(part->flags, PartFlag_Alive))
		{
			part->position += part->velocity * TimeScale;
			part->age += ElapsedMs;
			if (part->age >= part->lifeSpan)
			{
				FlagUnset(part->flags, PartFlag_Alive);
				system->numPartsAlive--;
			}
		}
	}
}

void RenderParticleSystem(ParticleSystem_t* system, u8 layers = PartLayer_All)
{
	NotNull2(system, system->parts);
	MemArena_t* scratch = GetScratchArena();
	
	for (u64 pIndex = 0; pIndex < system->maxNumParts; pIndex++)
	{
		Particle_t* part = &system->parts[pIndex];
		if (IsFlagSet(part->flags, PartFlag_Alive) && IsFlagSet(layers, part->layer))
		{
			NotNull(part->sheet);
			
			reci partRec = NewReci(
				Vec2Roundi(part->position) - NewVec2i(part->sheet->frameSize.width / 2, part->sheet->frameSize.height / 2),
				part->sheet->frameSize
			);
			PdDrawSheetFrame(*part->sheet, part->frame, partRec);
			
			if (IsFlagSet(part->flags, PartFlag_ShowNumber))
			{
				NotNull(part->displayNumberFont);
				MyStr_t displayNumStr = PrintInArenaStr(scratch, "%lld", part->displayNumber);
				v2i displayNumSize = MeasureText(part->displayNumberFont->font, displayNumStr);
				v2i displayNumPos = NewVec2i(
					partRec.x + partRec.width - displayNumSize.width,
					partRec.y + partRec.height - displayNumSize.height
				);
				PdBindFont(part->displayNumberFont);
				PdDrawText(displayNumStr, displayNumPos);
			}
		}
	}
	
	FreeScratchArena(scratch);
}

Particle_t* TryAllocParticle(ParticleSystem_t* system)
{
	NotNull2(system, system->parts);
	for (u64 pIndex = 0; pIndex < system->maxNumParts; pIndex++)
	{
		Particle_t* part = &system->parts[pIndex];
		if (!IsFlagSet(part->flags, PartFlag_Alive))
		{
			ClearPointer(part);
			FlagSet(part->flags, PartFlag_Alive);
			system->numPartsAlive++;
			return part;
		}
	}
	return nullptr;
}

Particle_t* TrySpawnParticle(ParticleSystem_t* system, PartLayer_t layer, v2 startPos, v2 velocity, SpriteSheet_t* sheet, v2i frame, r32 lifeSpan)
{
	Particle_t* result = TryAllocParticle(system);
	if (result == nullptr) { return nullptr; }
	result->layer = layer;
	result->position = startPos;
	result->velocity = velocity;
	result->sheet = sheet;
	result->frame = frame;
	result->lifeSpan = lifeSpan;
	return result;
}
void ParticleSetDisplayNumber(Particle_t* part, Font_t* font, i64 displayNumber)
{
	NotNull(font);
	if (part != nullptr)
	{
		FlagSet(part->flags, PartFlag_ShowNumber);
		part->displayNumberFont = font;
		part->displayNumber = displayNumber;
	}
}
