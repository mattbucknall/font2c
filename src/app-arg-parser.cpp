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
#include <iterator>

#include <fmt/format.h>

#include "app-arg-parser.hpp"

using namespace app;


ArgParserException::~ArgParserException() noexcept = default;
ArgParserHelpException::~ArgParserHelpException() noexcept = default;
ArgParserVersionException::~ArgParserVersionException() noexcept = default;


ArgParser::Option::Option(char short_name, std::string_view long_name, std::string_view description,
                          std::string_view value_name, bool value_required, ArgValueParser&& value_parser) :
        short_name(short_name),
        long_name(long_name),
        description(description),
        value_name(value_name),
        value_required(value_required),
        value_parser(std::move(value_parser)) {
}


std::string ArgParser::Option::name() const {
    if (short_name) {
        if (long_name.empty()) {
            return fmt::format("-{}", short_name);
        } else {
            return fmt::format("-{}, --{}", short_name, long_name);
        }
    } else {
        return fmt::format("--{}", long_name);
    }
}


ArgParser::ArgParser(std::string_view usage, std::string_view header, std::string_view footer,
                     bool help_option, bool version_option) :
        m_usage(fmt::format("[OPTION]... {}", usage)),
        m_header(header),
        m_footer(footer),
        m_help_requested(false),
        m_version_requested(false) {
    if (help_option) {
        option(m_help_requested, "help", "display this information and exit");
    }

    if (version_option) {
        option(m_version_requested, "version", "display version information and exit");
    }
}


std::optional<ArgParser::Option> ArgParser::find_option(char short_name) const {
    for (const auto& option: m_options) {
        if (option.short_name && option.short_name == short_name) {
            return option;
        }
    }

    return std::nullopt;
}


std::optional<ArgParser::Option> ArgParser::find_option(std::string_view long_name) const {
    for (const auto& option: m_options) {
        if (!option.long_name.empty() && option.long_name == long_name) {
            return option;
        }
    }

    return std::nullopt;
}


void ArgParser::add_option(char short_name, std::string_view long_name, std::string_view description,
                           std::string_view value_name, bool value_required, ArgValueParser&& value_parser) {
    if (!short_name && long_name.empty()) {
        throw app::Error("Cannot add option to ArgParser with neither a short name nor a long name");
    }

    if (short_name && find_option(short_name)) {
        throw app::Error("An option with short name '{}' already exists", short_name);
    }

    if (!long_name.empty() && find_option(long_name)) {
        throw app::Error("An option with long name '{}' already exists", long_name);
    }

    m_options.emplace_back(short_name, long_name, description, value_name, value_required, std::move(value_parser));
}


void ArgParser::parse(int& argc, char** argv) {
    int i = 1;

    m_prog_name.clear();
    m_help_requested = false;
    m_version_requested = false;

    if (argc > 0) {
        m_prog_name = std::filesystem::path(argv[0]).filename();
    }

    while (i < argc) {
        char* arg = argv[i];

        if (arg[0] == '-') {
            std::optional<Option> option;
            const char* value;
            std::string option_name;

            if (arg[1] == '-') {
                char* c = (arg + 2);

                while ((*c >= ' ') && (*c != '=')) c++;

                if (c == (arg + 2)) {
                    throw app::Error("Long option name missing");
                } else if (*c == '=') {
                    value = c + 1;
                } else {
                    value = nullptr;
                }

                *c = '\0';

                option = find_option(arg + 2);
            } else {
                if (arg[1] <= ' ') {
                    throw app::Error("Short option name missing");
                }

                value = arg[2] ? (arg + 2) : nullptr;
                option = find_option(arg[1]);
            }

            if (option == std::nullopt) {
                throw app::Error("Unrecognised option: {}", arg);
            }

            option_name = option->name();

            if (option->value_required && !value) {
                throw app::Error("Option {} requires arg value", option_name);
            }

            if (!option->value_required && value) {
                throw app::Error("Option {} does not require arg value", option_name);
            }

            option->value_parser(option_name, value ? value : "");

            for (int j = i + 1; j < argc; j++) argv[j - 1] = argv[j];
            argc--;
        } else i++;
    }

    if ( m_help_requested ) {
        throw ArgParserHelpException();
    }

    if ( m_version_requested ) {
        throw ArgParserVersionException();
    }
}


void ArgParser::display_help() const {
    fmt::print("Usage: {} {}\n\n", m_prog_name, m_usage);

    if ( !m_header.empty() ) {
        fmt::print("{}\n\n", m_header);
    }

    for (const auto& option: m_options) {
        std::string spec;
        auto bi = std::back_inserter(spec);

        fmt::format_to(bi, "  {}", option.name());

        if (!option.value_name.empty()) {
            if ( option.long_name.empty() ) {
                fmt::format_to(bi, "{}", option.value_name);
            } else {
                fmt::format_to(bi, "={}", option.value_name);
            }
        }

        while (spec.length() < 32) {
            spec.append(" ");
        }

        fmt::print("{}{}\n", spec, option.description);
    }

    if ( !m_footer.empty() ) {
        fmt::print("\n{}\n", m_footer);
    }
}
