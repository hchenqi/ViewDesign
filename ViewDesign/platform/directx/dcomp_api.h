#pragma once

#include <dcomp.h>


namespace ViewDesign {

namespace DirectX {

using DCompositionDevice = IDCompositionDevice;
using DCompositionTarget = IDCompositionTarget;


DCompositionDevice& GetDCompositionDevice();


} // namespace DirectX

} // namespace ViewDesign
