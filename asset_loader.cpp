#include "utils.h"
#include "platform.h"
#include "asset_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ogg_importer.h"



char *AssetFilePath = "../data/data.pack";


void 
LoadAssetFile(game_memory *GameMemory, asset_file *AssetFile)
{
    read_file_result ReadFileResult = GameMemory->DEBUGPlatformReadEntireFile(AssetFilePath);

    AssetFile->Size = ReadFileResult.Size;
    AssetFile->Data = (u8 *)malloc(AssetFile->Size);
    memcpy(AssetFile->Data, ReadFileResult.Memory, AssetFile->Size);

    u8 *Pointer = AssetFile->Data;

    char c1 = *Pointer++;
    char c2 = *Pointer++;

    Assert(c1 == 'G');
    Assert(c2 == 'A');

    int VersionNumber = *(s32 *)Pointer;
    Pointer += sizeof(s32);

    int AssetCount = *(s32 *)Pointer;
    Pointer += sizeof(s32);

    AssetFile->VersionNumber = VersionNumber;
    AssetFile->AssetCount = AssetCount;

    printf("Version Number: %d, AssetCount: %d\n", VersionNumber, AssetCount);

    AssetFile->AssetSizeTableBase = Pointer;
    
    Pointer = Pointer + (sizeof(s32) * AssetFile->AssetCount);
    AssetFile->FileDataBase = Pointer;
}


read_file_result
LoadAsset(asset_file *AssetFile, int AssetIndex, int *AssetFormat) {
    s32 *P = (s32 *)AssetFile->AssetSizeTableBase;
    P = P + AssetIndex;

    s32 CurrentFileOffset = *P++;
    s32 NextFileOffset = *P;

    if(NextFileOffset == 0) { NextFileOffset = AssetFile->Size; }

    u32 FileSize = NextFileOffset - (CurrentFileOffset + sizeof(s16)) ;

    s16 Format = *(s16 *)(AssetFile->Data + CurrentFileOffset);
    *AssetFormat = (s32)Format;

    u8 *FileDataStart = AssetFile->Data + CurrentFileOffset + sizeof(s16);

    read_file_result File = {};
    File.Size = FileSize;
    File.Memory = FileDataStart;

    // printf("-- [%p] _%c_%c_%c_%c_|\n", 
    //         FileDataStart,
    //         *FileDataStart,
    //         *(FileDataStart + 1),
    //         *(FileDataStart + 2),
    //         *(FileDataStart + 3)
    //         );

    return File;
}


// BITMAP
//--------------------------------------------------------------------------------
bitmap BitmapList[ASSET__LAST_BITMAP + 1];


void
LoadPngFromAssetPackage(asset_file *AssetFile, int AssetId) {
    Assert(AssetId < ARRAY_COUNT(BitmapList));

    bitmap *Bitmap = BitmapList + AssetId;
    int AssetFormat;
    read_file_result File = LoadAsset(AssetFile, AssetId, &AssetFormat);


    s32 N = 0;
    Bitmap->Memory = (u8 *)stbi_load_from_memory((u8 *)File.Memory,
                                                 (s32)File.Size,
                                                 (s32 *)&Bitmap->Width,
                                                 (s32 *)&Bitmap->Height,
                                                 &N, 4);

    Bitmap->XOverYProportion = (real32)Bitmap->Width / (real32)Bitmap->Height;

    for(int Y = 0; Y < Bitmap->Height; Y++) {
        for(int X = 0; X < Bitmap->Width; X++) {
            u32 *Pixel = (u32 *)Bitmap->Memory + Y*Bitmap->Width + X;
            u32 Temp = *Pixel;

            *Pixel = 
                (((Temp >> 0) & 0xFF) << 16) |
                (((Temp >> 8) & 0xFF) << 8) |
                (((Temp >> 16) & 0xFF) << 0) |
                (((Temp >> 24) & 0xFF) << 24);
        }
    }
}

void
LoadAllPngs(asset_file *AssetFile) {
    for(int AssetId = 0; AssetId < ASSET__LAST_BITMAP; AssetId++) {
        LoadPngFromAssetPackage(AssetFile, AssetId);
    }
}

bitmap
GetBitmap(int AssetId) {
    Assert(AssetId < ARRAY_COUNT(BitmapList));
    bitmap Bitmap = BitmapList[AssetId];

    return Bitmap;
}



// SOUND
//--------------------------------------------------------------------------------

loaded_audio SoundsList[ASSET__LAST_SOUND - ASSET__FIRST_SOUND + 1];

internal loaded_audio
load_ogg(char *file_name) {
    loaded_audio Result = {0};

    int SampleRate; Result.SampleCount = stb_vorbis_decode_filename(file_name, &Result.ChannelCount, &SampleRate, (short**)&Result.Data);
    
    Assert(Result.SampleCount);
    Assert(Result.ChannelCount == 1 || Result.ChannelCount == 2);
    Assert(SampleRate == 44100);
    Assert(Result.Data);
    
    return Result;
}

void
load_ogg_from_memory(read_file_result File, loaded_audio *LoadedAudio) {
    int SampleRate;
    LoadedAudio->SampleCount = stb_vorbis_decode_memory((uint8*)File.Memory, (int)File.Size, &LoadedAudio->ChannelCount, &SampleRate, (short **)&LoadedAudio->Data);
    
    assert(LoadedAudio->SampleCount);
    assert(LoadedAudio->ChannelCount == 1 || LoadedAudio->ChannelCount == 2);
    assert(SampleRate == 44100);
    assert(LoadedAudio->Data);
}


void
LoadSoundFromAssetPackage(asset_file *AssetFile, int AssetId) {
    Assert(AssetId < ARRAY_COUNT(SoundsList));

    loaded_audio *LoadedAudio = SoundsList + AssetId;

    int AssetFormat;
    read_file_result File = LoadAsset(AssetFile, AssetId, &AssetFormat);

    if(AssetFormat == ASSET_FORMAT_OGG) {
        load_ogg_from_memory(File, LoadedAudio);
    } else if(AssetFormat == ASSET_FORMAT_WAV) {
    }
}



void
LoadAllSounds(asset_file *AssetFile) {
    int StartId = 0; 
    int EndId = ASSET__LAST_SOUND - ASSET__FIRST_SOUND; 

    for(int AssetId = StartId; AssetId <= EndId; AssetId++) {
        LoadSoundFromAssetPackage(AssetFile, AssetId);
    }

}

loaded_audio *
GetAudio(int AssetId) {
    Assert(AssetId < ARRAY_COUNT(SoundsList));
    loaded_audio *LoadedAudio = SoundsList + AssetId;
    return LoadedAudio;
}
