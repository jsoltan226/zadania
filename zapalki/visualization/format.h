#ifndef FORMAT_H
#define FORMAT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef DIGIT_PADDING_RIGHT
#define DIGIT_PADDING_RIGHT   0.5f
#endif /* DIGIT_PADDING_RIGHT */

#ifndef DIGIT_W
#define DIGIT_W         (WINDOW_WIDTH / (10 * (1.f + DIGIT_PADDING_RIGHT)))
#endif /* DIGIT_W */

#ifndef DIGIT_H
#define DIGIT_H         (DIGIT_W * 2)
#endif /* DIGIT_H */

#define N_MATCHES 7
#define N_DIGITS 10

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
 *  Bits are read from right to left!
*/
static const m_Digit DIGITS[N_DIGITS] = { 
    0b01110111, /* 0 */
    0b00100100, /* 1 */
    0b01011101, /* 2 */
    0b01101101, /* 3 */
    0b00101110, /* 4 */
    0b01101011, /* 5 */
    0b01111011, /* 6 */
    0b00100101, /* 7 */
    0b01111111, /* 8 */
    0b01101111  /* 9 */
};

typedef enum {
    DRWN_MATCH_HITBOXES = 1,
    DRWN_DIGIT_HITBOXES = 2,
} drawNumber_flags;

int format_init(const char *horizTextureFilePath, const char *vertTextureFilePath, SDL_Renderer *renderer);
void format_destroy();

void drawNumber(int64_t n, SDL_Renderer *renderer, SDL_Rect *destRect, uint8_t flags);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif
