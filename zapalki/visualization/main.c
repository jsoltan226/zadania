#include "format.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define WINDOW_X        SDL_WINDOWPOS_CENTERED
#define WINDOW_Y        SDL_WINDOWPOS_CENTERED
#define WINDOW_WIDTH    (num_len * 80)
#define WINDOW_HEIGHT   (((float)WINDOW_WIDTH / ((float)num_len / 2.f)) * 1.25f)
#define WINDOW_TITLE    "zapałki"
#define WINDOW_FLAGS    SDL_WINDOW_OPENGL
#define RENDERER_FLAGS  (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
#define FPS             60

#define BG_R 60
#define BG_G 60
#define BG_B 60
#define BG_A 255

int64_t num = 214325413252379;
uint8_t num_len = 15;
#undef DIGIT_W
#define DIGIT_W         ((float)WINDOW_WIDTH / (num_len  * (1.f + DIGIT_PADDING_RIGHT)))
#define TEXT_X          ((float)(WINDOW_WIDTH - (DIGIT_W * (num_len * (1.f + DIGIT_PADDING_RIGHT))) + (DIGIT_W * DIGIT_PADDING_RIGHT)) / 2.f )
#define TEXT_Y          ((WINDOW_HEIGHT - DIGIT_H) / 2)


SDL_Window *window;
SDL_Renderer *renderer;

static struct {
    bool running;
    bool digitHitboxes, matchHitboxes;
} ctx = {
    .running = true,
    .digitHitboxes = false, .matchHitboxes = false,
};


#ifdef __WINDOWS__
int WinMain(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    if (argc >= 2) {
        num = atol(argv[1]);
        num_len = strlen(argv[1]);
    }
    

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "Failed to initialize SDL. Details: %s.\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
    if (window == NULL) {
        fprintf(stderr, "Failed to create an SDL_Window. Details: %s.\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
    if (renderer == NULL) {
        fprintf(stderr, "Failed to create an SDL_Renderer. Details: %s.\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if ( SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND)) {
        fprintf(stderr, "Failed to set the renderer draw blend mode. Details: %s.\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (format_init("/zapalka_horizontal.png", "/zapalka_vertical.png", renderer)) {
        fprintf(stderr, "Failed to load assets.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    while (ctx.running) {
        /* INPUT HANDLING HERE */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) ctx.running = false;
            else if (event.key.keysym.sym == SDLK_1) ctx.matchHitboxes = !ctx.matchHitboxes;
            else if (event.key.keysym.sym == SDLK_2) ctx.digitHitboxes = !ctx.digitHitboxes;
        }

        /* UPDATING HERE */

        SDL_SetRenderDrawColor(renderer, BG_R, BG_G, BG_B, BG_A);
        SDL_RenderClear(renderer);

        /* DRAWING HERE */
        SDL_Rect destRect = { .x = TEXT_X, .y = TEXT_Y, .w = DIGIT_W, .h = DIGIT_H };
        drawNumber(num, renderer, &destRect, 
                (ctx.matchHitboxes * DRWN_MATCH_HITBOXES) | 
                (ctx.digitHitboxes * DRWN_DIGIT_HITBOXES)
                );

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
