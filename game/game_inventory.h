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
	InvSlotType_Hand,
	InvSlotType_Research,
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
		case InvSlotType_Hand:        return "Hand";
		case InvSlotType_Research:    return "Research";
		default: return "Unknown";
	}
}

enum InvSlotFlags_t
{
	InvSlotFlags_None = 0x00,
	InvSlotFlags_Separate = 0x01,
	InvSlotFlags_AutoDump = 0x02,
	// InvSlotFlags_Unused = 0x04,
	// InvSlotFlags_Unused = 0x08,
	// InvSlotFlags_Unused = 0x10,
	// InvSlotFlags_Unused = 0x20,
	// InvSlotFlags_Unused = 0x40,
	// InvSlotFlags_Unused = 0x80,
	InvSlotFlags_NumFlags = 8,
};
const char* GetInvSlotFlagsStr(InvSlotFlags_t enumValue)
{
	switch (enumValue)
	{
		case InvSlotFlags_None:     return "None";
		case InvSlotFlags_Separate: return "Separate";
		case InvSlotFlags_AutoDump: return "AutoDump";
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
	u8 flags;
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
	u8 flags;
	
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

// +--------------------------------------------------------------+
// |                        Enum Questions                        |
// +--------------------------------------------------------------+
bool IsInvSlotTypeSelectable(InvSlotType_t slotType)
{
	switch (slotType)
	{
		case InvSlotType_Decor:  return false;
		default: return true;
	}
}
bool DoesInvSlotTypeHoldItems(InvSlotType_t slotType)
{
	switch (slotType)
	{
		case InvSlotType_Default:  return true;
		case InvSlotType_Hand:     return true;
		case InvSlotType_Research: return true;
		default: return false;
	}
}
bool IsInvSlotTypeVisible(InvSlotType_t slotType)
{
	switch (slotType)
	{
		case InvSlotType_GroupChange: return false;
		default: return true;
	}
}
bool DoesInvSlotTypeRenderOutline(InvSlotType_t slotType)
{
	switch (slotType)
	{
		case InvSlotType_Decor:       return false;
		case InvSlotType_GroupChange: return false;
		default: return true;
	}
}

#endif //  _GAME_INVENTORY_H
