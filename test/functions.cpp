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

#include "functions.hpp"

#include "osal/osal.hpp"
using namespace osal;

static string name = "Mario Bros";
static uint8_t age = 30;

void set_name(const char* name)
{
    ::name = name;
}

const char* get_name()
{
    return name.c_str();
}

void set_age(uint8_t age)
{
    ::age = age;
}

uint8_t get_age()
{
    return age;
}

const char* get_version()
{
    return "1.0.0";
}