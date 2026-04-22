#pragma once

#include "ViewDesign/geometry/border_helper.h"


namespace ViewDesign {

class ViewBase;

namespace Win32 {


void AeroSnapDraggingEffect(ViewBase& view);

void AeroSnapBorderResizingEffect(ViewBase& view, BorderPosition border_position);


} // namespace Win32

} // namespace ViewDesign
