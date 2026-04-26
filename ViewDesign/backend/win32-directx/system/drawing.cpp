#include "ViewDesign/system/drawing.h"
#include "ViewDesign/platform/win32/d2d_api.h"


namespace ViewDesign {


void BeginDraw() { Win32::BeginDraw(); }

void EndDraw() { Win32::EndDraw(); }


} // namespace ViewDesign
