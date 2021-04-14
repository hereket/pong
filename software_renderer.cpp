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
DrawRectInPixels(game_render_buffer *Buffer, int DesiredStartX, int DesiredStartY, int Width, int Height, u32 SrcColor, real32 Alpha = 1.0)
{
    int StartX = Clamp(0, DesiredStartX, Buffer->Width);
    int StartY = Clamp(0, DesiredStartY, Buffer->Height);
    int EndX   = Clamp(0, DesiredStartX + Width, Buffer->Width);
    int EndY   = Clamp(0, DesiredStartY + Height, Buffer->Height);

    for(int Y = StartY; Y < EndY; Y++) {
        u8 *Row = Buffer->Pixels + Y*Buffer->Pitch;
        for(int X = StartX; X < EndX; X++) { // TODO: Work out this logic
            u32 *Pixel = (u32 *)Row + X;

            if(Alpha < 1) {
                u32 DestColor = *Pixel;
                u8 DestR = DestColor >> 16;
                u8 DestG = DestColor >> 8;
                u8 DestB = DestColor >> 0;

                u8 SrcR = SrcColor >> 16;
                u8 SrcG = SrcColor >> 8;
                u8 SrcB = SrcColor >> 0;

                u8 R = (DestR * (1 - Alpha)) + (SrcR * Alpha);
                u8 G = (DestG * (1 - Alpha)) + (SrcG * Alpha);
                u8 B = (DestB * (1 - Alpha)) + (SrcB * Alpha);

                u32 Color = (R << 16) | (G << 8) | (B << 0);

                *Pixel = Color;
            } else {
                *Pixel = SrcColor;
            }
        }
    }
}

void
DrawRectInPixels(game_render_buffer *Buffer, float DesiredStartX, float DesiredStartY, float Width, float Height, u32 SrcColor, real32 Alpha = 1.0f)
{
    int StartX = floor(Clamp(0, DesiredStartX, Buffer->Width));
    int StartY = floor(Clamp(0, DesiredStartY, Buffer->Height));
    int EndX   = ceil(Clamp(0, DesiredStartX + Width, Buffer->Width));
    int EndY   = ceil(Clamp(0, DesiredStartY + Height, Buffer->Height));

    for(int Y = StartY; Y < EndY; Y++) {
        u8 *Row = Buffer->Pixels + Y*Buffer->Pitch;
        for(int X = StartX; X < EndX; X++) { // TODO: Work out this logic
            u32 *Pixel = (u32 *)Row + X;

            real32 LocalAlpha = Alpha;

            if(LocalAlpha < 1) {
                u32 DestColor = *Pixel;
                u8 DestR = DestColor >> 16;
                u8 DestG = DestColor >> 8;
                u8 DestB = DestColor >> 0;

                u8 SrcR = SrcColor >> 16;
                u8 SrcG = SrcColor >> 8;
                u8 SrcB = SrcColor >> 0;

                u8 R = (DestR * (1 - LocalAlpha)) + (SrcR * LocalAlpha);
                u8 G = (DestG * (1 - LocalAlpha)) + (SrcG * LocalAlpha);
                u8 B = (DestB * (1 - LocalAlpha)) + (SrcB * LocalAlpha);

                u32 Color = (R << 16) | (G << 8) | (B << 0);

                *Pixel = Color;
            } else {
                *Pixel = SrcColor;
            }
        }
    }
}

void
DrawRect(game_render_buffer *Buffer, v2 CameraP, v2 Position, v2 Size, u32 Color)
{
    ProfilerStart(PROFITEM_DRAW_RECT);

    real32 AspectMultiplier = CalculateAspectMultiplier(Buffer);
    real32 StepSize = Scale * AspectMultiplier;

    Position = Position + CameraP;

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

    ProfilerEnd(PROFITEM_DRAW_RECT);
}

void
DrawRectAlpha(game_render_buffer *Buffer, v2 CameraP, v2 Position, v2 Size, u32 Color, real32 Alpha)
{
    ProfilerStart(PROFITEM_DRAW_RECT_ALPHA);

    real32 AspectMultiplier = CalculateAspectMultiplier(Buffer);
    real32 StepSize = Scale * AspectMultiplier;

    Position = Position + CameraP;

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

    real32 XStartAlpha = Alpha * (1 - (X0 - floor(X0)));
    real32 XEndAlpha = Alpha * ((X0 + Size.X) - (int)(X0 + Size.X));
    real32 YStartAlpha = Alpha * (1 - (Y0 - floor(Y0)));
    real32 YEndAlpha = Alpha * ((Y0 + Size.Y) - (int)(Y0 + Size.Y));

    // DrawRectInPixels(Buffer, X0-1, Y0, 1.0f, SizeY, Color, XStartAlpha);
    // DrawRectInPixels(Buffer, X0+SizeX, Y0, 1.0f, SizeY, Color, XEndAlpha);
    //
    // DrawRectInPixels(Buffer, X0, Y0-1, SizeX, 1.0f, Color, YStartAlpha);
    // DrawRectInPixels(Buffer, X0, Y0+SizeY, SizeY, 1.0f, Color, YEndAlpha);

    DrawRectInPixels(Buffer, X0, Y0, SizeX, SizeY, Color, Alpha);

    ProfilerEnd(PROFITEM_DRAW_RECT_ALPHA);
}

void
DrawRectRotated(game_render_buffer *Buffer, v2 CameraP, v2 Position, v2 Size, u32 SrcColor, real32 Alpha, real32 Angle)
{
    real32 AspectMultiplier = CalculateAspectMultiplier(Buffer);
    real32 StepSize = Scale * AspectMultiplier;

    Position = Position + CameraP;

    Size.X     *= StepSize;
    Size.Y     *= StepSize;
    Position.X *= StepSize;
    Position.Y *= StepSize;
    Position.X += Buffer->Width * 0.5;
    Position.Y += Buffer->Height * 0.5;

    int SizeX = round(Size.X);
    int SizeY = round(Size.Y);
    int X0    = round(Position.X - (SizeX * 0.5));
    int Y0    = round(Position.Y - (SizeY * 0.5));

    int StartX = Clamp(0, X0, Buffer->Width);
    int StartY = Clamp(0, Y0, Buffer->Height);
    int EndX   = Clamp(0, X0 + SizeX, Buffer->Width);
    int EndY   = Clamp(0, Y0 + SizeY, Buffer->Height);

    // DrawRectInPixels(Buffer, (int)StartX, (int)StartY, (int)Size.X, (int)Size.Y, 0xff0000);

    // TODO: This is incorrect. Instead of going from source pixes to dest pixels, it should be
    // all backwards (at least for this algo). 

    real32 CosValue = cos(Angle);
    real32 SinValue = sin(Angle);
    for(int YIndex = StartY; YIndex < EndY; YIndex++) {
        for(int XIndex = StartX; XIndex < EndX; XIndex++) { // TODO: Work out this logic

            real32 XOrig = Position.X - XIndex;
            real32 YOrig = Position.Y - YIndex;

            real32 CosX = CosValue*(real32)XOrig;
            real32 SinX = SinValue*(real32)XOrig;
            real32 SinY = SinValue*(real32)YOrig;
            real32 CosY = CosValue*(real32)YOrig;

            real32 Xreal = CosX - SinY + (real32)Position.X;
            real32 Yreal = SinX + CosY + (real32)Position.Y;
            s32 X = round(Xreal);
            s32 Y = round(Yreal);

            u32 *Pixel = (u32 *)Buffer->Pixels + (Y*Buffer->Width) + X;

            if(Alpha < 1) {
                u32 DestColor = *Pixel;
                u8 DestR = DestColor >> 16;
                u8 DestG = DestColor >> 8;
                u8 DestB = DestColor >> 0;

                u8 SrcR = SrcColor >> 16;
                u8 SrcG = SrcColor >> 8;
                u8 SrcB = SrcColor >> 0;

                u8 R = (DestR * (1 - Alpha)) + (SrcR * Alpha);
                u8 G = (DestG * (1 - Alpha)) + (SrcG * Alpha);
                u8 B = (DestB * (1 - Alpha)) + (SrcB * Alpha);

                u32 Color = (R << 16) | (G << 8) | (B << 0);

                *Pixel = Color;
            } else {
                *Pixel = SrcColor;
            }
        }
    }


    // DrawRectInPixels(Buffer, Position.X, Position.Y, 4.0f, 4.0f, 0x000000);
    // DrawRectInPixels(Buffer, (real32)StartX, (real32)StartY, 4.0f, 4.0f, 0x000000);
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
        "1___1",
        "1___1",
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


character CharactersCaps[] = {
    {
        "_111_",
        "1___1",
        "1___1",
        "1___1",
        "11111",
        "1___1",
        "1___1",
    },
    {
        "1111_",
        "1___1",
        "1___1",
        "1111_",
        "1___1",
        "1___1",
        "1111_",
    },
    {
        "_111_",
        "1___1",
        "1____",
        "1____",
        "1____",
        "1___1",
        "_111_",
    },
    {
        "1111_",
        "1___1",
        "1___1",
        "1___1",
        "1___1",
        "1___1",
        "1111_",
    },
    {
        "11111",
        "1____",
        "1____",
        "1111_",
        "1____",
        "1____",
        "11111",
    },
    {
        "11111",
        "1____",
        "1____",
        "1111_",
        "1____",
        "1____",
        "1____",
    },
    {
        "_111_",
        "1___1",
        "1____",
        "1_111",
        "1___1",
        "1___1",
        "_111_",
    },
    {
        "1___1",
        "1___1",
        "1___1",
        "11111",
        "1___1",
        "1___1",
        "1___1",
    },
    {
        "11111",
        "__1__",
        "__1__",
        "__1__",
        "__1__",
        "__1__",
        "11111",
    },
    {
        "____1",
        "____1",
        "____1",
        "____1",
        "1___1",
        "1___1",
        "_111_",
    },
    {
        "1___1",
        "1__1_",
        "1_1__",
        "11___",
        "1_1__",
        "1__1_",
        "1___1",
    },
    {
        "1____",
        "1____",
        "1____",
        "1____",
        "1____",
        "1____",
        "11111",
    },
    {
        "1___1",
        "11_11",
        "1_1_1",
        "1___1",
        "1___1",
        "1___1",
        "1___1",
    },
    {
        "1___1",
        "1___1",
        "11__1",
        "1_1_1",
        "1__11",
        "1___1",
        "1___1",
    },
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
        "1111_",
        "1___1",
        "1___1",
        "1111_",
        "1____",
        "1____",
        "1____",
    },
    {
        "_111_",
        "1___1",
        "1___1",
        "1___1",
        "1_1_1",
        "1__1_",
        "_11_1",
    },
    {
        "1111_",
        "1___1",
        "1___1",
        "1111_",
        "1___1",
        "1___1",
        "1___1",
    },
    {
        "_111_",
        "1___1",
        "1____",
        "_111_",
        "____1",
        "1___1",
        "_111_",
    },
    {
        "11111",
        "__1__",
        "__1__",
        "__1__",
        "__1__",
        "__1__",
        "__1__",
    },
    {
        "1___1",
        "1___1",
        "1___1",
        "1___1",
        "1___1",
        "1___1",
        "_111_",
    },
    {
        "1___1",
        "1___1",
        "1___1",
        "1___1",
        "_1_1_",
        "_1_1_",
        "__1__",
    },
    {
        "1___1",
        "1___1",
        "1___1",
        "1_1_1",
        "1_1_1",
        "11_11",
        "1___1",
    },
    {
        "1___1",
        "1___1",
        "_1_1_",
        "__1__",
        "_1_1_",
        "1___1",
        "1___1",
    },
    {
        "1___1",
        "1___1",
        "_1_1_",
        "__1__",
        "__1__",
        "__1__",
        "__1__",
    },
    {
        "11111",
        "____1",
        "___1_",
        "__1__",
        "_1___",
        "1____",
        "11111",
    },

    {
        "_____",
        "_____",
        "_____",
        "11111",
        "_____",
        "_____",
        "_____",
    },
    {
        "_____",
        "____1",
        "___1_",
        "__1__",
        "_1___",
        "1____",
        "_____",
    },
    {
        "_____",
        "_____",
        "_____",
        "_____",
        "_____",
        "_____",
        "__1__",
    },
    {
        "_____",
        "_____",
        "_____",
        "_____",
        "_____",
        "_____",
        "11111",
    },
};


inline u32 
GetCharIndex(char Character) {
    u32 SpecialCharBase = 'Z' - 'A' + 1;
    u32 Result = 0; 

    if(Character - 'A' < SpecialCharBase) {
        Result = Character - 'A';
    } else {
        if(Character == '-')  { Result = SpecialCharBase + 0;}
        if(Character == '/')  { Result = SpecialCharBase + 1;}
        if(Character == '.')  { Result = SpecialCharBase + 2;}
        if(Character == '_')  { Result = SpecialCharBase + 3;}
    }

    return Result;
}


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

// void
// DrawCapsChar(game_render_buffer *Buffer, char RequestedChar, v2 P, real32 CharWidth, u32 Color) {
//     real32 Size = CharWidth / (CHAR_ROW_LENGTH - 1); // TODO: Add variable to remove this -1
//     real32 StartX = P.X;
//     real32 StartY = P.Y;
//     real32 Offset = 0.0;
//
//     s32 TestCharIndex = GetCharIndex(RequestedChar);
//     s32 CharIndex = Clamp(0, TestCharIndex, ARRAY_COUNT(CharactersCaps) - 1);
//     character CharCaps = CharactersCaps[CharIndex];
//
//     for(int YIndex = 0; YIndex < ARRAY_COUNT(CharCaps.data); YIndex++) {
//         for(int XIndex = 0; XIndex < (CHAR_ROW_LENGTH-1); XIndex++) {
//             char Value = CharCaps.data[YIndex][XIndex];
//             if(Value == '1')
//             {
//                 real32 X = StartX + (XIndex * Size) + (XIndex * Offset);
//                 real32 Y = StartY + (YIndex * Size) + (YIndex * Offset);
//                 DrawRect(Buffer, V2(X, Y), V2(Size, Size), Color);
//             }
//         }
//     }
// }


u32 
Lerp(u32 A, u32 B, real32 t) {
    // u32 Result = (A * t) + (B * (1 - t));
    u32 Result = (1 - t) * A + B * t;
    return Result;
}

u32
LerpColor(u32 StartColor, u32 EndColor, real32 t) {
    u32 Result = 0;

    u8 StartR = StartColor >> 16;
    u8 StartG = StartColor >> 8;
    u8 StartB = StartColor >> 0;

    u8 EndR = EndColor >> 16;
    u8 EndG = EndColor >> 8;
    u8 EndB = EndColor >> 0;

    u8 ResultR = Lerp(StartR, EndR, t);
    u8 ResultG = Lerp(StartG, EndG, t);
    u8 ResultB = Lerp(StartB, EndB, t);

    Result = (ResultR << 16) |
        (ResultG << 8) |
        (ResultB << 0);

    return Result;
}

v2
DrawCapsCharGradient(game_render_buffer *Buffer, char RequestedChar, v2 CameraP, v2 P, real32 CharWidth, 
        u32 StartColor, u32 EndColor, real32 GradientPositionReal) 
{
    P = P + CameraP;

    real32 Size = CharWidth / (CHAR_ROW_LENGTH - 1); // TODO: Add variable to remove this -1
    real32 StartX = P.X;
    real32 StartY = P.Y;
    real32 Offset = 0.0;

    int GradientPosition = round(GradientPositionReal * (float)CHAR_COLUMN_COUNT);

    s32 TestCharIndex = GetCharIndex(RequestedChar);
    s32 CharIndex = Clamp(0, TestCharIndex, ARRAY_COUNT(CharactersCaps) - 1);
    character CharCaps = CharactersCaps[CharIndex];

    v2 DrawPResult = {
        P.X + (Size *ARRAY_COUNT(CharCaps.data)),  
        P.Y
    };

    for(int YIndex = 0; YIndex < ARRAY_COUNT(CharCaps.data); YIndex++) {

        int SomeIndex = YIndex - GradientPosition;
        if(SomeIndex < 0) {
            SomeIndex = CHAR_COLUMN_COUNT + SomeIndex;
        }
        real32 t = (float)(SomeIndex + 1) / (float)CHAR_COLUMN_COUNT;
        u32 Color = LerpColor(StartColor, EndColor, t);

        for(int XIndex = 0; XIndex < (CHAR_ROW_LENGTH-1); XIndex++) {
            char Value = CharCaps.data[YIndex][XIndex];
            if(Value == '1')
            {
                real32 X = StartX + (XIndex * Size) + (XIndex * Offset);
                real32 Y = StartY + (YIndex * Size) + (YIndex * Offset);
                DrawRect(Buffer, CameraP, V2(X, Y), V2(Size, Size), Color);
            }
        }
    }

    // DrawRect(Buffer, DrawPResult, V2(1, 1), 0x000000);
    return DrawPResult;
}

void
DrawDigit(game_render_buffer *Buffer, char RequestedDigit, v2 CameraP, v2 P, real32 CharWidth, u32 Color) {
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
                DrawRect(Buffer, CameraP, V2(X, Y), V2(Size, Size), Color);
            }
        }
    }
}

v2 
DrawNumber(game_render_buffer *Buffer, s32 Number, v2 CameraP, v2 P, real32 Size, u32 Color) {
    s32 IterateNumber = Number;
    u32 InitialDigitCount = CalculateDigitCount(Number);
    u32 DigitCount = InitialDigitCount;

    bool32 IsNegative = Number < 0;

    real32 StartX = P.X;
    real32 StartY = P.Y;
    real32 Offset = (Size / (CHAR_ROW_LENGTH - 1));
    real32 CharWidthWithRightOffset = Size + Offset;

    v2 NextP = {StartX, StartY};

    bool32 FirstPass = true;
    if(IsNegative) {
        DrawRect(Buffer, CameraP, V2(StartX, StartY+1), V2(1, 0.4), Color);
        StartX += 1;
        IterateNumber *= -1;
    }

    while(FirstPass | IterateNumber) {
        FirstPass = false;

        char Digit = IterateNumber % 10;
        IterateNumber /= 10;
        // real32 X = StartX + CharWidthWithRightOffset * (DigitCount - 1);
        // real32 Y = StartY;

        NextP.X =  StartX + CharWidthWithRightOffset * (DigitCount - 1);
        NextP.Y = StartY;

        DrawDigit(Buffer, Digit, CameraP, NextP, Size, Color);
        // DrawRect(Buffer, CameraP, NextP, V2(1, 1), 0x000000);

        DigitCount--;
    }

    NextP.X = StartX + CharWidthWithRightOffset * (InitialDigitCount);
    NextP.Y = StartY;

    // DrawRect(Buffer, CameraP, V2(StartX, StartY-2), V2(1, 1), 0xff00ff);
    // DrawRect(Buffer, CameraP, NextP, V2(1, 1), 0x000000);

    return NextP;
}

v2
DrawNumberReal(game_render_buffer *Buffer, real32 Number, v2 CameraP, v2 P, real32 Size, u32 Color)
{
    s32 WholePart = (s32)Number;
    s32 FractionPart = (s32)(100 * ((double)Number - (double)WholePart));

    v2 NextP = DrawNumber(Buffer, WholePart, CameraP, P, Size, 0xffffffff);
    NextP.X -= (Size / 4.5);
    NextP = DrawCapsCharGradient(Buffer, '.', CameraP, NextP, Size, Color, Color, 0.0f);
    NextP.X -= (Size / 2.5);
    NextP = DrawNumber(Buffer, FractionPart, CameraP, NextP, Size, 0xffffffff);

    return NextP;
}

// void 
// DrawString(game_render_buffer *Buffer, char* String, v2 P, real32 Size, u32 Color) {
//     u32 CharCount = strlen(String);
//
//     v2 NextCharP = {P.X, P.Y};
//
//     for(int CharIndex = 0; CharIndex < CharCount; CharIndex++) {
//         char Character = String[CharIndex];
//         DrawCapsChar(Buffer, Character, NextCharP, Size, 0xffffff);
//
//         NextCharP.X += Size + (Size / 8);
//     }
// }

v2 
DrawStringGradient(game_render_buffer *Buffer, char* String, v2 CameraP, v2 P, real32 Size, 
        u32 StartColor, u32 EndColor, real32 GradientPosition) {

    u32 CharCount = strlen(String);

    v2 NextCharP = {P.X, P.Y};

    for(int CharIndex = 0; CharIndex < CharCount; CharIndex++) {
        char Character = String[CharIndex];
        if(Character == '\n') { 
            NextCharP.X = P.X;
            NextCharP.Y += Size*1.9;
            continue;
        }

        DrawCapsCharGradient(Buffer, Character, CameraP, NextCharP, Size, StartColor, EndColor, GradientPosition);

        NextCharP.X += Size + (Size / 8);
    }
    // NextCharP.Y -= 2;
    // DrawRect(Buffer, NextCharP, V2(1, 1), 0xff00ff);

    return NextCharP;
}

void 
DrawBitmap(game_render_buffer *Buffer, bitmap *Bitmap, v2 CameraP, v2 Position, v2 Size) {
    Position = Position + CameraP;
    real32 AspectMultiplier = CalculateAspectMultiplier(Buffer);
    real32 StepSize = Scale * AspectMultiplier;

    Size.X     *= StepSize;
    Size.Y     *= StepSize;
    Position.X *= StepSize;
    Position.Y *= StepSize;
    Position.X += Buffer->Width * 0.5;
    Position.Y += Buffer->Height * 0.5;

    int SizeX = round(Size.X);
    int SizeY = round(Size.Y);
    int X0    = round(Position.X - (SizeX * 0.5));
    int Y0    = round(Position.Y - (SizeY * 0.5));

    int StartX = Clamp(0, X0, Buffer->Width);
    int StartY = Clamp(0, Y0, Buffer->Height);
    int EndX   = Clamp(0, X0 + SizeX, Buffer->Width);
    int EndY   = Clamp(0, Y0 + SizeY, Buffer->Height);

    for(int YIndex = StartY; YIndex <= EndY; YIndex++) {
        real32 YProportion = ((float)YIndex - StartY) / (float)SizeY;
        s32 BitmapYIndex = YProportion * (Bitmap->Height - 1);
        for(int XIndex = StartX; XIndex <= EndX; XIndex++) { 
            real32 XProportion = ((float)XIndex - StartX) / (float)SizeX;
            s32 BitmapXIndex = XProportion * (Bitmap->Width - 1);
            
            u32 *SrcPixel = (u32 *)Bitmap->Memory + (BitmapYIndex * Bitmap->Width + BitmapXIndex);
            u8 AlphaChannel = *SrcPixel >> 24 & 0xFF;
            u32 *DestPixel = (u32 *)Buffer->Pixels + YIndex * Buffer->Width + XIndex;

            // printf("%d %d %d %d\n", 
            //         *SrcPixel >> 24 & 0xFF,
            //         *SrcPixel >> 16 & 0xFF,
            //         *SrcPixel >> 8 & 0xFF,
            //         *SrcPixel >> 0 & 0xFF
            //         );

            if(AlphaChannel < 255) {
                real32 Alpha = (real32)AlphaChannel / 255;
                u32 DestColor = *DestPixel;
                u32 SrcColor = *SrcPixel;

                u8 DestR = DestColor >> 16;
                u8 DestG = DestColor >> 8;
                u8 DestB = DestColor >> 0;

                u8 SrcR = SrcColor >> 16;
                u8 SrcG = SrcColor >> 8;
                u8 SrcB = SrcColor >> 0;

                u8 R = (DestR * (1 - Alpha)) + (SrcR * Alpha);
                u8 G = (DestG * (1 - Alpha)) + (SrcG * Alpha);
                u8 B = (DestB * (1 - Alpha)) + (SrcB * Alpha);

                u32 Color = (R << 16) | (G << 8) | (B << 0);

                *DestPixel = Color;
            } else {
                *DestPixel = *SrcPixel;
            }
        }
    }
}
