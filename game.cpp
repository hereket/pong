
#include "software_renderer.cpp"
#include "collision.cpp"

#define BALL_ACTIVE 0x1
#define BALL_DESTROYED_ON_DP_Y_DOWN 0x2


struct {
    v2 P;
    v2 dP;
    v2 Size;
    u32 Color;
} typedef paddle;

struct {
    u32 Flags;
    v2 P;
    v2 dP;
    v2 Size;
    v2 DesiredP;
    u32 Color;
} typedef ball;

struct {
    v2 P;
    v2 Size;
} typedef arena;

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

struct {
    v2 P;
    v2 Size;

    s32 Life;
    u32 Color;

    power Power;
} typedef block;


struct {
    power Powerups[16];
    s32 Next;

    // real32 InvincibilityTime;
    s32 NumberOfTripleShots;
}typedef powerups_state;

// struct {
//     union {
//         powerups_state Pow;
//     };
// } typedef game_mode_state;


#define BLOCK_COUNT 512
global_variable block BlockList[BLOCK_COUNT];

global_variable bool32 Initialized;
global_variable paddle Paddle;
// global_variable ball Ball;
global_variable arena Arena;
global_variable real32 Speed;
global_variable bool32 FirstBallMovement;
global_variable level GlobalCurrentLevel;
global_variable s32 BlockTotalCount;
global_variable bool32 AdvanceGameMode;
global_variable ball Balls[16];
global_variable s32 GlobalNumberOfTripleShots;
global_variable real32 InvincibilityTime;
global_variable real32 CometTime;
global_variable real32 StrongBlocksTime;
global_variable real32 InvertedControlsTime;

// global_variable game_mode_state GameModeState;
global_variable power PowerBlocks[16];
global_variable s32 NextPowerBlock;
global_variable v2 PowerBlockSize;


void
SimulateGameMode(game_render_buffer *Buffer)
{
    for(s32 Index = 0; Index < ARRAY_COUNT(PowerBlocks); Index++) {
        power *Power = &PowerBlocks[Index];
        if(Power->Type == POWER_INACTIVE) {
            continue;
        }

        if(Power->Type < POWER_LIMIT_BETWEEN_BAD_AND_GOOD) {
            DrawRect(Buffer, Power->P, V2(2, 2), 0xf05454);
        } else {
            DrawRect(Buffer, Power->P, V2(2, 2), 0xffff00);
        }
    }

    switch(GlobalCurrentLevel){
        case LEVEL_02_WALL: 
        {
        } break;

        default: { }
    }

}

void
SpawnPowerup(v2 P) 
{
    power *Power = PowerBlocks + NextPowerBlock;
    NextPowerBlock++;
    if(NextPowerBlock >= ARRAY_COUNT(PowerBlocks)) {
        NextPowerBlock = 0;
    }
    Power->P = P;
    // Power->Type = POWER_INVINCIBILITY;
    // Power->Type = POWER_TRIPLE_SHOT;
    // Power->Type = POWER_COMET;
    // Power->Type = POWER_INSTAKILL;
    // Power->Type = POWER_STRONG_BLOCKS;
    Power->Type = POWER_INVERTED_CONTROLS;
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

    SpawnPowerup(Block->P);

    switch(GlobalCurrentLevel) {
        case LEVEL_01_NORMAL: 
        {
            // SpawnPowerup(Block->P);
        } break;

        default: 
        {
        } break; 
    }
}


internal void 
StartGame(level GameMode)
{
    Speed = 30.0f;

    GlobalCurrentLevel = GameMode;
    AdvanceGameMode = false;

    FirstBallMovement = 1;

    Balls[0].P = V2(10, -40);
    Balls[0].dP = V2(0.0, 1.4);
    Balls[0].Size = V2(1, 1);
    Balls[0].Color = 0x00FFFFFF;
    Balls[0].Flags |= BALL_ACTIVE;

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

        case LEVEL_02_WALL:
        {
        } break;

        case LEVEL_03_STADIUM:
        {
        } break;

        case LEVEL_04_CHESS:
        {
        } break;

        case LEVEL_05_PONG:
        {
        } break;

        case LEVEL_06_INVADERS:
        {
        } break;

        default: {
        };
    }
}

void
SpawnTripleShotsBalls(v2 Origin)
{
    ball *Ball = 0;
    for(s32 i = 0; i < 2; i++) {
        for(ball *TempBall = Balls; TempBall != Balls + ARRAY_COUNT(Balls); TempBall++) {
            if(!(TempBall->Flags & BALL_ACTIVE)) {
                Ball = TempBall;
            }
        }

        if(Ball) {
            ZERO_STRUCT(*Ball);
            Ball->P.X = Origin.X;
            Ball->P.Y = Origin.Y;
            Ball->Size = V2(1,1);
            Ball->Color = 0xcccccc;
            Ball->Flags = BALL_ACTIVE | BALL_DESTROYED_ON_DP_Y_DOWN;
            Ball->dP.Y = -1;
            Ball->dP.X = 1;
        }

    }
    if(Ball) {
        Ball->dP.X = -1;
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
SimulateGame(game_render_buffer *Buffer, game_input *Input, real32 dt)
{
    if(!Initialized) {
        Initialized = true;
        GlobalCurrentLevel = LEVEL_01_NORMAL;
        PowerBlockSize = V2(2, 2);

        StartGame(GlobalCurrentLevel);
    }

    v2 PaddleDesiredP = PixelToWorldCoord(Buffer, Input->MouseP);
    PaddleDesiredP.Y = Paddle.P.Y;
    if(InvertedControlsTime > 0) {
        PaddleDesiredP.X *= -1;
    }

    Paddle.dP.X = PaddleDesiredP.X - Paddle.P.X;

    for(ball *Ball = Balls; Ball != Balls + ARRAY_COUNT(Balls); Ball++)
    {
        if(!(Ball->Flags & BALL_ACTIVE)) { continue; }

        Ball->DesiredP = Ball->P + (Ball->dP * dt * Speed);
        // v2 BallDesiredP = Ball.P + (Ball.dP * dt); 

        bool32 BallXOverRight  = Ball->DesiredP.X + (Ball->Size.X/2) >  (Arena.Size.X*0.5);
        bool32 BallXOverLeft   = Ball->DesiredP.X - (Ball->Size.X/2) < -(Arena.Size.X*0.5);
        bool32 BallYOverBottom = Ball->DesiredP.Y + (Ball->Size.Y/2) >  (Arena.Size.Y*0.5);
        bool32 YOverTop        = Ball->DesiredP.Y - (Ball->Size.Y/2) < -(Arena.Size.Y*0.5);

        if((YOverTop)) {
            ProcessBallOnDpYDown(Ball);
            if(YOverTop)        { Ball->DesiredP.Y = -Arena.Size.Y*0.5 + Ball->Size.Y*0.5; }
            if(BallYOverBottom) { Ball->DesiredP.Y =  Arena.Size.Y*0.5 - Ball->Size.Y*0.5; }
            Ball->dP.Y *= -1;
        } else if((BallXOverRight) || (BallXOverLeft)) {
            ProcessBallOnDpYDown(Ball);
            if(BallXOverRight) { Ball->DesiredP.X =  Arena.Size.X*0.5 - Ball->Size.X*0.5; }
            if(BallXOverLeft)  { Ball->DesiredP.X = -Arena.Size.X*0.5 + Ball->Size.X*0.5; }
            Ball->dP.X *= -1;
        }
        if(InvincibilityTime > 0) {
            if(BallYOverBottom) { 
                Ball->DesiredP.Y =  Arena.Size.Y*0.5 - Ball->Size.Y*0.5; 
                Ball->dP.Y *= -1;
            }
        }

        for(s32 Index = 0; Index < BLOCK_COUNT; Index++)
        {
            block *Block = BlockList + Index;
            if(Block->Life) {
                if(!FirstBallMovement) {
                    static int test = 0;
                    collision_type Collision = CollisionSide(Block->P, Block->Size, V2(Ball->DesiredP.X, Ball->DesiredP.Y), Ball->Size);
                    if(COLLISION_NONE != CollisionSide(Block->P, Block->Size, V2(Ball->DesiredP.X, Ball->DesiredP.Y), Ball->Size)) {

                        ProcessBallOnDpYDown(Ball);

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
            }
        }

        if(IsColliding(Ball->DesiredP, Ball->Size, PaddleDesiredP, Paddle.Size)) {
            FirstBallMovement = false;

            Ball->dP.Y *= -1;

            float MaxMoveSpeedX = 3;
            float MinusOneToOneRange = -1 * ((Paddle.P.X - Ball->DesiredP.X) / ((float)Paddle.Size.X / 2));
            Ball->dP.X = Clamp(-MaxMoveSpeedX, MaxMoveSpeedX * MinusOneToOneRange, MaxMoveSpeedX);

            if(GlobalNumberOfTripleShots > 0) {
                GlobalNumberOfTripleShots--;
                SpawnTripleShotsBalls(Ball->DesiredP);
            }
        } else {
            Ball->P.X = Ball->DesiredP.X;
            Ball->P.Y = Ball->DesiredP.Y;
        }
    }

    for(power *Power = PowerBlocks; 
        Power != PowerBlocks + ARRAY_COUNT(PowerBlocks);
        Power++) 
    {
        if(Power->Type == POWER_INACTIVE) { continue; }
        Power->P.Y += 1*dt*Speed;

        if(IsColliding(Power->P, PowerBlockSize, Paddle.P, Paddle.Size)) {
            // Paddle.Color = 0x00000000;
            switch(Power->Type) {
                case POWER_INVINCIBILITY: {
                    InvincibilityTime += 5.0f;
                } break;

                case POWER_COMET: {
                    CometTime += 5.0f;
                } break;

                case POWER_TRIPLE_SHOT: {
                    GlobalNumberOfTripleShots++;
                } break;

                case POWER_INSTAKILL: 
                {
                    StartGame(GlobalCurrentLevel);
                } break;

                case POWER_STRONG_BLOCKS: 
                {
                    StrongBlocksTime += 5.0f;
                } break;

                case POWER_INVERTED_CONTROLS: 
                {
                    InvertedControlsTime += 5.0f;
                } break;

                default: {
                }break;
            }

            Power->Type = POWER_INACTIVE;
        }

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

    if(InvincibilityTime > 0) { Paddle.Color = 0x00ffffff;}
    else                      { Paddle.Color = 0x0000ff00; }

    DrawRect(Buffer, Paddle.P, Paddle.Size, Paddle.Color);

    for(ball *Ball = Balls; Ball != Balls + ARRAY_COUNT(Balls); Ball++) {
        if(!(Ball->Flags & BALL_ACTIVE)) { continue; }
        DrawRect(Buffer, V2(Ball->P.X, Ball->P.Y), Ball->Size, Ball->Color);
    }

    if(Balls[0].P.Y > (Arena.Size.Y / 2)) {
        StartGame(GlobalCurrentLevel);
    }

    if(AdvanceGameMode) {
        GlobalCurrentLevel = (level)(GlobalCurrentLevel + 1);
        StartGame(GlobalCurrentLevel);
    }

    if(InvincibilityTime > 0) { InvincibilityTime -= dt; }
    if(CometTime > 0) { CometTime -= dt; }
    if(StrongBlocksTime > 0) { StrongBlocksTime -= dt; }
    if(InvertedControlsTime > 0) { InvertedControlsTime -= dt; }


#if DEVELOPMENT
    if(IS_CHANGED(Input, BUTTON_LEFT)) { StartGame((level)(GlobalCurrentLevel - 1)); }
    if(IS_CHANGED(Input, BUTTON_RIGHT)) { StartGame((level)(GlobalCurrentLevel + 1)); }
    if(IS_CHANGED(Input, BUTTON_UP)) { 
        if(InvincibilityTime > 0) { InvincibilityTime = 0.0f; }
        else { InvincibilityTime = 20.0f; }
    }
    // if(IS_PRESSED(Input, BUTTON_DOWN))  { Paddle.P.Y += Speed; }
#endif
}
