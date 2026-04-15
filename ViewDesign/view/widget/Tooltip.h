#pragma once

#include "../ViewBase.h"

#include <string>


namespace ViewDesign {


void ShowTooltip(ViewBase& view, const std::wstring& text);

void HideTooltip(ViewBase& view);

void DestroyTooltip(ViewBase& view);


} // namespace ViewDesign
