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
using namespace os;

#include <errno.h>

#define OS_HANDLE_A0(idx_0, type_0, func_0) \
if(data.tokens[idx_0].type == type_0)\
{\
    value ret;\
    auto a0 = func_0(data.tokens[idx_0], error);\
    if(error)\
    {\
        return exit::KO;\
    }\
    if(entry->func->execute(&ret, &a0) == exit::KO)\
    {\
        return exit::KO;\
    }\
    handle_ret(ret, data);\
    return exit::OK;\
}

#define OS_HANDLE_A0_A1(idx_0, type_0, func_0, idx_1, type_1, func_1) \
if(data.tokens[idx_0].type == type_0 && data.tokens[idx_1].type == type_1)\
{\
    value ret;\
    auto a0 = func_0(data.tokens[idx_0], error);\
    if(error)\
    {\
        return exit::KO;\
    }\
    auto a1 = func_1(data.tokens[idx_1], error);\
    if(error)\
    {\
        return exit::KO;\
    }\
    if(entry->func->execute(&ret, &a0, &a1) == exit::KO)\
    {\
        return exit::KO;\
    }\
    handle_ret(ret, data);\
    return exit::OK;\
}

#define OS_HANDLE_A0_A1_A2(idx_0, type_0, func_0, idx_1, type_1, func_1, idx_2, type_2, func_2) \
if(data.tokens[idx_0].type == type_0 && data.tokens[idx_1].type == type_1 && data.tokens[idx_2].type == type_2)\
{\
    value ret;\
    auto a0 = func_0(data.tokens[idx_0], error);\
    if(error)\
    {\
        return exit::KO;\
    }\
    auto a1 = func_1(data.tokens[idx_1], error);\
    if(error)\
    {\
        return exit::KO;\
    }\
    auto a2 = func_2(data.tokens[idx_2], error);\
    if(error)\
    {\
        return exit::KO;\
    }\
    if(entry->func->execute(&ret, &a0, &a1, &a2) == exit::KO)\
    {\
        return exit::KO;\
    }\
    handle_ret(ret, data);\
    return exit::OK;\
}

namespace hhg::parser
{
inline namespace v1
{

parser::parser(entry* entries_table, size_t entries_table_size) OS_NOEXCEPT
: entries_table(entries_table)
, entries_table_size(entries_table_size)
{

}

os::exit parser::execute(char full_cmd[], char ret_value[], uint32_t ret_value_len, error** error) const OS_NOEXCEPT
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
        memset(ret_value, '\0', ret_value_len);
        data.ret_buffer = ret_value;
        data.ret_buffer_len = ret_value_len;
    }

    if(tokenize(full_cmd, data, error) == exit::KO)
    {
        return exit::KO;
    }


    return execute(data, entries_table, entries_table_size, error);
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

    token* key = data.tokens;
    for(size_t i = 0; i < data.tokens_len && i < TOKEN_MAX; i++)
    {
        key = data.tokens + i;
        if(!key->key)
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

                return parser::execute(data, cursor,  error);

            }
            else
            {
                return parser::execute(data, cursor->next, cursor->next_size,  error);
            }
        }
    }

    return exit::KO;
}

os::exit parser::execute(cmd_data& data, const entry* entry, error** error) OS_NOEXCEPT
{
    if(entry == nullptr)
    {
        if(error)
        {
            *error = OS_ERROR_BUILD("Null entry.", error_type::OS_EINVAL);
            OS_ERROR_PTR_SET_POSITION(*error);
        }
        return exit::KO;
    }

    if(entry->func == nullptr && entry->custom_func == nullptr)
    {
        if(error)
        {
            *error = OS_ERROR_BUILD("entry->func and entry->custom_func are null.", error_type::OS_EINVAL);
            OS_ERROR_PTR_SET_POSITION(*error);
        }
    	return exit::KO;
    }

    size_t param_i = 0;
    for(size_t i = 0; i < data.tokens_len && i < TOKEN_MAX; i++)
    {
        if(!data.tokens[i].key)
        {
            param_i = i;
            break;
        }
    }

    switch (entry->func->get_args_count())
    {
        case 0:
        {
            value ret;

            if(entry->func->execute(&ret) == exit::KO)
            {
                return exit::KO;
            }

            handle_ret(ret, data);
            return exit::OK;
        }
        case 1:
        {
            OS_HANDLE_A0(param_i, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0(param_i, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0(param_i, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0(param_i, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0(param_i, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0(param_i, trait_type::DOUBLE, handle_arg_double)
            else
                return exit::KO;
        }
        case 2:
        {
            OS_HANDLE_A0_A1(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::DOUBLE, handle_arg_double)
            else
                return exit::KO;
        }
        case 3:
        {
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::CHAR, handle_arg_char, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::STR, handle_arg_str, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT32, handle_arg_int, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::INT64, handle_arg_long, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::FLOAT, handle_arg_float, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::CHAR, handle_arg_char, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::STR, handle_arg_str, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT32, handle_arg_int, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::INT64, handle_arg_long, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::FLOAT, handle_arg_float, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::CHAR, handle_arg_char)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::STR, handle_arg_str)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT32, handle_arg_int)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::INT64, handle_arg_long)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::FLOAT, handle_arg_float)
            else
            OS_HANDLE_A0_A1_A2(param_i, trait_type::DOUBLE, handle_arg_double, param_i + 1, trait_type::DOUBLE, handle_arg_double, param_i + 2, trait_type::DOUBLE, handle_arg_double)
            else
                return exit::KO;
        }
    }



    return exit::KO;
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
    if(data.tokens_len > 0 )
    {
        data.tokens_len++;
    }
    len = 0;

    for(auto&& token : data.tokens)
    {
        if(token.len && *token.start == '"' && *(token.start + token.len - 1) == '"')
        {
            memmove(token.start, token.start + 1, --token.len);
            *(token.start + --token.len) = '\0';
            char* ptr = strstr(token.start, "\\" );
            while(ptr)
            {
                memmove(ptr, ptr + 1, --token.len);
                ptr = strstr(ptr, "\\");
            }
        }
    }

    return exit::OK;
}

os::exit parser::typify(const entry* entry, cmd_data& data, error** error) OS_NOEXCEPT
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
                switch (entry->func->get_args_type()[args_i]) //function
                {
                    case trait_type::_VOID_:
                        type = trait_type::_VOID_;
                        break;
                    case trait_type::CHAR:
                        type = trait_type::CHAR;
                        break;
                    case trait_type::STRING:
                    case trait_type::STR:
                        type = trait_type::STR;
                        break;
                    case trait_type::BOOL:
                    case trait_type::INT8:
                    case trait_type::UINT8:
                    case trait_type::INT16:
                    case trait_type::UINT16:
                    case trait_type::INT32:
                    case trait_type::UINT32:
                        type = trait_type::INT32;
                        break;
                    case trait_type::INT64:
                    case trait_type::UINT64:
                        type = trait_type::INT64;
                        break;
                    case trait_type::FLOAT:
                        type = trait_type::FLOAT;
                        break;
                    case trait_type::DOUBLE:
                        type = trait_type::DOUBLE;
                        break;
                    default:
                        type = trait_type::CUSTOM;
                        break;
                }
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

inline char parser::handle_arg_char(const token& token, error** error) OS_NOEXCEPT
{
    return token.start[0];
}

inline char* parser::handle_arg_str(const token& token, error** error) OS_NOEXCEPT
{
    return token.start;
}

int32_t parser::handle_arg_int(const token& token, error** error) OS_NOEXCEPT
{
    char* end_ptr = nullptr;
    auto i = static_cast<int32_t>(strtol(token.start, &end_ptr, 10));
    if (end_ptr == token.start || *end_ptr != '\0' || errno != 0)
    {
        if(error)
        {
            *error = OS_ERROR_BUILD("Invalid conversion to int32.", error_type::OS_VALCONV);
            OS_ERROR_PTR_SET_POSITION(*error);
        }
    }
    return i;
}

uint64_t parser::handle_arg_long(const token& token, error** error) OS_NOEXCEPT
{
    char* end_ptr = nullptr;
    auto l = static_cast<int64_t>(strtol(token.start, &end_ptr, 10));
    if (end_ptr == token.start || *end_ptr != '\0' || errno != 0)
    {
        if(error)
        {
            *error = OS_ERROR_BUILD("Invalid conversion to int64.", error_type::OS_VALCONV);
            OS_ERROR_PTR_SET_POSITION(*error);
        }
    }
    return l;
}

float parser::handle_arg_float(const token& token, error** error) OS_NOEXCEPT
{
    char* end_ptr = nullptr;
    auto f = strtof(token.start, &end_ptr);
    if (end_ptr == token.start || *end_ptr != '\0' || errno != 0)
    {
        if(error)
        {
            *error = OS_ERROR_BUILD("Invalid conversion to float.", error_type::OS_VALCONV);
            OS_ERROR_PTR_SET_POSITION(*error);
        }
    }
    return f;
}

double parser::handle_arg_double(const token& token, error** error) OS_NOEXCEPT
{
    char* end_ptr = nullptr;
    auto d = strtod(token.start, &end_ptr);
    if (end_ptr == token.start || *end_ptr != '\0' || errno != 0)
    {
        if(error)
        {
            *error = OS_ERROR_BUILD("Invalid conversion to double.", error_type::OS_VALCONV);
            OS_ERROR_PTR_SET_POSITION(*error);
        }
    }
    return d;
}

os::exit parser::handle_ret(const value& value, cmd_data& data) OS_NOEXCEPT
{
    if(data.ret_buffer == nullptr && data.ret_buffer_len == 0)
    {
        return exit::OK;
    }

    switch (value.get_type())
    {
        case trait_type::_VOID_:
            break;
        case trait_type::CHAR:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_CHAR, value.get_char());
            break;
        case trait_type::STRING:
        case trait_type::STR:
            strncpy(data.ret_buffer, value.get_str(), data.ret_buffer_len);
            break;
        case trait_type::BOOL:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_UINT, value.get_bool());
            break;
        case trait_type::INT8:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_INT, value.get_int8());
            break;
        case trait_type::UINT8:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_UINT, value.get_uint8());
            break;
        case trait_type::INT16:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_INT, value.get_int16());
            break;
        case trait_type::UINT16:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_UINT, value.get_uint16());
            break;
        case trait_type::INT32:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_INT, value.get_int32());
            break;
        case trait_type::UINT32:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_UINT, value.get_uint32());
            break;
        case trait_type::INT64:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_INT, value.get_int8());
            break;
        case trait_type::UINT64:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_LONG, value.get_int64());
            break;
        case trait_type::FLOAT:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_FLOAT, value.get_float());
            break;
        case trait_type::DOUBLE:
            snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_DOUBLE, value.get_double());
            break;
        default:
            return exit::KO;
    }
    return exit::OK;
}

}
}

