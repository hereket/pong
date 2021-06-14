#include "collision.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



#define BALL_ACTIVE 0x1
#define BALL_DESTROYED_ON_DP_Y_DOWN 0x2


struct {
    v2 P;
    v2 dP;
    v2 Size;
    u32 Color;

    v2 VisualP;
    v2 VisualDp;
    v2 VisualSize;
} typedef paddle;

struct {
    v2 P;
    v2 DP;
    v2 Size;
    real32 Life;
    real32 Angle;
    u32 Color;
} typedef particle;

struct {
    u32 Flags;
    v2 P;
    v2 dP;
    v2 Size;
    v2 DesiredP;
    real32 BaseSpeed;
    u32 Color;
} typedef ball;

struct {
    v2 P;
    v2 Size;

    struct {
        real32 Left;
        real32 Right;
        real32 Top;
        real32 Bottom;
    } Margin;
} typedef arena;

typedef enum {
    GAME_MODE_GAMEPLAY,
    GAME_MODE_MENU,
} game_mode;


enum {
    LEVEL_01_NORMAL,
    LEVEL_02_WALL,

    LEVEL_05_PONG,
    LEVEL_06_INVADERS,

    LEVEL__COUNT,

    LEVEL_03_STADIUM,
    LEVEL_04_CHESS,
} typedef level;


enum {
    POWER_INACTIVE,
    POWER_INVINCIBILITY,
    POWER_TRIPLE_SHOT,
    POWER_COMET,

    POWER_LIMIT_BETWEEN_BAD_AND_GOOD,

    POWER_INSTAKILL,
    POWER_STRONG_BLOCKS,
    POWER_INVERTED_CONTROLS,
    POWER_SLOW_PLAYER,

    POWER__COUNT
} typedef power_type;

struct {
    power_type Type;
    v2 P;
} typedef power;

typedef struct {
    v2 P;
    v2 Size;
    v2 DistanceFromCenter;

    s32 Life;
    u32 Color;

    power Power;
} block;

typedef struct {
    s32 EnemyLife;
    v2 EnemyP;
    v2 EnemyDp;
    v2 EnemySize;
} level_state_pong;

typedef struct {
    v2 CenterP;
    v2 dP;
    v2 InvadersFullSize;
} level_state_invaders;

typedef struct {
    union {
        level_state_pong Pong;
        level_state_invaders Invaders;
    } Level;

    level CurrentLevel;
    u32 CurrentLevelScore;
} game_state;


typedef struct {
    bool32 Locked;
    s32 HighScore;
} level_state;

typedef struct {
    s32 Version;
    level_state Levels[LEVEL__COUNT];
} game_levels_state;


typedef struct {
    bitmap PowerupInvincibility;
    bitmap PowerupTripleShot;
    bitmap PowerupComet;

    bitmap LeftCurtain;
    bitmap RightCurtain;

    bitmap ForceField;
    bitmap LogoDark;
    bitmap LogoLight;
} image_assets;

typedef struct {
    image_assets Images;
} assets;


typedef struct {
    v2 P;
    v2 VisualP;
    v2 dP;
    v2 Size;
    v2 Color;
} wall;

typedef struct {
    wall Left;
    wall Top;
    wall Right;
} arena_walls;

global_variable arena_walls ArenaWalls;


global_variable game_levels_state GlobalGameLevelState;
global_variable v2 GlobalCameraP;
global_variable v2 GlobalCameraDP;




internal void
ChangeGameMode(game_memory GameMemory, game_mode GameMode);

void SaveGameFile(game_memory GameMemory);


#include "console.cpp"
#include "menu.cpp"

#define SAVE_FILE_VERSION 1


char *GameSafeFilename = "../data/savefile.data";

void
SaveGameFile(game_memory GameMemory)
{
    s32 Size = sizeof(game_levels_state);
    s64 BytesWritten = GameMemory.DEBUGPlatformWriteEntireFile(GameSafeFilename, (char *)&GlobalGameLevelState, Size);
}

void
LoadGameFile(game_memory GameMemory)
{
    read_file_result LoadedFile = GameMemory.DEBUGPlatformReadEntireFile(GameSafeFilename);
    GlobalGameLevelState = *((game_levels_state *)LoadedFile.Memory);
}





global_variable game_state GlobalGameState;

#define BLOCK_COUNT 1024 * 2
global_variable block BlockList[BLOCK_COUNT];

global_variable bool32 Initialized;
global_variable paddle Paddle;
// global_variable ball Ball;
global_variable arena Arena;
// global_variable real32 Speed;
global_variable bool32 FirstBallMovement;
global_variable s32 BlockTotalCount;
global_variable bool32 AdvanceLevel;
global_variable ball Balls[16];
global_variable s32 GlobalNumberOfTripleShots;
global_variable real32 InvincibilityTime;
global_variable real32 CometTime;
global_variable real32 StrongBlocksTime;
global_variable real32 InvertedControlsTime;

// global_variable game_mode_state Level;
global_variable power PowerBlocks[16];
global_variable s32 NextPowerBlock;
global_variable v2 PowerBlockSize;

global_variable v2 PaddleDesiredP;
global_variable s32 PlayerLifeCount;
global_variable real32 GlobalBallBaseSpeed = 40;

global_variable s32 GlobalBlockIndex;


global_variable particle GlobalParticles[1024];
global_variable s32 GlobalNextParticle;

global_variable assets GlobalAssets;



global_variable loaded_audio game_main_bg_sound;
global_variable loaded_audio ball_hit_1;
global_variable loaded_audio sound1;
global_variable loaded_audio sound2;
global_variable loaded_audio sound3;

global_variable game_mode GlobalGameMode;
global_variable real32 GlobalLevelTransitionTime;
global_variable bool32 TransitionStarted;



void
AddScreenshake(real32 Amplitude)
{
    GlobalCameraDP = V2( 
        RandomUniformRange(-Amplitude, Amplitude),
        RandomUniformRange(-Amplitude, Amplitude)
    );
}


void
CreateInvader(v2 P, real32 SizeX, real32 SizeY, v2 CenterP) {
#define INVADER_ROW_CHAR_COUNT 11
#define INVADER_COLUMN_CHAR_COUNT 8 
    char InvaderTemplate[][INVADER_ROW_CHAR_COUNT + 1] = {
        " 0       0 ",
        "  0     0  ",
        "  0000000  ",
        " 00 000 00 ",
        "00000000000",
        "0 0000000 0",
        "0 0     0 0",
        "   00 00   ",
    };

    // real32 InvaderWidth = Size;
    // real32 InvaderHeight = Size * INVADER_COLUMN_CHAR_COUNT / INVADER_ROW_CHAR_COUNT;
    real32 InvaderWidth = SizeX;
    real32 InvaderHeight = SizeY;
    real32 BlockSize = InvaderWidth / INVADER_ROW_CHAR_COUNT;


    for(int YIndex = 0; YIndex < INVADER_COLUMN_CHAR_COUNT; YIndex++){
        for(int XIndex = 0; XIndex < INVADER_ROW_CHAR_COUNT; XIndex++) {
            char Value = InvaderTemplate[YIndex][XIndex];
            if(Value == '0') {
                BlockTotalCount++;
                block *Block = BlockList + GlobalBlockIndex++;
                if(GlobalBlockIndex >= BLOCK_COUNT) { GlobalBlockIndex = 0; }
                Block->Life = 1;
                Block->Size.X = BlockSize;
                Block->Size.Y = BlockSize;
                Block->P.X = P.X - (InvaderWidth/2) + (XIndex*BlockSize);
                Block->P.Y = P.Y - (InvaderHeight/2) + (YIndex*BlockSize);
                Block->DistanceFromCenter.X = Block->P.X - CenterP.X;
                Block->DistanceFromCenter.Y = Block->P.Y - CenterP.Y;
            }
        }
    }
}

void
InitBall(ball *Ball) 
{
    Ball->BaseSpeed = GlobalBallBaseSpeed;
    Ball->P = V2(0, -50);
    Ball->dP = V2(0.0, Ball->BaseSpeed);
    Ball->Size = V2(1.5, 1.5);
    Ball->Color = 0x00FFFFFF;
    Ball->Flags |= BALL_ACTIVE;
}

internal void
SpawnParticle(v2 P, v2 Size, u32 Color, v2 DP, real32 Life, real32 Angle = 0.0)
{
    particle *Particle = GlobalParticles + GlobalNextParticle++;
    if(GlobalNextParticle >= ARRAY_COUNT(GlobalParticles)) { GlobalNextParticle = 0; }
    Particle->P = P;
    Particle->Size = Size;
    Particle->Life = Life;
    Particle->Color = Color;
    Particle->Angle = Angle;

    Particle->DP = DP;
}

internal void 
StartGame(level Level)
{
    // Speed = 30.0f;
    PlayerLifeCount = 3;

    GlobalGameState.CurrentLevel = Level;
    GlobalGameState.CurrentLevelScore = 0;

    AdvanceLevel = false;

    FirstBallMovement = 1;

    InitBall(&Balls[0]);


    Paddle.Size = V2(20, 4);
    Paddle.VisualSize = V2(Paddle.Size.X, Paddle.Size.Y);

    Paddle.P.Y = Arena.Margin.Bottom - 7;
    Paddle.Color = 0x0000ff00;
    Paddle.VisualP = V2(Paddle.P.X, Paddle.P.Y);

    s32 GlobalBlockIndex = 0;

    // float BlocksFullWidth = Arena.Size.X - 10;
    // float BlocksFullHeight = Arena.Size.Y - 10;
    float BlocksFullWidth = Arena.Margin.Right - Arena.Margin.Left - 10;
    float BlocksFullHeight = Arena.Margin.Top - Arena.Margin.Bottom - 10;

    BlockTotalCount = 0;

    for(s32 Index = 0; Index < BLOCK_COUNT; Index++) {
        block *Block = BlockList + Index;
        Block->Life = 0;
    }

    switch(Level)
    {
        case LEVEL_01_NORMAL:
        {
            s32 BlockXCount = 19;
            s32 BlockYCount = 9;

            s32 YColorStep = 255 / BlockYCount;
            s32 XColorStep = 255 / BlockXCount;

            real32 BlockOffsetX = 0.4;
            real32 BlockOffsetY = 0.4;

            real32 BlockSizeX = (BlocksFullWidth / BlockXCount) - BlockOffsetX;

            real32 StartX = -BlocksFullWidth/2 + ((BlockSizeX+BlockOffsetX)/2);
            real32 StartY = Arena.Margin.Top + 10;

            for(s32 Y = 0; Y < BlockYCount; Y++) {
                for(s32 X = 0; X < BlockXCount; X++) {
                    BlockTotalCount++; 
                    block *Block = BlockList + GlobalBlockIndex++;
                    if(GlobalBlockIndex >= BLOCK_COUNT) { GlobalBlockIndex = 0; }
                    Block->Life = 1;
                    Block->Color = (Y*YColorStep << 8) | 
                                   (X*XColorStep);
                    Block->Size.X = BlockSizeX;
                    Block->Size.Y = 3;
                    Block->P.X = StartX + X * (Block->Size.X + BlockOffsetX);
                    Block->P.Y = StartY + Y * (Block->Size.Y + BlockOffsetY);
                }
            }
        } break;

        case LEVEL_02_WALL:
        {
            s32 BlockXCount = 19;
            s32 BlockYCount = 12;

            s32 YColorStep = 255 / BlockYCount;
            s32 XColorStep = 1;

            real32 BlockSizeX = (BlocksFullWidth / BlockXCount);

            real32 StartX = -BlocksFullWidth/2 + (BlockSizeX/2);
            real32 StartY = Arena.P.Y - Arena.Size.Y/2 + 10;

            for(s32 Y = 0; Y < BlockYCount; Y++) {
                for(s32 X = 0; X < BlockXCount; X++) {
                    BlockTotalCount++; 
                    block *Block = BlockList + GlobalBlockIndex++;
                    if(GlobalBlockIndex >= BLOCK_COUNT) { GlobalBlockIndex = 0; }
                    Block->Life = 1;
                    Block->Color = (Y*YColorStep << 8) | 
                                   (X*XColorStep);
                    Block->Size.X = BlockSizeX;
                    Block->Size.Y = 3;
                    Block->P.Y = StartY + Y * (Block->Size.Y);
                    Block->P.X = StartX + X * (Block->Size.X);
                }
            }
        } break;

        case LEVEL_03_STADIUM:
        {
        } break;

        case LEVEL_04_CHESS:
        {
        } break;

        case LEVEL_05_PONG:
        {
            s32 BlockXCount = 15;
            s32 BlockYCount = 3;

            s32 YColorStep = 255 / BlockYCount;
            s32 XColorStep = 1;

            float BlocksFullWidth = (Arena.Size.X / 5);
            real32 BlockSizeX = (BlocksFullWidth / BlockXCount);

            real32 FromCenterStartOffsetX = -(BlocksFullWidth / 2);

            real32 StartX = -BlocksFullWidth/2 + (BlockSizeX/2);
            real32 StartY = Arena.P.Y - Arena.Size.Y/2 + 10;

            for(s32 Y = 0; Y < BlockYCount; Y++) {
                for(s32 X = 0; X < BlockXCount; X++) {
                    BlockTotalCount++; 
                    block *Block = BlockList + GlobalBlockIndex++;
                    if(GlobalBlockIndex >= BLOCK_COUNT) { GlobalBlockIndex = 0; }
                    Block->Life = 1;
                    Block->Color = (Y*YColorStep << 8) | (X*XColorStep);
                    Block->Size.X = BlockSizeX;
                    Block->Size.Y = 3;
                    Block->P.Y = StartY + Y * (Block->Size.Y);
                    Block->P.X = StartX + X * (Block->Size.X);

                    Block->DistanceFromCenter.X = FromCenterStartOffsetX + (X * BlockSizeX);
                    // Block->DistanceFromCenter.Y = ;
                }
            }


        } break;

        case LEVEL_06_INVADERS:
        {
            level_state_invaders *LevelInvaders = &GlobalGameState.Level.Invaders; 
            LevelInvaders->dP = V2(0.05, 0);

            GlobalBlockIndex = 0;

            s32 PaddingX = 2;
            s32 PaddingY = 2;

            s32 InvaderCountX = 5;
            s32 InvaderCountY = 3;

            real32 InvadersTotalWidth = (Arena.Size.X / 2);
            real32 WidthToHeightMultiplier = ((float)INVADER_COLUMN_CHAR_COUNT / (float)INVADER_ROW_CHAR_COUNT);
            real32 InvadersTotalHeight = InvadersTotalWidth * WidthToHeightMultiplier;

            real32 InvaderSizeX = (InvadersTotalWidth / InvaderCountX) - PaddingX;
            real32 InvaderSizeY = (InvaderSizeX * WidthToHeightMultiplier) - PaddingY;

            LevelInvaders->InvadersFullSize = V2(InvadersTotalWidth, InvadersTotalHeight);

            LevelInvaders->CenterP = V2(
                    -(Arena.Size.X/2) + InvadersTotalWidth/2, 
                    (Arena.Size.Y/2) - InvadersTotalHeight);

            v2 CenterP = LevelInvaders->CenterP;

            for(s32 YIndex = 0; YIndex < InvaderCountY; YIndex++) {
                for(s32 XIndex = 0; XIndex < InvaderCountX; XIndex++) {
                    real32 X = CenterP.X - (InvadersTotalWidth/2) + (XIndex * InvaderSizeX) + (XIndex * PaddingX) + (InvaderSizeX/2);
                    real32 Y = CenterP.Y - (InvadersTotalHeight/2) + (YIndex * InvaderSizeY) + (YIndex * PaddingY);
                    // v2 DistanceFromCenter = {
                    //     CenterP.X - (InvadersTotalWidth/2*XIndex),
                    //     CenterP.Y - (InvadersTotalHeight/2*YIndex),
                    // };
                    CreateInvader(V2(X, Y), InvaderSizeX, InvaderSizeY, CenterP);
                }
            }
        } break;

        default: {
        };
    }
}

internal void
ChangeGameMode(game_memory GameMemory, game_mode GameMode) {
    // AddScreenshake(50);
    GlobalGameMode = GameMode;
    SaveGameFile(GameMemory);

    if(GameMode == GAME_MODE_GAMEPLAY) {
        GlobalLevelTransitionTime = 1.0f;
        StartGame(GlobalGameState.CurrentLevel);
    }
}


void
SimulateLevel(game_render_buffer *Buffer, v2 CameraP)
{
    for(s32 Index = 0; Index < ARRAY_COUNT(PowerBlocks); Index++) {
        power *Power = &PowerBlocks[Index];
        if(Power->Type == POWER_INACTIVE) {
            continue;
        }

        v2 PowerupSize = {5, 5};

        if(Power->Type == POWER_INVINCIBILITY) {
            DrawBitmap(Buffer, &GlobalAssets.Images.PowerupInvincibility, CameraP, Power->P, PowerupSize);
        } else if(Power->Type == POWER_TRIPLE_SHOT) {
            DrawBitmap(Buffer, &GlobalAssets.Images.PowerupTripleShot, CameraP, Power->P, PowerupSize);
        } else if(Power->Type < POWER_LIMIT_BETWEEN_BAD_AND_GOOD) {
            DrawRect(Buffer, CameraP, Power->P, V2(2, 2), 0xf05454);
        } else {
            DrawRect(Buffer, CameraP, Power->P, V2(2, 2), 0xffff00);
        }
    }

    switch(GlobalGameState.CurrentLevel){
        case LEVEL_02_WALL: 
        {
        } break;

        default: { }
    }

}

void
SimulateLevelStateChanges(game_render_buffer *Buffer, v2 CameraP, level CurrentLevel, v2 PaddleP) {
   switch(CurrentLevel){
       case LEVEL_05_PONG: 
       {
           level_state_pong *Pong = &GlobalGameState.Level.Pong;

           ball *Ball = Balls;

           if(Pong->EnemyP.X < Ball->P.X) { Pong->EnemyDp.X = 0.2; }
           else                           { Pong->EnemyDp.X = -0.2; }

           Pong->EnemyP.X += Pong->EnemyDp.X; 

           for(s32 Index = 0; Index < ARRAY_COUNT(BlockList); Index++) {
               block *Block = BlockList + Index;
               Block->P.X = Pong->EnemyP.X + Block->DistanceFromCenter.X;
           }
       } break;

       case LEVEL_06_INVADERS: 
       {
           level_state_invaders *LevelInvaders = &GlobalGameState.Level.Invaders; 
           LevelInvaders->CenterP = LevelInvaders->CenterP + LevelInvaders->dP;

           real32 InvaderLeftMargin  = LevelInvaders->CenterP.X - LevelInvaders->InvadersFullSize.X/2;
           real32 InvaderRightMargin = LevelInvaders->CenterP.X + LevelInvaders->InvadersFullSize.X/2;

           if(InvaderLeftMargin < (-Arena.Size.X/2) ) {
               LevelInvaders->dP.X *= -1;
               LevelInvaders->CenterP.Y += 5;
           }else if(InvaderRightMargin > (Arena.Size.X/2)) {
               LevelInvaders->dP.X *= -1;
               LevelInvaders->CenterP.Y += 5;
           }

           for(s32 Index = 0; Index < ARRAY_COUNT(BlockList); Index++) {
               block *Block = BlockList + Index;
               Block->P.X = LevelInvaders->CenterP.X + Block->DistanceFromCenter.X;
               Block->P.Y = LevelInvaders->CenterP.Y + Block->DistanceFromCenter.Y;

               if(COLLISION_NONE != CollisionSide(Block->P, Block->Size, PaddleDesiredP, Paddle.VisualSize)) {
                   StartGame(GlobalGameState.CurrentLevel);
                   printf("Collided\n");
               }

           }

           DrawRect(Buffer, CameraP, V2(InvaderLeftMargin, -25), V2(0.1, 3), 0x00ffffff);
           DrawRect(Buffer, CameraP, LevelInvaders->CenterP, V2(0.1, 3), 0x00ffffff);
           DrawRect(Buffer, CameraP, V2(InvaderRightMargin, -25), V2(0.1, 3), 0x00ffffff);
       }
       
       default: {} break;
   }
}

void
SpawnPowerup(v2 P, power_type Type) 
{
    power *Power = PowerBlocks + NextPowerBlock;
    NextPowerBlock++;
    if(NextPowerBlock >= ARRAY_COUNT(PowerBlocks)) {
        NextPowerBlock = 0;
    }
    Power->P = P;
    Power->Type = Type;
    // Power->Type = POWER_INVINCIBILITY;
    // Power->Type = POWER_TRIPLE_SHOT;
    // Power->Type = POWER_COMET;
    // Power->Type = POWER_INSTAKILL;
    // Power->Type = POWER_STRONG_BLOCKS;
    // Power->Type = POWER_INVERTED_CONTROLS;
}

void
TestForWinCondition()
{
    BlockTotalCount--; 
    if(BlockTotalCount <= 0) {
        AdvanceLevel = true;
    }
}

void
BlockIsDestroyed(block *Block)
{
    TestForWinCondition();

    // SpawnPowerup(Block->P);

    power_type Type = POWER_TRIPLE_SHOT;

    for(s32 i = 0; i < 5; i++) {
        v2 DP = { 0.12 * RandomUniformRange(-1, 1), 0.1 * RandomUniformRange(-1, 1), };
        v2 Size = {Block->Size.X/5, Block->Size.X/5 };
        SpawnParticle(Block->P, Size, Block->Color, DP, 0.5);
    }

    switch(GlobalGameState.CurrentLevel) {
        case LEVEL_01_NORMAL: 
        {
            SpawnPowerup(Block->P, POWER_INVINCIBILITY);
        } break;

        case LEVEL_02_WALL: 
        {
            SpawnPowerup(Block->P, POWER_TRIPLE_SHOT);
        } break;

        default: 
        {
        } break; 
    }
}




void
SpawnTripleShotsBalls(v2 Origin, v2 Size, u32 Color)
{
    ball *Ball = 0; for(s32 i = 0; i < 2; i++) {
        for(ball *TempBall = Balls; TempBall != Balls + ARRAY_COUNT(Balls); TempBall++) {
            if(!(TempBall->Flags & BALL_ACTIVE)) {
                Ball = TempBall;
            }
        }

        if(Ball) {
            ZERO_STRUCT(*Ball);
            Ball->P.X = Origin.X;
            Ball->P.Y = Origin.Y;
            // Ball->Size = V2(2,2);
            Ball->Size = Size;
            Ball->Color = Color;
            Ball->Flags = BALL_ACTIVE | BALL_DESTROYED_ON_DP_Y_DOWN;
            Ball->dP.Y = -GlobalBallBaseSpeed;
            Ball->dP.X = GlobalBallBaseSpeed;
        }
    }
    if(Ball) {
        Ball->dP.X = -GlobalBallBaseSpeed;
    }
}

void 
ProcessBallOnDpYDown(ball *Ball) 
{
    if(Ball->Flags & BALL_DESTROYED_ON_DP_Y_DOWN) {
        Ball->Flags &= ~BALL_ACTIVE;
    }
}

void 
SpawnCollisionParticles(ball *Ball, v2 Size = V2(1,1)) {
    for(int i = 0; i < 10; i++) {
        v2 DP = { 0.32 * RandomUniformRange(-1, 1),
                  0.32 * RandomUniformRange(-1, 1), };
        // SpawnParticle(Ball->P, V2(1,1), 0x000000, DP, 0.3);
        SpawnParticle(Ball->P, Size, Ball->Color, DP, 0.3);
    }
}


void
LoseLife()
{
    PlayerLifeCount--;
    if(PlayerLifeCount <= 0) { StartGame(GlobalGameState.CurrentLevel); }

    FirstBallMovement = 1;
    InitBall(&Balls[0]);
}

void
DrawTest(game_render_buffer *Buffer, v2 CameraP)
{
    float Size = 1;
    float StartX = 0;

    for(int YIndex = 0; YIndex < 10; YIndex++) {
        for(int XIndex = 0; XIndex < 10; XIndex++) {
            float X = StartX + (XIndex * Size);
            float Y = YIndex;
            DrawRect(Buffer, CameraP, V2(X, Y), V2(Size, Size), 0x00000000);
        }
    }
}

void LoadPng(game_memory GameMemory, char *Filename, bitmap *Bitmap) {
    read_file_result Result = GameMemory.DEBUGPlatformReadEntireFile(Filename);

    s32 N = 0;
    Bitmap->Memory = (u8 *)stbi_load_from_memory((u8 *)Result.Memory,
                                                 (s32)Result.Size,
                                                 (s32 *)&Bitmap->Width,
                                                 (s32 *)&Bitmap->Height,
                                                 &N, 4);

    Bitmap->XOverYProportion = (real32)Bitmap->Width / (real32)Bitmap->Height;

    for(int Y = 0; Y < Bitmap->Height; Y++) {
        for(int X = 0; X < Bitmap->Width; X++) {
            u32 *Pixel = (u32 *)Bitmap->Memory + Y*Bitmap->Width + X;
            u32 Temp = *Pixel;

            *Pixel = 
                (((Temp >> 0) & 0xFF) << 16) |
                (((Temp >> 8) & 0xFF) << 8) |
                (((Temp >> 16) & 0xFF) << 0) |
                (((Temp >> 24) & 0xFF) << 24);
        }
    }
}

void
UpdateWallPosition(wall *Wall, real32 dt) {
    // TODO: Rework this
    // wall *Wall = &ArenaWalls.Left;
    real32 Time = 0.016;
    v2 DistanceFromCenter = Wall->P - Wall->VisualP;

    v2 Ddp = DistanceFromCenter * 1000 + DistanceFromCenter * 10;
    Wall->dP = Wall->dP + Ddp * Time;
    Wall->VisualP = Wall->VisualP + (Wall->dP * Time) + (Ddp * Time * Time * 0.5f);
}


void 
SimulateGame(game_memory GameMemory, game_render_buffer *Buffer, game_input *Input, real32 dt)
{
    if(IS_PRESSED(Input, BUTTON_DOWN)) {dt /= 5;}
    if(!Initialized) {
        Initialized = true;
        GlobalGameState.CurrentLevel = LEVEL_01_NORMAL;
        // GlobalCurrentLevel = LEVEL_05_PONG;
        // GlobalCurrentLevel = LEVEL_06_INVADERS;
        PowerBlockSize = V2(2, 2);

        LoadPng(GameMemory, "../data/invincibility.png", &GlobalAssets.Images.PowerupInvincibility);
        LoadPng(GameMemory, "../data/triple_shot.png", &GlobalAssets.Images.PowerupTripleShot);
        LoadPng(GameMemory, "../data/commet.png", &GlobalAssets.Images.PowerupComet);
        LoadPng(GameMemory, "../data/force_field.png", &GlobalAssets.Images.ForceField);

        LoadPng(GameMemory, "../data/left_curtain.png", &GlobalAssets.Images.LeftCurtain);
        LoadPng(GameMemory, "../data/right_curtain.png", &GlobalAssets.Images.RightCurtain);

        LoadPng(GameMemory, "../data/logo_dark.png", &GlobalAssets.Images.LogoDark);
        LoadPng(GameMemory, "../data/log_light.png", &GlobalAssets.Images.LogoLight);


        game_main_bg_sound = GameMemory.DEBUGPlatformLoadWav((u8 *)"../data/breakout_main.wav");
        ball_hit_1 = GameMemory.DEBUGPlatformLoadWav((u8 *)"../data/sfx/hit_5.wav");
        sound1 = GameMemory.DEBUGPlatformLoadWav((u8 *)"../data/test.wav");
        sound2 = GameMemory.DEBUGPlatformLoadWav((u8 *)"../data/sine.wav");
        sound3 = GameMemory.DEBUGPlatformLoadWav((u8 *)"../data/collision3.wav");

        GameMemory.DEBUGPlatformPlayWav(game_main_bg_sound, false);

        LoadGameFile(GameMemory);

        GlobalCameraP = V2(0, 0);
        GlobalCameraDP = V2(0, 0);


        Arena.P = V2(0, 0);
        // Arena.Size = V2(160, 100);
        Arena.Size = V2(180, 100);



        ArenaWalls.Left.Size = V2(15, Arena.Size.Y + 10);
        ArenaWalls.Right.Size = V2(15, Arena.Size.Y + 10);
        ArenaWalls.Top.Size = V2(Arena.Size.X + 20, 15);

        ArenaWalls.Left.P = V2(-0.5*Arena.Size.X, 0);
        ArenaWalls.Right.P = V2(0.5*Arena.Size.X, 0);
        ArenaWalls.Top.P = V2(0, -0.5*Arena.Size.Y);

        ArenaWalls.Left.VisualP = ArenaWalls.Left.P;
        ArenaWalls.Right.VisualP = ArenaWalls.Right.P;
        ArenaWalls.Top.VisualP = ArenaWalls.Top.P;

        Arena.Margin.Left = Arena.P.X - (0.5*Arena.Size.X) + (0.5*ArenaWalls.Left.Size.X);
        Arena.Margin.Right = Arena.P.X + (0.5*Arena.Size.X) - (0.5*ArenaWalls.Right.Size.X);
        Arena.Margin.Top = Arena.P.Y - (0.5*Arena.Size.Y) + (0.5*ArenaWalls.Top.Size.Y);
        Arena.Margin.Bottom = Arena.P.Y + (0.5*Arena.Size.Y);


        ChangeGameMode(GameMemory, GAME_MODE_MENU);
        StartGame(GlobalGameState.CurrentLevel);
    }

    u32 ColorArenaBg = 0xaa0000;


    // NOTE: Wall movement
    {
        UpdateWallPosition(&ArenaWalls.Left, dt);
        UpdateWallPosition(&ArenaWalls.Right, dt);
        UpdateWallPosition(&ArenaWalls.Top, dt);
    }


    // NOTE: Camera movement
    {
        // real32 Time = dt;
        // TODO: This is hack to remove visual errors due to variable dt. Think about this formula more.
        real32 Time = 1.0f / 60.f;
        v2 ScreenCenter = {0, 0};
        v2 PFromCenter = ScreenCenter - GlobalCameraP;

        v2 Ddp = (PFromCenter * 1000.0f) + (PFromCenter * 10.0f);
        GlobalCameraDP = GlobalCameraDP + (Ddp * Time);
        GlobalCameraP = GlobalCameraP + ((Ddp * Time * Time * 0.5f) + (GlobalCameraDP * Time));
        
        // v2 Ddp = PFromCenter;
        // GlobalCameraDP = GlobalCameraDP + Ddp ;
        // GlobalCameraP = GlobalCameraP  + GlobalCameraDP * dt * dt;

        real32 ZeroStep = 0.01;
        if(GlobalCameraP.X < ZeroStep && GlobalCameraP.X > -ZeroStep) { GlobalCameraP.X = 0; }
        if(GlobalCameraP.Y < ZeroStep && GlobalCameraP.Y > -ZeroStep) { GlobalCameraP.Y = 0; }
    }


    real32 PaddleSpeedMultiplier = 0.17;

    // PaddleDesiredP = PixelToWorldCoord(Buffer, Input->MouseP);
    if(InvertedControlsTime > 0) { PaddleDesiredP = Paddle.P - (Input->MouseDp * PaddleSpeedMultiplier); }
    else                         { PaddleDesiredP = Paddle.P + (Input->MouseDp * PaddleSpeedMultiplier); }
    PaddleDesiredP.Y = Paddle.P.Y;

    Paddle.dP.X = PaddleDesiredP.X - Paddle.P.X;




    ClearScreen(Buffer, 0x00551100);


    if(GlobalGameMode == GAME_MODE_GAMEPLAY) 
    {
        if(GlobalLevelTransitionTime > 0) {
            GlobalLevelTransitionTime -= dt;

            if(!TransitionStarted && GlobalLevelTransitionTime < 0.5) {
                TransitionStarted = true;
                real32 WallWiggleAmount = 10.0f;
                ArenaWalls.Left.VisualP.X += -WallWiggleAmount;
                ArenaWalls.Right.VisualP.X += WallWiggleAmount;
                ArenaWalls.Top.VisualP.Y += -WallWiggleAmount;

                // ArenaWalls.Left.dP = V2(-WallWiggleAmount, 0);
                // ArenaWalls.Right.dP = V2(WallWiggleAmount, 0);
                // ArenaWalls.Top.dP = V2(0, -WallWiggleAmount);
            }

            DrawLevelTransition(Buffer, GlobalCameraP, &GlobalAssets, dt, BlockList, BlockTotalCount, GlobalLevelTransitionTime, &Arena, &ArenaWalls, ColorArenaBg);
        } else {
            TransitionStarted = false;
            for(ball *Ball = Balls; Ball != Balls + ARRAY_COUNT(Balls); Ball++)
            {
                if(!(Ball->Flags & BALL_ACTIVE)) { continue; }

                Ball->DesiredP = Ball->P + (Ball->dP * dt);
                
                collision_type CollisionWallLeft = CollisionSide(ArenaWalls.Left.P, ArenaWalls.Left.Size, Ball->DesiredP, Ball->Size);
                collision_type CollisionWallRight = CollisionSide(ArenaWalls.Right.P, ArenaWalls.Right.Size, Ball->DesiredP, Ball->Size);
                collision_type CollisionWallTop = CollisionSide(ArenaWalls.Top.P, ArenaWalls.Top.Size, Ball->DesiredP, Ball->Size);

                real32 WallWiggleAmount = 50.0f;
                if(CollisionWallLeft != COLLISION_NONE) {
                    ArenaWalls.Left.dP = V2(-WallWiggleAmount, 0);
                    Ball->dP.X *= -1;
                }

                if(CollisionWallRight != COLLISION_NONE) {
                    ArenaWalls.Right.dP = V2(WallWiggleAmount, 0);
                    Ball->dP.X *= -1;
                }

                if(CollisionWallTop != COLLISION_NONE) {
                    ArenaWalls.Top.dP = V2(0, -WallWiggleAmount);
                    if(!FirstBallMovement) {
                        Ball->dP.Y *= -1;
                    } else if (Ball->dP.Y < 0) {
                        Ball->dP.Y *= -1;
                    }
                }

                if(Balls[0].P.Y > (Arena.Margin.Bottom)) {
                    if((InvincibilityTime > 0) || (FirstBallMovement)) {
                        Ball->DesiredP.Y =  Arena.Margin.Bottom - Ball->Size.Y*0.5; 
                        Ball->dP.Y *= -1;
                    } else {
                        LoseLife();
                    }
                }

                for(s32 Index = 0; Index < BLOCK_COUNT; Index++)
                {
                    block *Block = BlockList + Index;
                    if(Block->Life) { if(!FirstBallMovement) {
                            static int test = 0;
                            collision_type Collision = CollisionSide(Block->P, Block->Size, V2(Ball->DesiredP.X, Ball->DesiredP.Y), Ball->Size);
                            if(COLLISION_NONE != CollisionSide(Block->P, Block->Size, V2(Ball->DesiredP.X, Ball->DesiredP.Y), Ball->Size)) {

                                AddScreenshake(50);
                                

                                GlobalGameState.CurrentLevelScore += 1;

                                // GameMemory.DEBUGPlatformPlayWav(sound3, false);
                                GameMemory.DEBUGPlatformPlayWav(ball_hit_1, false);

                                ProcessBallOnDpYDown(Ball);

                                SpawnCollisionParticles(Ball);

                                if(! (CometTime > 0)) {
                                    if(Collision == COLLISION_LEFT) { 
                                        Ball->dP.X *= -1;
                                    }
                                    if(Collision == COLLISION_RIGHT) {
                                        Ball->dP.X *= -1;
                                    }
                                    if(Collision == COLLISION_TOP) {
                                        Ball->dP.Y *= -1;
                                    }
                                    if(Collision == COLLISION_BOTTOM) {
                                        Ball->dP.Y *= -1;
                                    }
                                }

                                if(!(StrongBlocksTime > 0)) {
                                    Block->Life -= 1;
                                    BlockIsDestroyed(Block);
                                }

                                break;
                            }
                        }

                        if(Ball->dP.Y > 0) {
                            CometTime = 0;
                        }

                    }
                }

                if(IsColliding(Ball->DesiredP, Ball->Size, PaddleDesiredP, Paddle.VisualSize)) {
                    FirstBallMovement = false;

                    SpawnCollisionParticles(Ball);

                    Ball->dP.Y *= -1;

                    float MaxMoveSpeedX = GlobalBallBaseSpeed * 2;
                    float MinusOneToOneRange = -1 * ((Paddle.P.X - Ball->DesiredP.X) / ((float)Paddle.VisualSize.X / 2));
                    Ball->dP.X = Clamp(-MaxMoveSpeedX, MaxMoveSpeedX * MinusOneToOneRange, MaxMoveSpeedX);

                    if(GlobalNumberOfTripleShots > 0) {
                        GlobalNumberOfTripleShots--;
                        SpawnTripleShotsBalls(Ball->DesiredP, Ball->Size, 0xff00ff);
                    }
                } else {
                    Ball->P.X = Ball->DesiredP.X;
                    Ball->P.Y = Ball->DesiredP.Y;
                }


                {
                    v2 DP = { 0.02 * RandomUniformRange(-1, 1), 0.02 * RandomUniformRange(-1, 1), };

                    SpawnParticle(Ball->P, Ball->Size, Ball->Color, DP, 0.5);
                }
            }

            for(power *Power = PowerBlocks; 
                    Power != PowerBlocks + ARRAY_COUNT(PowerBlocks);
                    Power++) 
            {
                if(Power->Type == POWER_INACTIVE) { continue; }
                real32 Speed = 30;
                Power->P.Y += 1*dt*Speed;

                if(IsColliding(Power->P, PowerBlockSize, Paddle.P, Paddle.VisualSize)) {
                    // Paddle.Color = 0x00000000;
                    switch(Power->Type) {
                        case POWER_INVINCIBILITY: 
                            {
                                InvincibilityTime += 5.0f;
                            } break;

                        case POWER_COMET: 
                            {
                                CometTime += 5.0f;
                            } break;

                        case POWER_TRIPLE_SHOT: 
                            {
                                GlobalNumberOfTripleShots++;
                            } break;

                        case POWER_INSTAKILL: 
                            {
                                // StartGame(GlobalCurrentLevel);
                                LoseLife();
                            } break;

                        case POWER_STRONG_BLOCKS: 
                            {
                                StrongBlocksTime += 5.0f;
                            } break;

                        case POWER_INVERTED_CONTROLS: 
                            {
                                InvertedControlsTime += 5.0f;
                            } break;

                        default: 
                            {
                            }break;
                    }

                    Power->Type = POWER_INACTIVE;
                }

            }


            // 
            // Rendering
            //
            
            // DrawRect(Buffer, GlobalCameraP, Arena.P, Arena.Size, 0x00aa0000);
            DrawRect(Buffer, GlobalCameraP, Arena.P, Arena.Size, ColorArenaBg);

            u32 ColorWalls = 0x0000ff;
            DrawRect(Buffer, GlobalCameraP, ArenaWalls.Left.VisualP, ArenaWalls.Left.Size, ColorWalls);
            DrawRect(Buffer, GlobalCameraP, ArenaWalls.Right.VisualP, ArenaWalls.Right.Size, ColorWalls);
            DrawRect(Buffer, GlobalCameraP, ArenaWalls.Top.VisualP, ArenaWalls.Top.Size, ColorWalls);



            for(s32 Index = 0; Index < BLOCK_COUNT; Index++)
            {
                block *Block = BlockList + Index;
                if(Block->Life) 
                {
                    DrawRect(Buffer, GlobalCameraP, Block->P, Block->Size, Block->Color);
                }
            }


            if(InvincibilityTime > 0) {
                real32 BitmapHeight = 20;
                DrawBitmap(Buffer, &GlobalAssets.Images.ForceField, 
                        GlobalCameraP, 
                        V2(0, Arena.Margin.Bottom - BitmapHeight/2 + 2), 
                        V2(Arena.Size.X, BitmapHeight));
            }

            SimulateLevel(Buffer, GlobalCameraP);

            SimulateLevelStateChanges(Buffer, GlobalCameraP, GlobalGameState.CurrentLevel, Paddle.P);

            // NOTE: -- PARTICLE stuff
            {
                // NOTE: Simulate
                for(int i = 0; i < ARRAY_COUNT(GlobalParticles); i++) {
                    particle *Particle = GlobalParticles + i;
                    Particle->P = Particle->P + Particle->DP;
                }

                // NOTE: Render
                for(int i = 0; i < ARRAY_COUNT(GlobalParticles); i++) {
                    particle *Particle = GlobalParticles + i;
                    if(Particle->Life <= 0) { continue; } 

                    // DrawRectAlpha(Buffer, Particle->P, Particle->Size, 0x000000, Particle->Life);
                    DrawRectAlpha(Buffer, GlobalCameraP, Particle->P, Particle->Size, Particle->Color, Particle->Life);
                    DrawRectRotated(Buffer, GlobalCameraP, Particle->P, Particle->Size, Particle->Color, Particle->Life, Particle->Angle);
                    Particle->Life -= dt;
                    Particle->Angle += 0.1;
                }
            }



            if(AdvanceLevel) {
                GlobalGameState.CurrentLevel = (level)(GlobalGameState.CurrentLevel + 1);
                StartGame(GlobalGameState.CurrentLevel);
            }

            if(InvincibilityTime > 0) { InvincibilityTime -= dt; }
            if(CometTime > 0) { CometTime -= dt; }
            if(StrongBlocksTime > 0) { StrongBlocksTime -= dt; }
            if(InvertedControlsTime > 0) { InvertedControlsTime -= dt; }

            for(int i = 0; i < PlayerLifeCount; i++) {
                real32 Size = 2;
                real32 Offset = 0.7;
                real32 StartX = Arena.P.X - (Arena.Size.X/2) + 5;
                real32 X = StartX + i*Size + i*Offset;
                real32 Y = Arena.P.Y - (Arena.Size.Y/2) + 2.5;
                DrawRect(Buffer, GlobalCameraP, V2(X, Y), V2(Size, Size), 0x0000FF00);
                DrawRect(Buffer, GlobalCameraP, V2(X, Y), V2(Size-0.8, Size-0.8), 0xff00ff);
            }

            for(ball *Ball = Balls; Ball != Balls + ARRAY_COUNT(Balls); Ball++) {
                if(!(Ball->Flags & BALL_ACTIVE)) { continue; }

                DrawRect(Buffer, GlobalCameraP, V2(Ball->P.X, Ball->P.Y), Ball->Size, Ball->Color);
            }


            DrawNumber(Buffer, GlobalGameState.CurrentLevelScore, GlobalCameraP, V2(-Arena.Size.X/2 + 20, -Arena.Size.Y/2 + 1.5), 1.5, 0xffffffff);
        }
    } else if (GlobalGameMode == GAME_MODE_MENU) {
        DrawMenu(Buffer, GameMemory, Input, &Arena, GlobalCameraP, Paddle, &GlobalGameLevelState, &GlobalGameState, &GlobalAssets);
    }

    
    // PADDLE Render
    {
        if(InvincibilityTime > 0) { Paddle.Color = 0x00ffffff;}
        else                      { Paddle.Color = 0x0000ff00; }

        // TODO: This is a lot of garbage work, make better data structures to remove this repeated work
        real32 LeftMarginForPaddle = Arena.Margin.Left + Paddle.Size.X/2;
        real32 RightMarginForPaddle = Arena.Margin.Right - Paddle.Size.X/2;

        Paddle.P = PaddleDesiredP;

        {
            Paddle.VisualDp.X = (Paddle.P.X - Paddle.VisualP.X);
            Paddle.VisualP.X += (Paddle.VisualDp.X) * 0.2;

            real32 ScaleFactor = Absolute(Paddle.VisualDp.X) / 40.0f; // NOTE: 40 is just arbitrary value

            real32 PaddleScaleX = Paddle.Size.X * ScaleFactor;
            real32 PaddleScaleY = Paddle.Size.Y/2 * ScaleFactor;

            Paddle.VisualSize.X = Paddle.Size.X + PaddleScaleX;
            Paddle.VisualSize.Y = Paddle.Size.Y - PaddleScaleY;

            Paddle.P.X = Clamp(LeftMarginForPaddle, PaddleDesiredP.X, RightMarginForPaddle);
        }

        // DrawRectAlpha(Buffer, Paddle.P, Paddle.Size, 0xffffff, 0.1);
        DrawRect(Buffer, GlobalCameraP, Paddle.VisualP, Paddle.VisualSize,Paddle.Color);
    }




    DrawConsoleMessages(Buffer, &Arena, dt);


    if(IS_CHANGED(Input, BUTTON_ACTION)) { 
        if(GlobalGameMode == GAME_MODE_GAMEPLAY) {
            ChangeGameMode(GameMemory, GAME_MODE_MENU);
        } else if (GlobalGameMode == GAME_MODE_MENU) {
            ChangeGameMode(GameMemory, GAME_MODE_GAMEPLAY);
        }
    }

    static real32 t = 1.0f;
    t -= 0.005;
    if (t < 0) { t = 1.0f - t; }

    // DrawStringGradient(Buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", V2(-80,-20), 5, 0xffffff, 0x000000, t);
    // DrawStringGradient(Buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", V2(-80,-20), 5, 0xffda7f, 0xff0080, 0.1);
    // DrawStringGradient(Buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", V2(-80,-20), 5, 0xffffff, 0x000000, t);


    // ###    HUD    ###
    {
        v2 ImageP = {-85, 35};
        v2 ImageSize = {5, 5};

        v2 NumberP = { ImageP.X + ImageSize.X, ImageP.Y - 1 };
        real32 Offset = 2.0;

        static real32 b = 1234.0f;

        if(InvincibilityTime > 0) {
            // Invincibility
            DrawBitmap(Buffer, &GlobalAssets.Images.PowerupInvincibility, GlobalCameraP, ImageP, V2(5, 5));
            DrawNumberReal(Buffer, InvincibilityTime, GlobalCameraP, NumberP, 1.5, 0xffffffff);
        }

        if(GlobalNumberOfTripleShots > 0) {
            NumberP.Y -= ImageSize.Y + Offset;
            ImageP.Y -= ImageSize.Y + Offset;
            DrawBitmap(Buffer, &GlobalAssets.Images.PowerupTripleShot, GlobalCameraP, ImageP, V2(5, 5));
            DrawNumber(Buffer, GlobalNumberOfTripleShots, GlobalCameraP, NumberP, 1.5, 0xffffffff);
        }

        if(CometTime > 0) {
            NumberP.Y -= ImageSize.Y + Offset;
            ImageP.Y -= ImageSize.Y + Offset;
            DrawBitmap(Buffer, &GlobalAssets.Images.PowerupComet, GlobalCameraP, ImageP, V2(5, 5));
            DrawNumberReal(Buffer, CometTime, GlobalCameraP, NumberP, 1.5, 0xffffffff);
        }
    }


    DrawRect(Buffer, GlobalCameraP, V2(Arena.Margin.Left, 0), V2(1, 20), 0xffff00);
    DrawRect(Buffer, GlobalCameraP, V2(Arena.Margin.Right, 0), V2(1, 20), 0xffff00);
    DrawRect(Buffer, GlobalCameraP, V2(0, Arena.Margin.Top), V2(20, 1), 0xffff00);
    DrawRect(Buffer, GlobalCameraP, V2(0, Arena.Margin.Bottom), V2(20, 1), 0xffff00);


#if DEVELOPMENT
    if(IS_CHANGED(Input, BUTTON_LEFT)) { StartGame((level)(GlobalGameState.CurrentLevel - 1)); }
    if(IS_CHANGED(Input, BUTTON_RIGHT)) { StartGame((level)(GlobalGameState.CurrentLevel + 1)); }

    // static v2 Test = {0, 0};
    // if(IS_PRESSED(Input, BUTTON_LEFT)) { Test.X -= 1; printf("%.2f %.2f\n", Test.X, Test.Y); }
    // if(IS_PRESSED(Input, BUTTON_RIGHT)) { Test.X += 1; printf("%.2f %.2f\n", Test.X, Test.Y); }
    // if(IS_PRESSED(Input, BUTTON_UP)) { Test.Y -= 1; printf("%.2f %.2f\n", Test.X, Test.Y); }
    // if(IS_PRESSED(Input, BUTTON_DOWN)) { Test.Y += 1; printf("%.2f %.2f\n", Test.X, Test.Y); }
    // DrawRect(Buffer, GlobalCameraP, Test, V2(1, 1), 0x000000);
    
    // if(IS_PRESSED(Input, BUTTON_DOWN)) { 
    //     static int test = 0;
    //     printf("test: %d\n", test);
    //     ConsolePrintInt(test++);
    // }

    if(IS_CHANGED(Input, BUTTON_UP)) { 
        if(InvincibilityTime > 0) { InvincibilityTime = 0.0f; }
        else { InvincibilityTime = 20.0f; }
    }
#endif

}
