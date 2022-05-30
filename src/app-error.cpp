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

#include <fmt/core.h>

#include "app-error.hpp"

using namespace app;


static const char* UNDEFINED_STR = "undefined";


Error::Error(std::string_view message) noexcept {
    try {
        m_message = std::make_unique<std::string>(message);
    } catch (...) {
        // empty
    }
}


Error::Error(const Error& e) noexcept {
    try {
        if (e.m_message) {
            m_message = std::make_unique<std::string>(*e.m_message);
        }
    } catch (...) {
        // empty
    }
}


Error::Error(Error&& e) noexcept:
        m_message(std::move(e.m_message)) {
    // empty
}


Error::~Error() noexcept {
    try {
        m_message.reset();
    } catch (...) {
        // empty
    }
}


void Error::swap(Error& e) {
    std::swap(m_message, e.m_message);
}


Error& Error::operator=(const Error& e) noexcept {
    Error temp(e);
    swap(temp);
    return *this;
}


Error& Error::operator=(Error&& e) noexcept {
    Error temp(std::move(e));
    swap(temp);
    return *this;
}


const char* Error::what() const noexcept {
    return m_message ? m_message->c_str() : UNDEFINED_STR;
}


std::string_view Error::message() const noexcept {
    return m_message ? std::string_view(*m_message) :
           std::string_view(UNDEFINED_STR);
}


void Error::prefix(std::string_view message) noexcept {
    try {
        if (m_message) {
            *m_message = fmt::format("{}: {}", message, *m_message);
        } else {
            m_message = std::make_unique<std::string>(message);
        }
    } catch (...) {
        // empty
    }
}


void Error::suffix(std::string_view message) noexcept {
    try {
        if (m_message) {
            *m_message = fmt::format("{}: {}", *m_message, message);
        } else {
            m_message = std::make_unique<std::string>(message);
        }
    } catch (...) {
        // empty
    }
}
