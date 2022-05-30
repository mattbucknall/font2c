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

#include <functional>
#include <optional>
#include <string_view>
#include <vector>

#include <font2c-types.h>

#include "app-glyph.hpp"
#include "app-options.hpp"


namespace app {

    class OutputModel final {
    public:

        typedef std::function<void (app::OutputModel& output_model, const app::Glyph& glyph)> RasterizerFunc;

        OutputModel(int depth, bool msb_first, RasterizerFunc rasterizer_func);

        [[nodiscard]]
        int line_ascent() const;

        [[nodiscard]]
        int line_descent() const;

        [[nodiscard]]
        int line_height() const;

        [[nodiscard]]
        std::optional<font2c_glyph_t> find_glyph(char32_t codepoint) const;

        [[nodiscard]]
        const std::vector<uint8_t>& pixel_data() const;

        void add_glyph(const app::Glyph& glyph);

        void add_pixel(uint8_t opacity);

        void flush_pixels();

        void write(std::string_view path, std::string_view font_path, const app::Options& options) const;


    private:

        RasterizerFunc m_rasterizer_func;
        int m_shift;
        int m_start;
        int m_delta;
        int m_line_ascent;
        int m_line_descent;
        int m_line_height;
        int m_bit_pos;
        uint8_t m_current_byte;
        std::vector<font2c_glyph_t> m_glyphs;
        std::vector<uint8_t> m_pixel_data;
    };

}
