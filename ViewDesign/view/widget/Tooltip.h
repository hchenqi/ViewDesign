#pragma once

#include "ViewDesign/view/ViewBase.h"


namespace ViewDesign {


void ShowTooltip(ViewBase& view, const u16string& text);

void HideTooltip(ViewBase& view);

void DestroyTooltip(ViewBase& view);


} // namespace ViewDesign
