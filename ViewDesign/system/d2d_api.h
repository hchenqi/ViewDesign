#pragma once

#include "../drawing/color.h"


struct ID2D1Factory8;
struct ID2D1DeviceContext7;
struct ID2D1SolidColorBrush;


namespace ViewDesign {


ID2D1Factory8& GetD2DFactory();
ID2D1DeviceContext7& GetD2DDeviceContext();
ID2D1SolidColorBrush& GetD2DSolidColorBrush(Color color);

void BeginDraw();
void EndDraw();


} // namespace ViewDesign
