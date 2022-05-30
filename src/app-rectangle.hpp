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

#include <algorithm>


namespace app {

    class Rectangle {
    public:

        int x1;
        int y1;
        int x2;
        int y2;

        Rectangle() noexcept;

        Rectangle(int x, int y, int width, int height) noexcept;

        [[nodiscard]]
        bool empty() const noexcept;

        [[nodiscard]]
        int width() const noexcept;

        [[nodiscard]]
        int height() const noexcept;

        [[nodiscard]]
        bool contains(int x, int y) const noexcept;

        [[nodiscard]]
        bool overlaps(const Rectangle& other) const noexcept;

        void intersect(const Rectangle& other) noexcept;
    };


#ifndef _DOXYGEN

    inline Rectangle::Rectangle() noexcept:
        x1(0),
        y1(0),
        x2(0),
        y2(0) {
    }


    inline Rectangle::Rectangle(int x, int y, int width, int height) noexcept:
        x1(x),
        y1(y),
        x2(x + width),
        y2(y + height) {
    }


    inline bool Rectangle::empty() const noexcept {
        return x2 <= x1 || y2 <= y1;
    }


    inline int Rectangle::width() const noexcept {
        return x2 - x1;
    }


    inline int Rectangle::height() const noexcept {
        return y2 - y1;
    }


    inline bool Rectangle::contains(int x, int y) const noexcept {
        return x >= x1 && x < x2 && y >= y1 && y < y2;
    }


    inline bool Rectangle::overlaps(const Rectangle& other) const noexcept {
        return x1 < other.x2 && other.x1 < x2 && y1 < other.y2 && other.y1 < y2;
    }


    inline void Rectangle::intersect(const Rectangle& other) noexcept {
        x1 = std::max(x1, other.x1);
        y1 = std::max(y1, other.y1);
        x2 = std::max(other.x1, std::min(x2, other.x2));
        y2 = std::max(other.y1, std::min(y2, other.y2));
    }

#endif // _DOXYGEN

}