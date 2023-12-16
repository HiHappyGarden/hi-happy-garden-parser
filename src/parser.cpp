/***************************************************************************
 *
 * Hi Happy Garden
 * Copyright (C) 202X  Antonio Salsi <passy.linux@zresa.it>
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
using namespace osal;

namespace hhg::parser
{
inline namespace v1
{

    parser::parser(entry* entries_table, size_t entries_table_size) OS_NOEXCEPT
    {

    }

    os::exit parser::execute(char full_cmd[], char ret_value[], uint32_t ret_value_size, error** error) OS_NOEXCEPT
    {
        if(full_cmd == nullptr)
        {
            if(error)
            {
                *error = OS_ERROR_BUILD("Invalid argument.", error_type::OS_EINVAL);
                OS_ERROR_PTR_SET_POSITION(*error);
            }
            return exit::KO;
        }

        struct cmd_data data;
        if(ret_value)
        {
            data.ret_buffer = ret_value;
            data.ret_buffer_size = ret_value_size;
        }

        if(tokenize(full_cmd, data, error) == exit::KO)
        {
            return exit::KO;
        }


        return exit::OK;
    }

    os::exit parser::tokenize(char* full_cmd, cmd_data& func, error** error) OS_NOEXCEPT
    {
        if(full_cmd == nullptr)
        {
            if(error)
            {
                *error = OS_ERROR_BUILD("Invalid argument.", error_type::OS_EINVAL);
                OS_ERROR_PTR_SET_POSITION(*error);
            }
            return exit::KO;
        }

        size_t len = 0;
        uint8_t token_idx = 0;
        bool is_last_char_a_space = false;
        bool open_double_quotes = false;
        char* cursor = full_cmd;
        while (*cursor != '\0')
        {
            if(*cursor == ' ' && !open_double_quotes)
            {
                if(!is_last_char_a_space)
                {
                    *cursor = '\0';
                }
                is_last_char_a_space = true;
            }
            else
            {
                if(*cursor == '"')
                {
                    open_double_quotes = !open_double_quotes;
                }
                if(is_last_char_a_space)
                {
                    if(token_idx + 1 < TOKEN_MAX)
                    {
                        func.tokens[token_idx].start = cursor;
                        func.tokens[token_idx].len = len;
                        len = 0;
                        token_idx++;
                    }
                    else
                    {
                        return exit::OK;
                    }
                }
                len++;
                is_last_char_a_space = false;
            }
            cursor++;
        }


        return exit::OK;
    }
}
}

