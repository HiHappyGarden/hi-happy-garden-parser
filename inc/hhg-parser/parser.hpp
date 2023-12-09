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
#include <stdint.h>

#define DEFINE_PRINT_TYPE( type ) \
template<>\
const char* printtype<type>() {\
   return #type;\
}

namespace hhg::parser
{
inline namespace v1
{

constexpr const uint8_t MAX_PARAM = 6;

struct no_class final
{
    constexpr no_class() = default;
};

struct func_reference { };

template <typename T, typename V, typename R,
        typename A1 = no_class, typename A2 = no_class, typename A3 = no_class,
        typename A4 = no_class, typename A5 = no_class, typename A6 = no_class>
class method final : public func_reference
{
    T* target = nullptr;
    const uint8_t args_count = 0;
public:
    method(T *target, R (V::*method)());
    method(T *target, R (V::*method)(A1));
    method(T *target, R (V::*method)(const A1&));
    method(T *target, R (V::*method)(A1, A2));
    method(T *target, R (V::*method)(const A1&, const A2&));
    method(T *target, R (V::*method)(A1, A2, A3));
    method(T *target, R (V::*method)(const A1&, const A2&, const A3&));

    [[nodiscard]] inline T* get_target() const
    {
        return target;
    }

    [[nodiscard]] inline uint8_t get_args_count() const
    {
        return args_count;
    }
};

template<typename T, typename V, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
method<T, V, R, A1, A2, A3, A4, A5, A6>::method(T* target, R (V::*method)())
: target(target)
{ }

template<typename T, typename V, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
method<T, V, R, A1, A2, A3, A4, A5, A6>::method(T* target, R (V::*method)(A1))
: args_count(1)
, target(target)
{ }

template<typename T, typename V, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
method<T, V, R, A1, A2, A3, A4, A5, A6>::method(T* target, R (V::*method)(const A1&))
: args_count(1)
, target(target)
{ }

template<typename T, typename V, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
method<T, V, R, A1, A2, A3, A4, A5, A6>::method(T* target, R (V::*method)(A1, A2))
: args_count(2)
, target(target)
{ }

template<typename T, typename V, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
method<T, V, R, A1, A2, A3, A4, A5, A6>::method(T* target, R (V::*method)(const A1&, const A2&))
: args_count(2)
, target(target)
{ }

template<typename T, typename V, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
method<T, V, R, A1, A2, A3, A4, A5, A6>::method(T* target, R (V::*method)(A1, A2, A3))
: args_count(3)
, target(target)
{ }

template<typename T, typename V, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
method<T, V, R, A1, A2, A3, A4, A5, A6>::method(T* target, R (V::*method)(const A1&, const A2&, const A3&))
: args_count(3)
, target(target)
{ }



template <typename R,
        typename A1 = no_class, typename A2 = no_class, typename A3 = no_class,
        typename A4 = no_class, typename A5 = no_class, typename A6 = no_class>
class function final : public func_reference
{
    const uint8_t args_count = 0;
public:
    explicit function(R (*function)());
    explicit function(R (*function)(A1));
    explicit function(R (*function)(const A1&));
    explicit function(R (*function)(A1, A2));
    explicit function(R (*function)(const A1&, const A2&));
    explicit function(R (*function)(A1, A2, A3));
    explicit function(R (*function)(const A1&, const A2&, const A3&));

    [[nodiscard]] inline uint8_t get_args_count() const
    {
        return args_count;
    }
};

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
function<R, A1, A2, A3, A4, A5, A6>::function( R (*function)())
{ }

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
function<R, A1, A2, A3, A4, A5, A6>::function( R (*function)(A1))
: args_count(1)
{ }

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
function<R, A1, A2, A3, A4, A5, A6>::function( R (*function)(const A1&))
: args_count(1)
{ }

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
function<R, A1, A2, A3, A4, A5, A6>::function( R (*function)(A1, A2))
: args_count(2)
{ }

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
function<R, A1, A2, A3, A4, A5, A6>::function( R (*function)(const A1&, const A2&))
: args_count(2)
{ }

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
function<R, A1, A2, A3, A4, A5, A6>::function( R (*function)(A1, A2, A3))
: args_count(3)
{ }

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
function<R, A1, A2, A3, A4, A5, A6>::function( R (*function)(const A1&, const A2&, const A3&))
: args_count(3)
{ }



}
}

