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

#include <exception>
#include <memory>
#include <string>
#include <string_view>

#include <fmt/format.h>


namespace app {

    class Error : public std::exception {
    public:

        explicit Error(std::string_view message) noexcept;

        template<typename... Args>
        explicit Error(std::string_view fmt, Args&& ... args);

        Error(const Error& e) noexcept;

        Error(Error&& e) noexcept;

        ~Error() noexcept override;

        void swap(Error& e);

        Error& operator=(const Error& e) noexcept;

        Error& operator=(Error&& e) noexcept;

        [[nodiscard]]
        const char* what() const noexcept override;

        [[nodiscard]]
        std::string_view message() const noexcept;

        void prefix(std::string_view message) noexcept;

        template<typename... Args>
        void prefix(std::string_view fmt, Args&& ... args);

        void suffix(std::string_view message) noexcept;

        template<typename... Args>
        void suffix(std::string_view fmt, Args&& ... args);

    private:



        std::unique_ptr<std::string> m_message;
    };


#ifndef _DOXYGEN

    template<typename... Args>
    Error::Error(std::string_view fmt, Args&& ... args):
            Error(fmt::vformat(fmt, fmt::make_format_args(std::forward<Args>(args)...))) {
        // empty
    }


    template<typename... Args>
    void Error::prefix(std::string_view fmt, Args&& ... args) {
        prefix(fmt::vformat(fmt, fmt::make_format_args(std::forward<Args>(args)...)));
    }


    template<typename... Args>
    void Error::suffix(std::string_view fmt, Args&& ... args) {
        suffix(fmt::vformat(fmt, fmt::make_format_args(std::forward<Args>(args)...)));
    }

#endif // _DOXYGEN

}
