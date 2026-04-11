#pragma once

#include "../ViewFrame.h"
#include "../../figure/layer.h"
#include "../../geometry/region.h"


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

	// paint
protected:
	Scale scale;
	Layer layer;
	Region invalid_region;
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override;
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override;
};


template<class WidthType, class HeightType>
class LayerFrame : public _LayerFrame_Base, public LayoutType<WidthType, HeightType> {
public:
	LayerFrame(view_ptr<WidthType, HeightType> child) : LayerFrame(0xFF, std::move(child)) {}
	LayerFrame(uchar opacity, view_ptr<WidthType, HeightType> child) : _LayerFrame_Base(opacity, std::move(child)) {}
};


template<class T>
LayerFrame(T) -> LayerFrame<extract_width_type<T>, extract_height_type<T>>;

template<class T>
LayerFrame(uchar, T) -> LayerFrame<extract_width_type<T>, extract_height_type<T>>;


} // namespace ViewDesign
