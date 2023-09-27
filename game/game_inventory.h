/*
File:   game_inventory.h
Author: Taylor Robbins
Date:   09\11\2023
*/

#ifndef _GAME_INVENTORY_H
#define _GAME_INVENTORY_H

#define INV_SLOT_SIZE         42 //px
#define INV_SLOT_LARGE_SIZE   68 //px
#define INV_SLOT_MARGIN       2 //px
#define INV_TOOL_BTN_SIZE     24 //px
#define INV_ITEM_SIZE         40 //px
#define INV_PADDING           4 //px
#define INV_BORDER_THICKNESS  2 //px

#define STACK_MAX  16

#define INV_SLOT_GROW_BIG_ANIM_TIME  200 //ms
#define INV_SCROLL_CRANK_HINT_INACTIVE_DISPLAY_TIME   2000 //ms

enum InvSlotType_t
{
	InvSlotType_Default = 0,
	InvSlotType_Button,
	InvSlotType_ToolBtn,
	InvSlotType_NumTypes,
};
const char* GetInvSlotTypeStr(InvSlotType_t enumValue)
{
	switch (enumValue)
	{
		case InvSlotType_Default: return "Default";
		case InvSlotType_Button:  return "Button";
		case InvSlotType_ToolBtn: return "ToolBtn";
		default: return "Unknown";
	}
}

struct InvSlot_t
{
	u64 index;
	u64 groupId;
	InvSlotType_t type;
	v2i gridPos;
	ItemStack_t stack;
	bool isSelected;
	r32 selectedAnimProgress;
	reci mainRec; //only size is used when in scrollView mode
};

struct Inventory_t
{
	MemArena_t* allocArena;
	
	u64 numSlots;
	InvSlot_t* slots;
	
	bool inScrollView;
	
	i64 selectionIndex;
	
	r32 scroll;
	r32 scrollMin;
	r32 scrollMax;
	
	bool showCrankHint;
	
	reci contentRec;
	reci mainRec;
};

#endif //  _GAME_INVENTORY_H
