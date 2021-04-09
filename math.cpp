#include <limits.h>

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
