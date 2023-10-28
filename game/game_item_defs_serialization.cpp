/*
File:   game_item_defs_serialization.cpp
Author: Taylor Robbins
Date:   10\27\2023
Description: 
	** Holds the function that deserializes text files that hold the info for item definitions
*/

#define ITEM_BOOK_FILE_PREFIX        "# Item Definitions"
#define ITEM_BOOK_FILE_PREFIX_LENGTH 18

enum TryDeserItemBookError_t
{
	TryDeserItemBookError_None = 0,
	TryDeserItemBookError_CouldntOpenFile,
	TryDeserItemBookError_EmptyFile,
	TryDeserItemBookError_MissingOrCorruptHeader,
	TryDeserItemBookError_NumErrors,
};
const char* GetTryDeserItemBookErrorStr(TryDeserItemBookError_t enumValue)
{
	switch (enumValue)
	{
		case TryDeserItemBookError_None:                   return "None";
		case TryDeserItemBookError_CouldntOpenFile:        return "CouldntOpenFile";
		case TryDeserItemBookError_EmptyFile:              return "EmptyFile";
		case TryDeserItemBookError_MissingOrCorruptHeader: return "MissingOrCorruptHeader";
		default: return "Unknown";
	}
}

void FinishItemDef(ProcessLog_t* log, ItemBook_t* bookOut, ItemDef_t* currentItem)
{
	if (currentItem->runtimeId != 0)
	{
		AddItemDef(bookOut, currentItem);
	}
	ClearPointer(currentItem);
	currentItem->displayName = NewStr("[Unnamed Item]");
	currentItem->displayNamePlural = NewStr("[Unnamed Items]");
	currentItem->frame = INVALID_FRAME;
}

bool TryDeserItemBook(MyStr_t fileContents, ProcessLog_t* log, ItemBook_t* bookOut, MemArena_t* memArena, bool isInclude = false)
{
	NotNullStr(&fileContents);
	NotNull3(log, bookOut, memArena);
	TryParseFailureReason_t parseFailureReason = TryParseFailureReason_None;
	
	if (!isInclude)
	{
		SetProcessLogName(log, NewStr("TryDeserItemBook"));
		LogWriteLine_N(log, "Entering TryDeserItemBook...");
	}
	
	if (fileContents.length < ITEM_BOOK_FILE_PREFIX_LENGTH)
	{
		LogPrintLine_E(log, "File %.*s was only %llu bytes when we expected at least %u bytes for header", log->filePath.length, log->filePath.chars, fileContents.length, ITEM_BOOK_FILE_PREFIX_LENGTH);
		LogExitFailure(log, TryDeserItemBookError_EmptyFile);
		return false;
	}
	if (MyMemCompare(fileContents.chars, ITEM_BOOK_FILE_PREFIX, ITEM_BOOK_FILE_PREFIX_LENGTH) != 0)
	{
		LogPrintLine_E(log, "Invalid header found in file %.*s. Expected \"%s\"", log->filePath.length, log->filePath.chars, ITEM_BOOK_FILE_PREFIX);
		LogExitFailure(log, TryDeserItemBookError_MissingOrCorruptHeader);
		return false;
	}
	
	fileContents.chars += ITEM_BOOK_FILE_PREFIX_LENGTH;
	fileContents.length -= ITEM_BOOK_FILE_PREFIX_LENGTH;
	
	if (!isInclude)
	{
		InitItemBook(bookOut, memArena);
	}
	else { Assert(IsInitialized(bookOut)); }
	
	ItemDef_t currentItem = {};
	FinishItemDef(log, bookOut, &currentItem);
	
	MemArena_t* scratch = GetScratchArena(memArena);
	MyStr_t oldFilePath = AllocString(scratch, &log->filePath);
	
	TextParser_t textParser = NewTextParser(fileContents);
	ParsingToken_t token = {};
	while (TextParserGetToken(&textParser, &token))
	{
		if (token.type == ParsingTokenType_KeyValuePair)
		{
			if (currentItem.runtimeId == 0 &&
				!StrEqualsIgnoreCase(token.key, "Include") &&
				!StrEqualsIgnoreCase(token.key, "Item"))
			{
				LogPrintLine_W(log, "Got key \"%.*s\" BEFORE \"Item\" was declared in %.*s line %llu", token.key.length, token.key.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
				log->hadWarnings = true;
				continue;
			}
			
			// +==============================+
			// |             Item             |
			// +==============================+
			if (StrEqualsIgnoreCase(token.key, "Item"))
			{
				FinishItemDef(log, bookOut, &currentItem);
				
				ItemDef_t* existingItem = FindItemDef(bookOut, token.value);
				if (existingItem != nullptr)
				{
					LogPrintLine_W(log, "Duplicate ItemId \"%.*s\" in %.*s line %llu", token.value.length, token.value.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
					log->hadWarnings = true;
				}
				else
				{
					currentItem.idStr = token.value;
					currentItem.runtimeId = (u16)(bookOut->items.length + 1);
				}
			}
			// +==============================+
			// |            Flags             |
			// +==============================+
			else if (StrEqualsIgnoreCase(token.key, "Flag") || StrEqualsIgnoreCase(token.key, "Flags"))
			{
				PushMemMark(scratch);
				u64 numPieces = 0;
				MyStr_t* pieces = SplitString(scratch, token.value, ",", &numPieces);
				for (u64 pIndex = 0; pIndex < numPieces; pIndex++)
				{
					MyStr_t piece = pieces[pIndex];
					TrimWhitespace(&piece);
					
					ItemFlags_t flag = ItemFlags_None;
					if (TryParseBitfieldEnum(piece, &flag, ItemFlags_NumFlags, GetItemFlagsStr, &parseFailureReason))
					{
						FlagSet(currentItem.flags, flag);
					}
					else
					{
						LogPrintLine_W(log, "Unknown flag \"%.*s\" on Item \"%.*s\" in %.*s line %llu", piece.length, piece.chars, currentItem.idStr.length, currentItem.idStr.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
						log->hadWarnings = true;
					}
				}
				PopMemMark(scratch);
			}
			// +==============================+
			// |           Display            |
			// +==============================+
			else if (StrEqualsIgnoreCase(token.key, "Display"))
			{
				MyStr_t pieces[2];
				if (SplitStringFixed(token.value, ",", ArrayCount(pieces), pieces))
				{
					currentItem.displayName = pieces[0];
					currentItem.displayNamePlural = pieces[1];
				}
				else
				{
					LogPrintLine_W(log, "Expected %u, comma separated, names for Display: \"%.*s\" in %.*s line %llu", ArrayCount(pieces), token.value.length, token.value.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
					log->hadWarnings = true;
				}
			}
			// +==============================+
			// |             Drop             |
			// +==============================+
			else if (StrEqualsIgnoreCase(token.key, "Drop") || StrEqualsIgnoreCase(token.key, "Drops"))
			{
				MyStr_t pieces[2];
				if (SplitStringFixed(token.value, ",", ArrayCount(pieces), pieces))
				{
					ItemDef_t* dropItemDef = FindItemDef(bookOut, pieces[0]);
					u8 dropCount = 0;
					if (dropItemDef == nullptr)
					{
						LogPrintLine_W(log, "Unknown item \"%.*s\" for Drop on \"%.*s\" in %.*s line %llu", pieces[0].length, pieces[0].chars, currentItem.idStr.length, currentItem.idStr.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
						log->hadWarnings = true;
					}
					else if (!TryParseU8(pieces[1], &dropCount, &parseFailureReason))
					{
						LogPrintLine_W(log, "Couldn't parse Drop count as u8: \"%.*s\" on \"%.*s\" in %.*s line %llu", pieces[1].length, pieces[1].chars, currentItem.idStr.length, currentItem.idStr.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
						log->hadWarnings = true;
					}
					else
					{
						currentItem.dropStack.id = dropItemDef->runtimeId;
						currentItem.dropStack.count = dropCount;
					}
				}
				else
				{
					LogPrintLine_W(log, "Expected %u, comma separated, names for Drop: \"%.*s\" in %.*s line %llu", ArrayCount(pieces), token.value.length, token.value.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
					log->hadWarnings = true;
				}
			}
			// +==============================+
			// |            Frame             |
			// +==============================+
			else if (StrEqualsIgnoreCase(token.key, "Frame"))
			{
				if (!TryParseV2i(token.value, &currentItem.frame, &parseFailureReason))
				{
					LogPrintLine_W(log, "Couldn't parse Frame \"%.*s\" as vec2i (%s) in %.*s line %llu", token.value.length, token.value.chars, GetTryParseFailureReasonStr(parseFailureReason), log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
					log->hadWarnings = true;
				}
			}
			// +==============================+
			// |       Value/Cost/Worth       |
			// +==============================+
			else if (StrEqualsIgnoreCase(token.key, "Value") || StrEqualsIgnoreCase(token.key, "Cost") || StrEqualsIgnoreCase(token.key, "Worth"))
			{
				if (!TryParseU8(token.value, &currentItem.value, &parseFailureReason))
				{
					LogPrintLine_W(log, "Couldn't parse Cost \"%.*s\" as u8 (%s) in %.*s line %llu", token.value.length, token.value.chars, GetTryParseFailureReasonStr(parseFailureReason), log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
					log->hadWarnings = true;
				}
			}
			// +==============================+
			// |          Inventory           |
			// +==============================+
			else if (StrEqualsIgnoreCase(token.key, "Inventory"))
			{
				if (!TryParseEnum(token.value, &currentItem.inventoryType, InvType_NumTypes, GetInvTypeStr, &parseFailureReason))
				{
					LogPrintLine_W(log, "Unknown Inventory type \"%.*s\" in %.*s line %llu", token.value.length, token.value.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
					log->hadWarnings = true;
				}
			}
			// +==============================+
			// |           Include            |
			// +==============================+
			else if (StrEqualsIgnoreCase(token.key, "Include"))
			{
				FinishItemDef(log, bookOut, &currentItem);
				
				MemArena_t* scratch2 = GetScratchArena(scratch, memArena);
				MyStr_t includePath = token.value;
				MyStr_t includedFileContents = MyStr_Empty;
				if (ReadEntireFile(false, includePath, &includedFileContents, scratch2))
				{
					SetProcessLogFilePath(log, includePath);
					if (TryDeserItemBook(includedFileContents, log, bookOut, memArena, true))
					{
						MyStr_t fileName = GetFileNamePart(oldFilePath);
						MyStr_t includeFileName = GetFileNamePart(includePath);
						LogPrintLine_D(log, "Successfully included %.*s from %.*s line %llu", includeFileName.length, includeFileName.chars, fileName.length, fileName.chars, textParser.lineParser.lineIndex);
					}
					else
					{
						LogPrintLine_W(log, "Included in %.*s line %llu", log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
						FreeScratchArena(scratch);
						FreeScratchArena(scratch2);
						return false;
					}
					SetProcessLogFilePath(log, oldFilePath);
				}
				else
				{
					LogPrintLine_W(log, "WARNING: Failed to find/read/open Included file \"%.*s\" in %.*s line %llu", includePath.length, includePath.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex);
				}
				FreeScratchArena(scratch2);
			}
			// +==============================+
			// |         Unknown Key          |
			// +==============================+
			else
			{
				LogPrintLine_W(log, "WARNING: Unknown key in %.*s line %llu: \"%.*s\"", log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex, token.key.length, token.key.chars);
				log->hadWarnings = true;
			}
		}
		else if (token.type == ParsingTokenType_Comment)
		{
			// Do nothing
		}
		else if (token.type == ParsingTokenType_Unknown)
		{
			LogPrintLine_W(log, "WARNING: Unknown syntax in %.*s line %llu: \"%.*s\"", log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex, token.str.length, token.str.chars);
			log->hadWarnings = true;
		}
		else
		{
			LogPrintLine_W(log, "WARNING: Unhandled token type %s in %.*s line %llu: \"%.*s\"", GetParsingTokenTypeStr(token.type), log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex, token.str.length, token.str.chars);
			log->hadWarnings = true;
		}
	}
	
	FinishItemDef(log, bookOut, &currentItem);
	
	if (!isInclude) { LogExitSuccess(log); }
	FreeScratchArena(scratch);
	return true;
}

bool TryLoadItemBook(bool fromDataDir, MyStr_t filePath, ProcessLog_t* log, ItemBook_t* bookOut, MemArena_t* memArena)
{
	NotNullStr(&filePath);
	NotNull3(log, bookOut, memArena);
	MemArena_t* scratch = GetScratchArena(memArena);
	bool result = false;
	
	SetProcessLogName(log, NewStr("TryLoadItemBook"));
	SetProcessLogFilePath(log, filePath);
	
	MyStr_t fileContents = {};
	if (ReadEntireFile(fromDataDir, filePath, &fileContents, scratch))
	{
		result = TryDeserItemBook(fileContents, log, bookOut, memArena);
	}
	else
	{
		LogPrintLine_E(log, "Failed to open file at \"%.*s\"", filePath.length, filePath.chars);
		LogExitFailure(log, TryDeserItemBookError_CouldntOpenFile);
	}
	
	FreeScratchArena(scratch);
	
	return result;
}

bool TryLoadAllItemDefs(ItemBook_t* bookOut, MemArena_t* memArena)
{
	NotNull2(bookOut, memArena);
	MemArena_t* scratch = GetScratchArena(memArena);
	ProcessLog_t deserLog;
	CreateProcessLog(&deserLog, Kilobytes(64), scratch, scratch);
	ItemBook_t tempBook;
	bool deserSuccess = TryLoadItemBook(false, NewStr(ITEM_BOOK_PATH), &deserLog, &tempBook, scratch);
	if (deserLog.hadErrors || deserLog.hadWarnings)
	{
		DumpProcessLog(&deserLog, "Item Book Deser Log", DbgLevel_Warning);
	}
	if (!deserSuccess)
	{
		PrintLine_D("Failed to load item book: %s", GetTryDeserItemBookErrorStr((TryDeserItemBookError_t)deserLog.errorCode));
		FreeScratchArena(scratch);
		return false;
	}
	
	FreeItemBook(bookOut);
	InitItemBook(bookOut, memArena, tempBook.items.length);
	VarArrayLoop(&tempBook.items, rIndex)
	{
		VarArrayLoopGet(ItemDef_t, tempItem, &tempBook.items, rIndex);
		AddItemDef(bookOut, tempItem, false);
	}
	bookOut->nextId = tempBook.nextId;
	
	FreeScratchArena(scratch);
	return true;
}
