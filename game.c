
#include "software_renderer.c"
#include "collision.c"

struct {
    /* real32 X; */
    /* real32 Y; */
    v2 P;
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

#define BLOCK_COUNT 256
global_variable block BlockList[BLOCK_COUNT];

global_variable bool32 Initialized;
global_variable paddle Paddle;
global_variable ball Ball;
global_variable arena Arena;

s32 
RoundBottom(real32 Value)
{
    s32 Result = (s32)(Value + 0.5);
    return Result;
}


void SimulateGame(game_render_buffer *Buffer, game_input *Input, real32 dt)
{
    real32 Speed = 100 * dt;

    if(!Initialized)  {
        Initialized = true;
        Ball.P = V2(10, -40);
        Ball.dP = V2(0.3, 0.4);
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

        int BlocksFullWidth = ArenaWidth - 10;
        int BlocksFullHeight = ArenaHeight - 10;

        real32 BlockOffsetX = 0.3;
        real32 BlockOffsetY = 0.3;
        real32 BlockSizeX = (BlocksFullWidth / 10) - BlockOffsetX;
        real32 StartX = -BlocksFullWidth/2 + ((BlockSizeX+BlockOffsetX)/2);
        real32 StartY = -ArenaHeight/2 + 10;

        for(s32 Y = 0; Y < 10; Y++) {
            int YColorStep = 255 / 10;
            int XColorStep = 255 / 10;
            for(s32 X = 0; X < 10; X++) {
                block *Block = BlockList + BlockIndex++;
                if(BlockIndex >= BLOCK_COUNT) { BlockIndex = 0; }
                Block->Life = 1;
                /* Block->Color = 0xffffffff; */
                Block->Color = 
                    /* (0xff << 16) |  */
                    (Y*YColorStep << 8) | 
                    (X*XColorStep);
                Block->Size.X = BlockSizeX;
                Block->Size.Y = 3;
                Block->P.X = StartX + X * (Block->Size.X + BlockOffsetX);
                Block->P.Y = StartY + Y * (Block->Size.Y + BlockOffsetY);
            }
        }
    }


    if(IS_PRESSED(Input, BUTTON_DOWN))  { Paddle.P.Y += Speed; }
    if(IS_PRESSED(Input, BUTTON_UP))    { Paddle.P.Y -= Speed; }
    if(IS_PRESSED(Input, BUTTON_LEFT))  { Paddle.P.X -= Speed; }
    if(IS_PRESSED(Input, BUTTON_RIGHT)) { Paddle.P.X += Speed; }

    real32 ExpectedX = Ball.P.X + Ball.dP.X;
    real32 ExpectedY = Ball.P.Y + Ball.dP.Y;

    real32 e = 0.0f;
    bool32 XOverRight  = ExpectedX + (Ball.Size.X/2 - e) >  (Arena.Size.X*0.5);
    bool32 XOverLeft   = ExpectedX - (Ball.Size.X/2 - e) < -(Arena.Size.X*0.5);
    bool32 YOverBottom = ExpectedY + (Ball.Size.Y/2 - e) >  (Arena.Size.Y*0.5);
    bool32 YOverTop    = ExpectedY - (Ball.Size.Y/2 - e) < -(Arena.Size.Y*0.5);

    if((YOverBottom) || (YOverTop)) 
    /* if((YOverTop))   */
    {
        if(YOverTop)    { ExpectedY = -Arena.Size.Y*0.5 + Ball.Size.Y*0.5; }
        if(YOverBottom) { ExpectedY =  Arena.Size.Y*0.5 - Ball.Size.Y*0.5; }
        Ball.dP.Y *= -1;
    }

    if((XOverRight) || (XOverLeft)) 
    {
        if(XOverRight) { ExpectedX =  Arena.Size.X*0.5 - Ball.Size.X*0.5; }
        if(XOverLeft)  { ExpectedX = -Arena.Size.X*0.5 + Ball.Size.X*0.5; }
        Ball.dP.X *= -1;
    }


    for(s32 Index = 0; Index < BLOCK_COUNT; Index++)
    {
        block *Block = BlockList + Index;
        if(Block->Life) {
            static int test = 0;
            enum collision_type collision = CollisionSide(Block->P, Block->Size, V2(ExpectedX, ExpectedY), Ball.Size);
            if(COLLISION_NONE != CollisionSide(Block->P, Block->Size, V2(ExpectedX, ExpectedY), Ball.Size)) {
                Block->Color = 0x00000000;
                Block->Life -= 1;
                if(collision == COLLISION_LEFT) { 
                    Ball.dP.X *= -1;
                }
                if(collision == COLLISION_RIGHT) {
                    Ball.dP.X *= -1;
                }
                if(collision == COLLISION_TOP) {
                    Ball.dP.Y *= -1;
                }
                if(collision == COLLISION_BOTTOM) {
                    Ball.dP.Y *= -1;
                }

                break;
            }
        }
    }

    v2 BallExpectedP = {ExpectedX, ExpectedY};
    if(IsColliding(BallExpectedP, Ball.Size, Paddle.P, Paddle.Size)) {
        /* Ball.dP.X *= -1; */
        Ball.dP.Y *= -1;

        /* Ball.P.X = Paddle.P.X - (Paddle.Size.X/2); */
        /* Ball.P.Y = Paddle.P.Y + Paddle.Size.Y/2); */

        /* Ball.Color = 0x00ffffff; */
    } else {
        /* Ball.Color = 0x00000000; */
        Ball.P.X = ExpectedX;
        Ball.P.Y = ExpectedY;
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

    /* v2 Position = {Paddle.P.X, Paddle.P.Y}; */
    /* v2 Size = {2, 2}; */
    /* DrawRect(Buffer, Paddle.P, Paddle.Size, 0x0000FF00); */

    v2 P = PixelToWorldCoord(Buffer, Input->MouseP);
    Paddle.P.X = P.X;

    DrawRect(Buffer, Paddle.P, Paddle.Size, Paddle.Color);
    DrawRect(Buffer, V2(Ball.P.X, Ball.P.Y), Ball.Size, Ball.Color);

    /* DrawRect(Buffer, V2(P.X, 0), V2(98, 98), 0x0000FF00); */
}
