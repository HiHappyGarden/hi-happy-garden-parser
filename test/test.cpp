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

TEST(foo, test_a)
{
    const os::function f = os::function{set_age};

    f.get_function().function_a0(12);

    const function_base& fb  = f;

    auto t = dynamic_cast<  const os::function<void, uint8_t>  *>(&fb);

    f.get_function().function_a0(12);

}

TEST(foo, test_test)
{
    char cmd[] = "^TEST";
    struct parser parser{get_table_commands(), get_table_size_commands()};

    char buffer[129];

    parser.execute(cmd, buffer, sizeof(buffer) - 1);

}

TEST(foo, test_user)
{
    struct parser parser{get_table_commands(), get_table_size_commands()};

    {
        char cmd[] = "^USR 1 20";
        ASSERT_EQ(parser.execute(cmd), os::exit::OK);
    }

    {
        char buffer[129];
        char cmd[] = "^USR 2";
        ASSERT_EQ(parser.execute(cmd, buffer, sizeof(buffer) - 1), os::exit::OK);
        ASSERT_TRUE(strncmp(buffer, "20", sizeof(buffer) - 1) == 0);
    }

}

TEST(foo, test_str)
{
    struct parser parser{get_table_commands(), get_table_size_commands()};

    {
        char buffer[129];
        char cmd[] = R"(^USR 5 "[\"Gilbert\", \"2013\", 24, true]")";
        ASSERT_EQ(parser.execute(cmd, buffer, sizeof(buffer) - 1), os::exit::OK);
        ASSERT_TRUE(strncmp(buffer, "1", sizeof(buffer) - 1) == 0);
    }

    {
        char buffer[129];
        char cmd[] = R"(^USR 6)";
        ASSERT_EQ(parser.execute(cmd, buffer, sizeof(buffer) - 1), os::exit::OK);
        ASSERT_TRUE(strncmp(buffer, R"(["Gilbert", "2013", 24, true])", sizeof(buffer) - 1) == 0);
    }
}

TEST(foo, test_custom)
{
    struct parser parser{get_table_commands(), get_table_size_commands()};

    {
        char buffer[129];
        char cmd[] = R"(^CUST 12 34 5")";
        ASSERT_EQ(parser.execute(cmd, buffer, sizeof(buffer) - 1), os::exit::OK);
        ASSERT_TRUE(strncmp(buffer, "bye", sizeof(buffer) - 1) == 0);
    }

}


TEST(foo, fail)
{
    struct parser parser{get_table_commands(), get_table_size_commands()};

    {
        char cmd[] = "^USR 1";
        ASSERT_EQ(parser.execute(cmd), os::exit::KO);
    }

    {
        char buffer[129];
        char cmd[] = "^PIPPO";
        ASSERT_EQ(parser.execute(cmd, buffer, sizeof(buffer) - 1), os::exit::KO);
    }

}
