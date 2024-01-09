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

#include "hhg-parser/parser.hpp"
#include "functions.hpp"

namespace
{
    struct test_t
    {
        char str[128];
        uint32_t i  = 23;
        uint32_t get_i()
        {
            return i;
        }

        bool set_str(const char* str)
        {
            strncpy(test_t::str, str, sizeof(test_t::str) - 1);
            return true;
        }

        const char* get_str()
        {
            return test_t::str;
        }

    };


    test_t test_one;

    hhg::parser::entry commands_user[] =
    {

    {.key = "1", .func = new hhg::parser::function{set_age}, .description = "Set User Age"},
    {.key = "2", .func = new hhg::parser::function{get_age}, .description = "Get User Age"},
    {.key = "3", .func = new hhg::parser::function{set_name}, .description = "Set User Name"},
    {.key = "4", .func = new hhg::parser::function{get_name}, .description = "Get User Name"},
    {.key = "5", .func = new hhg::parser::method{&test_one, &test_t::get_str}, .description = "Get string"}
    };
    constexpr const size_t commands_user_size = sizeof(commands_user) / sizeof(commands_user[0]);

    hhg::parser::entry commands[] =
    {

    {.key = "^VER", .func = new hhg::parser::function{get_version}, .description = "Get App version"},
    {.key = "^TEST", .func = new hhg::parser::method{&test_one, &test_t::get_i}, .description = "Get App version"},
    {.key = "^TEST_STR", .func = new hhg::parser::method{&test_one, &test_t::set_str}, .description = "Get App version"},
    {.key = "^USR", .next = commands_user, .next_size = commands_user_size}
    };
    constexpr const size_t commands_size = sizeof(commands) / sizeof(commands[0]);

}

hhg::parser::entry* get_table_commands()
{
    return commands;
}

size_t get_table_size_commands()
{
    return commands_size;
}