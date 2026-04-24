#include "ViewDesign/common/unicode.h"


namespace ViewDesign {

namespace Win32 {

static_assert(sizeof(u16char) == sizeof(wchar_t));


inline wchar_t* as_wchar_str(u16char* str) { return reinterpret_cast<wchar_t*>(str); }
inline const wchar_t* as_wchar_str(const u16char* str) { return reinterpret_cast<const wchar_t*>(str); }


} // namespace Win32

} // namespace ViewDesign
