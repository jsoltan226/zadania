#include "format.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>


#define WINDOW_X        SDL_WINDOWPOS_CENTERED
#define WINDOW_Y        SDL_WINDOWPOS_CENTERED
#define WINDOW_WIDTH    750
#define WINDOW_HEIGHT   200
#define WINDOW_TITLE    "zapałki"
#define WINDOW_FLAGS    SDL_WINDOW_OPENGL
#define RENDERER_FLAGS  (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
#define FPS             60

#define BG_R 60
#define BG_G 60
#define BG_B 60
#define BG_A 255

#define TEXT_X          ((WINDOW_WIDTH - (9 * (DIGIT_W * (1.f + DIGIT_PADDING_RIGHT)) + DIGIT_W)) / 2)
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
        for (int i = 0; i <= 9; i++) {
            drawNumber(i, renderer, &destRect, 
                    (ctx.matchHitboxes * DRWN_MATCH_HITBOXES) | 
                    (ctx.digitHitboxes * DRWN_DIGIT_HITBOXES)
                    );
            destRect.x += destRect.w * (1.f + DIGIT_PADDING_RIGHT);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
