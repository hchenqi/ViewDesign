#pragma once

#include "../geometry/border_helper.h"


namespace ViewDesign {

class WndObject;


void AeroSnapDraggingEffect(WndObject& wnd);

void AeroSnapBorderResizingEffect(WndObject& wnd, BorderPosition border_position);


} // namespace ViewDesign
