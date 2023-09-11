/*
File:   main_menu.h
Author: Taylor Robbins
Date:   09\10\2023
*/

#ifndef _MAIN_MENU_H
#define _MAIN_MENU_H

#define MMENU_BTN_SPACING            10 //px
#define MMENU_BTN_HORIZONTAL_PADDING 15 //px
#define MMENU_BTN_VERTICAL_PADDING   8 //px

enum MMenuAction_t
{
	MMenuAction_None = 0,
	MMenuAction_Play,
	MMenuAction_Settings,
	MMenuAction_Exit,
	MMenuAction_NumActions,
};
const char* GetMMenuActionStr(MMenuAction_t enumValue)
{
	switch (enumValue)
	{
		case MMenuAction_None:     return "None";
		case MMenuAction_Play:     return "Play";
		case MMenuAction_Settings: return "Settings";
		case MMenuAction_Exit:     return "Exit";
		default: return "Unknown";
	}
}

struct MMenuBtn_t
{
	MMenuAction_t action;
	MyStr_t text;
	MyStr_t displayText;
	
	reci mainRec;
	v2i displayTextSize;
	v2i displayTextPos; //relative
};

struct MainMenuState_t
{
	bool initialized;
	
	SpriteSheet_t handSheet;
	Font_t buttonFont;
	
	i64 selectionIndex;
	VarArray_t buttons; //MMenuBtn_t
	
	reci titleRec;
	reci buttonListRec;
};

#endif //  _MAIN_MENU_H
