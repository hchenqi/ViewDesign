#include "ViewDesign/drawing/figure.h"
#include "ViewDesign/backend/win32/directx_helper.h"


namespace ViewDesign {


const Size GetTargetSize(const RenderTarget& target) {
	return AsSize(target.GetSize());
}


} // namespace ViewDesign
