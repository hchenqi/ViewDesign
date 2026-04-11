#pragma once

#include <stdexcept>
#include <cassert>


#define ABSTRACT_BASE _declspec(novtable)
#define pure = 0


namespace ViewDesign {


template<class T> using ref_ptr = T*;
template<class T> using alloc_ptr = T*;

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using uint64 = unsigned long long;
using wchar = wchar_t;


template<class T> constexpr T max(T a, T b) { return a > b ? a : b; }
template<class T> constexpr T min(T a, T b) { return a < b ? a : b; }


} // namespace ViewDesign
