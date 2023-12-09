/***************************************************************************
 *
 * Hi Happy Garden
 * Copyright (C) 2024 Antonio Salsi <passy.linux@zresa.it>
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
#include <osal/osal.hpp>
#include <osal/osal.hpp>

namespace hhg::parser
{
inline namespace v1
{

constexpr uint8_t MAX_PARAM = 6;

struct no_class
{
    constexpr no_class() = default;
};

template <typename A1 = no_class, typename A2 = no_class, typename A3 = no_class,
        typename A4 = no_class, typename A5 = no_class, typename A6 = no_class>
class method
{
    template<typename T, typename V>
    method(T *target, void (V::*method)());

    template<typename T, typename V>
    method(T *target, void (V::*method)(A1));

    template<typename T, typename V>
    method(T *target, void (V::*method)(const A1&));
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
template<typename T, typename V>
method<A1, A2, A3, A4, A5, A6>::method(T* target, void (V::*method)())
{

}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
template<typename T, typename V>
method<A1, A2, A3, A4, A5, A6>::method(T* target, void (V::*method)(A1))
{

}

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
template<typename T, typename V>
method<A1, A2, A3, A4, A5, A6>::method(T* target, void (V::*method)(const A1&))
{

}


}
}

