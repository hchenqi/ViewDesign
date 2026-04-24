#pragma once

#include "ViewDesign/common/unicode.h"


namespace ViewDesign {

namespace Win32 {


void SetClipboardText(u16string text);

void GetClipboardText(u16string& text);


} // namespace Win32

} // namespace ViewDesign
