
#include "game.h"

real32 Scale = 0.01;

real32
CalculateAspectMultiplier(game_render_buffer *Buffer)
{
    real32 AspectMultiplier = Buffer->Height;
    real32 AspectRatio = 16.0f / 9.0f; // 1.77
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
    int StartX = Clamp(0, DesiredStartX, Buffer->Width);
    int StartY = Clamp(0, DesiredStartY, Buffer->Height);
    int EndX   = Clamp(0, DesiredStartX + Width, Buffer->Width);
    int EndY   = Clamp(0, DesiredStartY + Height, Buffer->Height);

    // for(int Y = StartY; Y <= EndY; Y++) {
    for(int Y = StartY; Y < EndY; Y++) {
        u8 *Row = Buffer->Pixels + Y*Buffer->Pitch;
        for(int X = StartX; X < EndX; X++) { // TODO: Work out this logic
        // for(int X = StartX; X <= EndX; X++) {
            u32 *Pixel = (u32 *)Row + X;
            *Pixel = Color;
        }
    }
}

void
DrawRectInPixels(game_render_buffer *Buffer, float DesiredStartX, float DesiredStartY, float Width, float Height, u32 Color)
{
    // int StartX = floor(Clamp(0, DesiredStartX, Buffer->Width));
    // int StartY = floor(Clamp(0, DesiredStartY, Buffer->Height));
    // int EndX   = ceil(Clamp(0, DesiredStartX + Width, Buffer->Width));
    // int EndY   = ceil(Clamp(0, DesiredStartY + Height, Buffer->Height));

    int StartX = floor(Clamp(0, DesiredStartX, Buffer->Width));
    int StartY = floor(Clamp(0, DesiredStartY, Buffer->Height));
    int EndX   = ceil(Clamp(0, DesiredStartX + Width, Buffer->Width));
    int EndY   = ceil(Clamp(0, DesiredStartY + Height, Buffer->Height));

    for(int Y = StartY; Y < EndY; Y++) {
        u8 *Row = Buffer->Pixels + Y*Buffer->Pitch;
        for(int X = StartX; X < EndX; X++) { // TODO: Work out this logic
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

    float X0    = Position.X - (Size.X * 0.5);
    float Y0    = Position.Y - (Size.Y * 0.5);
    float SizeX = Size.X;
    float SizeY = Size.Y;

    DrawRectInPixels(Buffer, X0, Y0, SizeX, SizeY, Color);
}

void
ClearScreen(game_render_buffer *Buffer, u32 Color) 
{
    int StartX = 0;
    int StartY = 0;
    int EndX = StartX + Buffer->Width;
    int EndY = StartY + Buffer->Height;
    DrawRectInPixels(Buffer, StartX, StartY, Buffer->Width, Buffer->Height, Color);
}


#define CHAR_ROW_LENGTH 6
#define CHAR_COLUMN_COUNT 7
// #define CHAR_ROW_ELEM_COUNT CHAR_ROW_LENGTH - 1
typedef struct {
    char data[CHAR_COLUMN_COUNT][CHAR_ROW_LENGTH];
} character;

character AllChars[256] = {};
character Digits[] = {
    {
        "_111_",
        "1___1",
        "1___1",
        "1___1",
        "1___1",
        "1___1",
        "_111_",
    },
    {
        "__1__",
        "_11__",
        "__1__",
        "__1__",
        "__1__",
        "__1__",
        "11111",
    },
    {
        "_111_",
        "1___1",
        "____1",
        "___1_",
        "__1__",
        "_1___",
        "11111",
    },
    {
        "_111_",
        "1___1",
        "____1",
        "__11_",
        "____1",
        "1___1",
        "_111_",
    },
    {
        "_1__1",
        "_1__1",
        "1___1",
        "11111",
        "____1",
        "____1",
        "____1",
    },
    {
        "11111",
        "1____",
        "1____",
        "1111_",
        "____1",
        "1___1",
        "_111_",
    },
    {
        "_111_",
        "1____",
        "1____",
        "1111_",
        "1___1",
        "1___1",
        "_111_",
    },
    {
        "11111",
        "____1",
        "____1",
        "___1_",
        "__1__",
        "__1__",
        "__1__",
    },
    {
        "_111_",
        "1___1",
        "1___1",
        "_111_",
        "1___1",
        "1___1",
        "_111_",
    },
    {
        "_111_",
        "1___1",
        "1___1",
        "_1111",
        "____1",
        "1___1",
        "_111_",
    },
};



u32
CalculateDigitCount(s32 Number) {
    u32 DigitCount = 1;
    Number /= 10;
    while(Number != 0) {
        Number /= 10;
        DigitCount++;
    }
    return DigitCount;
}


void
DrawDigit(game_render_buffer *Buffer, char RequestedDigit, v2 P, real32 CharWidth, u32 Color) {
    real32 Size = CharWidth / (CHAR_ROW_LENGTH - 1); // TODO: Add variable to remove this -1
    real32 StartX = P.X;
    real32 StartY = P.Y;
    real32 Offset = 0.0;

    // StartX += Size/2;
    // StartY += Size/2;

    // s32 DigitIndex = Clamp(0, digit - '0', ARRAY_COUNT(Digits));
    s32 DigitIndex = Clamp(0, RequestedDigit, ARRAY_COUNT(Digits) - 1);
    character Digit = Digits[DigitIndex];

    for(int YIndex = 0; YIndex < ARRAY_COUNT(Digit.data); YIndex++) {
        for(int XIndex = 0; XIndex < (CHAR_ROW_LENGTH-1); XIndex++) {
            char Value = Digit.data[YIndex][XIndex];
            if(Value == '1')
            {
                real32 X = StartX + (XIndex * Size) + (XIndex * Offset);
                real32 Y = StartY + (YIndex * Size) + (YIndex * Offset);
                DrawRect(Buffer, V2(X, Y), V2(Size, Size), Color);
            }
        }
    }
}

void 
DrawNumber(game_render_buffer *Buffer, s32 Number, v2 P, real32 Size, u32 Color) {
    s32 IterateNumber = Number;
    u32 DigitCount = CalculateDigitCount(Number);

    real32 StartX = P.X;
    real32 StartY = P.Y;
    real32 Offset = (Size / (CHAR_ROW_LENGTH - 1));
    real32 CharWidthWithRightOffset = Size + Offset;

    bool32 FirstPass = true;

    while(FirstPass | IterateNumber) {
        FirstPass = false;

        char Digit = IterateNumber % 10;
        IterateNumber /= 10;
        real32 X = StartX + CharWidthWithRightOffset * (DigitCount - 1);
        real32 Y = StartY;

        DrawDigit(Buffer, Digit, V2(X, Y), Size, Color);

        DigitCount--;
    }
}
