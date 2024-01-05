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
        uint32_t i  = 23;
        uint32_t get_i()
        {
            return i;
        }
    };


    test_t test;

    hhg::parser::entry commands_user[] =
    {

         {.key = "1", .func = new hhg::parser::function{set_age, hhg::parser::trait_type::_VOID_, hhg::parser::trait_type::UINT8}, .description = "Set User Age"},
         {.key = "2", .func = new hhg::parser::function{get_age, hhg::parser::trait_type::UINT8}, .description = "Get User Age"},
         {.key = "3", .func = new hhg::parser::function{set_name, hhg::parser::trait_type::_VOID_, hhg::parser::trait_type::STR}, .description = "Set User Name"},
         {.key = "4", .func = new hhg::parser::function{get_name, hhg::parser::trait_type::STR}, .description = "Set User Name"}
    };
    constexpr const size_t commands_user_size = sizeof(commands_user) / sizeof(commands_user[0]);

    hhg::parser::entry commands[] =
    {

            {.key = "^VER", .func = new hhg::parser::function{get_version, hhg::parser::trait_type::_VOID_}, .description = "Get App version"},
            {.key = "^TEST", .func = new hhg::parser::method{&test, &test_t::get_i, hhg::parser::trait_type::UINT32}, .description = "Get App version"},
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