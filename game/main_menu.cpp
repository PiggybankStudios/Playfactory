/*
File:   main_menu.cpp
Author: Taylor Robbins
Date:   09\10\2023
Description: 
	** Holds the code for the Main Menu AppState
*/

MainMenuState_t* mmenu = nullptr;

// +--------------------------------------------------------------+
// |                           Helpers                            |
// +--------------------------------------------------------------+
void ClearButtonsMainMenu()
{
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		FreeString(mainHeap, &button->displayText);
	}
	VarArrayClear(&mmenu->buttons);
}
MMenuBtn_t* AddButtonMainMenu(MMenuAction_t action, MyStr_t displayText)
{
	MMenuBtn_t* result = VarArrayAdd(&mmenu->buttons, MMenuBtn_t);
	NotNull(result);
	ClearPointer(result);
	result->action = action;
	result->displayText = AllocString(mainHeap, &displayText);
	NotNullStr(&result->displayText);
	result->displayTextSize = MeasureText(mmenu->buttonFont.font, result->displayText);
	result->mainRec.size = result->displayTextSize + NewVec2i(2*MMENU_BTN_HORIZONTAL_PADDING, 2*MMENU_BTN_VERTICAL_PADDING);
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		if (button->mainRec.width > result->mainRec.width) { result->mainRec.width = button->mainRec.width; }
		else if (button->mainRec.width < result->mainRec.width) { button->mainRec.width = result->mainRec.width; }
		button->displayTextPos = NewVec2i(button->mainRec.width/2 - button->displayTextSize.width/2, button->mainRec.height/2 - button->displayTextSize.height/2);
	}
	return result;
}
void PlaceButtonsListMainMenu()
{
	v2i totalListSize = Vec2i_Zero;
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		if (bIndex > 0) { totalListSize.height += MMENU_BTN_SPACING; }
		button->mainRec.y = totalListSize.y;
		totalListSize.width = MaxI32(totalListSize.width, button->mainRec.width);
		totalListSize.height += button->mainRec.height;
	}
	mmenu->buttonListRec.size = totalListSize;
	mmenu->buttonListRec.x = ScreenSize.width/2 - mmenu->buttonListRec.width/2;
	mmenu->buttonListRec.y = (ScreenSize.height + (mmenu->titleRec.y + mmenu->titleRec.height)) / 2 - (mmenu->buttonListRec.height/2);
}

// +--------------------------------------------------------------+
// |                            Start                             |
// +--------------------------------------------------------------+
void StartAppState_MainMenu(bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	if (initialize)
	{
		mmenu->handSheet = LoadSpriteSheet(NewStr("Resources/Sheets/hand"), 1);
		Assert(mmenu->handSheet.isValid);
		
		mmenu->buttonFont = LoadFont(NewStr("Resources/Fonts/blocky"));
		Assert(mmenu->buttonFont.isValid);
		
		mmenu->titleRec.size = MeasureText(gl->titleFont.font, NewStr(PROJECT_NAME));
		mmenu->titleRec.topLeft = NewVec2i(ScreenSize.width/2 - mmenu->titleRec.size.width/2, ScreenSize.height/4 - mmenu->titleRec.size.height/2);
		
		CreateVarArray(&mmenu->buttons, mainHeap, sizeof(MMenuBtn_t));
		
		ClearButtonsMainMenu();
		AddButtonMainMenu(MMenuAction_Play,     NewStr("PLAY"));
		AddButtonMainMenu(MMenuAction_Settings, NewStr("SETTINGS"));
		AddButtonMainMenu(MMenuAction_Exit,     NewStr("EXIT"));
		PlaceButtonsListMainMenu();
		
		mmenu->selectionIndex = 0;
		
		mmenu->initialized = true;
	}
}

// +--------------------------------------------------------------+
// |                             Stop                             |
// +--------------------------------------------------------------+
void StopAppState_MainMenu(bool deinitialize, AppState_t nextState)
{
	if (deinitialize)
	{
		//TODO: Free mmenu->handSheet
		ClearButtonsMainMenu();
		FreeVarArray(&mmenu->buttons);
		ClearPointer(mmenu);
	}
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void UpdateAppState_MainMenu()
{
	MemArena_t* scratch = GetScratchArena();
	
	if (BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		if (mmenu->selectionIndex >= 0 && (u64)mmenu->selectionIndex < mmenu->buttons.length)
		{
			MMenuBtn_t* selectedBtn = VarArrayGetHard(&mmenu->buttons, (u64)mmenu->selectionIndex, MMenuBtn_t);
			switch (selectedBtn->action)
			{
				// +==============================+
				// |      Play Btn Selected       |
				// +==============================+
				case MMenuAction_Play:
				{
					PushAppState(AppState_Game);
				} break;
				
				// +==============================+
				// |     Unknown Btn Selected     |
				// +==============================+
				default:
				{
					PrintLine_E("The %s button doesn't work yet!", GetMMenuActionStr(selectedBtn->action));
				} break;
			}
		}
		
	}
	
	// +==============================+
	// |        Handle Up/Down        |
	// +==============================+
	if (BtnPressed(Btn_Up))
	{
		HandleBtnExtended(Btn_Up);
		if (mmenu->buttons.length > 0)
		{
			if (mmenu->selectionIndex < 0)
			{
				mmenu->selectionIndex = 0;
			}
			else
			{
				mmenu->selectionIndex--;
				if (mmenu->selectionIndex < 0) { mmenu->selectionIndex = mmenu->buttons.length-1; }
			}
			//TODO: Play a sound effect
		}
		else { mmenu->selectionIndex = -1; }
	}
	if (BtnPressed(Btn_Down))
	{
		HandleBtnExtended(Btn_Down);
		if (mmenu->buttons.length > 0)
		{
			if (mmenu->selectionIndex < 0)
			{
				mmenu->selectionIndex = 0;
			}
			else
			{
				mmenu->selectionIndex++;
				if ((u64)mmenu->selectionIndex >= mmenu->buttons.length) { mmenu->selectionIndex = 0; }
			}
			//TODO: Play a sound effect
		}
		else { mmenu->selectionIndex = -1; }
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Render                            |
// +--------------------------------------------------------------+
void RenderAppState_MainMenu(bool isOnTop)
{
	MemArena_t* scratch = GetScratchArena();
	
	pd->graphics->clear(kColorWhite);
	PdSetDrawMode(kDrawModeCopy);
	
	PdBindFont(&gl->titleFont);
	PdDrawText(NewStr(PROJECT_NAME), mmenu->titleRec.topLeft);
	
	// +==============================+
	// |        Render Buttons        |
	// +==============================+
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		reci mainRec = button->mainRec + mmenu->buttonListRec.topLeft;
		
		PdDrawRec(mainRec, kColorWhite);
		PdDrawRecOutline(mainRec, 2, false, kColorBlack);
		
		v2i displayTextPos = mainRec.topLeft + button->displayTextPos;
		PdBindFont(&mmenu->buttonFont);
		PdDrawText(button->displayText, displayTextPos);
		
		if (mmenu->selectionIndex >= 0 && (u64)mmenu->selectionIndex == bIndex)
		{
			reci handRec;
			handRec.size = mmenu->handSheet.frameSize;
			handRec.x = mainRec.x - handRec.width + 2;
			handRec.y = mainRec.y + mainRec.height/2 - handRec.height/2;
			handRec.x += RoundR32i(Oscillate(-3, 1, 1000));
			PdDrawSheetFrame(mmenu->handSheet, NewVec2i(0, 0), handRec);
		}
	}
	
	if (pig->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		
		v2i textPos = NewVec2i(1, 1);
		if (pig->perfGraph.enabled) { textPos.y += pig->perfGraph.mainRec.y + pig->perfGraph.mainRec.height + 1; }
		PdBindFont(&pig->debugFont);
		i32 stepY = pig->debugFont.lineHeight + 1;
		
		PdDrawTextPrint(textPos, "Game: v%u.%u(%03u)", GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_BUILD);
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "Engine: v%u.%u(%03u)", PIG_VERSION_MAJOR, PIG_VERSION_MINOR, PIG_VERSION_BUILD);
		textPos.y += stepY;
		
		PdSetDrawMode(oldDrawMode);
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                           Register                           |
// +--------------------------------------------------------------+
void RegisterAppState_MainMenu()
{
	mmenu = (MainMenuState_t*)RegisterAppState(
		AppState_MainMenu,
		sizeof(MainMenuState_t),
		StartAppState_MainMenu,
		StopAppState_MainMenu,
		UpdateAppState_MainMenu,
		RenderAppState_MainMenu
	);
}

