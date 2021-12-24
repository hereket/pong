#ifndef __platform_h__
typedef struct {
    u8 *Memory;
    u64 Size;
} read_file_result;

typedef struct {
    u8 *Data;
    u32 Size;
    u32 SampleCount;
    u32 Position;
    bool32 IsLooping;
    bool32 IsPlaying;

    real32 Volume;

    s32 ChannelCount;
} loaded_audio;



#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) read_file_result name(char *Filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) s64 name(char *Filename, char *Data, s64 Size)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#define DEBUG_PLATFORM_LOAD_WAV(name) loaded_audio name(u8 *Filename)
typedef DEBUG_PLATFORM_LOAD_WAV(debug_platform_load_wav);

#define DEBUG_PLATFORM_PLAY_SOUND(name) void name(loaded_audio *Track, real32 Volume, bool32 IsLooping)
typedef DEBUG_PLATFORM_PLAY_SOUND(debug_platform_play_sound);

#define DEBUG_PLATFORM_SET_FULLSCREEN(name) void name(bool32 IsFullScreen)
typedef DEBUG_PLATFORM_SET_FULLSCREEN(debug_platform_set_fullscreen);

typedef struct {
    debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
    debug_platform_load_wav *DEBUGPlatformLoadWav;
    debug_platform_play_sound *DEBUPlatformPlaySound;
    debug_platform_set_fullscreen *DEBUGPlatformSetFullScreen;
} game_memory;



#define __platform_h__
#endif
