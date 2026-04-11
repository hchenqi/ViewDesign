#pragma once

#include "../common/core.h"

#include <string>


namespace ViewDesign {


void SetClipboardData(std::wstring text);

void GetClipboardData(std::wstring& text);


} // namespace ViewDesign
