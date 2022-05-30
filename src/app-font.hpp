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

#include <string_view>

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
}

#include "app-error.hpp"
#include "app-ft-lib.hpp"


namespace app {

    class Font final {
    public:

        Font(std::string_view path, int size);

        Font(const Font&) = delete;

        ~Font() noexcept;

        Font& operator= (const Font&) = delete;

        operator FT_Face&(); // NOLINT(google-explicit-constructor)

    private:

        app::FtLib m_lib;
        FT_Face m_face;
    };

}
