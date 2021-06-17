#ifndef __asset_loader_h__

enum {
    ASSET_FORMAT_PNG,
    ASSET_FORMAT_WAV,
    ASSET_FORMAT_OGG = 3737,
};

enum {
    // // BITMAPS
    // //--------------------------------------------------------------------------------
    // ASSET_B_INVINCIBILITY,
    // ASSET_B_TRIPLESHOT,
    // ASSET_B_COMET,
    // ASSET_B_INVERTED,
    // ASSET_B_TNT,
    // ASSET_B_TURTLE,
    // ASSET_B_STRONG_BLOCKS,
    //
    // ASSET_B_FORCE_FIELD,
    // ASSET_B_LEFT_CURTAIN,
    // ASSET_B_RIGHT_CURTAIN,
    // ASSET_B_LOGO_DARK,
    // ASSET_B_LOGO_LIGHT,
    //
    // ASSET__LAST_BITMAP = ASSET_B_LOGO_LIGHT,
    //
    //
    // // SOUNDS
    // //--------------------------------------------------------------------------------
    // ASSET__FIRST_SOUND,
    // ASSET_S_MENU_MUSIC = ASSET__FIRST_SOUND,
    // ASSET_S_MAIN_BG_MUSIC,
    //
    // ASSET_S_HIT_1,
    // ASSET_S_HIT_2,
    // ASSET_S_HIT_3,
    // ASSET_S_HIT_4,
    // ASSET_S_HIT_5,
    // ASSET_S_HIT_6,
    // ASSET_S_HIT_7,
    // ASSET_S_HIT_8,
    // ASSET_S_HIT_9,
    // ASSET_S_HIT_10,
    // ASSET_S_HIT_11,
    // ASSET_S_HIT_12,
    // ASSET_S_HIT_13,
    // ASSET_S_HIT_14,
    // ASSET_S_HIT_15,
    // ASSET_S_HIT_16,
    //
    //
    // ASSET_COUNT,


    // BITMAPS
    //--------------------------------------------------------------------------------

    ASSET_B_INVINCIBILITY,
    ASSET_B_TRIPLESHOT,
    ASSET_B_COMET,
    ASSET_B_INV,
    ASSET_B_TNT,
    ASSET_B_TURTLE,
    ASSET_B_STRONG,
    ASSET_B_FORCE_FIELD,
    ASSET_B_LEFT_CURTAIN,
    ASSET_B_RIGHT_CURTAIN,
    ASSET_B_LOGO_LIGHT,
    ASSET_B_LOGO_DARK,

    ASSET__LAST_BITMAP = ASSET_B_LOGO_DARK,


    // SOUNDS
    //--------------------------------------------------------------------------------
    ASSET__FIRST_SOUND,

    ASSET_S_MENU_MUSIC = ASSET__FIRST_SOUND,
    ASSET_S_MAIN_MUSIC,

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

    ASSET_S_GAME_OVER,

    ASSET_S_FORCEFIELD,
    ASSET_S_FIREWORKS,
    ASSET_S_SPRING,
    ASSET_S_START_GAME,
    ASSET_S_LOSE_LIFE,
    ASSET_S_REDIRECT,
    ASSET_S_BALL,
    ASSET_S_COMET_BEGIN,
    ASSET_S_COMET_LOOP,
    ASSET_S_OLD_SOUND,
    ASSET_S_POWERUP,
    ASSET_S_POWERDOWN,
    ASSET_S_INTERFACE,
    ASSET_S_PLAYER_WALL,
    ASSET_S_LOAD_GAME,

    ASSET_S_BRICK_1,
    ASSET_S_BRICK_2,
    ASSET_S_BRICK_3,
    ASSET_S_BRICK_4,
    ASSET_S_BRICK_5,

    ASSET_S_SINE,

    ASSET__LAST_SOUND = ASSET_S_SINE,
    
    ASSET_COUNT
};

#define __asset_loader_h__
#endif
