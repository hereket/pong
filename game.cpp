
#include "software_renderer.cpp"
#include "collision.cpp"

struct {
    v2 P;
    v2 dP;
    v2 Size;
    u32 Color;
} typedef paddle;

struct {
    v2 P;
    v2 dP;
    v2 Size;
    u32 Color;
} typedef ball;

struct {
    v2 P;
    v2 Size;
} typedef arena;

struct {
    v2 P;
    v2 Size;
    s32 Life;
    u32 Color;
} typedef block;

enum {
    GAMEMODE_NORMAL,
    GAMEMODE_SPACED,
    GAMEMODE_WALL,
    GAMEMODE_CONSTRUCTION,

    GM_COUNT,

    GM_PONG,
} typedef game_mode;

enum {
    POWERUP_INACTIVE,
    POWERUP_INVINCIBILITY,
    POWERUP_TRIPLE_SHOT,
} typedef powerup_type;

struct {
    powerup_type Type;
    v2 P;
} typedef powerup;

struct {
    powerup Powerups[16];
    s32 Next;

    real32 InvincibilityTime;
    s32 NumberOfTripleShots;
}typedef powerups_state;

struct {
    union {
        powerups_state Pow;
    };
} typedef game_mode_state;


#define BLOCK_COUNT 512
global_variable block BlockList[BLOCK_COUNT];

global_variable bool32 Initialized;
global_variable paddle Paddle;
global_variable ball Ball;
global_variable arena Arena;
global_variable real32 Speed;
global_variable bool32 FirstBallMovement;
global_variable game_mode CurrentGameMode;
global_variable s32 BlockTotalCount;
global_variable bool32 IsInvincible;
global_variable bool32 AdvanceGameMode;
global_variable game_mode_state GameModeState;


void
SimulateGameMode(game_render_buffer *Buffer)
{
    switch(CurrentGameMode){
        case GAMEMODE_WALL: 
        {
            for(s32 Index = 0; Index < ARRAY_COUNT(GameModeState.Pow.Powerups); Index++) {
                powerup *Powerup = &GameModeState.Pow.Powerups[Index];
                if(Powerup->Type == POWERUP_INACTIVE) {
                    continue;
                }

                DrawRect(Buffer, Powerup->P, V2(2, 2), 0xffff00);
            }
        } break;

        default: { }
    }

}

void
SpawnPowerup(v2 P) {
    powerup *Powerup = GameModeState.Pow.Powerups + GameModeState.Pow.Next;
    GameModeState.Pow.Next++;
    if(GameModeState.Pow.Next >= ARRAY_COUNT(GameModeState.Pow.Powerups)) {
        GameModeState.Pow.Next = 0;
    }
    Powerup->P = P;
    Powerup->Type = POWERUP_INVINCIBILITY;
}

void
TestForWinCondition()
{
    BlockTotalCount--; 
    if(BlockTotalCount <= 0) {
        AdvanceGameMode = true;
    }
}

void
BlockIsDestroyed(block *Block)
{
    TestForWinCondition();
    switch(CurrentGameMode) {
        case GAMEMODE_WALL: 
        {
            SpawnPowerup(Block->P);
        } break;

        default: 
        {
        } break; 
    }
}


internal void 
StartGame(game_mode GameMode)
{
    Speed = 30.0f;

    CurrentGameMode = GameMode;
    AdvanceGameMode = false;
    GameModeState = (game_mode_state){};

    FirstBallMovement = 1;

    Ball.P = V2(10, -40);
    Ball.dP = V2(0.0, 1.4);
    Ball.Size = V2(1, 1);
    Ball.Color = 0x00000000;

    int ArenaWidth = 160;
    int ArenaHeight = 90;
    Arena.P = V2(0, 0);
    Arena.Size = V2(ArenaWidth, ArenaHeight);

    Paddle.Size = V2(20, 2);
    Paddle.P.Y = 40;
    Paddle.Color = 0x0000ff00;

    s32 BlockIndex = 0;

    float BlocksFullWidth = ArenaWidth - 10;
    float BlocksFullHeight = ArenaHeight - 10;

    BlockTotalCount = 0;

    for(s32 Index = 0; Index < BLOCK_COUNT; Index++) {
        block *Block = BlockList + Index;
        Block->Life = 0;
    }

    switch(GameMode)
    {
        case GAMEMODE_NORMAL:
        {
            s32 BlockXCount = 19;
            s32 BlockYCount = 9;

            s32 YColorStep = 255 / BlockYCount;
            s32 XColorStep = 255 / BlockXCount;

            real32 BlockOffsetX = 0.4;
            real32 BlockOffsetY = 0.4;

            real32 BlockSizeX = (BlocksFullWidth / BlockXCount) - BlockOffsetX;

            real32 StartX = -BlocksFullWidth/2 + ((BlockSizeX+BlockOffsetX)/2);
            real32 StartY = -ArenaHeight/2 + 10;

            for(s32 Y = 0; Y < BlockYCount; Y++) {
                for(s32 X = 0; X < BlockXCount; X++) {
                    BlockTotalCount++; 
                    block *Block = BlockList + BlockIndex++;
                    if(BlockIndex >= BLOCK_COUNT) { BlockIndex = 0; }
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

        case GAMEMODE_SPACED:
        {
            s32 BlockXCount = 2;
            s32 BlockYCount = 2;

            s32 YColorStep = 255 / BlockYCount;
            s32 XColorStep = 255 / BlockXCount;

            real32 BlockOffsetX = 14.4;
            real32 BlockOffsetY = 14.4;

            real32 BlockSizeX = (BlocksFullWidth / BlockXCount) - BlockOffsetX;

            real32 StartX = -BlocksFullWidth/2 + ((BlockSizeX+BlockOffsetX)/2);
            real32 StartY = -ArenaHeight/2 + 10;

            for(s32 Y = 0; Y < BlockYCount; Y++) {
                for(s32 X = 0; X < BlockXCount; X++) {
                    BlockTotalCount++; 
                    block *Block = BlockList + BlockIndex++;
                    if(BlockIndex >= BLOCK_COUNT) { BlockIndex = 0; }
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

        case GAMEMODE_WALL:
        {
            s32 BlockXCount = 20;
            s32 BlockYCount = 9;

            s32 YColorStep = 255 / BlockYCount;
            s32 XColorStep = 255 / BlockXCount;

            real32 BlockOffsetX = 0.4;
            real32 BlockOffsetY = 0.4;

            real32 BlockSizeX = (BlocksFullWidth / BlockXCount) - BlockOffsetX;

            real32 StartX = -BlocksFullWidth/2 + ((BlockSizeX+BlockOffsetX)/2);
            real32 StartY = -ArenaHeight/2 + 10;

            for(s32 Y = 0; Y < BlockYCount; Y++) {
                for(s32 X = 0; X < BlockXCount; X++) {
                    BlockTotalCount++; 
                    block *Block = BlockList + BlockIndex++;
                    if(BlockIndex >= BLOCK_COUNT) { BlockIndex = 0; }
                    Block->Life = 1;
                    Block->Color = (Y*YColorStep << 8) | 
                        (X*XColorStep);
                    Block->Size.X = BlockSizeX;
                    Block->Size.Y = 3;

                    if(Y % 2) {
                        Block->P.X = StartX + X * (Block->Size.X + BlockOffsetX);
                    } else {
                        Block->P.X = StartX - Block->Size.X/2 + X * (Block->Size.X + BlockOffsetX);
                    }

                    Block->P.Y = StartY + Y * (Block->Size.Y + BlockOffsetY);
                }
            }

        } break;

        case GAMEMODE_CONSTRUCTION:
        {
            s32 BlockXCount = 20;
            s32 BlockYCount = 9;

            s32 YColorStep = 255 / BlockYCount;
            s32 XColorStep = 255 / BlockXCount;

            real32 BlockOffsetX = 0.4;
            real32 BlockOffsetY = 0.4;

            real32 BlockSizeX = (BlocksFullWidth / BlockXCount) - BlockOffsetX;

            real32 StartX = -BlocksFullWidth/2 + ((BlockSizeX+BlockOffsetX)/2);
            real32 StartY = -ArenaHeight/2 + 10;

            for(s32 Y = 0; Y < BlockYCount; Y++) {
                for(s32 X = 0; X < BlockXCount; X++) {
                    BlockTotalCount++; 
                    block *Block = BlockList + BlockIndex++;
                    if(BlockIndex >= BLOCK_COUNT) { BlockIndex = 0; }
                    Block->Life = 1;
                    Block->Color = (Y*YColorStep << 8) | 
                        (X*XColorStep);
                    Block->Size.X = BlockSizeX;
                    Block->Size.Y = 3;

                    Block->P.X = StartX + X * (Block->Size.X);
                    Block->P.Y = StartY + Y * (Block->Size.Y*2);
                }
            }

        } break;

        default: {
        };
    }
}


void 
SimulateGame(game_render_buffer *Buffer, game_input *Input, real32 dt)
{
    dt *= Speed;

    if(!Initialized) {
        CurrentGameMode = GAMEMODE_NORMAL;
        Initialized = true;
        StartGame(CurrentGameMode);
    }

    // if(IS_PRESSED(Input, BUTTON_DOWN))  { Paddle.P.Y += Speed; }
    // if(IS_PRESSED(Input, BUTTON_UP))    { Paddle.P.Y -= Speed; }
    // if(IS_PRESSED(Input, BUTTON_LEFT))  { Paddle.P.X -= Speed; }
    // if(IS_PRESSED(Input, BUTTON_RIGHT)) { Paddle.P.X += Speed; }

    v2 PaddleDesiredP = PixelToWorldCoord(Buffer, Input->MouseP);
    PaddleDesiredP.Y = Paddle.P.Y;
    v2 BallDesiredP = Ball.P + (Ball.dP * dt); 

    Paddle.dP.X = PaddleDesiredP.X - Paddle.P.X;

    real32 e = 0.0f;
    bool32 BallXOverRight  = BallDesiredP.X + (Ball.Size.X/2 - e) >  (Arena.Size.X*0.5);
    bool32 BallXOverLeft   = BallDesiredP.X - (Ball.Size.X/2 - e) < -(Arena.Size.X*0.5);
    bool32 BallYOverBottom = BallDesiredP.Y + (Ball.Size.Y/2 - e) >  (Arena.Size.Y*0.5);
    bool32 YOverTop        = BallDesiredP.Y - (Ball.Size.Y/2 - e) < -(Arena.Size.Y*0.5);

    // if((BallYOverBottom) || (YOverTop)) 
    if((YOverTop))  
    {
        if(YOverTop)        { BallDesiredP.Y = -Arena.Size.Y*0.5 + Ball.Size.Y*0.5; }
        if(BallYOverBottom) { BallDesiredP.Y =  Arena.Size.Y*0.5 - Ball.Size.Y*0.5; }
        Ball.dP.Y *= -1;
    } 
    else if((BallXOverRight) || (BallXOverLeft)) 
    {
        if(BallXOverRight) { BallDesiredP.X =  Arena.Size.X*0.5 - Ball.Size.X*0.5; }
        if(BallXOverLeft)  { BallDesiredP.X = -Arena.Size.X*0.5 + Ball.Size.X*0.5; }
        Ball.dP.X *= -1;
    }
    if(IsInvincible) {
        if(BallYOverBottom) { 
            BallDesiredP.Y =  Arena.Size.Y*0.5 - Ball.Size.Y*0.5; 
            Ball.dP.Y *= -1;
        }
    }

    for(s32 Index = 0; Index < BLOCK_COUNT; Index++)
    {
        block *Block = BlockList + Index;
        if(Block->Life) {
            if(!FirstBallMovement) {
                static int test = 0;
                collision_type Collision = CollisionSide(Block->P, Block->Size, V2(BallDesiredP.X, BallDesiredP.Y), Ball.Size);
                if(COLLISION_NONE != CollisionSide(Block->P, Block->Size, V2(BallDesiredP.X, BallDesiredP.Y), Ball.Size)) {
                    Block->Color = 0x00000000;
                    Block->Life -= 1;
                    if(Collision == COLLISION_LEFT) { 
                        Ball.dP.X *= -1;
                    }
                    if(Collision == COLLISION_RIGHT) {
                        Ball.dP.X *= -1;
                    }
                    if(Collision == COLLISION_TOP) {
                        Ball.dP.Y *= -1;
                    }
                    if(Collision == COLLISION_BOTTOM) {
                        Ball.dP.Y *= -1;
                    }

                    BlockIsDestroyed(Block);

                    break;
                }
            }
        }
    }

    if(IsColliding(BallDesiredP, Ball.Size, PaddleDesiredP, Paddle.Size)) {
        Ball.dP.Y *= -1;
        Ball.dP.X += Paddle.dP.X;
        FirstBallMovement = false;
    } else {
        /* Ball.Color = 0x00000000; */
        Ball.P.X = BallDesiredP.X;
        Ball.P.Y = BallDesiredP.Y;
    }



    // 
    // Rendering
    //

    ClearScreen(Buffer, 0x00551100);
    DrawRect(Buffer, Arena.P, Arena.Size, 0x00aa0000);

    for(s32 Index = 0; Index < BLOCK_COUNT; Index++)
    {
        block *Block = BlockList + Index;
        if(Block->Life) 
        {
            DrawRect(Buffer, Block->P, Block->Size, Block->Color);
        }
    }

    Paddle.P = PaddleDesiredP;

    SimulateGameMode(Buffer);

    if(IsInvincible) { Paddle.Color = 0x00ffffff;}
    else             { Paddle.Color = 0x0000ff00; }

    DrawRect(Buffer, Paddle.P, Paddle.Size, Paddle.Color);
    DrawRect(Buffer, V2(Ball.P.X, Ball.P.Y), Ball.Size, Ball.Color);

    if(Ball.P.Y > (Arena.Size.Y / 2)) {
        StartGame(CurrentGameMode);
    }

    if(AdvanceGameMode) {
        CurrentGameMode = (game_mode)(CurrentGameMode + 1);
        StartGame(CurrentGameMode);
    }

#if DEVELOPMENT
    if(IS_CHANGED(Input, BUTTON_LEFT)) { StartGame((game_mode)(CurrentGameMode - 1)); }
    if(IS_CHANGED(Input, BUTTON_RIGHT)) { StartGame((game_mode)(CurrentGameMode + 1)); }
    if(IS_CHANGED(Input, BUTTON_UP)) { IsInvincible = !IsInvincible;}
    // if(IS_PRESSED(Input, BUTTON_DOWN))  { Paddle.P.Y += Speed; }
#endif
}
