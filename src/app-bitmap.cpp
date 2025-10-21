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

#include "app-bitmap.hpp"

using namespace app;


Bitmap::Bitmap(const FT_Bitmap& bitmap)
    : m_width(bitmap.width),
      m_height(bitmap.rows) {

    m_pixels.reserve(static_cast<size_t>(m_width) * m_height);

    const bool flip = (bitmap.pitch < 0);
    const uint8_t* row = flip
        ? bitmap.buffer + (-bitmap.pitch * (bitmap.rows - 1))
        : bitmap.buffer;

    for (unsigned int y = 0; y < bitmap.rows; ++y) {
        const uint8_t* pixel = row;
        for (unsigned int x = 0; x < m_width; ++x)
            m_pixels.push_back(pixel[x]);

        row += bitmap.pitch;
    }
}


uint8_t* Bitmap::pixels() noexcept {
    return m_pixels.data();
}


const uint8_t* Bitmap::pixels() const noexcept {
    return m_pixels.data();
}


unsigned int Bitmap::width() const noexcept {
    return m_width;
}


unsigned int Bitmap::height() const noexcept {
    return m_height;
}


void Bitmap::make_mono(const std::uint8_t threshold) {
    for (auto& pixel: m_pixels) {
        pixel = (pixel >= threshold) ? 0xFF : 0x00;
    }
}


void Bitmap::increase_weight() {
    std::vector<std::uint8_t> new_pixels((m_width + 1) * m_height, 0x00);

    make_mono(1);

    auto src_row_i = m_pixels.cbegin();
    const auto src_row_e = m_pixels.cend();
    auto dest_row_i = new_pixels.begin();

    while (src_row_i < src_row_e) {
        auto src_pixel_i = src_row_i;
        const auto src_pixel_e = src_pixel_i + m_width;
        auto dest_pixel_i = dest_row_i;

        while (src_pixel_i < src_pixel_e) {
            dest_pixel_i[0] |= *src_pixel_i;
            dest_pixel_i[1] |= *src_pixel_i;

            ++src_pixel_i;
            ++dest_pixel_i;
        }

        src_row_i += m_width;
        dest_row_i += m_width + 1;
    }

    m_pixels.swap(new_pixels);
    m_width += 1;
}
