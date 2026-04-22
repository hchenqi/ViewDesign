#include "ViewDesign/drawing/figure.h"
#include "ViewDesign/platform/win32/directx_helper.h"


namespace ViewDesign {

using namespace Win32;


const Size GetTargetSize(const RenderTarget& target) {
	return AsSize(target.GetSize());
}


} // namespace ViewDesign
