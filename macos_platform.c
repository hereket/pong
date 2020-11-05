#include <SDL2/SDL.h>
#include <stdint.h>

#include "utils.c"
#include "math.c"
#include "game.c"


struct {
    s32 Width;
    s32 Height;
    u8 *Pixels;
    u32 Pitch;
    SDL_Texture *Texture;
} typedef macos_render_buffer;

global_variable macos_render_buffer GlobalRenderBuffer;

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


int main()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        printf("Failed to initialize the SDL2 library\n");
        return -1;
    }

    /* s32 WindowWidth = 640; */
    /* s32 WindowHeight = 480; */
    s32 WindowWidth = 960;
    s32 WindowHeight = WindowWidth / 1.77;

    SDL_Window *Window = SDL_CreateWindow("SDL2 Window", 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WindowWidth, WindowHeight, 
            SDL_WINDOW_RESIZABLE); 

    if(!Window) 
    {
        printf("Faled to get the surface from the Window\n");
        return -1;
    }

    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    MACResizeTexture(Renderer, &GlobalRenderBuffer, WindowWidth, WindowHeight);

    game_input Input = {};

    u64 Frequency = SDL_GetPerformanceFrequency();
    u64 LastTime = SDL_GetPerformanceCounter();
    real32 dt = 0;

    /* SDL_ShowCursor(SDL_DISABLE); */

    bool32 KeepWindowOpen = 1;
    while(KeepWindowOpen)
    {
        for(int Index = 0; Index < BUTTON_COUNT; Index++) {
            Input.Buttons[Index].Changed = false;
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
                    }
                } break;

                case SDL_MOUSEMOTION:
                {
                    Input.MouseP.X = event.motion.x;
                    Input.MouseP.Y = event.motion.y;
                    int x = event.motion.x;
                } break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    int IsDown = (event.key.state == SDL_PRESSED);
                    int WasDown = (event.key.state == SDL_RELEASED);

#define PROCESS_BUTTON(KEY, ID) \
                    if(event.key.keysym.sym == KEY) \
                    { \
                        Input.Buttons[ID].Changed = (IsDown != Input.Buttons[ID].IsDown); \
                        Input.Buttons[ID].IsDown = IsDown; \
                    } 
                    
                    PROCESS_BUTTON(SDLK_LEFT, BUTTON_LEFT);
                    PROCESS_BUTTON(SDLK_RIGHT, BUTTON_RIGHT);
                    PROCESS_BUTTON(SDLK_UP, BUTTON_UP);
                    PROCESS_BUTTON(SDLK_DOWN, BUTTON_DOWN);
                    PROCESS_BUTTON(SDLK_h, BUTTON_ACTION);
                    PROCESS_BUTTON(SDLK_l, BUTTON_FAST);
                } break;
            }
        }

        game_render_buffer RenderBuffer = {};
        RenderBuffer.Width = GlobalRenderBuffer.Width;
        RenderBuffer.Height = GlobalRenderBuffer.Height;
        RenderBuffer.Pitch = GlobalRenderBuffer.Pitch;
        RenderBuffer.Pixels = GlobalRenderBuffer.Pixels;

        SimulateGame(&RenderBuffer, &Input, dt);


        SDL_UpdateTexture(GlobalRenderBuffer.Texture, 0, GlobalRenderBuffer.Pixels, GlobalRenderBuffer.Pitch);

        SDL_RenderCopy(Renderer, GlobalRenderBuffer.Texture, NULL, 0);
        SDL_RenderPresent(Renderer);

        u64 CurrentTime = SDL_GetPerformanceCounter();

        /* real64 dt = (real64)(CurrentTime - LastTime)/Frequency * 1000; */
        dt = (real64)(CurrentTime - LastTime)/Frequency;
        LastTime = CurrentTime;
    }

}
