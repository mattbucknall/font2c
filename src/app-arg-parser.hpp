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

#include <charconv>
#include <exception>
#include <functional>
#include <optional>
#include <vector>

#include "app-error.hpp"


namespace app {

    class ArgParserException : public std::exception {
    public:

        using std::exception::exception;

        ~ArgParserException() noexcept override;
    };


    class ArgParserHelpException : public app::ArgParserException {
    public:

        using app::ArgParserException::ArgParserException;

        ~ArgParserHelpException() noexcept override;
    };


    class ArgParserVersionException : public app::ArgParserException {
    public:

        using app::ArgParserException::ArgParserException;

        ~ArgParserVersionException() noexcept override;
    };


    class ArgParser final {
    public:

        typedef std::function<void(std::string_view name, std::string_view arg)> ArgValueParser;

        struct Option {
            char short_name;
            std::string long_name;
            std::string description;
            std::string value_name;
            bool value_required;
            ArgValueParser value_parser;

            Option(char short_name, std::string_view long_name, std::string_view description,
                   std::string_view value_name, bool value_required, ArgValueParser&& value_parser);

            [[nodiscard]]
            std::string name() const;
        };

        explicit ArgParser(std::string_view usage = std::string_view(),
                  std::string_view header = std::string_view(),
                  std::string_view footer = std::string_view(),
                  bool help_option = true,
                  bool version_option = true);

        ArgParser(const ArgParser&) = delete;

        ArgParser& operator= (const ArgParser&) = delete;

        template<typename T>
        void option(T& value, std::string_view value_name, char short_name, std::string_view description);

        template<typename T>
        void option(T& value, std::string_view value_name, std::string_view long_name, std::string_view description);

        template<typename T>
        void option(T& value, std::string_view value_name, char short_name, std::string_view long_name,
                    std::string_view description);

        template<typename T>
        void option(T& value, char short_name, std::string_view description);

        template<typename T>
        void option(T& value, std::string_view long_name, std::string_view description);

        template<typename T>
        void option(T& value, char short_name, std::string_view long_name, std::string_view description);

        [[nodiscard]]
        std::optional<Option> find_option(char short_name) const;

        [[nodiscard]]
        std::optional<Option> find_option(std::string_view long_name) const;

        void parse(int& argc, char** argv);

        void display_help() const;

    private:

        const std::string m_usage;
        const std::string m_header;
        const std::string m_footer;
        std::vector<Option> m_options;
        std::string m_prog_name;
        bool m_help_requested;
        bool m_version_requested;

        void add_option(char short_name, std::string_view long_name, std::string_view description,
                        std::string_view value_name, bool value_required, ArgValueParser&& value_parser);
    };


#ifndef _DOXYGEN

    template<typename T>
    inline void ArgParser::option(T& value, std::string_view value_name, char short_name, std::string_view long_name,
                                  std::string_view description) {
        add_option(short_name, long_name, description, value_name, true,
                   [&](std::string_view name, std::string_view arg) {
                       auto result = std::from_chars(arg.data(), arg.data() + arg.length(), value);

                       if (result.ec != std::errc()) {
                           throw app::Error("Invalid value provided for option {}", name);
                       }
                   });
    }


    template<>
    inline void ArgParser::option(bool& value, std::string_view value_name, char short_name,
                                  std::string_view long_name, std::string_view description) {
        (void) value_name;

        add_option(short_name, long_name, description, std::string_view(), false,
                   [&](std::string_view name, std::string_view arg) {
                       value = true;
                   });
    }


    template<>
    inline void ArgParser::option(std::string& value, std::string_view value_name, char short_name,
                                  std::string_view long_name, std::string_view description) {
        (void) value_name;

        add_option(short_name, long_name, description, value_name, true,
                   [&](std::string_view name, std::string_view arg) {
                       value = arg;
                   });
    }


    template<typename T>
    inline void ArgParser::option(T& value, std::string_view value_name, char short_name,
                                  std::string_view description) {
        option(value, value_name, short_name, std::string_view(), description);
    }


    template<typename T>
    inline void ArgParser::option(T& value, std::string_view value_name, std::string_view long_name,
                                  std::string_view description) {
        option(value, value_name, '\0', long_name, description);
    }


    template<typename T>
    inline void ArgParser::option(T& value, char short_name, std::string_view description) {
        option(value, std::string_view(), short_name, description);
    }


    template<typename T>
    inline void ArgParser::option(T& value, std::string_view long_name, std::string_view description) {
        option(value, std::string_view(), long_name, description);
    }


    template<typename T>
    inline void ArgParser::option(T& value, char short_name, std::string_view long_name,
                                  std::string_view description) {
        option(value, std::string_view(), short_name, long_name, description);
    }

#endif // _DOXYGEN
}
