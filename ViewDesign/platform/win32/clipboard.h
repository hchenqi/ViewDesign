#pragma once

#include <string>


namespace ViewDesign {

namespace Win32 {


void SetClipboardText(std::wstring text);

void GetClipboardText(std::wstring& text);


} // namespace Win32

} // namespace ViewDesign
