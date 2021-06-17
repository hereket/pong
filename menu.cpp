
char *LevelNames[] = {
    "BREAK\nOUT",
    "SUPER\nBREAKOUT",
    "PONG\nSPACE",
    "SPACE\nINVADERS"
};

global_variable v2 GlobalLeftImageP = {-60, -21};
global_variable v2 GlobalRightImageP = {60, -21};

global_variable u32 GlobalColorMenuBg = 0x000000;

void 
DrawMenu(game_render_buffer *Buffer, game_memory GameMemory, game_input *Input, arena *Arena,
        v2 CameraP,
        paddle Paddle, game_levels_state *LevelStates,
        game_state *GameState) {
    
    DrawRect(Buffer, CameraP, Arena->P, Arena->Size, GlobalColorMenuBg);
    DrawRect(Buffer, CameraP, V2(0, -20), V2(180, 35), 0xff0000);

    int Width = Arena->Size.X  / LEVEL__COUNT;;
    int NumberSize = 5;

    level CurrentLevel = GameState->CurrentLevel;
    s32 SavedHighScore = LevelStates->Levels[CurrentLevel].HighScore;

    real32 S = 60.0f;
    bitmap LogoDark = GetBitmap(ASSET_B_LOGO_DARK);
    // v2 LogoSize = {S, S / Assets->Images.LogoDark.XOverYProportion};
    v2 LogoSize = {S, S / LogoDark.XOverYProportion};

    DrawBitmap(Buffer, &LogoDark, CameraP, V2(0, -35), LogoSize);

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

    GlobalLeftImageP = V2(-60, -21);
    GlobalRightImageP = V2(60, -21);

    bitmap LeftCurtain = GetBitmap(ASSET_B_LEFT_CURTAIN);
    bitmap RightCurtain = GetBitmap(ASSET_B_RIGHT_CURTAIN);

    DrawBitmap(Buffer, &LeftCurtain, CameraP, GlobalLeftImageP, V2(60, 60));
    DrawBitmap(Buffer, &RightCurtain, CameraP, GlobalRightImageP, V2(60, 60));

    if(IS_CHANGED(Input, BUTTON_MOUSE_LEFT)) { 
        GameState->CurrentLevel = (level)SelectedLevel;
        ChangeGameMode(GameMemory, GAME_MODE_GAMEPLAY);
    }
}


void 
DrawLevelTransition(game_render_buffer *Buffer, v2 CameraP, real32 dt, 
        block *Blocks, s32 BlockCount, real32 TransitionTime, arena *Arena, arena_walls *Walls, u32 LevelColor)
{
    real32 t = fmax(0, 1 - TransitionTime);
    u32 ColorMenuTransitionBg = LerpColor(GlobalColorMenuBg, LevelColor, t);

    DrawRect(Buffer, CameraP, Arena->P, Arena->Size, ColorMenuTransitionBg);

    for(int i = 0; i < BlockCount; i++) {
        block *Block = Blocks + i;

        if(Block->Life) {

            real32 a = (1 - TransitionTime) + (0.3f* (real32)i/(real32)BlockCount);
            a = fmin(1, a);

            v2 PositionOffset = Block->P + V2(0, -100);
            
            v2 P = Lerp(PositionOffset, Block->P, a);

            DrawRect(Buffer, CameraP, P, Block->Size, Block->Color);
        }
    }


    if(TransitionTime < 0.5)
    {
        u32 ColorWalls = 0x0000ff;
        DrawRect(Buffer, GlobalCameraP, Walls->Left.VisualP, Walls->Left.Size, ColorWalls);
        DrawRect(Buffer, GlobalCameraP, Walls->Right.VisualP, Walls->Right.Size, ColorWalls);
        DrawRect(Buffer, GlobalCameraP, Walls->Top.VisualP, Walls->Top.Size, ColorWalls);
    }


    real32 StepSize = 60 * dt;
    GlobalLeftImageP.X -= StepSize;
    GlobalRightImageP.X += StepSize;
    GlobalLeftImageP.Y -= StepSize;
    GlobalRightImageP.Y -= StepSize;

    bitmap LeftCurtain = GetBitmap(ASSET_B_LEFT_CURTAIN);
    bitmap RightCurtain = GetBitmap(ASSET_B_RIGHT_CURTAIN);

    DrawBitmap(Buffer, &LeftCurtain, CameraP, GlobalLeftImageP, V2(60, 60));
    DrawBitmap(Buffer, &RightCurtain, CameraP, GlobalRightImageP, V2(60, 60));
}
