#if PROFILING

real32 DEBUGPlatformGetTimeNanoseconds();
real32 DEBUGPlatformGetTimeElapsedMilliseconds(real32 Time);

global_variable perf_data GlobalProfilingItems[PROFITEM__COUNT];

static char * 
GetName(profiling_item Item) {
    if(Item == PROFITEM_GAME) { return "GAME"; }
    else if(Item == PROFITEM_DRAW_RECT) { return "DRAW_RECT"; }
    else if(Item == PROFITEM_DRAW_RECT_ALPHA) { return "DRAW_RECT_ALPHA"; }
    else if(Item == PROFITEM_INPUT) { return "INPUT"; }
    else if(Item == PROFITEM_INPUT) { return "FLIP"; }

    else { return "----"; }
}

void
SetMax(real32 *Main, real32 Test) {
    if(Test > *Main) {
        *Main = Test;
    }
}

void 
ProfilerClearData() {
    ZERO_ARRAY(GlobalProfilingItems);
}

void 
ProfilerStart(profiling_item Item) {
    perf_data *Data = GlobalProfilingItems + Item;
    Data->HitCount++;
    Data->BeginTime = DEBUGPlatformGetTimeNanoseconds();
}

void 
ProfilerEnd(profiling_item Item) {
    perf_data *Data = GlobalProfilingItems + Item;
    Data->Time += DEBUGPlatformGetTimeElapsedMilliseconds(Data->BeginTime);
}

void
RenderProfiler(game_render_buffer *Buffer, real32 dt) {
    v2 InitialP = {-80, -40};
    v2 P = InitialP;
    real32 Size = 1.2;
    real32 c1 =-999999;

    v2 CameraP = {0, 0};

    for(int i = 0; i < PROFITEM__COUNT; i++) {
        perf_data *Data = GlobalProfilingItems + i;
        P.Y += Size + 2.5; 

        v2 NextP = DrawStringGradient(Buffer, GetName((profiling_item)i), CameraP, P, Size, 0xffffff, 0xffffff, 0);
        SetMax(&c1, NextP.X);
    }


    P = InitialP;
    P.X = c1 + 3;
    for(int i = 0; i < PROFITEM__COUNT; i++) {
        perf_data *Data = GlobalProfilingItems + i;
        P.Y += Size + 2.5; 

        v2 NextP = DrawNumber(Buffer, Data->HitCount, CameraP, P, Size, 0xffffffff);
        SetMax(&c1, NextP.X);
    }

    P = InitialP;
    P.X = c1 + 3;
    for(int i = 0; i < PROFITEM__COUNT; i++) {
        perf_data *Data = GlobalProfilingItems + i;
        P.Y += Size + 2.5; 

        v2 NextP = DrawNumberReal(Buffer, Data->Time, CameraP, P, Size, 0xffffffff);
        SetMax(&c1, NextP.X);
    }

    // static s64 test = 0;
    // static float val = dt * 1000;
    // if(test % 8 == 0) {
    //     val = dt * 1000;
    // }

    // v2 NextP = DrawNumberReal(Buffer, val, V2(-80, -40), Size*1.4, 0xffffffff);
    v2 NextP = DrawNumberReal(Buffer, dt*1000, CameraP, V2(-80, -40), Size*1.4, 0xffffffff);
    // test++;
}


#else

#define ProfilerClearData(...)
#define ProfilerEnd(...)
#define ProfilerStart(...)
#define RenderProfiler(...)

#endif
