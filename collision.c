
bool32
IsColliding(v2 AP, v2 ASize, v2 BP, v2 BSize)
{
    bool32 Result = false;

    real32 AX0 = AP.X - (ASize.X / 2);
    real32 AX1 = AP.X + (ASize.X / 2);
    real32 AY0 = AP.Y - (ASize.Y / 2);
    real32 AY1 = AP.Y + (ASize.Y / 2);

    real32 BX0 = BP.X - (BSize.X / 2);
    real32 BX1 = BP.X + (BSize.X / 2);
    real32 BY0 = BP.Y - (BSize.Y / 2);
    real32 BY1 = BP.Y + (BSize.Y / 2);

    if(
        (AX0 < BX1) &&
        (AX1 > BX0) &&
        (AY0 < BY1) &&
        (AY1 > BY0) 
    ) {
        Result = true;
    }

    return Result;
}

enum collision_type{
    COLLISION_NONE,
    COLLISION_LEFT,
    COLLISION_RIGHT,
    COLLISION_TOP,
    COLLISION_BOTTOM,
};

s32
CollisionSide(v2 AP, v2 ASize, v2 BP, v2 BSize)
{
    s32 Result = 0;
    
    real32 dX = fabs(AP.X - BP.X);
    real32 dY = fabs(AP.Y - BP.Y);
    real32 width = (ASize.X + BSize.X) / 2;
    real32 height = (ASize.Y + BSize.Y) / 2;

    real32 CrossWidth = width * dY;
    real32 CrossHeight = height * dX;

    if(dX < width && dY < height) {
        if(CrossWidth > CrossHeight) {
            Result = (CrossWidth > (-CrossHeight)) ? COLLISION_BOTTOM : COLLISION_LEFT;
        } else {
            Result = (CrossWidth > (-CrossHeight)) ? COLLISION_RIGHT : COLLISION_TOP;
        }
    }

    return Result;
}
