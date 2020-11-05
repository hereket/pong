

int
Clamp(int Min, int Max, int Value)
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

