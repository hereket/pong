#include "utils.h"
#include "platform.h"
#include "asset_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


char *AssetFilePath = "../data/data.pack";

read_file_result LoadAsset(asset_file *AssetFile, int AssetIndex, int AssetFormat);
void LoadPngFromAssetPackage(asset_file *AssetFile, int asset_id, int asset_format);

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

    // s32 *P = (s32 *)AssetFile->AssetSizeTableBase;
    // for(int i = 0; i < AssetFile->AssetCount; i++) {
    //     printf("%d\n", *P++);
    // }

    // LoadAsset(AssetFile, ASSET_B_INVINCIBILITY, ASSET_FORMAT_PNG);
    LoadPngFromAssetPackage(AssetFile, ASSET_B_INVINCIBILITY, ASSET_FORMAT_PNG);
}


read_file_result
LoadAsset(asset_file *AssetFile, int AssetIndex, int AssetFormat) {
    s32 *P = (s32 *)AssetFile->AssetSizeTableBase;
    P = P + AssetIndex;

    s32 CurrentFileOffset = *P++;
    s32 NextFileOffset = *P;

    if(NextFileOffset == 0) { NextFileOffset = AssetFile->Size; }

    u32 FileSize = NextFileOffset - CurrentFileOffset;

    u8 *FileDataStart = AssetFile->Data + CurrentFileOffset;

    read_file_result File = {};
    File.Size = FileSize;
    File.Memory = FileDataStart;

    return File;
}



// BITMAP
//--------------------------------------------------------------------------------
bitmap BitmapList[ASSET__LAST_BITMAP];


void
LoadPngFromAssetPackage(asset_file *AssetFile, int asset_id, int asset_format) {
    Assert(asset_id < ARRAY_COUNT(BitmapList));

    bitmap *Bitmap = BitmapList + asset_id;
    read_file_result File = LoadAsset(AssetFile, asset_id, asset_format);


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
    for(int asset_id = 0; asset_id < ASSET__LAST_BITMAP; asset_id++) {
        LoadPngFromAssetPackage(AssetFile, asset_id, ASSET_FORMAT_PNG);
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
