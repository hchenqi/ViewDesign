#include "ViewDesign/system/desktop.h"

#include <ViewDesign/platform/win32/geometry_helper.h>

namespace ViewDesign {

using namespace Win32;


SizeU GetDesktopSize() {
	RECT rect;
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0);
	return AsRectI(rect).size;
}


} // namespace ViewDesign
