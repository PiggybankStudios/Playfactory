/*
File:   game_recipes_serialization.cpp
Author: Taylor Robbins
Date:   10\25\2023
Description: 
	** Holds the TryDeserRecipeBook function which can take an input string from
	** a file and deserialize it into a book of recipes
*/

#define RECIPE_BOOK_FILE_PREFIX        "# Recipe Book"
#define RECIPE_BOOK_FILE_PREFIX_LENGTH 13

enum TryDeserRecipeBookError_t
{
	TryDeserRecipeBookError_None = 0,
	TryDeserRecipeBookError_CouldntOpenFile,
	TryDeserRecipeBookError_EmptyFile,
	TryDeserRecipeBookError_MissingOrCorruptHeader,
	TryDeserRecipeBookError_NumErrors,
};
const char* GetTryDeserRecipeBookErrorStr(TryDeserRecipeBookError_t enumValue)
{
	switch (enumValue)
	{
		case TryDeserRecipeBookError_None:                   return "None";
		case TryDeserRecipeBookError_CouldntOpenFile:        return "CouldntOpenFile";
		case TryDeserRecipeBookError_EmptyFile:              return "EmptyFile";
		case TryDeserRecipeBookError_MissingOrCorruptHeader: return "MissingOrCorruptHeader";
		default: return "Unknown";
	}
}

bool TryDeserRecipeBook(MyStr_t fileContents, ProcessLog_t* log, ItemBook_t* itemBook, RecipeBook_t* bookOut, MemArena_t* memArena, bool isInclude = false)
{
	NotNullStr(&fileContents);
	NotNull3(log, bookOut, memArena);
	TryParseFailureReason_t parseFailureReason = TryParseFailureReason_None;
	
	if (!isInclude)
	{
		SetProcessLogName(log, NewStr("TryDeserRecipeBook"));
		LogWriteLine_N(log, "Entering TryDeserRecipeBook...");
	}
	
	if (fileContents.length < RECIPE_BOOK_FILE_PREFIX_LENGTH)
	{
		LogPrintLine_E(log, "File %.*s was only %llu bytes when we expected at least %u bytes for header", log->filePath.length, log->filePath.chars, fileContents.length, RECIPE_BOOK_FILE_PREFIX_LENGTH);
		LogExitFailure(log, TryDeserRecipeBookError_EmptyFile);
		return false;
	}
	if (MyMemCompare(fileContents.chars, RECIPE_BOOK_FILE_PREFIX, RECIPE_BOOK_FILE_PREFIX_LENGTH) != 0)
	{
		LogPrintLine_E(log, "Invalid header found in file %.*s. Expected \"%s\"", log->filePath.length, log->filePath.chars, RECIPE_BOOK_FILE_PREFIX);
		LogExitFailure(log, TryDeserRecipeBookError_MissingOrCorruptHeader);
		return false;
	}
	
	fileContents.chars += RECIPE_BOOK_FILE_PREFIX_LENGTH;
	fileContents.length -= RECIPE_BOOK_FILE_PREFIX_LENGTH;
	
	if (!isInclude)
	{
		InitRecipeBook(bookOut, memArena);
	}
	else { Assert(IsInitialized(bookOut)); }
	Recipe_t* currentRecipe = nullptr;
	
	MemArena_t* scratch = GetScratchArena(memArena);
	MyStr_t oldFilePath = AllocString(scratch, &log->filePath);
	
	TextParser_t textParser = NewTextParser(fileContents);
	ParsingToken_t token = {};
	while (TextParserGetToken(&textParser, &token))
	{
		if (token.type == ParsingTokenType_KeyValuePair)
		{
			// +==============================+
			// |            Recipe            |
			// +==============================+
			if (StrEqualsIgnoreCase(token.key, "Recipe"))
			{
				u16 itemIds[3] = { ITEM_ID_NONE, ITEM_ID_NONE, ITEM_ID_NONE };
				
				PushMemMark(scratch);
				u64 numPieces = 0;
				MyStr_t* pieces = SplitString(scratch, token.value, ",", &numPieces);
				if (numPieces == 3)
				{
					for (u64 pIndex = 0; pIndex < numPieces; pIndex++)
					{
						MyStr_t piece = pieces[pIndex];
						TrimWhitespace(&piece);
						if (IsEmptyStr(piece))
						{
							LogPrintLine_W(log, "Empty piece found in Recipe definition in %.*s line %llu: %.*s", log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex, token.value.length, token.value.chars);
							log->hadWarnings = true;
							break;
						}
						// ItemDef_t* FindItemDef(ItemBook_t* book, u16 runtimeId)
						else
						{
							ItemDef_t* itemDef = FindItemDef(itemBook, piece);
							if (itemDef != nullptr)
							{
								itemIds[pIndex] = itemDef->runtimeId;
							}
							else
							{
								LogPrintLine_W(log, "Unknown Item \"%.*s\" in %.*s line %llu in Recipe: %.*s", piece.length, piece.chars, log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex, token.value.length, token.value.chars);
								log->hadWarnings = true;
								break;
							}
						}
					}
				}
				else
				{
					LogPrintLine_W(log, "Too %s pieces in %.*s line %llu. Expected 3 pieces, not %llu", ((numPieces > 3) ? "many" : "few"), log->filePath.length, log->filePath.chars, textParser.lineParser.lineIndex, numPieces);
					log->hadWarnings = true;
				}
				PopMemMark(scratch);
				
				if (itemIds[0] != ITEM_ID_NONE && itemIds[1] != ITEM_ID_NONE && itemIds[2] != ITEM_ID_NONE)
				{
					currentRecipe = AddRecipeToBook(bookOut, NewRecipe(itemIds[0], itemIds[1], itemIds[2]));
					NotNull(currentRecipe);
					LogPrintLine_D(log, "Found recipe for %s", GetItemDisplayNameNt(itemBook, currentRecipe->outputId));
				}
			}
			// +==============================+
			// |           Include            |
			// +==============================+
			else if (StrEqualsIgnoreCase(token.key, "Include"))
			{
				MemArena_t* scratch2 = GetScratchArena(scratch, memArena);
				MyStr_t includePath = token.value;
				MyStr_t includedFileContents = MyStr_Empty;
				if (ReadEntireFile(false, includePath, &includedFileContents, scratch2))
				{
					SetProcessLogFilePath(log, includePath);
					if (TryDeserRecipeBook(includedFileContents, log, itemBook, bookOut, memArena, true))
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
	
	if (!isInclude) { LogExitSuccess(log); }
	FreeScratchArena(scratch);
	return true;
}

bool TryLoadRecipeBook(bool fromDataDir, MyStr_t filePath, ProcessLog_t* log, ItemBook_t* itemBook, RecipeBook_t* bookOut, MemArena_t* memArena)
{
	NotNullStr(&filePath);
	NotNull3(log, bookOut, memArena);
	MemArena_t* scratch = GetScratchArena(memArena);
	bool result = false;
	
	SetProcessLogName(log, NewStr("TryLoadRecipeBook"));
	SetProcessLogFilePath(log, filePath);
	
	MyStr_t fileContents = {};
	if (ReadEntireFile(fromDataDir, filePath, &fileContents, scratch))
	{
		result = TryDeserRecipeBook(fileContents, log, itemBook, bookOut, memArena);
	}
	else
	{
		LogPrintLine_E(log, "Failed to open file at \"%.*s\"", filePath.length, filePath.chars);
		LogExitFailure(log, TryDeserRecipeBookError_CouldntOpenFile);
	}
	
	FreeScratchArena(scratch);
	
	return result;
}

bool TryLoadAllRecipes(ItemBook_t* itemBook, RecipeBook_t* bookOut, MemArena_t* memArena)
{
	NotNull2(bookOut, memArena);
	MemArena_t* scratch = GetScratchArena(memArena);
	ProcessLog_t deserLog;
	CreateProcessLog(&deserLog, Kilobytes(64), scratch, scratch);
	RecipeBook_t tempBook;
	bool deserSuccess = TryLoadRecipeBook(false, NewStr(RECIPE_BOOK_PATH), &deserLog, itemBook, &tempBook, scratch);
	if (deserLog.hadErrors || deserLog.hadWarnings)
	{
		DumpProcessLog(&deserLog, "Recipe Book Deser Log", DbgLevel_Warning);
	}
	if (!deserSuccess)
	{
		PrintLine_D("Failed to load recipe book: %s", GetTryDeserRecipeBookErrorStr((TryDeserRecipeBookError_t)deserLog.errorCode));
		FreeScratchArena(scratch);
		return false;
	}
	
	FreeRecipeBook(bookOut);
	InitRecipeBook(bookOut, memArena, tempBook.recipes.length);
	VarArrayLoop(&tempBook.recipes, rIndex)
	{
		VarArrayLoopGet(Recipe_t, tempRecipe, &tempBook.recipes, rIndex);
		AddRecipeToBook(bookOut, *tempRecipe);
	}
	
	FreeScratchArena(scratch);
	return true;
}
