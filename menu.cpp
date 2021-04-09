
void 
DrawMenu(game_render_buffer *Buffer, game_memory GameMemory, game_input *Input, arena *Arena, paddle Paddle, 
        game_levels_state *LevelStates,
        game_state *GameState) {
    
    int Width = Arena->Size.X  / LEVEL__COUNT;;
    int NumberSize = 5;

    level CurrentLevel = GameState->CurrentLevel;
    s32 SavedHighScore = LevelStates->Levels[CurrentLevel].HighScore;

    if(GameState->CurrentLevelScore > SavedHighScore) {
        LevelStates->Levels[CurrentLevel].HighScore = GameState->CurrentLevelScore;
        SaveGameFile(GameMemory);
    }

    int SelectedLevel = (Paddle.P.X + (Arena->Size.X / 2)) / Width;

    for(int i = 0; i < LEVEL__COUNT; i++) {
        int X = (-(Arena->Size.X/2) + (Width)*i) + (Width/2 - NumberSize);
        int Y = 20;

        s32 LevelScore = LevelStates->Levels[i].HighScore;

        if(i == SelectedLevel) {
            DrawNumber(Buffer, i, V2(X, Y), NumberSize, 0x000000);
            DrawNumber(Buffer, LevelScore, V2(X, Y-10), 1.5, 0x000000);
        } else {
            DrawNumber(Buffer, i, V2(X, Y), NumberSize, 0xffffff);
            DrawNumber(Buffer, LevelScore, V2(X, Y-10), 1.5, 0xffffff);
        }
    }

    if(IS_CHANGED(Input, BUTTON_MOUSE_LEFT)) { 
        GameState->CurrentLevel = (level)SelectedLevel;
        ChangeGameMode(GameMemory, GAME_MODE_GAMEPLAY);
    }
}
