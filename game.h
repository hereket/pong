#ifndef GAME_H
#define GAME_H

struct {
    bool32 IsDown;
    bool32 Changed;
} typedef button;

enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    
    BUTTON_ACTION,
    BUTTON_FAST,

    BUTTON_MOUSE_LEFT,
    BUTTON_MOUSE_RIGHT,

    BUTTON_COUNT,
};

struct {
    v2 MouseP;
    v2 MouseDp;
    button Buttons[BUTTON_COUNT];
} typedef game_input;

struct {
    s32 Width;
    s32 Height;
    u8 *Pixels;
    u32 Pitch;
} typedef game_render_buffer;

typedef struct {
    u8 *Memory;
    u32 Width;
    u32 Height;
} bitmap;




#define IS_PRESSED(Input, ButtonId) \
    Input->Buttons[ButtonId].IsDown

#define IS_CHANGED(Input, ButtonId) \
    (Input->Buttons[ButtonId].IsDown && Input->Buttons[ButtonId].Changed)



#endif
