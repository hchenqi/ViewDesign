#include "ViewDesign/system/desktop.h"
#include "ViewDesign/platform/win32/geometry_helper.h"

namespace ViewDesign {


Size GetDesktopSize() {
	RECT rect;
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0);
	return Win32::AsRect(rect).size;
}


} // namespace ViewDesign
