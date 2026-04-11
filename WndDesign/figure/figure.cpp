#include "figure.h"
#include "../system/directx_helper.h"


namespace ViewDesign {


const Size GetTargetSize(const RenderTarget& target) {
	return AsSize(target.GetSize());
}


} // namespace ViewDesign
