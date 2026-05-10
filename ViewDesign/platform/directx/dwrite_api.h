#pragma once

#include <dwrite_3.h>


namespace ViewDesign {

namespace DirectX {

using DWriteFactory = IDWriteFactory2;
using DWriteTextLayout = IDWriteTextLayout2;


DWriteFactory& GetDWriteFactory();


} // namespace DirectX

} // namespace ViewDesign
