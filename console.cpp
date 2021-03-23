#if DEVELOPMENT

typedef struct {
    int Value;
    real32 Timer;
} message;

global_variable message GlobalMessageList[32];
global_variable u32 GlobalMessageIndex;

void ConsolePrintInt(s32 Value) {
    message *Message = GlobalMessageList + GlobalMessageIndex++;
    if(GlobalMessageIndex >= ARRAY_COUNT(GlobalMessageList)) { GlobalMessageIndex = 0; }

    Message->Value = Value;
    Message->Timer = 2.0f;
}

void DrawConsoleMessages(game_render_buffer *Buffer, arena *Arena, float dt) {
    real32 StartX = (Arena->Size.X/2) - 10;
    real32 StartY = -(Arena->Size.Y/2) + 10;

    v2 P = {StartX, StartY};

    for(u32 i = 0; i < ARRAY_COUNT(GlobalMessageList); i++) {
        s32 Index = (GlobalMessageIndex - 1) - i;
        if(Index < 0) { Index = ARRAY_COUNT(GlobalMessageList) + Index;};

        message *Message = GlobalMessageList + Index;
        if(Message->Timer <= 0.0f) { continue; }

        Message->Timer -= dt;
        s32 Value = Message->Value;

        P.Y += 2.7;
        DrawNumber(Buffer, Value, P, 1.5, 0xffffffff);
    }
}

#else

#define DrawConsoleMessages(...)

#endif
