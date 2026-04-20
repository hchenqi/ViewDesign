#pragma once

#include <stdexcept>


namespace ViewDesign {


template<class T> using ref_ptr = T*;
template<class T> using owner_ptr = T*;

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using uint64 = unsigned long long;
using wchar = wchar_t;


} // namespace ViewDesign
