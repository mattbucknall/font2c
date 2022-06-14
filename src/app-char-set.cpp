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

#include <filesystem>
#include <fstream>
#include <sstream>

#include "app-char-set.hpp"


typedef std::istream_iterator<std::uint8_t> CharSetFileInput;


app::CharSet app::char_set_default() {
    app::CharSet cs;

    for (char32_t c = 32; c < 127; c++) {
        cs.insert(c);
    }

    return cs;
}


static void expect_input(CharSetFileInput& i, CharSetFileInput& e) {
    if ( i == e ) {
        throw app::Error("Encountered incomplete UTF-8 sequence");
    }
}


static app::CharSet load_txt(std::string_view path) {
    static const uint8_t LENGTH_LUT[] = {
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 0,
            2, 2, 2, 2, 3, 3, 4, 0
    };

    static const uint8_t MASK_LUT[] = {
            0x00, 0x7f, 0x1f, 0x0f, 0x07
    };

    static const uint8_t SHIFT_C_LUT[] = {
            0, 18, 12, 6, 0
    };

    std::ifstream ifs(path.data());

    ifs.unsetf(std::ios_base::skipws);

    CharSetFileInput input_i(ifs);
    CharSetFileInput input_e;
    app::CharSet s;

    if (!ifs) {
        throw app::Error("Unable to load character set '{}'", path);
    }

    while (input_i != input_e && ifs) {
        uint8_t byte = *input_i++;
        uint8_t len = LENGTH_LUT[byte >> 3];
        auto c = static_cast<char32_t>((byte & MASK_LUT[len]) << 18);

        if (len > 1) {
            expect_input(input_i, input_e);
            c |= static_cast<char32_t>(*input_i++ & 0x3F) << 12;
        }

        if (len > 2) {
            expect_input(input_i, input_e);
            c |= static_cast<char32_t>(*input_i++ & 0x3F) << 6;
        }

        if (len > 3) {
            expect_input(input_i, input_e);
            c |= static_cast<char32_t>(*input_i++ & 0x3F);
        }

        c >>= SHIFT_C_LUT[len];

        if (c >= 32) {
            s.insert(c);
        }
    }

    return s;
}


static app::CharSet load_hex(std::string_view path) {
    std::ifstream ifs(path.data());
    app::CharSet s;
    std::string line;

    if (!ifs) {
        throw app::Error("Unable to load character set '{}'", path);
    }

    while (std::getline(ifs, line)) {
        try {
            s.insert(stoul(line, nullptr, 16));
        } catch(std::out_of_range&) {
            // ignore
        } catch(std::invalid_argument&) {
            // ignore
        }
    }

    return s;
}



app::CharSet app::char_set_load(std::string_view path) {
    if ( std::filesystem::path(path).extension() == ".hex" ) {
        return load_hex(path);
    } else {
        return load_txt(path);
    }
}
