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

#include <algorithm>
#include <array>
#include <utility>

#include <spng.h>

#include "app-canvas.hpp"

#define BYTES_PER_PIXEL         3

using namespace app;


Canvas::Canvas(int width, int height):
    m_width(std::max(1, width)),
    m_height(std::max(1, height)),
    m_pitch(BYTES_PER_PIXEL * m_width),
    m_bitmap(m_pitch * m_height),
    m_color{0, 0, 0},
    m_clip_region(0, 0, width, height) {
}


int Canvas::width() const noexcept {
    return m_width;
}


int Canvas::height() const noexcept {
    return m_height;
}


void Canvas::set_color(uint8_t r, uint8_t g, uint8_t b) noexcept {
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
}


void Canvas::reset_clip_region() noexcept {
    m_clip_region = app::Rectangle(0, 0, m_width, m_height);
}


app::Rectangle Canvas::set_clip_region(app::Rectangle region) noexcept {
    auto previous = m_clip_region;
    m_clip_region = region;
    return previous;
}


app::Rectangle Canvas::modify_clip_region(app::Rectangle region) noexcept {
    auto previous = m_clip_region;
    m_clip_region.intersect(region);
    return previous;
}


void Canvas::draw_pixel(int x, int y) noexcept {
    if ( m_clip_region.contains(x, y) ) {
        uint8_t* bitmap = m_bitmap.data();
        uint8_t* pixel = bitmap + (m_pitch * y) + (BYTES_PER_PIXEL * x);

        pixel[0] = m_color[0];
        pixel[1] = m_color[1];
        pixel[2] = m_color[2];
    }
}


void Canvas::draw_fill() noexcept {
    draw_fill(0, 0, m_width, m_height);
}


void Canvas::draw_fill(int x, int y, int width, int height) noexcept {
    Rectangle r(x, y, width, height);

    r.intersect(m_clip_region);

    if ( !r.empty() ) {
        uint8_t* bitmap = m_bitmap.data();
        uint8_t* row_i = bitmap + (m_pitch * r.y1);
        uint8_t* row_e = bitmap + (m_pitch * r.y2);

        while(row_i < row_e) {
            uint8_t* pixel_i = row_i + (BYTES_PER_PIXEL * r.x1);
            uint8_t* pixel_e = row_i + (BYTES_PER_PIXEL * r.x2);

            while(pixel_i < pixel_e) {
                pixel_i[0] = m_color[0];
                pixel_i[1] = m_color[1];
                pixel_i[2] = m_color[2];
                pixel_i += BYTES_PER_PIXEL;
            }

            row_i += m_pitch;
        }
    }
}


std::optional<font2c_glyph_t> Canvas::draw_glyph(int x, int y, const app::OutputModel& output_model,
                                                 char32_t codepoint) noexcept {
    auto glyph = output_model.find_glyph(codepoint);

    if ( !glyph ) {
        return std::nullopt;
    }

    Rectangle r(x + glyph->x_bearing, y - glyph->y_bearing, glyph->width, glyph->height);
    int src_x = r.x1;
    int src_y = r.y1;

    r.intersect(m_clip_region);

    if ( !r.empty() ) {
        uint8_t* bitmap = m_bitmap.data();
        uint8_t* row_i = bitmap + (m_pitch * r.y1);
        uint8_t* row_e = bitmap + (m_pitch * r.y2);

        src_x = r.x1 - src_x;
        src_y = r.y1 - src_y;

        const uint8_t* src_bitmap = output_model.pixel_data().data() + glyph->offset;
        const uint8_t* src_row_i = src_bitmap + (glyph->width * src_y);

        while(row_i < row_e) {
            uint8_t* pixel_i = row_i + (BYTES_PER_PIXEL * r.x1);
            uint8_t* pixel_e = row_i + (BYTES_PER_PIXEL * r.x2);
            const uint8_t* src_pixel_i = src_row_i + src_x;

            while(pixel_i < pixel_e) {
                uint8_t opacity = *src_pixel_i;

                pixel_i[0] = (opacity * m_color[0]) / 255;
                pixel_i[1] = (opacity * m_color[1]) / 255;
                pixel_i[2] = (opacity * m_color[2]) / 255;

                pixel_i += BYTES_PER_PIXEL;
                src_pixel_i += 1;
            }

            row_i += m_pitch;
            src_row_i += glyph->width;
        }
    }

    return glyph;
}


static void check_spng_err(int err) {
    if ( err != 0 ) {
        throw app::Error("{}", spng_strerror(err));
    }
}


void Canvas::write_png(std::string_view path) const {
    try {
        struct File {
            FILE* f;

            explicit File(std::string_view path) :
                    f(std::fopen(path.data(), "wb")) {
                if (f == nullptr) {
                    throw app::Error("{}", std::strerror(errno));
                }
            }

            ~File() {
                std::fclose(f);
            }

            operator FILE*() const { // NOLINT(google-explicit-constructor)
                return f;
            }
        };


        struct Ctx {
            spng_ctx* ctx;

            Ctx() :
                    ctx(spng_ctx_new(SPNG_CTX_ENCODER)) {
            }

            ~Ctx() {
                spng_ctx_free(ctx);
            }

            operator spng_ctx*() const { // NOLINT(google-explicit-constructor)
                return ctx;
            }
        };

        File f(path);
        Ctx ctx;
        spng_ihdr header = {0};

        header.width = m_width;
        header.height = m_height;
        header.color_type = SPNG_COLOR_TYPE_TRUECOLOR;
        header.bit_depth = 8;
        header.interlace_method = SPNG_INTERLACE_NONE;

        check_spng_err(spng_set_png_file(ctx, f));
        check_spng_err(spng_set_ihdr(ctx, &header));
        check_spng_err(spng_encode_image(ctx, m_bitmap.data(), m_bitmap.size(), SPNG_FMT_PNG, SPNG_ENCODE_FINALIZE));
    } catch(app::Error& e) {
        e.prefix("Unable to create preview file '{}'", path);
        throw;
    }
}
