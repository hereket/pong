
char *LevelNames[] = {
    "BREAK\nOUT",
    "SUPER\nBREAKOUT",
    "PONG\nSPACE",
    "SPACE\nINVADERS"
};

void 
DrawMenu(game_render_buffer *Buffer, game_memory GameMemory, game_input *Input, arena *Arena,
        v2 CameraP,
        paddle Paddle, game_levels_state *LevelStates,
        game_state *GameState, assets *Assets) {
    
    int Width = Arena->Size.X  / LEVEL__COUNT;;
    int NumberSize = 5;

    level CurrentLevel = GameState->CurrentLevel;
    s32 SavedHighScore = LevelStates->Levels[CurrentLevel].HighScore;

    real32 S = 60.0f;
    v2 LogoSize = {S, S / Assets->Images.LogoDark.XOverYProportion};
    DrawBitmap(Buffer, &Assets->Images.LogoDark, CameraP, V2(0, -35), LogoSize);

    if(GameState->CurrentLevelScore > SavedHighScore) {
        LevelStates->Levels[CurrentLevel].HighScore = GameState->CurrentLevelScore;
        SaveGameFile(GameMemory);
    }

    int SelectedLevel = (Paddle.P.X + (Arena->Size.X / 2)) / Width;

    static real32 t = 1.0f;
    t -= 0.03;
    if (t < 0) { t = 1.0f - t; }

    for(int i = 0; i < LEVEL__COUNT; i++) {
        int X = (-(Arena->Size.X/2) + (Width)*i) + (Width/2 - NumberSize);
        int Y = 20;

        s32 LevelScore = LevelStates->Levels[i].HighScore;

        char *LevelName = LevelNames[i];

        real32 CharSize = 3.3;

        if(i == SelectedLevel) {
            DrawStringGradient(Buffer, LevelName, CameraP, V2(X, Y-1), CharSize, 0xffda7f, 0xff0080, t);
            DrawNumber(Buffer, LevelScore, CameraP, V2(X, Y-4), 1.5, 0xffffff);
        } else {
            DrawStringGradient(Buffer, LevelName, CameraP, V2(X,Y), CharSize, 0xffffff, 0xffffff, 1.0f);
            DrawNumber(Buffer, LevelScore, CameraP, V2(X, Y-4), 1.5, 0xffffff);
        }
    }

    if(IS_CHANGED(Input, BUTTON_MOUSE_LEFT)) { 
        GameState->CurrentLevel = (level)SelectedLevel;
        ChangeGameMode(GameMemory, GAME_MODE_GAMEPLAY);
    }
}
