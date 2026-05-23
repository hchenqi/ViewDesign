#pragma once

#include "ViewDesign/geometry/border_position.h"


namespace ViewDesign {


namespace Win32 {


void AeroSnapDraggingEffect(Handle window);

void AeroSnapBorderResizingEffect(Handle window, BorderPosition border_position);


} // namespace Win32

} // namespace ViewDesign
