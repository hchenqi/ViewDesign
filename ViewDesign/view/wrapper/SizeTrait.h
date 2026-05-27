#pragma once

#include "ViewDesign/view/view_traits.h"


namespace ViewDesign {


template<size_trait WidthTrait, size_trait HeightTrait, view_type View>
class SizeTraitOverride : public View {
protected:
	using Base = SizeTraitOverride;

public:
	using width_trait = WidthTrait;
	using height_trait = HeightTrait;

public:
	using View::View;
};


} // namespace ViewDesign
