#pragma once

#include "ViewDesign/geometry/border_helper.h"


namespace ViewDesign {

class ViewBase;


void AeroSnapDraggingEffect(ViewBase& view);

void AeroSnapBorderResizingEffect(ViewBase& view, BorderPosition border_position);


} // namespace ViewDesign
