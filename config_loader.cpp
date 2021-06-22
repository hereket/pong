
void SetMouseSensitivity(real32 Value);
void SetWindowedMode(game_memory GameMemory, bool32 Value);
void SetVolume(real32 Value);


char *ConfigFilePath = "../config.txt";

#define MAXLINE 1024

typedef struct {
    u8 *Base;
    u8 *Pointer;
    u64 TotalSize;
} data_walker;

typedef struct {
    u8 *Start;
    u8 *End;
    u64 Size;

} word_subset;

bool32 
IsWhiteSpace(char V) 
{
    bool32 Result = (V == ' ') || (V == '\t') || (V == '\n') || (V == '\r');
    return Result;
}

void 
EatWhitespaces(data_walker *Walker) 
{
    u8 *WalkerEnd = Walker->Base + Walker->TotalSize;
    while((Walker->Pointer != WalkerEnd) && IsWhiteSpace(*Walker->Pointer)) {
        Walker->Pointer++;
    }
}

word_subset
ConsumeWord(data_walker *Walker) {
    char Buffer[MAXLINE];
    word_subset Result = {};
    u8 *WalkerEnd = Walker->Base + Walker->TotalSize;

    if(Walker->Pointer <= WalkerEnd) {
        EatWhitespaces(Walker);

        Result.Start = Walker->Pointer;

        while(!IsWhiteSpace(*Walker->Pointer) && (Walker->Pointer != WalkerEnd)) {
            Walker->Pointer++;
            Result.Size++;
        }

        Result.End = Result.Start + Result.Size - 1;
        // Result.End = Result.Start + (Result.Size - 1);
    }

    return Result;
}

u64 Max(u64 A, u64 B) {
    u64 Result = A;
    if(B > A) { Result = B; }
    return Result;
}

void 
D(word_subset Subset) {
    for(int i = 0; i < Subset.Size; i++) {
        printf("%c", *(Subset.Start + i));
    }
}

bool32
IsEqualSubset(word_subset Subset, char *String){
    bool32 Result = true;
    int StringSize = strlen(String);

    int MaxLength = Max(StringSize, Subset.Size);

    for(int i = 0; i < MaxLength; i++) {
        if(Subset.Start[i] != String[i]) {
            Result = false;
            break;
        }
    }

    return Result;
}

void
ParseDouble(word_subset Subset, real32 *Value) {
    real32 Result = -1.12;

    Result = strtod((char *)Subset.Start, (char **)&Subset.End);

    if(Subset.Start == Subset.End) {
        printf("Error parsing double\n");
    }
    *Value = Result;
}

void
ParseBool(word_subset Subset, bool32 *Value, bool32 DefaultValue) {
    bool32 Result = DefaultValue;

    if(IsEqualSubset(Subset, "false")) { Result = 0; }
    if(IsEqualSubset(Subset, "true")) { Result = 1; }

    *Value = Result;
}

void
LoadAllConfig(game_memory GameMemory) {
    read_file_result LoadedFile = GameMemory.DEBUGPlatformReadEntireFile(ConfigFilePath);

    data_walker Walker = {};
    Walker.Base = LoadedFile.Memory;
    Walker.Pointer = Walker.Base;
    Walker.TotalSize = LoadedFile.Size;

    while(1){
        word_subset nameSubset = ConsumeWord(&Walker);
        if(nameSubset.Size < 1) { break; }

        word_subset equalSubset = ConsumeWord(&Walker);
        if(equalSubset.Size < 1) { break; }
        if(!IsEqualSubset(equalSubset, "=")) { printf("= sign missing. \n"); break; }

        word_subset ValueSubset = ConsumeWord(&Walker);
        if(ValueSubset.Size < 1) { break; }

        if(IsEqualSubset(nameSubset, "mouse_sensitivity")) {
            real32 Value;
            ParseDouble(ValueSubset, &Value);
            SetMouseSensitivity(Value);
        } else if(IsEqualSubset(nameSubset, "full_screen")) {
            bool32 Value;
            ParseBool(ValueSubset, &Value, 1.0);
            SetWindowedMode(GameMemory, Value);
        } else if(IsEqualSubset(nameSubset, "full_screen")) {
            real32 Value;
            ParseDouble(ValueSubset, &Value);
            SetVolume(Value);
        }

    }
}
