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
using namespace osal;

namespace hhg::parser
{
inline namespace v1
{

    parser::parser(entry* entries_table, size_t entries_table_size) OS_NOEXCEPT
    : entries_table(entries_table)
    , entries_table_size(entries_table_size)
    {

    }

    os::exit parser::execute(char full_cmd[], char ret_value[], uint32_t ret_value_len, error** error) OS_NOEXCEPT
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

        cmd_data data;
        if(ret_value)
        {
            data.ret_buffer = ret_value;
            data.ret_buffer_len = ret_value_len;
        }

        if(tokenize(full_cmd, data, error) == exit::KO)
        {
            return exit::KO;
        }


        return execute(data, entries_table, entries_table_size, error);
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
            *error = OS_ERROR_BUILD("Null entry.", error_type::OS_EINVAL);
            OS_ERROR_PTR_SET_POSITION(*error);
        }
        return exit::KO;
    }

    if(entry->custom_func)
    {
        return entry->custom_func(entry, data, error);
    }

    switch (entry->func->get_args_count())
    {
        case 0:
        {
            if(entry->func->get_ret_type() == trait_type::_VOID_)
            {
                if(entry->func->get_type() == function_base::FUNCTION)
                {

                }
            }
            break;
        }
        case 1:
        {
            switch (entry->func->get_ret_type())
            {
                case trait_type::_VOID_:
                {
                    if(entry->func->get_type() == function_base::FUNCTION)
                    {
                        param p0;
                        if(valorize_param(data.tokens[2] , p0, error) == exit::KO)
                        {
                            return exit::KO;
                        }

                        auto f = (const function<void, uint8_t>*)(entry->func.get());
                        f->get_function().function_a0(1);

                    }
                    else
                    {
                        return exit::OK;
                    }
                    break;
                }
                case trait_type::CHAR:
                    break;
                case trait_type::BOOL:
                    break;
                case trait_type::STRING:
                case trait_type::STR:
                    break;
                case trait_type::INT8:
                    break;
                case trait_type::UINT8:
                    break;
                case trait_type::INT16:
                    break;
                case trait_type::UINT16:
                    break;
                case trait_type::INT32:
                    break;
                case trait_type::UINT32:
                    break;
                case trait_type::INT64:
                    break;
                case trait_type::UINT64:
                    break;
                case trait_type::FLOAT:
                    break;
                case trait_type::DOUBLE:
                    break;
                default:
                    return exit::OK;
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
        }

        return exit::OK;
    }

os::exit parser::tokenize(char* full_cmd, cmd_data& data, error** error) OS_NOEXCEPT
{
    if(full_cmd == nullptr)
    {
        if(entry == nullptr)
        {
            if(error)
            {
                *error = OS_ERROR_BUILD("Invalid argument.", error_type::OS_EINVAL);
                OS_ERROR_PTR_SET_POSITION(*error);
            }
            return exit::KO;
        }

        uint8_t args_count = entry->func.get_args_count();
        if(args_count)
        {
            uint8_t args_i = 0;
            for(auto&& [start, len, type, key] : data.tokens)
            {
                if(key)
                {
                    continue;
                }
                if(args_i < args_count)
                {
                    type = entry->func.get_args_type()[args_i];
                }
                else
                {
                    break;
                }
                args_i++;
            }
        }

        return exit::OK;
    }

    os::exit parser::execute(cmd_data& data, const entry* entries, size_t entries_size, error** error) OS_NOEXCEPT
    {
        if(entries == nullptr || entries_size == 0)
        {
            if(error)
            {
                *error = OS_ERROR_BUILD("Null entries_table.", error_type::OS_EINVAL);
                OS_ERROR_PTR_SET_POSITION(*error);
            }
            return exit::KO;
        }

        token* key = nullptr;
        for(size_t i = 0; i < data.tokens_len && i < TOKEN_MAX; i++)
        {
            *error = OS_ERROR_BUILD("Invalid argument.", error_type::OS_EINVAL);
            OS_ERROR_PTR_SET_POSITION(*error);
        }
        return exit::KO;
    }

    uint8_t args_count = entry->func->get_args_count();
    if(args_count)
    {
        uint8_t args_i = 0;
        for(auto&& [start, len, type, key] : data.tokens)
        {
            if(key)
            {
                continue;
            }
            if(args_i < args_count)
            {
                type = entry->func->get_args_type()[args_i];
            }
            else
            {
                break;
            }
        }

        for(size_t i = 0; i < entries_size; i++)
        {
            const entry* cursor = entries + i;
            if(strncmp(key->start, cursor->key, sizeof(cursor->key)) == 0)
            {
                key->key = true;
                if(cursor->next == nullptr)
                {
                    if(typify(cursor, data, error) == exit::KO)
                    {
                        return exit::KO;
                    }
                }
                else
                {
                    return parser::execute(data, cursor->next, cursor->next_size,  error);
                }
            }
        }

        return exit::KO;
    }
    return exit::OK;
}
}

