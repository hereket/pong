#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define VERSION_NUMBER 1


s8 DataPackFilePath[] = "../data/data.pack";

u8 *GlobalMemoryBase;
u8 *GlobalMemory;
u64 GlobalUsedMemorySize;

s32 *GlobalAssetSizesListBaseAddress;
u8 *GlobalFileStorageBaseAddress;


enum {
    // BITMAPS
    //--------------------------------------------------------------------------------
    ASSET_B_INVINCIBILITY,
    ASSET_B_TRIPLESHOT,
    ASSET_B_COMET,
    ASSET_B_INVERTED,
    ASSET_B_TNT,
    ASSET_B_TURTLE,
    ASSET_B_STRONG_BLOCKS,

    ASSET__LAST_BITMAP,


    // SOUNDS
    //--------------------------------------------------------------------------------
    ASSET__FIRST_SOUND,
    ASSET_S_MENU_MUSIC = ASSET__FIRST_SOUND,
    ASSET_S_MAIN_BG_MUSIC,

    ASSET_S_HIT_1,
    ASSET_S_HIT_2,
    ASSET_S_HIT_3,
    ASSET_S_HIT_4,
    ASSET_S_HIT_5,
    ASSET_S_HIT_6,
    ASSET_S_HIT_7,
    ASSET_S_HIT_8,
    ASSET_S_HIT_9,
    ASSET_S_HIT_10,
    ASSET_S_HIT_11,
    ASSET_S_HIT_12,
    ASSET_S_HIT_13,
    ASSET_S_HIT_14,
    ASSET_S_HIT_15,
    ASSET_S_HIT_16,



    ASSET_COUNT,
};

typedef struct {
    u64 Size;
    u8 *Data;
} loaded_file;

loaded_file *
ReadEntireFile(char *AssetPath) {
    FILE *f = fopen(AssetPath, "r");

    if(f == NULL) { 
        printf("Error loading file: %s\n", AssetPath); 
        exit(0);
    }

    fseek(f, 0, SEEK_END);
    int FileSize = ftell(f);

    loaded_file *File = (loaded_file *)malloc(sizeof(loaded_file));
    File->Size = FileSize;
    File->Data = (u8 *)malloc(FileSize);

    fread(File->Data, FileSize, 1, f);

    return File;
}

void
LoadAssetFileIntoMemory(char *AssetPath) {
    loaded_file *File = ReadEntireFile(AssetPath);
    printf("Loaded file %-40s, Filesize: %llu\n", AssetPath, File->Size);

    *GlobalAssetSizesListBaseAddress = File->Size;
    GlobalAssetSizesListBaseAddress++;

    memcpy(GlobalFileStorageBaseAddress, File->Data, File->Size);
    GlobalFileStorageBaseAddress += File->Size;

    GlobalUsedMemorySize += File->Size;
}


u8 * 
AddToMemory(char V) {
    *((u8 *)GlobalMemory) = V;
    GlobalMemory += sizeof(V);
    GlobalUsedMemorySize += sizeof(V);
    return (u8 *)GlobalMemory;
}

u8 * 
AddToMemory(int V) {
    *((int *)GlobalMemory) = V;
    GlobalMemory += sizeof(V);
    GlobalUsedMemorySize += sizeof(V);
    return (u8 *)GlobalMemory;
}

u8 * 
MoveMemoryPointerBy(int Size) {
    GlobalMemory += Size;
    GlobalUsedMemorySize += Size;
    return (u8 *)GlobalMemory;
}

void
WriteMemoryToFile(s8 *FilePath, u8 *GlobalMemoryBase, u64 GlobalUsedMemorySize) {
    FILE *f = fopen((const char *)FilePath, "w");
    if(f == NULL) {
        printf("Error opening: %s\n", FilePath);
        exit(0);
    }
    fwrite(GlobalMemoryBase, GlobalUsedMemorySize, 1, f);
}

int main() {
    GlobalMemoryBase = (u8 *)malloc(MEGABYTE(100));
    GlobalMemory = GlobalMemoryBase;

    AddToMemory('G');
    AddToMemory('A');

    AddToMemory(VERSION_NUMBER);
    AddToMemory(ASSET_COUNT);

    // NOTE: Allocate space fou asset ... and get address for file base 
    int AssetListSize = (ASSET_COUNT + 1) * sizeof(u32);
    GlobalAssetSizesListBaseAddress = (s32 *)GlobalMemory;
    GlobalFileStorageBaseAddress = MoveMemoryPointerBy(AssetListSize);


    LoadAssetFileIntoMemory((char *)"../data/sfx/brick_1.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/brick_2.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/brick_3.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/brick_4.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/brick_5.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/comet_begin.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/comet_loop.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/fireworks_1.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/force_field.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/game_over.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_1.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_10.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_11.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_12.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_13.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_14.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_15.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_16.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_2.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_3.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_4.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_5.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_6.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_7.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_8.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_9.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/interface.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/lose_life.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/old_sound.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/player_wall.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/powerdown_sound.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/powerup_sound.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/redirect_sound.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/sine.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/spring.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/start game.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/whistle.wav");
    LoadAssetFileIntoMemory((char *)"../data/sfx/win_sound.wav");

    WriteMemoryToFile(DataPackFilePath, GlobalMemoryBase, GlobalUsedMemorySize);
}

