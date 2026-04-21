#pragma once

#include "ViewDesign/drawing/color.h"


namespace ViewDesign {

struct D2DFactory;
struct D2DDeviceContext;
struct D2DSolidColorBrush;


D2DFactory& GetD2DFactory();
D2DDeviceContext& GetD2DDeviceContext();
D2DSolidColorBrush& GetD2DSolidColorBrush(Color color);

void BeginDraw();
void EndDraw();


} // namespace ViewDesign
