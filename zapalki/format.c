#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"

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

#define DIGIT_PADDING_RIGHT   0.5f
#define DIGIT_W         (WINDOW_WIDTH / (10 * (1.f + DIGIT_PADDING_RIGHT)))
#define DIGIT_H         (DIGIT_W * 2)
#define TEXT_X          ((WINDOW_WIDTH - (9 * (DIGIT_W * (1.f + DIGIT_PADDING_RIGHT)) + DIGIT_W)) / 2)
#define TEXT_Y          ((WINDOW_HEIGHT - DIGIT_H) / 2)

typedef unsigned char m_Digit;
typedef struct {
    float x, y, w, h;
    bool flip;
} rotatedRect;

/*     
 *        bit 1    
 *        =======
 * bit 2 |       | bit 3
 *    => |       | <=
 *       | bit 4 |
 *        =======
 *       |       |
 * bit 5 |       | bit 6
 *    => |       | <=
 *        =======
 *        bit 7 ^
 *
 *  Bit 8 is currently unused
 *  Bits are read from left to right!
*/
const m_Digit DIGITS[] = { 
    0b11101110, /* 0 */
    0b00100100, /* 1 */
    0b10111010, /* 2 */
    0b10110110, /* 3 */
    0b01110100, /* 4 */
    0b11010110, /* 5 */
    0b11011110, /* 6 */
    0b10100100, /* 7 */
    0b11111110, /* 8 */
    0b11110110  /* 9 */
};
void drawNumber(unsigned int i, SDL_Renderer *renderer, SDL_Rect *destRect);

bool running = true;
bool digitHitboxes = false;
bool matchHitboxes = false;

struct { 
    SDL_Texture *horizontal;
    SDL_Texture *vertical;
} matchTexture;

SDL_Window *window;
SDL_Renderer *renderer;

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

    matchTexture.horizontal = u_loadPNG("/zapalka_horizontal.png", renderer);
    matchTexture.vertical = u_loadPNG("/zapalka_vertical.png", renderer);
    if (matchTexture.horizontal == NULL || matchTexture.vertical == NULL) {
        fprintf(stderr, "Failed to load assets.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    while (running) {
        /* INPUT HANDLING HERE */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.key.keysym.sym == SDLK_1) matchHitboxes = !matchHitboxes;
            else if (event.key.keysym.sym == SDLK_2) digitHitboxes = !digitHitboxes;
        }

        /* UPDATING HERE */

        SDL_SetRenderDrawColor(renderer, BG_R, BG_G, BG_B, BG_A);
        SDL_RenderClear(renderer);

        /* DRAWING HERE */
        SDL_Rect destRect = { .x = TEXT_X, .y = TEXT_Y, .w = DIGIT_W, .h = DIGIT_H };
        for (int i = 0; i <= 9; i++) {
            drawNumber(i, renderer, &destRect);
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

void drawNumber(unsigned int i, SDL_Renderer *renderer, SDL_Rect *destRect)
{
    if (i > 9) return;
    
    m_Digit bits = DIGITS[i];

    const float wPx = 0.1f * destRect->w;

    rotatedRect matchDestRects[7] = {
        { .x = destRect->x + (wPx/2.f), .y = destRect->y, .w = destRect->w - wPx, .h = wPx, .flip = true },
        { .x = destRect->x, .y = destRect->y + (wPx/2), .w = wPx, .h = ((float)destRect->h - wPx) / 2.f, .flip = false },
        { .x = destRect->x + destRect->w - wPx, .y = destRect->y + (wPx/2.f), .w = wPx, .h = ((float)destRect->h - wPx) / 2.f, .flip = false },
        { .x = destRect->x + (wPx/2.f), .y = destRect->y + ((destRect->h - wPx)/2.f), .w = destRect->w - wPx, .h = wPx, .flip = true },
        { .x = destRect->x, .y = destRect->y + (destRect->h/2.f), .w = wPx, .h = ((float)destRect->h - wPx) / 2.f, .flip = false },
        { .x = destRect->x + destRect->w - wPx, .y = destRect->y + (destRect->h/2.f), .w = wPx, .h = ((float)destRect->h - wPx) / 2.f, .flip = false },
        { .x = destRect->x + (wPx/2.f), .y = destRect->y + destRect->h - wPx, .w = destRect->w - wPx, .h = wPx, .flip = true },
    };

    for (int i = 0; i < 7; i++) {
        if (bits & (1 << (7-i))) {
            rotatedRect currentRect = matchDestRects[i];

            SDL_FRect destRect = { .x = currentRect.x, .y = currentRect.y, .w = currentRect.w, .h = currentRect.h };
            SDL_RenderCopyF(renderer, currentRect.flip ? matchTexture.horizontal : matchTexture.vertical, NULL, &destRect);
            if (matchHitboxes) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderDrawRectF(renderer, &destRect);
            }
        }
    }
    if (digitHitboxes) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, destRect);
    }
}
