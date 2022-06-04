/*
 * font2c - Command-line utility for converting font glyphs into bitmap images
 * embeddable in C source code.
 *
 * https://github.com/mattbucknall/font2c
 *
 * Copyright (C) 2022 Matthew T. Bucknall
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "app-glyph.hpp"

using namespace app;


GlyphError::~GlyphError() noexcept = default;


Glyph::Glyph(Font& font, char32_t codepoint, bool anti_aliased, bool no_hinting):
    m_codepoint(codepoint),
    m_lib(app::FtLib::singleton()) {
    FT_Error err;
    FT_Int32 load_flags;
    FT_Render_Mode render_mode;
    unsigned int index;
    FT_GlyphSlot glyph;

    try {
        load_flags = FT_LOAD_RENDER;

        if (no_hinting) {
            load_flags |= FT_LOAD_NO_HINTING;
        }

        if (anti_aliased) {
            render_mode = FT_RENDER_MODE_NORMAL;
        } else {
            render_mode = FT_RENDER_MODE_MONO;
            load_flags |= FT_LOAD_TARGET_MONO | FT_LOAD_MONOCHROME;
        }

        index = FT_Get_Char_Index(font, codepoint);

        if (index == 0) {
            throw app::GlyphError("Font does not contain glyph for this codepoint");
        }

        err = FT_Load_Glyph(font, index, load_flags);

        if (err) {
            throw app::GlyphError("Unable to load glyph for this codepoint");
        }

        glyph = static_cast<FT_Face>(font)->glyph;

        err = FT_Render_Glyph(glyph, render_mode);

        if ( err ) {
            throw app::GlyphError("Unable to render glyph for this codepoint");
        }

        FT_Bitmap_Init(&m_bitmap);
        err = FT_Bitmap_Convert(m_lib, &glyph->bitmap, &m_bitmap, 1);

        if ( err ) {
            throw app::GlyphError("Unable to normalize pixel depth for this codepoint");
        }

        if ( render_mode == FT_RENDER_MODE_MONO ) {
            uint8_t* row_i = m_bitmap.buffer;
            uint8_t* row_e = row_i + (m_bitmap.pitch * m_bitmap.rows);

            while(row_i < row_e) {
                uint8_t* pixel_i = row_i;
                uint8_t* pixel_e = pixel_i + m_bitmap.width;

                while(pixel_i < pixel_e) {
                    *pixel_i = (*pixel_i > 0) ? 0xFF : 0x00;
                    pixel_i++;
                }

                row_i += m_bitmap.pitch;
            }
        }

        m_x_bearing = static_cast<int>(glyph->bitmap_left);
        m_y_bearing = static_cast<int>(glyph->bitmap_top - 1);
        m_x_advance = static_cast<int>(glyph->advance.x / 64);
        m_y_advance = static_cast<int>(glyph->advance.y / 64);
    } catch(app::Error& e) {
        e.prefix("Codepoint U+{:04X}", static_cast<uint32_t>(codepoint));
        throw;
    }
}


Glyph::~Glyph() noexcept {
    FT_Bitmap_Done(m_lib, &m_bitmap);
}


char32_t Glyph::codepoint() const noexcept {
    return m_codepoint;
}


int Glyph::x_bearing() const noexcept {
    return m_x_bearing;
}


int Glyph::y_bearing() const noexcept {
    return m_y_bearing;
}


int Glyph::x_advance() const noexcept {
    return m_x_advance;
}


int Glyph::y_advance() const noexcept {
    return m_y_advance;
}


int Glyph::width() const noexcept {
    return static_cast<int>(m_bitmap.width);
}


int Glyph::height() const noexcept {
    return static_cast<int>(m_bitmap.rows);
}


const uint8_t* Glyph::buffer() const noexcept {
    return m_bitmap.buffer;
}


int Glyph::pitch() const noexcept {
    return m_bitmap.pitch;
}
