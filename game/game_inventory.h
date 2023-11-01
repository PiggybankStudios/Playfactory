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

#define ITEM_NAME_PRICE_SPACING  20 //px

enum InvSlotType_t
{
	InvSlotType_Default = 0,
	InvSlotType_Button,
	InvSlotType_ToolBtn,
	InvSlotType_Sell,
	InvSlotType_Buy,
	InvSlotType_Decor,
	InvSlotType_GroupChange,
	InvSlotType_NumTypes,
};
const char* GetInvSlotTypeStr(InvSlotType_t enumValue)
{
	switch (enumValue)
	{
		case InvSlotType_Default:     return "Default";
		case InvSlotType_Button:      return "Button";
		case InvSlotType_ToolBtn:     return "ToolBtn";
		case InvSlotType_Sell:        return "Sell";
		case InvSlotType_Buy:         return "Buy";
		case InvSlotType_Decor:       return "Decor";
		case InvSlotType_GroupChange: return "GroupChange";
		default: return "Unknown";
	}
}

enum InvButton_t
{
	InvButton_None = 0,
	InvButton_Combine,
	InvButton_NumButtons,
};
const char* GetInvButtonStr(InvButton_t enumValue)
{
	switch (enumValue)
	{
		case InvButton_None:    return "None";
		case InvButton_Combine: return "Combine";
		default: return "Unknown";
	}
}
const char* GetInvButtonDisplayStr(InvButton_t enumValue)
{
	switch (enumValue)
	{
		case InvButton_Combine: return "Combine";
		default: return "?";
	}
}

struct InvSlot_t
{
	u64 index;
	u64 group;
	InvSlotType_t type;
	reci gridRec;
	ItemStack_t stack;
	InvButton_t button;
	bool isSelected;
	r32 selectedAnimProgress;
	Texture_t* texturePntr;
	u64 newGroup;
	u16 buyItemId;
	reci mainRec; //only size is used when in scrollView mode
};

struct InvGroup_t
{
	reci contentRec;
	reci mainRec;
	i64 prevSelectionIndex;
};

struct Inventory_t
{
	MemArena_t* allocArena;
	InvType_t type;
	
	u64 numSlots;
	InvSlot_t* slots;
	
	bool inScrollView;
	
	i64 selectionIndex;
	v2i selectionGridPos;
	
	r32 scroll;
	r32 scrollMin;
	r32 scrollMax;
	
	bool showCrankHint;
	
	r32 groupSlidePos;
	u64 currentGroup;
	u64 numGroups;
	InvGroup_t groups[MAX_NUM_INV_GROUPS];
};

#endif //  _GAME_INVENTORY_H
