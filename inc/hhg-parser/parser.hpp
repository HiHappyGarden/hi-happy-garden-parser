/***************************************************************************
 *
 * Hi Happy Garden
 * Copyright (C) 2024 Antonio Salsi <passy.linux@zresa.it>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***************************************************************************/

#pragma once
#include <osal/osal.hpp>
#include <stdint.h>


namespace hhg::parser
{
inline namespace v1
{
    using namespace os;

    constexpr const uint8_t KEY_MAX = 32;
    constexpr const uint8_t TOKEN_MAX = 6;

    struct entry
    {
        char key[KEY_MAX]{};

        const entry* next = nullptr;
        uint8_t next_size = 0;

        const function_base& func;

        string<32> description;
    };

    struct token
    {
        char* start = nullptr;
        size_t len = 0;
        trait_type type = trait_type::VOID;
        bool key = false;
    };

    struct data
    {
        char* full_cmd = nullptr;

        token tokens[TOKEN_MAX]{};
        uint8_t tokens_len = 0;

        char* ret_buffer;
        size_t ret_buffer_max;
    };
    
    class parser final
    {
    public:
        parser() OS_NOEXCEPT;
        parser(const parser&) = delete;
        parser& operator=(const parser&) = delete;
        parser(parser&&) = delete;
        parser& operator=(parser&&) = delete;
    };

}
}

