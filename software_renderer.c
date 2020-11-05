
#include "game.h"

real32 Scale = 0.01;

real32
CalculateAspectMultiplier(game_render_buffer *Buffer)
{
    real32 AspectMultiplier = Buffer->Height;
    real32 AspectRatio = 16 / 9; // 1.77
    if(((real32)Buffer->Width / (real32)Buffer->Height) < AspectRatio)
    {
        AspectMultiplier = (real32)Buffer->Width / AspectRatio;
    }
    return AspectMultiplier;
}

v2 
PixelToWorldCoord(game_render_buffer *Buffer, v2 P)
{
    v2 Result = {};
    real32 AspectMultiplier = CalculateAspectMultiplier(Buffer);

    Result.X = P.X - Buffer->Width*0.5;
    Result.Y = P.Y - Buffer->Height*0.5;

    Result.X /= AspectMultiplier;
    Result.X /= Scale;

    Result.Y /= AspectMultiplier;
    Result.Y /= Scale;

    return Result;
}


void
DrawRectInPixels(game_render_buffer *Buffer, int DesiredStartX, int DesiredStartY, int Width, int Height, u32 Color)
{
    int StartX = Clamp(0, Buffer->Width, DesiredStartX);
    int StartY = Clamp(0, Buffer->Height, DesiredStartY);
    int EndX = Clamp(0, Buffer->Width, DesiredStartX + Width);
    int EndY = Clamp(0, Buffer->Height, DesiredStartY + Height);

    for(int Y = StartY; Y < EndY; Y++) {
        u8 *Row = Buffer->Pixels + Y*Buffer->Pitch;
        for(int X = StartX; X < EndX; X++) {
            u32 *Pixel = (u32 *)Row + X;
            *Pixel = Color;
        }
    }
}



void
DrawRect(game_render_buffer *Buffer, v2 Position, v2 Size, u32 Color)
{
    real32 AspectMultiplier = CalculateAspectMultiplier(Buffer);
    real32 StepSize = Scale * AspectMultiplier;

    Size.X     *= StepSize;
    Size.Y     *= StepSize;
    Position.X *= StepSize;
    Position.Y *= StepSize;
    Position.X += Buffer->Width * 0.5;
    Position.Y += Buffer->Height * 0.5;

    s32 X0 = Position.X - (Size.X * 0.5); 
    s32 Y0 = Position.Y - (Size.Y * 0.5); 
    s32 SizeX = Size.X;
    s32 SizeY = Size.Y;

    DrawRectInPixels(Buffer, X0, Y0, SizeX, SizeY, Color);
}

void
ClearScreen(game_render_buffer *Buffer, u32 Color) {
    int StartX = 0;
    int StartY = 0;
    int EndX = StartX + Buffer->Width;
    int EndY = StartY + Buffer->Height;
    DrawRectInPixels(Buffer, StartX, StartY, Buffer->Width, Buffer->Height, Color);
}
