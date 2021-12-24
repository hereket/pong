#include <limits.h>

#define PI 3.141527
#define TAU PI * 2

s32 
RoundBottom(real32 Value)
{
    s32 Result = (s32)(Value + 0.5);
    return Result;
}


int
Clamp(int Min, int Value, int Max) 
{
    int Result = Value;
    if(Value < Min) {
        Result = Min;
    } else if (Value > Max) {
        Result = Max;
    }

    return Result;
}

int
ClampF(float Min, float Value, float Max) 
{
    float Result = Value;
    if(Value < Min) {
        Result = Min;
    } else if (Value > Max) {
        Result = Max;
    }

    return Result;
}

inline s32 
Absolute(real32 Value)
{
    real32 Result = Value;
    if(Value < 0) {
        Result *= -1;
    }
    return Result;
}



struct {
    real32 X;
    real32 Y;
}typedef v2;


v2 
V2(real32 X, real32 Y)
{
    v2 Result = {};
    Result.X = X;
    Result.Y = Y;
    return Result;
}


v2 
operator*(v2 A, real32 Value) {
    v2 Result = {};
    Result.X = A.X * Value;
    Result.Y = A.Y * Value;
    return Result;
}

v2 
operator+(v2 A, v2 B) {
    v2 Result = {};
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return Result;
}

v2 
operator/(v2 A, real32 B) {
    v2 Result = {};
    Result.X = A.X / B;
    Result.Y = A.Y / B;
    return Result;
}


v2 
operator-(v2 A, v2 B) {
    v2 Result = {};
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return Result;
}



u32 
Lerp(u32 A, u32 B, real32 t) {
    // u32 Result = (A * t) + (B * (1 - t));
    u32 Result = (1 - t) * A + B * t;
    return Result;
}

real32 
LerpReal32(real32 A, real32 B, real32 t) {
    // u32 Result = (A * t) + (B * (1 - t));
    real32 Result = (1 - t) * A + B * t;
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
Lerp(v2 A, v2 B, real32 T) {

    v2 Result = {};
    Result.X = LerpReal32(A.X, B.X, T);
    Result.Y = LerpReal32(A.Y, B.Y, T);

    return Result;
}




/*********************************************************************************
 **********    RANDOM    *********************************************************
 *********************************************************************************
 */

u32 GlobalRandomState = 31415;


static inline u32 
RandomU32() {
    u32 Result = GlobalRandomState;
    Result ^= Result << 13;
    Result ^= Result >> 17;
    Result ^= Result << 5;
    GlobalRandomState = Result;
    return Result;
}

static inline real32
RandomUniform() {
    real32 Result = (real32)RandomU32() / (real32)UINT_MAX;
    return Result;
}

static inline real32
RandomUniformRange(real32 Min, real32 Max) {
    real32 Result = Min + RandomUniform() * (Max - Min);
    return Result;
}
