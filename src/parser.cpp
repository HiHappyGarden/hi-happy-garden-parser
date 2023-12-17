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
        if(TOKEN_MAX < 4)
        {
            if(error)
            {
                *error = OS_ERROR_BUILD("TOKEN_MAX too small", error_type::OS_EINVAL);
                OS_ERROR_PTR_SET_POSITION(*error);
            }
            return exit::KO;
        }

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

    os::exit parser::tokenize(char* full_cmd, cmd_data& data, error** error) OS_NOEXCEPT
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

        data.full_cmd = full_cmd;

        size_t len = 0;
        bool is_last_char_a_space = false;
        bool is_last_char_a_backslash = false;
        bool open_double_quotes = false;
        char* cursor = data.full_cmd;
        data.tokens[0].start = cursor;
        data.tokens_len      = 0;
        while (*cursor != '\0')
        {
            if(*cursor == ' ' && !open_double_quotes)
            {
                if(!is_last_char_a_space)
                {
                    *cursor = '\0';
                    data.tokens[data.tokens_len].len = len;
                    len = 0;
                    if(data.tokens_len + 1 < TOKEN_MAX)
                    {
                        data.tokens_len++;
                    }
                    else
                    {
                        return exit::OK;
                    }
                }
                is_last_char_a_space = true;
            }
            else
            {
                if(*cursor == '"' && !is_last_char_a_backslash)
                {
                    open_double_quotes = !open_double_quotes;
                }

                is_last_char_a_backslash = *cursor == '\\';

                if(is_last_char_a_space)
                {
                    data.tokens[data.tokens_len].start = cursor;
                }
                len++;
                is_last_char_a_space = false;
            }
            cursor++;
        }

        data.tokens[data.tokens_len].len = len;
        len = 0;

        for(auto token : data.tokens)
        {
            if(token.len && *token.start == '"' && *(token.start + token.len - 1) == '"')
            {
                //token.start = &token.start[1];
                token.start++;
                *(token.start + token.len - 2) = '\0';
                token.len -= 2;
            }
            printf("token.start: %s\n", token.start);
            fflush(stdout);
        }



        return exit::OK;
    }
}
}

