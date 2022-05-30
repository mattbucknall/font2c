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

#include "app-ft-lib.hpp"

using namespace app;


struct FtLib::Private {
    FT_Library m_lib;

    Private():
        m_lib(nullptr) {
        auto err = FT_Init_FreeType(&m_lib);

        if ( err ) {
            throw app::Error("Unable to initialise FreeType");
        }
    }

    Private(const Private&) = delete;

    ~Private() {
        FT_Done_FreeType(m_lib);
    }

    Private& operator= (const Private&) = delete;
};


FtLib& FtLib::singleton() {
    static FtLib instance;
    return instance;
}


FtLib::FtLib():
    p(new Private()) {
}


FtLib::operator FT_Library&() {
    return p->m_lib;
}
