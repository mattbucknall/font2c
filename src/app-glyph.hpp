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

#pragma once

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
}

#include "app-error.hpp"
#include "app-font.hpp"
#include "app-ft-lib.hpp"


namespace app {

class GlyphError: public app::Error {
public:

    using app::Error::Error;

    ~GlyphError() noexcept;
};

    class Glyph final {
    public:

        Glyph(Font& font, char32_t codepoint, bool anti_aliased, bool no_hinting = false);

        Glyph(const Glyph&) = delete;

        ~Glyph() noexcept;

        Glyph& operator= (const Glyph&) = delete;

        [[nodiscard]]
        char32_t codepoint() const noexcept;

        [[nodiscard]]
        int x_bearing() const noexcept;

        [[nodiscard]]
        int y_bearing() const noexcept;

        [[nodiscard]]
        int x_advance() const noexcept;

        [[nodiscard]]
        int y_advance() const noexcept;

        [[nodiscard]]
        int width() const noexcept;

        [[nodiscard]]
        int height() const noexcept;

        [[nodiscard]]
        const uint8_t* buffer() const noexcept;

        [[nodiscard]]
        int pitch() const noexcept;

    private:

        char32_t m_codepoint;
        int m_x_bearing;
        int m_y_bearing;
        int m_x_advance;
        int m_y_advance;
        app::FtLib m_lib;
        FT_Bitmap m_bitmap;
    };

}
