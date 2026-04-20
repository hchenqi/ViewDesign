#pragma once

#include "ViewDesign/view/ViewFrame.h"
#include "ViewDesign/geometry/region.h"
#include "ViewDesign/drawing/layer.h"


namespace ViewDesign {


class _LayerFrame_Base : public ViewFrame {
protected:
	_LayerFrame_Base(uchar opacity, view_ptr<> child) : ViewFrame(std::move(child)), opacity(opacity) {}

	// style
protected:
	uchar opacity;
public:
	uchar GetOpacity() const { return opacity; }
	void SetOpacity(uchar opacity) { this->opacity = opacity; Redraw(region_infinite); }

	// layout
protected:
	Size size;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override;
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override;

	// drawing
protected:
	Scale scale;
	Layer layer;
	Region invalid_region;
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override;
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override;
};


template<class WidthTrait, class HeightTrait>
class LayerFrame : public _LayerFrame_Base, public SizeTrait<WidthTrait, HeightTrait> {
public:
	LayerFrame(view_ptr<WidthTrait, HeightTrait> child) : LayerFrame(0xFF, std::move(child)) {}
	LayerFrame(uchar opacity, view_ptr<WidthTrait, HeightTrait> child) : _LayerFrame_Base(opacity, std::move(child)) {}
};


template<class T>
LayerFrame(T) -> LayerFrame<extract_width_trait<T>, extract_height_trait<T>>;

template<class T>
LayerFrame(uchar, T) -> LayerFrame<extract_width_trait<T>, extract_height_trait<T>>;


} // namespace ViewDesign
