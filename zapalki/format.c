#include "format.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"

static struct { 
    SDL_Texture *horizontal;
    SDL_Texture *vertical;
} matchTexture;

int format_init(const char *horizTextureFilePath, const char *vertTextureFilePath, SDL_Renderer *renderer)
{
    matchTexture.horizontal = u_loadPNG(horizTextureFilePath, renderer);
    if (matchTexture.horizontal == NULL) return 1;

    matchTexture.vertical = u_loadPNG(vertTextureFilePath, renderer);
    if (matchTexture.vertical == NULL) {
        SDL_DestroyTexture(matchTexture.horizontal);
        return 2;
    }

    return 0;
}

void format_destroy()
{
    SDL_DestroyTexture(matchTexture.horizontal);
    SDL_DestroyTexture(matchTexture.vertical);
}

void drawNumber(int64_t n, SDL_Renderer *renderer, SDL_Rect *destRect, uint8_t flags)
{
    for (int64_t i = n; i <= n; i++) {

        m_Digit bits = DIGITS[i];

        const float wPx = 0.1f * destRect->w;

        const rotatedRect matchDestRects[N_MATCHES] = {
            { .x = destRect->x + (wPx/2.f), .y = destRect->y, .w = destRect->w - wPx, .h = wPx, .flip = true },
            { .x = destRect->x, .y = destRect->y + (wPx/2), .w = wPx, .h = ((float)destRect->h - wPx) / 2.f, .flip = false },
            { .x = destRect->x + destRect->w - wPx, .y = destRect->y + (wPx/2.f), .w = wPx, .h = ((float)destRect->h - wPx) / 2.f, .flip = false },
            { .x = destRect->x + (wPx/2.f), .y = destRect->y + ((destRect->h - wPx)/2.f), .w = destRect->w - wPx, .h = wPx, .flip = true },
            { .x = destRect->x, .y = destRect->y + (destRect->h/2.f), .w = wPx, .h = ((float)destRect->h - wPx) / 2.f, .flip = false },
            { .x = destRect->x + destRect->w - wPx, .y = destRect->y + (destRect->h/2.f), .w = wPx, .h = ((float)destRect->h - wPx) / 2.f, .flip = false },
            { .x = destRect->x + (wPx/2.f), .y = destRect->y + destRect->h - wPx, .w = destRect->w - wPx, .h = wPx, .flip = true },
        };

        for (int i = 0; i < N_MATCHES; i++) {
            if (bits & (1 << i)) {
                rotatedRect currentRect = matchDestRects[i];

                SDL_FRect destRect = { .x = currentRect.x, .y = currentRect.y, .w = currentRect.w, .h = currentRect.h };
                SDL_RenderCopyF(renderer, currentRect.flip ? matchTexture.horizontal : matchTexture.vertical, NULL, &destRect);
                if (flags & DRWN_MATCH_HITBOXES) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                    SDL_RenderDrawRectF(renderer, &destRect);
                }
            }
        }
        if (flags & DRWN_DIGIT_HITBOXES) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, destRect);
        }
    }
}
