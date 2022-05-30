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

#include "app-font.hpp"
#include "app-ft-lib.hpp"

using namespace app;


Font::Font(std::string_view path, int size):
    m_lib(app::FtLib::singleton()),
    m_face(nullptr) {
    FT_Error err;

    err = FT_New_Face(m_lib, path.data(), 0, &m_face);

    if ( err ) {
        throw app::Error("Unable to load font '{}'", path);
    }

    err = FT_Set_Pixel_Sizes(m_face, 0, size);

    if ( err ) {
        throw app::Error("Unable to set font size to {}", size);
    }
}


Font::~Font() noexcept {
    FT_Done_Face(m_face);
}


Font::operator FT_Face&() {
    return m_face;
}
