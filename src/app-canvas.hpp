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
#include <string_view>
#include <vector>

#include "app-error.hpp"
#include "app-output-model.hpp"
#include "app-rectangle.hpp"


namespace app {

    class Canvas final {
    public:

        Canvas(int width, int height);

        [[nodiscard]]
        int width() const noexcept;

        [[nodiscard]]
        int height() const noexcept;

        void set_color(uint8_t r, uint8_t g, uint8_t b) noexcept;

        void reset_clip_region() noexcept;

        app::Rectangle set_clip_region(app::Rectangle region) noexcept;

        app::Rectangle modify_clip_region(app::Rectangle region) noexcept;

        void draw_pixel(int x, int y) noexcept;

        void draw_fill() noexcept;

        void draw_fill(int x, int y, int width, int height) noexcept;

        std::optional<font2c_glyph_t> draw_glyph(int x, int y, const app::OutputModel& output_model,
                                                  char32_t codepoint) noexcept;

        void write_png(std::string_view path) const;

    private:

        const int m_width;
        const int m_height;
        const int m_pitch;
        std::vector<uint8_t> m_bitmap;
        uint8_t m_color[3];
        app::Rectangle m_clip_region;
    };

}
