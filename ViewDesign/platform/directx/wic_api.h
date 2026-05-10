#pragma once

#include <wincodec.h>


namespace ViewDesign {

namespace DirectX {

using WICFactory = IWICImagingFactory;
using WICFormatConverter = IWICFormatConverter;


WICFactory& GetWICFactory();


} // namespace DirectX

} // namespace ViewDesign
