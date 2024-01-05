/***************************************************************************
 *
 * Hi Happy Garden Parser
 * Copyright (C) 2023-2024  Antonio Salsi <passy.linux@zresa.it>
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

    struct param;
    struct cmd_data;
    struct entry
    {
        using custom_function = os::exit (*)(const entry* entry, const cmd_data& data, error** error);

        char key[KEY_MAX]{};

        const entry* next = nullptr;
        uint8_t next_size = 0;

        function_base::ptr func;
        custom_function custom_func = nullptr;

        char const description[128]{};
    };

    struct token
    {
        char* start = nullptr;
        size_t len = 0;
        trait_type type = trait_type::_VOID_;
        bool key = false;
    };

    struct cmd_data
    {
        char* full_cmd = nullptr;

        token tokens[TOKEN_MAX]{};
        uint8_t tokens_len = 0;

        char* ret_buffer = nullptr;
        size_t ret_buffer_len = 0;

        const struct entry* entry = nullptr;
    };

    class parser final
    {
        entry* entries_table = nullptr;
        size_t entries_table_size = 0;
    public:
        parser(entry* entries_table, size_t entries_table_size) OS_NOEXCEPT;
        parser(const parser&) = delete;
        parser& operator=(const parser&) = delete;
        parser(parser&&) = delete;
        parser& operator=(parser&&) = delete;

        os::exit execute(char full_cmd[], char ret_value[] = nullptr, uint32_t ret_value_len = 0, error** error = nullptr) const OS_NOEXCEPT;
    private:
        static os::exit execute(cmd_data& data, const entry* entries, size_t entries_size, error** error) OS_NOEXCEPT;
        static os::exit execute(cmd_data& data, const entry* entry, error** error) OS_NOEXCEPT;
//        static os::exit perform(cmd_data& data, const entry* entry, error** error) OS_NOEXCEPT;

        static os::exit tokenize(char* full_cmd, cmd_data& data, error** error) OS_NOEXCEPT;
        static os::exit typify(const entry* entry, cmd_data& data, error** error) OS_NOEXCEPT;
        static os::exit valorize_param(const token& token, param& param, error** error) OS_NOEXCEPT;


    };

}
}

