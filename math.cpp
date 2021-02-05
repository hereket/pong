
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
