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
#include <gtest/gtest.h>
#include <iostream>

#include <hhg-parser/parser.hpp>
using namespace hhg::parser;
using namespace os;

extern hhg::parser::entry* get_table_commands();
extern size_t get_table_size_commands();

extern void set_age(uint8_t age);

void test_func(uint8_t i)
{
    std::cout << "-----" << std::to_string(i) << std::endl;
}

//TEST(foo, test_a)
//{
//    const os::function f = os::function{set_age, trait_type::_VOID_, trait_type::UINT8};
//
//    f.get_function().function_a0(12);
//
//    const function_base& fb  = f;
//
//    auto t = dynamic_cast<  const os::function<void, uint8_t>  *>(&fb);
//
//    f.get_function().function_a0(12);
//
//}
//
//
//TEST(foo, test_b)
//{
//    const auto& fb  = ((get_table_commands() + 2)->next->func);
//
//    auto t = dynamic_cast<  const os::function<void, uint8_t>  *>(&fb);
//
//    t->get_function().function_a0(12);
//
//    //os::function f = *get_table_commands();
////
////    f.get_function().function_a0(12);
////
////    function_base& fb  = f;
////
////    auto t = dynamic_cast<  os::function<void, uint8_t>  *>(&fb);
////
////    f.get_function().function_a0(12);
//
//}


TEST(foo, test_int)
{
    char cmd[] = "^TEST";
    struct parser parser{get_table_commands(), get_table_size_commands()};

    char buffer[129];

    parser.execute(cmd, buffer, sizeof(buffer) - 1);

}



