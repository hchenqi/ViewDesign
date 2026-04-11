#pragma once

#include "../geometry/geometry.h"


namespace ViewDesign {

struct RenderTarget;

struct ABSTRACT_BASE Figure {
	virtual void DrawOn(RenderTarget& target, Point point) const pure;
};


const Size GetTargetSize(const RenderTarget& target);


} // namespace ViewDesign
