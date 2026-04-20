#pragma once

#include "ViewDesign/geometry/geometry.h"


namespace ViewDesign {

struct RenderTarget;


struct Figure {
	virtual void DrawOn(RenderTarget& target, Point point) const = 0;
};


const Size GetTargetSize(const RenderTarget& target);


} // namespace ViewDesign
