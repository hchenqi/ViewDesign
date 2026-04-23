#pragma once

#include "ViewDesign/geometry/geometry.h"


namespace ViewDesign {

struct RenderTarget;


struct Figure {
	virtual ~Figure() = default;
	virtual void DrawOn(RenderTarget& target, Point point) const = 0;
};


} // namespace ViewDesign
