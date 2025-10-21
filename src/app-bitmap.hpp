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

#include <cstdint>
#include <vector>

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
}


namespace app {

    class Bitmap final {
    public:

        explicit Bitmap(const FT_Bitmap& bitmap);

        [[nodiscard]]
        std::uint8_t* pixels() noexcept;

        [[nodiscard]]
        const std::uint8_t* pixels() const noexcept;

        [[nodiscard]]
        unsigned int width() const noexcept;

        [[nodiscard]]
        unsigned int height() const noexcept;

        void make_mono(std::uint8_t threshold = 1);

        void increase_weight();

    private:

        unsigned int m_width;
        unsigned int m_height;
        std::vector<std::uint8_t> m_pixels;
    };

}

