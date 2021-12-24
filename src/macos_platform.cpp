#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

#include "math.cpp"
#include "game.h"
#include "profiler.h"

#include "software_renderer.cpp"
#include "asset_loader.cpp"

#include "profiler.cpp"

#include "platform.h"
#include "config_loader.cpp"

#include "game.cpp"

#include <mach/mach_time.h>



struct {
    s32 Width;
    s32 Height;
    u8 *Pixels;
    u32 Pitch;
    SDL_Texture *Texture;
} typedef macos_render_buffer;

global_variable macos_render_buffer GlobalRenderBuffer;

global_variable int GlobalWindowIsFocused;
global_variable double GlobalFrequencyCounter;

void
MACResizeTexture(SDL_Renderer *Renderer,  macos_render_buffer *Buffer, int Width, int Height)
{
    Buffer->Width = Width;
    Buffer->Height = Height;

    if(Buffer->Pixels)  { free(Buffer->Pixels); }
    if(Buffer->Texture) { SDL_DestroyTexture(Buffer->Texture); }

    s32 BytesPerPixel = 4;

    Buffer->Pitch = Buffer->Width * BytesPerPixel;
    s32 BufferSize = Buffer->Pitch * Buffer->Height;
    Buffer->Pixels = (u8 *)malloc(BufferSize);

    Buffer->Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, 
                                        SDL_TEXTUREACCESS_STREAMING, Width, Height);
}

inline u64 
DEBUGPlatformGetTimeNanoseconds() 
{
    // clock_gettime_nsec_np(CLOCK_UPTIME_RAW) * 1000;
    
    u64 Ticks = mach_absolute_time();
    u64 Nanoseconds = (u64)((double)Ticks * (double)GlobalFrequencyCounter);
    return Nanoseconds;
}


inline u64
DEBUGPlatformGetTimeElapsedNanoseconds(u64 OldTimeNanosecods) 
{
    u64 CurrentNanoseconds = DEBUGPlatformGetTimeNanoseconds();
    u64 TimeElapsedNanoseconds = CurrentNanoseconds - OldTimeNanosecods;
    return TimeElapsedNanoseconds;

}


real32
DEBUGPlatformGetTimeElapsedMilliseconds(real32 Milliseconds) 
{
    double TimeElapsedMilliseconds = (double)DEBUGPlatformGetTimeElapsedNanoseconds(Milliseconds * 1000000) / 1000000.0;
    printf("%f\n",TimeElapsedMilliseconds);
    return TimeElapsedMilliseconds;
}


DEBUG_PLATFORM_READ_ENTIRE_FILE(ReadEntireFile) 
{
    FILE *File = fopen((const char *)Filename, "r");
    read_file_result LoadedFile = {};

    if(File != NULL) {
        fseek(File, 0, SEEK_END);
        u64 FileSize = ftell(File);
        rewind(File);

        LoadedFile.Memory = (u8 *)malloc(FileSize);
        LoadedFile.Size = FileSize;

        fread(LoadedFile.Memory, 1, LoadedFile.Size, File);
    }

    return LoadedFile;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(WriteEntireFile)
{
    s64 BytesWritten = 0;
    FILE *f = fopen(Filename, "wb");
    // BytesWritten = fwrite(Data, Size, 1, f);
    BytesWritten = fwrite(Data, 1, Size, f);
    fclose(f);
    return BytesWritten;
}


int AMPLITUDE = 28000;
int SAMPLE_RATE = 44100;

// void AudioCallback(void *user_data, Uint8 *raw_buffer, int bytes)
// {
//     Sint16 *buffer = (Sint16*)raw_buffer;
//     int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
//     int &sample_nr(*(int*)user_data);
//
//     for(int i = 0; i < length; i++, sample_nr++)
//     {
//         double time = (double)sample_nr / (double)SAMPLE_RATE;
//         buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
//     }
// }

#define PLAYING_SOUNDS_COUNT 16
loaded_audio PlayingSounds[PLAYING_SOUNDS_COUNT];

DEBUG_PLATFORM_PLAY_SOUND(PlaySound)
{
    Track->IsPlaying = true;
    Track->IsLooping = IsLooping;

    bool32 AddedTrack = false;

    Track->Volume = Volume;

    for(s32 i = 0; i < PLAYING_SOUNDS_COUNT; i++) {
        loaded_audio *OldTrack = PlayingSounds + i;

        if(Track && !OldTrack->IsPlaying) {
            // PlayingSounds[i] = Track;
            memcpy(OldTrack, Track, sizeof(loaded_audio));
            AddedTrack = true;
            break;
        }
    }

    if(!AddedTrack) {
        printf("Couldn't add track. Probably no more empty slots left.\n");
    }

}

global_variable SDL_Window *GlobalMainWindow;
global_variable bool32 GlobalFullScreen;

DEBUG_PLATFORM_SET_FULLSCREEN(SetFullScreen) {
    GlobalFullScreen = IsFullScreen;
    if(IsFullScreen == 0) {
        SDL_SetWindowFullscreen(GlobalMainWindow, 0);
    } else {
        SDL_SetWindowFullscreen(GlobalMainWindow, SDL_WINDOW_FULLSCREEN);
    }

}


DEBUG_PLATFORM_LOAD_WAV(LoadWav)
{
    SDL_AudioSpec AudioSpec;
    loaded_audio Track = {};
    SDL_LoadWAV((char *)Filename, &AudioSpec, &Track.Data, &Track.Size);
    Track.SampleCount = Track.Size / 2;
    Track.Volume = 1.0f;

    return Track;
}

void AudioCallback(void *UserData, u8 *Stream, int RequestedBytes)
{
    s32 ChannelCount = 2; // TODO: Get this infor from ...?

    s16 *DestBuffer = (s16 *)Stream;
    s32 Length = RequestedBytes / ChannelCount;

    for(int i = 0; i < Length; i++) {
        DestBuffer[i] = 0;
    }

    for(int i = 0; i < PLAYING_SOUNDS_COUNT; i++) {
        loaded_audio *Track = PlayingSounds + i;
        if(Track && Track->IsPlaying) {
            s16 *SrcBuffer = (s16 *)Track->Data;

            for(int i = 0; i < Length; i++) {
                u32 SrcIndex = Track->Position++;
                if(Track->Position >= Track->SampleCount) { 
                    if(Track->IsLooping) {
                        Track->Position = 0; 
                    } else {
                        Track->IsPlaying = false;
                        break;
                    }
                }

                s32 SrcValue = SrcBuffer[SrcIndex];
                s32 Value = (s32)((real32)SrcValue * (real32)Track->Volume);
                s32 TestValue = DestBuffer[i] + Value;
                s32 ClampedValue = Clamp(-AMPLITUDE, TestValue, AMPLITUDE);

                // printf("SrcValue: %d, Value: %d, TestValue: %d, ClampedValue: %d, %f\n", SrcValue, Value, TestValue, ClampedValue, Track->Volume);
                // printf("SrcBuffer: %d, Value: %d, TestValue: %d, ClampedValue: %d\n", SrcBuffer[SrcIndex], Value, TestValue, ClampedValue);

                DestBuffer[i] = ClampedValue;
            }

        }
    }
}





global_variable real32 ListOfDt[60];
global_variable int DtIndex;

int main()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) { printf("Failed to initialize the SDL2 library\n"); return -1; }
    if(SDL_Init(SDL_INIT_AUDIO) < 0) { printf("Failed to init sdl audio\n"); return - 1; }

    {
        SDL_AudioSpec WantedAudioSpec;
        WantedAudioSpec.freq = SAMPLE_RATE;
        WantedAudioSpec.format = AUDIO_S16;
        WantedAudioSpec.channels = 2;
        WantedAudioSpec.samples = 512;
        WantedAudioSpec.callback = AudioCallback;

        if(SDL_OpenAudio(&WantedAudioSpec, NULL)) { printf("Failed to open audio"); }

        SDL_PauseAudio(0);
    }

    // u64 loopTotal = 10;
    // u64 small = 5;
    // for(u64 i = 0; i < loopTotal; i++) {
    //     printf("%llu %% %llu = %llu\n", i, small, i % small);
    // }
    //
    // return 0;




    game_memory GameMemory = {};
    GameMemory.DEBUGPlatformReadEntireFile = ReadEntireFile;
    GameMemory.DEBUGPlatformWriteEntireFile = WriteEntireFile;

    GameMemory.DEBUGPlatformLoadWav = LoadWav;
    GameMemory.DEBUPlatformPlaySound = PlaySound;
    GameMemory.DEBUGPlatformSetFullScreen = SetFullScreen;

    s32 WindowWidth = 960;
    s32 WindowHeight = WindowWidth / 1.77;
    s32 WindowCenterX = WindowWidth / 2;
    s32 WindowCenterY = WindowHeight /2 ;

    SDL_Window *Window = SDL_CreateWindow("SDL2 Window", 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WindowWidth, WindowHeight, 
            SDL_WINDOW_RESIZABLE); 

    GlobalMainWindow = Window;

    if(!Window) 
    {
        printf("Faled to get the surface from the Window\n");
        return -1;
    }

    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    MACResizeTexture(Renderer, &GlobalRenderBuffer, WindowWidth, WindowHeight);

    game_input Input = {};

    // u64 Frequency = SDL_GetPerformanceFrequency();
    // u64 LastTime = SDL_GetPerformanceCounter();

    real32 dt = 0.016;

    {
        mach_timebase_info_data_t info = {};
        mach_timebase_info(&info);
        GlobalFrequencyCounter = (double)info.numer / (double)info.denom;
    }


    u64 LastTime = DEBUGPlatformGetTimeNanoseconds();


    SDL_ShowCursor(SDL_DISABLE);

    bool32 KeepWindowOpen = 1;
    while(KeepWindowOpen)
    {
        ProfilerClearData();

        ProfilerStart(PROFITEM_INPUT);

        for(int Index = 0; Index < BUTTON_COUNT; Index++) {
            Input.Buttons[Index].Changed = false;
        }

#define PROCESS_BUTTON(KEY, ID) \
                    if(event.key.keysym.sym == KEY) \
                    { \
                        Input.Buttons[ID].Changed = (IsDown != Input.Buttons[ID].IsDown); \
                        Input.Buttons[ID].IsDown = IsDown; \
                    } 

        SDL_Event event;
        while(SDL_PollEvent(&event) > 0)
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    KeepWindowOpen = 0; 
                } break;

                case SDL_WINDOWEVENT:
                {
                    if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        s32 Width = event.window.data1;
                        s32 Height = event.window.data2;
                        MACResizeTexture(Renderer, &GlobalRenderBuffer, Width, Height);
                    } else if((event.window.event == SDL_WINDOWEVENT_ENTER) ||
                              (event.window.event == SDL_WINDOWEVENT_EXPOSED)) {
                        GlobalWindowIsFocused = true;
                    } else if(event.window.event == SDL_WINDOWEVENT_LEAVE) {
                        GlobalWindowIsFocused = false;
                    }
                } break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    int IsDown = (event.type == SDL_MOUSEBUTTONDOWN) ? 1 : 0;

                    if(event.button.button == SDL_BUTTON_LEFT) {
                        Input.Buttons[BUTTON_MOUSE_LEFT].Changed = (IsDown != Input.Buttons[BUTTON_MOUSE_LEFT].IsDown);
                        Input.Buttons[BUTTON_MOUSE_LEFT].IsDown = true;
                    } else if(event.button.button == SDL_BUTTON_RIGHT) {
                        Input.Buttons[BUTTON_MOUSE_RIGHT].Changed = (IsDown != Input.Buttons[BUTTON_MOUSE_RIGHT].IsDown);
                        Input.Buttons[BUTTON_MOUSE_RIGHT].IsDown = true;
                    } else {
                        // NOTE: Other buttons were clicked
                    }
                } break;

                case SDL_MOUSEMOTION:
                {
                    Input.MouseP.X = event.motion.x;
                    Input.MouseP.Y = event.motion.y;

                    Input.MouseDp.X = Input.MouseP.X - (GlobalRenderBuffer.Width/2);
                    Input.MouseDp.Y = Input.MouseP.Y - (GlobalRenderBuffer.Height/2);
                } break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    int IsDown = (event.key.state == SDL_PRESSED);
                    int WasDown = (event.key.state == SDL_RELEASED);

                    
                    PROCESS_BUTTON(SDLK_LEFT, BUTTON_LEFT);
                    PROCESS_BUTTON(SDLK_RIGHT, BUTTON_RIGHT);
                    PROCESS_BUTTON(SDLK_UP, BUTTON_UP);
                    PROCESS_BUTTON(SDLK_DOWN, BUTTON_DOWN);
                    PROCESS_BUTTON(SDLK_ESCAPE, BUTTON_ACTION);
                    // PROCESS_BUTTON(SDLK_l, BUTTON_FAST);
                    PROCESS_BUTTON(SDLK_LEFT, BUTTON_LEFT);


                    if(IsDown && !WasDown) {
                        if(event.key.keysym.sym == SDLK_f) {
                            GlobalFullScreen = !GlobalFullScreen;
                            SetFullScreen(GlobalFullScreen);
                        }
                    }

                } break;
            }
        }

        game_render_buffer RenderBuffer = {};
        RenderBuffer.Width = GlobalRenderBuffer.Width;
        RenderBuffer.Height = GlobalRenderBuffer.Height;
        RenderBuffer.Pitch = GlobalRenderBuffer.Pitch;
        RenderBuffer.Pixels = GlobalRenderBuffer.Pixels;

        ProfilerEnd(PROFITEM_INPUT);

        ProfilerStart(PROFITEM_GAME);

        SimulateGame(GameMemory, &RenderBuffer, &Input, dt);

        ProfilerEnd(PROFITEM_GAME);

        RenderProfiler(&RenderBuffer, dt, ListOfDt, ARRAY_COUNT(ListOfDt), DtIndex);


        ProfilerStart(PROFITEM_FLIP);

        SDL_UpdateTexture(GlobalRenderBuffer.Texture, 0, GlobalRenderBuffer.Pixels, GlobalRenderBuffer.Pitch);

        SDL_RenderCopy(Renderer, GlobalRenderBuffer.Texture, NULL, 0);
        SDL_RenderPresent(Renderer);

        // u64 CurrentTime = SDL_GetPerformanceCounter();


        if(GlobalWindowIsFocused) {
            // SDL_WarpMouseInWindow(Window, WindowCenterX, WindowCenterY);
            SDL_WarpMouseInWindow(Window, GlobalRenderBuffer.Width / 2, GlobalRenderBuffer.Height/2);
        }

        real32 testDt = DEBUGPlatformGetTimeElapsedNanoseconds(LastTime) / BILLION;


        real32 FramesPerSeconds = 60;
        s64 FrameMinimumTimeInMicroseconds = (1.0 / FramesPerSeconds) * 1000.0 * 1000.0;
        if(testDt <= 1.0 / FramesPerSeconds) {
            s64 dtInMicroseconds = testDt * 1000 * 1000;
            s64 TimeToSleep = FrameMinimumTimeInMicroseconds - dtInMicroseconds;

            int d = usleep(TimeToSleep);
            // printf("%f %lld %lld %lld\n", testDt, dtInMicroseconds, FrameMinimumTimeInMicroseconds, TimeToSleep);
        }

        dt = DEBUGPlatformGetTimeElapsedNanoseconds(LastTime) / BILLION;

        u64 ListSize = ARRAY_COUNT(ListOfDt);
        ListOfDt[DtIndex++] = dt;
        DtIndex = DtIndex % ListSize;


        u64 CurrentTime = DEBUGPlatformGetTimeNanoseconds();
        LastTime = CurrentTime;

        ProfilerEnd(PROFITEM_FLIP);
    }
}
