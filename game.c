
#include "software_renderer.c"

struct {
    real32 X;
    real32 Y;
} typedef paddle;

struct {
    v2 P;
    v2 dP;
    v2 Size;
} typedef ball;

struct {
    v2 P;
    v2 Size;
} typedef arena;

global_variable bool32 Initialized;
global_variable paddle Paddle;
global_variable ball Ball;
global_variable arena Arena;

s32 
RoundBottom(real32 Value)
{
    s32 Result = (int)(Value + 0.5);
    return Result;
}


void SimulateGame(game_render_buffer *Buffer, game_input *Input, real32 dt)
{
    if(!Initialized)  {
        Initialized = true;
        Ball.P = V2(10, -40);
        Ball.dP = V2(0.0, 0.9);
        Ball.Size = V2(2, 2);

        Arena.P = V2(0, 0);
        Arena.Size = V2(90 * 16/9, 90);
    }

#define IS_PRESSED(Input, ButtonId) \
    Input->Buttons[ButtonId].IsDown

#define IS_CHANGED(Input, ButtonId) \
    (Input->Buttons[ButtonId].IsDown && Input->Buttons[ButtonId].Changed)

    real32 Speed = 100 * dt;

    if(IS_CHANGED(Input, BUTTON_DOWN))  { Paddle.Y += Speed; }
    if(IS_CHANGED(Input, BUTTON_UP))    { Paddle.Y -= Speed; }
    if(IS_CHANGED(Input, BUTTON_LEFT))  { Paddle.X -= Speed; }
    if(IS_CHANGED(Input, BUTTON_RIGHT)) { Paddle.X += Speed; }

    real32 ExpectedX = Ball.P.X + Ball.dP.X;
    real32 ExpectedY = Ball.P.Y + Ball.dP.Y;

    real32 e = 0.0f;
    bool32 XOverRight  = ExpectedX + (Ball.Size.X/2 - e) >  (Arena.Size.X*0.5);
    bool32 XOverLeft   = ExpectedX - (Ball.Size.X/2 - e) < -(Arena.Size.X*0.5);
    bool32 YOverBottom = ExpectedY + (Ball.Size.Y/2 - e) >  (Arena.Size.Y*0.5);
    bool32 YOverTop    = ExpectedY - (Ball.Size.Y/2 - e) < -(Arena.Size.Y*0.5);

    if((YOverBottom) || (YOverTop)) 
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

    Ball.P.X = ExpectedX;
    Ball.P.Y = ExpectedY;




    // 
    // Rendering
    //
    ClearScreen(Buffer, 0x00551100);
    DrawRect(Buffer, Arena.P, Arena.Size, 0x00aa0000);

    v2 Position = {Paddle.X, Paddle.Y};
    v2 Size = {2, 2};
    DrawRect(Buffer, Position, Size, 0x0000FF00);

    v2 P = PixelToWorldCoord(Buffer, Input->MouseP);

    DrawRect(Buffer, V2(P.X, 45), V2(20, 2), 0x0000FF00);
    DrawRect(Buffer, V2(Ball.P.X, Ball.P.Y), Ball.Size, 0x0000FF00);

}
