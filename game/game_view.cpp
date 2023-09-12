/*
File:   game_view.cpp
Author: Taylor Robbins
Date:   09\11\2023
Description: 
	** Handles the view that follows the player around the world and tries to
	** move in a way that allows the player to see where they are going while
	** moving in a direction for a while, but minimizing the movement when the
	** player is working in a specific place
*/

void GameViewUpdateWorldRec(GameView_t* view)
{
	NotNull(view);
	view->worldRec = NewRecCentered(view->position, ToVec2(ScreenSize));
	view->worldReci = NewReci(Vec2Roundi(view->worldRec.topLeft), ScreenSize);
}
v2 ClampGameViewToWorld(v2 position, v2 viewSize, v2i worldSize)
{
	v2 result = position;
	if (result.x < viewSize.width/2) { result.x = viewSize.width/2; }
	if (result.y < viewSize.height/2) { result.y = viewSize.height/2; }
	if (result.x > worldSize.width - viewSize.width/2) { result.x = worldSize.width - viewSize.width/2; }
	if (result.y > worldSize.height - viewSize.height/2) { result.y = worldSize.height - viewSize.height/2; }
	return result;
}

void InitGameView(GameView_t* view, v2 startPos, v2i worldSize)
{
	NotNull(view);
	ClearPointer(view);
	view->position = ClampGameViewToWorld(startPos, ToVec2(ScreenSize), worldSize);
	view->targetPos = view->position;
	GameViewUpdateWorldRec(view);
	view->isMoving = false;
}

void UpdateGameView(GameView_t* view, v2 playerPos, v2 playerInputVec, v2i worldSize)
{
	NotNull(view);
	bool isPlayerMoving = (playerInputVec != Vec2_Zero);
	
	bool isPlayerNearScreenEdge = false;
	if (playerPos.x < view->worldRec.x + GAME_VIEW_SCREEN_EDGE_PADDING) { isPlayerNearScreenEdge = true; }
	if (playerPos.y < view->worldRec.y + GAME_VIEW_SCREEN_EDGE_PADDING) { isPlayerNearScreenEdge = true; }
	if (playerPos.x > view->worldRec.x + view->worldRec.width - GAME_VIEW_SCREEN_EDGE_PADDING) { isPlayerNearScreenEdge = true; }
	if (playerPos.y > view->worldRec.y + view->worldRec.height - GAME_VIEW_SCREEN_EDGE_PADDING) { isPlayerNearScreenEdge = true; }
	if (!view->isMoving && isPlayerNearScreenEdge)
	{
		view->isMoving = true;
	}
	
	if (view->isMoving)
	{
		if (isPlayerMoving || isPlayerNearScreenEdge)
		{
			view->targetPos = playerPos;
			if (isPlayerMoving) { view->targetPos += Vec2Multiply(Vec2Normalize(playerInputVec), GAME_VIEW_VEL_BASED_LOOK_AHEAD_DIST); }
			view->targetPos = ClampGameViewToWorld(view->targetPos, view->worldRec.size, worldSize);
		}
		
		if (Vec2Length(view->targetPos - view->position) <= GAME_VIEW_STOP_MOVE_DIST_TO_TARGET)
		{
			view->isMoving = false;
		}
	}
	
	v2 targetDelta = view->targetPos - view->position;
	r32 targetDist = Vec2Length(targetDelta);
	r32 moveLag = (r32)(view->isMoving ? GAME_VIEW_MOVE_LAG_WHILE_MOVING : GAME_VIEW_MOVE_LAG_WHILE_STOPPED);
	r32 viewMoveSpeed = targetDist / moveLag; //TODO: Factor in TimeScale!
	if (targetDist > GAME_VIEW_JUMP_TO_TARGET_DIST)
	{
		view->position += (targetDelta / targetDist) * viewMoveSpeed;
	}
	else
	{
		view->position = view->targetPos;
	}
	
	GameViewUpdateWorldRec(view);
}
