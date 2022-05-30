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

#include "app-canvas.hpp"
#include "app-preview.hpp"


void app::preview_generate(std::string_view path, app::Font& font, const app::CharSet& char_set, int depth,
                      bool antialiasing, bool no_hinting) {
    int total_width = 0;

    app::OutputModel output_model(8, false, [=] (app::OutputModel& output_model, const app::Glyph& glyph) {
        int pitch = glyph.pitch();
        const uint8_t* row_i = glyph.buffer();
        const uint8_t* row_e = row_i + (pitch * glyph.height());
        int scale = 255 / ((1 << depth) - 1);
        int shift = 8 - depth;

        while (row_i < row_e) {
            const uint8_t* pixel_i = row_i;
            const uint8_t* pixel_e = pixel_i + glyph.width();

            while (pixel_i < pixel_e) {
                output_model.add_pixel(scale * (*pixel_i >> shift));
                pixel_i += 1;
            }

            output_model.flush_pixels();
            row_i += pitch;
        }
    });

    for (auto codepoint: char_set) {
        try {
            app::Glyph glyph(font, codepoint, antialiasing, no_hinting);
            total_width += glyph.x_bearing() + glyph.width() + 4;
            output_model.add_glyph(glyph);
        } catch(app::GlyphError&) {
            // ignore
        }
    }

    app::Canvas canvas(total_width, output_model.line_height());
    int cursor_x = 4;
    int cursor_y = output_model.line_ascent();

    canvas.set_color(0, 64, 64);
    canvas.draw_fill();

    for (auto codepoint: char_set) {
        canvas.set_color(255, 255, 255);

        auto f2c_glyph = canvas.draw_glyph(cursor_x, cursor_y, output_model, codepoint);

        if ( f2c_glyph ) {
            canvas.set_color(255, 0 ,0);
            canvas.draw_pixel(cursor_x, cursor_y);

            cursor_x += f2c_glyph->x_bearing + f2c_glyph->width + 4;
        }
    }

    canvas.write_png(path);
}
