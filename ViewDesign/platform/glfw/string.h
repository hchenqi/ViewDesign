#pragma once

#include "ViewDesign/common/unicode.h"


namespace ViewDesign {

namespace GLFW {

static_assert(sizeof(u8char) == sizeof(char));


inline char* as_char_str(u8char* str) { return reinterpret_cast<char*>(str); }
inline const char* as_char_str(const u8char* str) { return reinterpret_cast<const char*>(str); }

inline u8char* as_u8char_str(char* str) { return reinterpret_cast<u8char*>(str); }
inline const u8char* as_u8char_str(const char* str) { return reinterpret_cast<const u8char*>(str); }


} // namespace GLFW

} // namespace ViewDesign
