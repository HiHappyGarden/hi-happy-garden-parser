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
#include "osal/osal.hpp"

#ifdef INCLUDE_HHG_PARSER_CONFIG
#include "hhg-parser-config.h"
#endif

#include <stdint.h>

#ifndef HHG_PARSER_FORMAT_CHAR
    #define HHG_PARSER_FORMAT_CHAR "%c"
#endif

#ifndef HHG_PARSER_FORMAT_SHORT
    #define HHG_PARSER_FORMAT_SHORT "%d"
#endif

#ifndef HHG_PARSER_FORMAT_USHORT
	#define HHG_PARSER_FORMAT_USHORT "%u"
#endif

#ifndef HHG_PARSER_FORMAT_INT
    #define HHG_PARSER_FORMAT_INT "%d"
#endif

#ifndef HHG_PARSER_FORMAT_UINT
	#define HHG_PARSER_FORMAT_UINT "%u"
#endif

#ifndef HHG_PARSER_FORMAT_LONG
    #define HHG_PARSER_FORMAT_LONG "%ld"
#endif

#ifndef HHG_PARSER_FORMAT_ULONG
	#define HHG_PARSER_FORMAT_ULONG "%lu"
#endif

#ifndef HHG_PARSER_FORMAT_FLOAT
    #define HHG_PARSER_FORMAT_FLOAT "%f"
#endif

#ifndef HHG_PARSER_FORMAT_DOUBLE
    #define HHG_PARSER_FORMAT_DOUBLE "%lf"
#endif

#ifndef HHG_PARSER_ARGS_MAX
    #define HHG_PARSER_ARGS_MAX (2)
#endif

namespace hhg::parser
{
inline namespace v1
{

constexpr const uint8_t KEY_MAX = 32;
constexpr const uint8_t TOKEN_MAX = 6;

struct cmd_data;
struct entry
{
	using custom_function = os::exit (*)(const cmd_data& data, const entry* entry, os::error** error);

	char key[KEY_MAX]{};

	const entry* next = nullptr;
	uint8_t next_size = 0;

	os::function_base::ptr func;
	custom_function custom_func = nullptr;

	char const description[128]{};
};

struct token
{
	char* start = nullptr;
	size_t len = 0;
	os::trait_type type = os::trait_type::_VOID_;
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
	constexpr static char OK[] = "OK";
	constexpr static char KO[] = "KO";

	parser(entry* entries_table, size_t entries_table_size) OS_NOEXCEPT;
	parser(const parser&) = delete;
	parser& operator=(const parser&) = delete;
	parser(parser&&) = delete;
	parser& operator=(parser&&) = delete;

	os::exit set(char full_cmd[], os::function_base* func, os::error** error = nullptr) OS_NOEXCEPT;
private:
	os::exit set(cmd_data& data, entry* entries, size_t entries_size, os::function_base* func, os::error** error) OS_NOEXCEPT;

public:
	os::exit execute(char full_cmd[], char ret_value[] = nullptr, uint32_t ret_value_len = 0, os::error** error = nullptr) const OS_NOEXCEPT;


private:
	static os::exit execute(cmd_data& data, const entry* entries, size_t entries_size, os::error** error) OS_NOEXCEPT;
	static os::exit execute(cmd_data& data, const entry* entry, os::error** error) OS_NOEXCEPT;

	static os::exit tokenize(char* full_cmd, cmd_data& data, os::error** error) OS_NOEXCEPT;
	static os::exit typifies(const entry* entry, cmd_data& data, os::error **error) OS_NOEXCEPT;


	static char handle_arg_char(const token& token, os::error** error) OS_NOEXCEPT;
	static char* handle_arg_str(const token& token, os::error** error) OS_NOEXCEPT;
	static int32_t handle_arg_int(const token& token, os::error** error) OS_NOEXCEPT;
	static uint64_t handle_arg_long(const token& token, os::error** error) OS_NOEXCEPT;
	static float handle_arg_float(const token& token, os::error** error) OS_NOEXCEPT;
	static double handle_arg_double(const token& token, os::error** error) OS_NOEXCEPT;

	static os::exit handle_ret(const os::value& value, cmd_data& data) OS_NOEXCEPT;

};

}
}

