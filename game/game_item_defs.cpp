/*
File:   game_item_defs.cpp
Author: Taylor Robbins
Date:   10\27\2023
Description: 
	** Holds code that manages item definitions and the book of all items
	** Definitions hold info about how to display a particular item in the game
	** Items can be tiles, which exist in the world, or they can be regular items which only exist in the inventory
*/

bool IsInitialized(ItemBook_t* itemBook)
{
	NotNull(itemBook);
	return (itemBook->allocArena != nullptr);
}

void FreeItemDef(ItemBook_t* itemBook, ItemDef_t* itemDef)
{
	NotNull3(itemBook, itemDef, itemBook->allocArena);
	FreeString(itemBook->allocArena, &itemDef->idStr);
	FreeString(itemBook->allocArena, &itemDef->displayName);
	FreeString(itemBook->allocArena, &itemDef->displayNamePlural);
	ClearPointer(itemDef);
}
void FreeItemBook(ItemBook_t* itemBook)
{
	NotNull(itemBook);
	if (itemBook->allocArena != nullptr)
	{
		VarArrayLoop(&itemBook->items, iIndex)
		{
			VarArrayLoopGet(ItemDef_t, item, &itemBook->items, iIndex);
			FreeItemDef(itemBook, item);
		}
		FreeVarArray(&itemBook->items);
	}
	ClearPointer(itemBook);
}

void InitItemBook(ItemBook_t* itemBook, MemArena_t* memArena, u64 numItemsExpected = 0)
{
	NotNull2(itemBook, memArena);
	ClearPointer(itemBook);
	itemBook->allocArena = memArena;
	itemBook->nextId = 1;
	CreateVarArray(&itemBook->items, memArena, sizeof(ItemDef_t), numItemsExpected);
}

ItemDef_t* AddItemDef(ItemBook_t* book, const ItemDef_t* itemDef, bool generateRuntimeId = true)
{
	NotNull3(book, itemDef, book->allocArena);
	ItemDef_t* result = VarArrayAdd(&book->items, ItemDef_t);
	NotNull(result);
	MyMemCopy(result, itemDef, sizeof(ItemDef_t));
	result->idStr = AllocString(book->allocArena, &itemDef->idStr);
	result->displayName = AllocString(book->allocArena, &itemDef->displayName);
	result->displayNamePlural = AllocString(book->allocArena, &itemDef->displayNamePlural);
	if (generateRuntimeId)
	{
		result->runtimeId = book->nextId;
		book->nextId++;
	}
	return result;
}

ItemDef_t* FindItemDef(ItemBook_t* book, u16 runtimeId)
{
	NotNull2(book, book->allocArena);
	if (runtimeId == ITEM_ID_NONE) { return nullptr; }
	VarArrayLoop(&book->items, iIndex)
	{
		VarArrayLoopGet(ItemDef_t, item, &book->items, iIndex);
		if (item->runtimeId == runtimeId) { return item; }
	}
	return nullptr;
}
ItemDef_t* FindItemDef(ItemBook_t* book, MyStr_t idStr)
{
	NotNull2(book, book->allocArena);
	VarArrayLoop(&book->items, iIndex)
	{
		VarArrayLoopGet(ItemDef_t, item, &book->items, iIndex);
		if (StrEquals(item->idStr, idStr)) { return item; }
	}
	return nullptr;
}
u16 LookupRuntimeId(ItemBook_t* book, MyStr_t idStr)
{
	ItemDef_t* itemDef = FindItemDef(book, idStr);
	return ((itemDef != nullptr) ? itemDef->runtimeId : ITEM_ID_NONE);
}

MyStr_t GetItemIdStr(ItemBook_t* book, u16 runtimeId)
{
	NotNull(book);
	ItemDef_t* itemDef = FindItemDef(book, runtimeId);
	if (itemDef != nullptr) { return itemDef->idStr; }
	else { return NewStr("[Invalid]"); }
}
MyStr_t GetItemDisplayName(ItemBook_t* book, u16 runtimeId, bool plural = false)
{
	NotNull(book);
	ItemDef_t* itemDef = FindItemDef(book, runtimeId);
	if (itemDef != nullptr) { return (plural ? itemDef->displayNamePlural : itemDef->displayName); }
	else { return NewStr("[Invalid]"); }
}
const char* GetItemDisplayNameNt(ItemBook_t* book, u16 runtimeId, bool plural = false)
{
	return GetItemDisplayName(book, runtimeId, plural).chars;
}
v2i GetItemFrame(ItemBook_t* book, u16 runtimeId)
{
	NotNull(book);
	ItemDef_t* itemDef = FindItemDef(book, runtimeId);
	if (itemDef != nullptr) { return itemDef->frame; }
	else { return INVALID_FRAME; }
}
bool IsItemSolid(ItemBook_t* book, u16 runtimeId)
{
	NotNull(book);
	ItemDef_t* itemDef = FindItemDef(book, runtimeId);
	if (itemDef != nullptr) { return IsFlagSet(itemDef->flags, ItemFlags_Solid); }
	else { return false; }
}
ItemStack_t GetItemDrop(ItemBook_t* book, u16 runtimeId)
{
	NotNull(book);
	ItemDef_t* itemDef = FindItemDef(book, runtimeId);
	if (itemDef != nullptr) { return itemDef->dropStack; }
	else { return NewItemStack(ITEM_ID_NONE, 0); }
}
ItemDef_t* GetRandomItemWithFlag(ItemBook_t* book, u8 flag, u8 notFlag = ItemFlags_None)
{
	u64 numMatches = 0;
	VarArrayLoop(&book->items, iIndex)
	{
		VarArrayLoopGet(ItemDef_t, itemDef, &book->items, iIndex);
		if (IsFlagSet(itemDef->flags, flag) && (notFlag == 0 || !IsFlagSet(itemDef->flags, notFlag))) { numMatches++; }
	}
	
	if (numMatches == 0) { return nullptr; }
	
	u64 randomNumber = GetRandU64(&pig->random, 0, numMatches);
	
	u64 foundIndex = 0;
	VarArrayLoop(&book->items, iIndex)
	{
		VarArrayLoopGet(ItemDef_t, itemDef, &book->items, iIndex);
		if (IsFlagSet(itemDef->flags, flag) && (notFlag == 0 || !IsFlagSet(itemDef->flags, notFlag)))
		{
			if (foundIndex == randomNumber) { return itemDef; }
			foundIndex++;
		}
	}
	
	AssertMsg(false, "Unreachable!");
	return nullptr;
}