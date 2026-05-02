#pragma once

#include "ViewDesign/drawing/color.h"


namespace ViewDesign {

namespace Win32 {

struct D2DFactory;
struct D2DDeviceContext;
struct D2DSolidColorBrush;


D2DFactory& GetD2DFactory();
D2DDeviceContext& GetD2DDeviceContext();
D2DSolidColorBrush& GetD2DSolidColorBrush(Color color);


} // namespace Win32

} // namespace ViewDesign
