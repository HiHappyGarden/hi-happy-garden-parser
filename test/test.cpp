/***************************************************************************
 *
 * OSAL
 * Copyright (C) 2023  Antonio Salsi <passy.linux@zresa.it>
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

#include <hhg-parser/parser.hpp>

using namespace hhg::parser;




TEST(foo, test)
{

    char cmd[] = "$USR \"{\\\"key\\\":\\\"json test\\\"}\"    test more spaced string";
    //char cmd[] = "$USR \" questa è una bella roba  \\\"bello carico\\\"  |\"    test unica stringa staccata";

    struct parser parser{nullptr, 0};

    parser.execute(cmd);


}


