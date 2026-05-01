#pragma once

#include "ViewDesign/geometry/border_helper.h"


namespace ViewDesign {

using Handle = void*;


namespace Win32 {


void AeroSnapDraggingEffect(Handle handle);

void AeroSnapBorderResizingEffect(Handle handle, BorderPosition border_position);


} // namespace Win32

} // namespace ViewDesign
