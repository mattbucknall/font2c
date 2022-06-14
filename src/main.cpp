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
#include <map>

#include <fmt/core.h>

#include "app-arg-parser.hpp"
#include "app-char-set.hpp"
#include "app-error.hpp"
#include "app-font.hpp"
#include "app-glyph.hpp"
#include "app-options.hpp"
#include "app-output-model.hpp"
#include "app-preview.hpp"
#include "app-version.hpp"


struct Rasterizer {
    std::string_view description;
    app::OutputModel::RasterizerFunc func;
};


typedef std::map<std::string_view, Rasterizer> RasterizerMap;


static void lrtb_rasterizer(app::OutputModel& output_model, const app::Glyph& glyph) {
    int pitch = glyph.pitch();
    const uint8_t* row_i = glyph.buffer();
    const uint8_t* row_e = row_i + (pitch * glyph.height());

    while (row_i < row_e) {
        const uint8_t* pixel_i = row_i;
        const uint8_t* pixel_e = pixel_i + glyph.width();

        while (pixel_i < pixel_e) {
            output_model.add_pixel(*pixel_i);
            pixel_i += 1;
        }

        output_model.flush_pixels();
        row_i += pitch;
    }
}


static const RasterizerMap& rasterizer_map() {
    static const RasterizerMap m = {
            {"lrtb",    {"Scans left-to-right, top-to-bottom",          lrtb_rasterizer}}
    };

    return m;
}


static void parse_args(int& argc, char** argv, app::Options& options) {
    app::ArgParser p("[FONT PATH] [OUTPUT PATH]",
                     "Convert font glyphs into bitmap images embeddable in C source code.",
                     "If no character set file is specified, a default character set consisting of\n"
                     "ASCII codes 32-126 (inclusive) will be used.");

    try {
        p.option(options.size, "PIXELS", 's', "size", fmt::format("Font size (default = {})", options.size));

        p.option(options.raster_type, "TYPE", 'r', "raster-type",
                 fmt::format("Rasterization type (default = {})", options.raster_type));

        p.option(options.char_set_path, "PATH", 'c', "char-set", "Path to character set file");

        p.option(options.symbol_name, "NAME", 'y', "symbol", "Symbol name for font2c_face_t object");

        p.option(options.pixel_depth, "BPP", 'd', "depth",
                 fmt::format("Pixel depth (must be 1, 2, 4 or 8, default = {})", options.pixel_depth));

        p.option(options.msb_first, 'm', "msb-first", "Pack most-significant bits first");

        p.option(options.antialiasing, 'a', "antialiasing", "Enable antialiasing");

        p.option(options.no_hinting, 'i', "no-hinting", "Disable hinting");

        p.option(options.preview_path, "PATH", 'p', "preview", "Preview output file path");

        p.option(options.center_adjust, "PIXELS", 'j', "center-adj", "Number of pixels to adjust font center line by");

        p.parse(argc, argv);

        if (options.pixel_depth != 1 && options.pixel_depth != 2 && options.pixel_depth != 4 &&
            options.pixel_depth != 8) {
            throw app::Error("Pixel depth must be 1, 2, 4 or 8 bits-per-pixel");
        }

        if (options.pixel_depth == 1) {
            options.antialiasing = false;
        }
    } catch (app::ArgParserHelpException&) {
        p.display_help();

        fmt::print("\nSupported raster types:\n");

        for (const auto& r: rasterizer_map()) {
            fmt::print("  {:<12}{}\n", r.first, r.second.description);
        }

        fmt::print("\n");
        throw;
    } catch (app::ArgParserVersionException&) {
        fmt::print("{}\n", APP_VERSION_STR);
        throw;
    }
}


int main(int argc, char* argv[]) {
    int exit_code = EXIT_FAILURE;

    try {
        app::Options options;
        app::CharSet char_set;

        parse_args(argc, argv, options);

        if (argc < 2) {
            throw app::Error("No font file specified");
        }

        if (argc < 3) {
            throw app::Error("No output file specified");
        }

        if (options.char_set_path.empty()) {
            char_set = app::char_set_default();
        } else {
            char_set = app::char_set_load(options.char_set_path);
        }

        auto rm = rasterizer_map();
        auto ri = rm.find(options.raster_type);

        if (ri == rm.end()) {
            throw app::Error("Unrecognized raster type: {}", options.raster_type);
        }

        app::Font font(argv[1], options.size);
        app::OutputModel output_model(options.pixel_depth, options.msb_first, ri->second.func);

        for (auto codepoint: char_set) {
            try {
                app::Glyph glyph(font, codepoint, options.antialiasing, options.no_hinting);
                output_model.add_glyph(glyph);
            } catch (app::GlyphError& e) {
                fmt::print(stderr, "Warning: {}\n", e.what());
            }
        }

        output_model.write(argv[2], argv[1], options);

        if (!options.preview_path.empty()) {
            app::preview_generate(options.preview_path, font, char_set, options.pixel_depth, options.antialiasing,
                                           options.no_hinting);
        }

        exit_code = EXIT_SUCCESS;
    } catch (app::ArgParserException&) {
        exit_code = EXIT_SUCCESS;
    } catch (app::Error& e) {
        fmt::print(stderr, "Fatal error: {}\n", e.what());
    }

    return exit_code;
}
