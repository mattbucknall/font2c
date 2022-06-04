/*
 * The following statement applies to font2c-types.h only:
 *
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 *         binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 *         of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 *         relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *         OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org/>
 */

#ifndef _FONT2C_TYPES_H_
#define _FONT2C_TYPES_H_

#ifdef __cplusplus

#include <cstdint>

extern "C" {
#else // __cplusplus
#include <stdint.h>
#endif // __cplusplus


typedef enum {
    FONT2C_COMPRESSION_NONE
} font2c_compression_t;


typedef struct {
    uint32_t codepoint;                 // glyph's unicode codepoint
    uint32_t offset;                    // offset of first bitmap byte in pixel table
    int16_t x_bearing;                  // horizontal offset of glyph bitmap's top-left corner relative to its origin
    int16_t y_bearing;                  // vertical offset of glyph's bitmap's top-left corner relative to its origin
    uint16_t width;                     // width of glyph's bitmap
    uint16_t height;                    // height of glyph's bitmap
    int16_t x_advance;                  // distance to advance cursor horizontally after rendering glyph
} font2c_glyph_t;


typedef struct {
    const uint8_t* pixels;              // pointer to font's bitmap data
    const font2c_glyph_t* glyphs;       // pointer to font's glyph lookup table
    uint32_t n_glyphs;                  // number of glyphs in lookup table
    int16_t ascent;                     // font's longest ascender
    int16_t descent;                    // font's longest descender
    int16_t line_height;                // minimum distance that should be left between lines
    font2c_compression_t compression;   // pixel data compression scheme
} font2c_font_t;

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // _FONT2C_TYPES_H_
