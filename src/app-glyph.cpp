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

#include "app-ft-lib.hpp"
#include "app-glyph.hpp"

using namespace app;


GlyphError::~GlyphError() noexcept = default;


Glyph::Glyph(Font& font, char32_t codepoint, bool anti_aliased, bool no_hinting, bool pseudo_bold):
    m_codepoint(codepoint) {
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

        if ( pseudo_bold ) {
            anti_aliased = false;
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

        FT_Bitmap bitmap;
        FT_Bitmap_Init(&bitmap);

        try {
            err = FT_Bitmap_Convert(app::FtLib::singleton(), &glyph->bitmap, &bitmap, 1);

            if ( err ) {
                throw app::GlyphError("Unable to normalize pixel depth for this codepoint");
            }

            m_bitmap = std::make_shared<app::Bitmap>(bitmap);
        } catch (...) {
            FT_Bitmap_Done(app::FtLib::singleton(), &bitmap);
            throw;
        }

        if ( render_mode == FT_RENDER_MODE_MONO ) {
            m_bitmap->make_mono();
        }

        m_x_bearing = glyph->bitmap_left;
        m_y_bearing = glyph->bitmap_top - 1;
        m_x_advance = static_cast<int>((glyph->advance.x + 32) / 64);
        m_y_advance = static_cast<int>((glyph->advance.y + 32) / 64);

        if ( pseudo_bold ) {
            m_bitmap->increase_weight();
            m_x_advance += 1;
        }
    } catch(app::Error& e) {
        e.prefix("Codepoint U+{:04X}", static_cast<uint32_t>(codepoint));
        throw;
    }
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


unsigned int Glyph::width() const noexcept {
    return m_bitmap->width();
}


unsigned int Glyph::height() const noexcept {
    return m_bitmap->height();
}


const uint8_t* Glyph::buffer() const noexcept {
    return m_bitmap->pixels();
}


unsigned int Glyph::pitch() const noexcept {
    return m_bitmap->width();
}
