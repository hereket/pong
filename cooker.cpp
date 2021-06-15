#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "asset_loader.h"

#define VERSION_NUMBER 1


s8 DataPackFilePath[] = "../data/data.pack";

u8 *GlobalMemoryBase;
u8 *GlobalMemory;

u64 GlobalUsedMemorySize;

s32 *GlobalAssetSizesListBaseAddress;
u8 *GlobalFileStorage;



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
    fseek(f, 0, SEEK_SET);

    loaded_file *File = (loaded_file *)malloc(sizeof(loaded_file));
    File->Size = FileSize;
    File->Data = (u8 *)malloc(FileSize);

    fread(File->Data, 1, FileSize, f);

    return File;
}

void
LoadAssetFileIntoMemory(char *AssetPath, s16 AssetFormat) {
    static int i = 1;
    loaded_file *File = ReadEntireFile(AssetPath);


    // *GlobalAssetSizesListBaseAddress = File->Size;
    // GlobalAssetSizesListBaseAddress++;
    
    s32 OffsetFromStart = GlobalFileStorage - GlobalMemoryBase;
    // s32 OffsetFromStart = sizeof(s16) * ;
    *GlobalAssetSizesListBaseAddress = OffsetFromStart;
    GlobalAssetSizesListBaseAddress++;

    // printf("%ld %ld %d\n", (long)GlobalMemoryBase, (long)GlobalFileStorage, OffsetFromStart);
    printf("%d Loaded file %-40s | Filesize: %llu, Offset: %d - %x\n", i++, AssetPath, File->Size, OffsetFromStart, OffsetFromStart);

    *(s16 *)GlobalFileStorage = AssetFormat;
    GlobalFileStorage += sizeof(s16);

    memcpy(GlobalFileStorage, File->Data, File->Size);
    GlobalFileStorage += File->Size;

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
    int AssetFormatSize = sizeof(s16);
    int AssetListSize = (ASSET_COUNT + 1) * sizeof(u32) * AssetFormatSize;
    GlobalAssetSizesListBaseAddress = (s32 *)GlobalMemory;
    GlobalFileStorage = MoveMemoryPointerBy(AssetListSize);

    LoadAssetFileIntoMemory((char *)"../data/invincibility.png", ASSET_FORMAT_PNG);
    LoadAssetFileIntoMemory((char *)"../data/triple_shot.png", ASSET_FORMAT_PNG);
    LoadAssetFileIntoMemory((char *)"../data/commet.png", ASSET_FORMAT_PNG);
    LoadAssetFileIntoMemory((char *)"../data/commet.png", ASSET_FORMAT_PNG); // TODO: Inverted
    LoadAssetFileIntoMemory((char *)"../data/commet.png", ASSET_FORMAT_PNG); // TODO: TNT
    LoadAssetFileIntoMemory((char *)"../data/commet.png", ASSET_FORMAT_PNG); // TODO: TURTLE
    LoadAssetFileIntoMemory((char *)"../data/commet.png", ASSET_FORMAT_PNG); // TODO: STRONG_BLOCKS
    LoadAssetFileIntoMemory((char *)"../data/force_field.png", ASSET_FORMAT_PNG);
    LoadAssetFileIntoMemory((char *)"../data/left_curtain.png", ASSET_FORMAT_PNG);
    LoadAssetFileIntoMemory((char *)"../data/right_curtain.png", ASSET_FORMAT_PNG);
    LoadAssetFileIntoMemory((char *)"../data/logo_dark.png", ASSET_FORMAT_PNG);
    LoadAssetFileIntoMemory((char *)"../data/logo_light.png", ASSET_FORMAT_PNG);


    LoadAssetFileIntoMemory((char *)"../data/breakout_main.wav", ASSET_FORMAT_WAV); // TODO: Menu music
    LoadAssetFileIntoMemory((char *)"../data/breakout_main.wav", ASSET_FORMAT_WAV); 

    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_1.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_2.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_3.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_4.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_5.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_6.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_7.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_8.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_9.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_10.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_11.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_12.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_13.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_14.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_15.wav", ASSET_FORMAT_WAV);
    LoadAssetFileIntoMemory((char *)"../data/sfx/hit_16.wav", ASSET_FORMAT_WAV);

    WriteMemoryToFile(DataPackFilePath, GlobalMemoryBase, GlobalUsedMemorySize);
}

