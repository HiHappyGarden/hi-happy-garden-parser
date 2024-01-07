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

//#define HHG_PARSER_3_PARAM(enum_0, type_0, func_0, enum_1, type_1, func_1, enum_2, type_2, fun_2) \
//if (args[0] == enum_0 && args[1] == enum_1 && args[2] == enum_2) \
//{ \
//switch (entry->func->get_ret_type()) \
//{ \
//    case trait_type::_VOID_: \
//    { \
//        auto f = dynamic_cast<const function<void, uint8_t>*>(entry->func.get()); \
//        f->get_function().function_a0(1); \
//        return exit::OK; \
//    } \
//    case trait_type::CHAR: \
//
//    break;
//    case trait_type::STRING:
//
//    break;
//    case trait_type::INT32:
//
//    break;
//    case trait_type::INT64:
//
//    break;
//    case trait_type::FLOAT:
//
//    break;
//    case trait_type::DOUBLE:
//
//    break;
//    default:
//
//    return exit::KO;
//}\
//}




namespace hhg::parser
{
inline namespace v1
{

    os::exit pippo(array<trait_type, function_base::MAX_PARAM> params_types,  size_t param_i, cmd_data& data, const entry* entry, error** error)
    {
        if(1==1)
        {
            auto p0 = parser::handle_param_int(data.tokens[param_i], error);
            if(error)
            {
                return exit::KO;
            }
            auto p1 = parser::handle_param_int(data.tokens[param_i + 1], error);
            if(error)
            {
                return exit::KO;
            }
            auto p2 = parser::handle_param_int(data.tokens[param_i + 2], error);
            if(error)
            {
                return exit::KO;
            }
            switch (entry->func->get_ret_type())
            {
                case trait_type::_VOID_:
                {
                    if(entry->func->get_type() == osal::function_base::FUNCTION)
                    {
//                        auto f = dynamic_cast<const function<Test, void, int32_t, int32_t, int32_t>*>(entry->func.get());
//                        f->get_function().function_a0_a1_a2(p0, p1, p2);
                        return exit::OK;
                    }
                    else if(entry->func->get_type() == osal::function_base::METHOD)
                    {
                        auto m = dynamic_cast<const method<void, int32_t, int32_t, int32_t>*>(entry->func.get());

                        //m->.function_a0_a1_a2(p0, p1, p2);
                        return exit::OK;
                    }
                    else
                    {
                        return exit::KO;
                    }
                }
                case trait_type::CHAR:

                    break;
                case trait_type::STRING:

                    break;
                case trait_type::INT32:

                    break;
                case trait_type::INT64:

                    break;
                case trait_type::FLOAT:

                    break;
                case trait_type::DOUBLE:

                    break;
                default:

                    return exit::KO;
            }
        }

        return exit::KO;
    }


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

    token* key = nullptr;
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


    array<trait_type, function_base::MAX_PARAM> params_types(trait_type::_VOID_);


    size_t param_i = 0;
    for(size_t i = 0; i < data.tokens_len && i < TOKEN_MAX; i++)
    {
        if(!data.tokens[i].key)
        {
            if(param_i == 0)
            {
                param_i = i;
            }
            params_types << data.tokens[i].type;
        }
    }

    switch (entry->func->get_args_count())
    {
        case 0:
        {

            break;
        }
        case 1:
        {
            if(entry->func->get_type() == function_base::FUNCTION)
            {
                //function
                switch (entry->func->get_ret_type())
                {
                    case trait_type::_VOID_:
                    {
                        auto f = dynamic_cast<const function<void, uint8_t>*>(entry->func.get());
                        f->get_function().function_a0(1);



                        break;
                    }
                    case trait_type::CHAR:

                        break;
                    case trait_type::STRING:

                        break;
                    case trait_type::INT32:

                        break;
                    case trait_type::INT64:

                        break;
                    case trait_type::FLOAT:

                        break;
                    case trait_type::DOUBLE:

                        break;
                    default:

                        return exit::KO;
                }
            }
            else
            {
                //method


            }
            break;
        }
        case 2:
        {

            break;
        }
        case 3:
        {

            break;
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
                    {
                        type = trait_type::_VOID_;
                        break;
                    }
                    case trait_type::CHAR:
                    {
                        type = trait_type::CHAR;
                        break;
                    }
                    case trait_type::STRING:
                    case trait_type::STR:
                    {
                        type = trait_type::STRING;
                        break;
                    }
                    case trait_type::BOOL:
                    case trait_type::INT8:
                    case trait_type::UINT8:
                    case trait_type::INT16:
                    case trait_type::UINT16:
                    case trait_type::INT32:
                    case trait_type::UINT32:
                    {
                        type = trait_type::INT32;
                        break;
                    }
                    case trait_type::INT64:
                    case trait_type::UINT64:
                    {
                        type = trait_type::INT64;
                        break;
                    }
                    case trait_type::FLOAT:
                    {
                        type = trait_type::FLOAT;
                        break;
                    }
                    case trait_type::DOUBLE:
                    {
                        type = trait_type::DOUBLE;
                        break;
                    }
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

char parser::handle_param_char(const token& token, error** error) OS_NOEXCEPT
{

    return 1;
}

char* parser::handle_param_str(const token& token, error** error) OS_NOEXCEPT
{
    return nullptr;
}

int32_t parser::handle_param_int(const token& token, error** error) OS_NOEXCEPT
{
    return 1;
}

uint64_t parser::handle_param_long(const token& token, error** error) OS_NOEXCEPT
{
    return 1;
}

float parser::handle_param_float(const token& token, error** error) OS_NOEXCEPT
{
    return 1;
}

double parser::handle_param_double(const token& token, error** error) OS_NOEXCEPT
{
    return 1;
}


inline void parser::handle_ret_char(cmd_data& data, char c) OS_NOEXCEPT
{
    snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_CHAR, c);
}

inline void parser::handle_ret_str(cmd_data& data, const char* str) OS_NOEXCEPT
{
    strncpy(data.ret_buffer, str, data.ret_buffer_len);
}

inline void parser::handle_ret_int(cmd_data& data, int32_t i) OS_NOEXCEPT
{
    snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_INT, i);
}

inline void parser::handle_ret_long(cmd_data& data, uint64_t i) OS_NOEXCEPT
{
    snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_LONG, i);
}

inline void parser::handle_ret_float(cmd_data& data, float f) OS_NOEXCEPT
{
    snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_FLOAT, f);
}

inline void parser::handle_ret_double(cmd_data& data, double d) OS_NOEXCEPT
{
    snprintf(data.ret_buffer, data.ret_buffer_len, HHG_PARSER_FORMAT_DOUBLE, d);
}

}
}

