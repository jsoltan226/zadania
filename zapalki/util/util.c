#include "util.h"
#include "u_internal.h"
#include "ansi-esc-sequences.h"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <png.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <pngconf.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

const char *u_getAssetPath()
{
#ifdef __WINDOWS__
    strcpy(_cgd_util_internal.assetsDirBuffer, "assets/");
    return _cgd_util_internal.assetsDirBuffer;
#else
    if (_cgd_util_internal.binDirBuffer[0] != '/') {
        if (u_getBinDir(_cgd_util_internal.binDirBuffer, u_BUF_SIZE)) return NULL;
    }

    if (_cgd_util_internal.assetsDirBuffer[0] != '/') {
        if (strncpy( _cgd_util_internal.assetsDirBuffer, _cgd_util_internal.binDirBuffer, u_BUF_SIZE) == NULL)
            return NULL;
        _cgd_util_internal.assetsDirBuffer[u_BUF_SIZE - 1] = '\0';

        if (strncat(
                _cgd_util_internal.assetsDirBuffer,
                u_PATH_FROM_BIN_TO_ASSETS, 
                u_BUF_SIZE - strlen(_cgd_util_internal.assetsDirBuffer) - 1
        ) == NULL)
            return NULL;
        _cgd_util_internal.assetsDirBuffer[u_BUF_SIZE - 1] = '\0';
    }

    return _cgd_util_internal.assetsDirBuffer;
#endif
}

SDL_Texture* u_loadPNG(const char* filePath, SDL_Renderer* renderer)
{
    /* Error handling stuff */
    char _ERR_STR[u_BUF_SIZE] = { 0 };
    enum EXIT_CODES {
        EXIT_OK,
        ERR_OTHER,
        ERR_GET_REAL_PATH,
        ERR_OPEN_FILE,
        ERR_NOT_PNG,
        ERR_INIT_PNG_STRUCT,
        ERR_INIT_PNG_INFO,
        ERR_SETJMP,
        ERR_ALLOC_PIXELDATA,
        ERR_CREATE_SURFACE,
        ERR_CREATE_TEXTURE,
        ERR_MAX,
    };
    enum EXIT_CODES EXIT_CODE;

    /* A little shortcut for calling the error label */
#define jmpErrorLabel(exit_code, errStr)    \
    do { \
        if(strlen(errStr) < u_BUF_SIZE - 1) { \
            strncpy(_ERR_STR, errStr, u_BUF_SIZE - 1); \
        } else { \
            strncpy(_ERR_STR, errStr, u_BUF_SIZE - 2); \
            _ERR_STR[u_BUF_SIZE - 2] = '.'; \
        } \
        _ERR_STR[u_BUF_SIZE - 1] = '\0'; \
        EXIT_CODE = exit_code; \
        goto err_l; \
    } while(0);

    /* Constants used throughout the function */
#define N_PNG_SIG_BYTES         8

#define N_CHANNELS              4
#define N_BITS_PER_CHANNEL      8
#define PIXEL_FORMAT            SDL_PIXELFORMAT_RGBA32

    /* Declarations of variables used throughout the function.
     * Needed here for the 'err_l' label to work properly */
    png_byte header[N_PNG_SIG_BYTES];
    SDL_Surface *tmpSurface = NULL;
    png_bytep *rowPtrs = NULL;
    int imageW = 0, imageH = 0;

    /* Get the real path to the asset */
    char realPath[u_BUF_SIZE] = { 0 };
    if(strncpy(realPath, u_getAssetPath(), u_BUF_SIZE - 1) == NULL) 
        jmpErrorLabel(ERR_GET_REAL_PATH, filePath);
    realPath[u_BUF_SIZE - 1] = '\0';

    if( strncat(realPath, filePath, u_BUF_SIZE - strlen(realPath) - 1) == NULL)
        jmpErrorLabel(ERR_GET_REAL_PATH, filePath);
    realPath[u_BUF_SIZE - 1] = '\0';

    /* (Try to) Open the given file in binary read mode */
    FILE *file = fopen(realPath, "rb");
    if (file == NULL) jmpErrorLabel(ERR_OPEN_FILE, realPath);

    /* Read PNG header to check whether the given file contains a valid PNG signature */
    fread(header, 1, N_PNG_SIG_BYTES, file);
    if (png_sig_cmp(header, 0, N_PNG_SIG_BYTES)) jmpErrorLabel(ERR_NOT_PNG, realPath); 

    /* Initialize libpng structures */
    png_structp pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
        NULL, NULL, NULL
    );
    if (pngStruct == NULL) jmpErrorLabel(ERR_INIT_PNG_STRUCT, "");

    /* Initialize png info struct */
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == NULL) jmpErrorLabel(ERR_INIT_PNG_INFO, "");

    /* From the libpng manual:
     * "When libpng encounters an error, it expects to longjmp back
to your routine.  Therefore, you will need to call setjmp and pass
your png_jmpbuf(png_ptr)."
    */
    if (setjmp(png_jmpbuf(pngStruct))) jmpErrorLabel(ERR_SETJMP, "");

    /* Set up the PNG input code */
    png_init_io(pngStruct, file);

    /* Inform libpng that we have read the signature bytes */
    png_set_sig_bytes(pngStruct, N_PNG_SIG_BYTES);

    /* Retrieve all needed png metadata */
    png_read_info(pngStruct, pngInfo);

    imageW = png_get_image_width(pngStruct, pngInfo);
    imageH = png_get_image_height(pngStruct, pngInfo);
    png_byte colorType = png_get_color_type(pngStruct, pngInfo);
    png_byte bitDepth = png_get_bit_depth(pngStruct, pngInfo);

    /* Based on the metadata, set various libpng stuff to what it needs to be */
    /* And yes, any other self-respecting library would do that automatically for you... */
    if (bitDepth == 16)
        png_set_strip_16(pngStruct);

    if (colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(pngStruct);

    if (png_get_valid(pngStruct, pngInfo, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(pngStruct);

    if (colorType == PNG_COLOR_TYPE_RGB ||
        colorType == PNG_COLOR_TYPE_GRAY ||
        colorType == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_filler(pngStruct, 0xFF, PNG_FILLER_AFTER);
    }

    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(pngStruct);

    if (colorType == PNG_COLOR_TYPE_GRAY)
        png_set_expand_gray_1_2_4_to_8(pngStruct);

    /* Update the info struct with all the random changes that were made */
    png_read_update_info(pngStruct, pngInfo);

    /* Allocate memory for the pixel data */
    rowPtrs = malloc(imageH * sizeof(png_bytep));
    if(rowPtrs == NULL) jmpErrorLabel(ERR_ALLOC_PIXELDATA, "");

    for (int i = 0; i < imageH; i++) {
        rowPtrs[i] = malloc(png_get_rowbytes(pngStruct, pngInfo));
        if(rowPtrs[i] == NULL) jmpErrorLabel(ERR_ALLOC_PIXELDATA, "");
    }

    /* And now, read the pixel data!!! FINALLY!!! */
    png_read_image(pngStruct, rowPtrs);

    /* Copy over the pixels to an SDL_Surface */
    /* We use 32 as the pixel format, becasuse each of our pixels has 4 channels 8 bits each, so 4 * 8 = 32 */
    tmpSurface = SDL_CreateRGBSurfaceWithFormat(0, imageW, imageH, N_CHANNELS * N_BITS_PER_CHANNEL, SDL_PIXELFORMAT_RGBA32);
    if (tmpSurface == NULL) jmpErrorLabel(ERR_CREATE_SURFACE, SDL_GetError());

    /* Now go through each pixel CHANNEL and copy its data to the SDL_Surface equivalent.
     * This cannot just be done with memcpy, because it will cause issues with the pitch and whatnot */
    SDL_LockSurface(tmpSurface);
    for (int y = 0; y < imageH; y++) {
        for (int x = 0; x < imageW; x++) {
            Uint8 *pixelPtr = (Uint8*)tmpSurface->pixels + y*tmpSurface->pitch + x*4;
            for (int i = 0; i < N_CHANNELS; i++) {
                *(pixelPtr + i) = rowPtrs[y][x*N_CHANNELS + i];
            }
        }
    }
    SDL_UnlockSurface(tmpSurface);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    if (texture == NULL) jmpErrorLabel(ERR_CREATE_TEXTURE, SDL_GetError());

    /* Do the cleanup. There's a surprisingly small amount of it to do considering how many png_something functions we've called... */
    SDL_FreeSurface(tmpSurface);

    for (int i = 0; i < imageH; i++) {
        free(rowPtrs[i]);
        rowPtrs[i] = NULL;
    }
    free(rowPtrs);
    rowPtrs = NULL;

    png_destroy_read_struct(&pngStruct, &pngInfo, NULL);
    fclose(file);

    return texture;

err_l:
    /* Make sure to print the error message BEFORE attempting any shenanigans with freeing the resources */
    ;
    const char *errorMessages[ERR_MAX] = {
        [EXIT_OK]                       = "Everything is supposed to be OK, and yet the 'err_l' label is used. The developer is an idiot",
        [ERR_OTHER]                     = "An unknown error occured. This should never happen (i. e. the developer fucked up)",
        [ERR_GET_REAL_PATH]             = "Failed to get the real path of the given asset. Given path (relative to assets directory): ",
        [ERR_OPEN_FILE]                 = "Failed to open the asset file. Given path (converted): ",
        [ERR_NOT_PNG]                   = "The given file does not contain a valid PNG signature. File path: ",
        [ERR_INIT_PNG_STRUCT]           = "Failed to initialize PNG structures.",
        [ERR_INIT_PNG_INFO]             = "Failed to initialize PNG info struct.",
        [ERR_SETJMP]                    = "Failed to set up the far jump required by png_error",
        [ERR_ALLOC_PIXELDATA]           = "Failed to allocate memory for the pixel data",
        [ERR_CREATE_SURFACE]            = "Failed to create the temporary SDL_Surface. Details: ",
        [ERR_CREATE_TEXTURE]            = "Failed to create the final texture. Details: ", 
    };
    if (EXIT_CODE >= EXIT_OK && EXIT_CODE < ERR_MAX) {
        u_error("[u_loadPNG]: " es_BOLD es_RED "ERROR" es_COLOR_RESET ": %s%s.\n",
                errorMessages[EXIT_CODE], _ERR_STR);
    } else {
        u_error(errorMessages[ERR_OTHER]);
    }

    /* Utilize fall-through behaviour to free, based on the exit code, the resources that had been allocated up to the point when the error occured */
    switch (EXIT_CODE) {
        case ERR_CREATE_TEXTURE: SDL_FreeSurface(tmpSurface);
        case ERR_CREATE_SURFACE:
            for (int i = 0; i < imageH; i++) {
                free(rowPtrs[i]);
                rowPtrs[i] = NULL;
            }
            free(rowPtrs);
            rowPtrs = NULL;
        case ERR_ALLOC_PIXELDATA:
        case ERR_SETJMP: png_destroy_info_struct(pngStruct, &pngInfo);
        case ERR_INIT_PNG_INFO: png_destroy_read_struct(&pngStruct, NULL, NULL);
        case ERR_INIT_PNG_STRUCT: 
        case ERR_GET_REAL_PATH: 
        case ERR_NOT_PNG: fclose(file);
        case ERR_OPEN_FILE:
            break;
        default: case ERR_OTHER: case ERR_MAX:
            break;
        case EXIT_OK:
            break;
    }

    return NULL;
#undef setErrStr
#undef jmpErrorLabel

#undef N_PNG_SIG_BYTES
#undef N_CHANNELS
#undef N_BITS_PER_CHANNEL
#undef PIXEL_FORMAT
}

/* You already know what this does */
int u_max(int a, int b)
{
    if(a > b)
        return a;
    else 
        return b;
}

/* Same here */
int u_min(int a, int b)
{
    if(a < b)
        return a;
    else 
        return b;
}

/* The simplest collision checking implementation;
 * returns true if 2 rectangles overlap 
 */
bool u_collision(const SDL_Rect *r1, const SDL_Rect *r2)
{
    return (
            r1->x + r1->w >= r2->x &&
            r1->x <= r2->x + r2->w &&
            r1->y + r1->h >= r2->y &&
            r1->y <= r2->y + r2->h 
           );
}

void u_error(const char *fmt, ...)
{
    va_list vaList;
    va_start(vaList, fmt);
    vfprintf(stderr, fmt, vaList);
    va_end(vaList);
}

#ifndef __WINDOWS__
int u_getBinDir(char *buf, size_t buf_size)
{
    if (buf == NULL) {
        fprintf(stderr, "[getBinDir]: ERROR: The provided string is a null pointer.\n");
        return EXIT_FAILURE;
    }
    memset(buf, 0, buf_size);

    char fullPathBuf[buf_size];
    fullPathBuf[buf_size - 1] = '\0';

    if (!readlink("/proc/self/exe", fullPathBuf, buf_size - 1)) {
        fprintf(stderr, "[getBinDir]: ERROR: Readlink for /proc/self/exe failed.\n");
        return EXIT_FAILURE;
    }

    int i = 0;
    int binDirLen  = 0;
    do {
        if(fullPathBuf[i] == '/') binDirLen = i + 1; /* We want the trailing '/' */
    } while (fullPathBuf[i++]);

    if (strncpy(buf, fullPathBuf, binDirLen) == NULL) {
        fprintf(stderr, "[getBinDir]: ERROR: Failed to copy the bin dir path into the provided buffer.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
#endif
