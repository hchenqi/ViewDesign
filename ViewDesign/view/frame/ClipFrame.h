#pragma once

#include "../ViewFrame.h"


namespace ViewDesign {


struct Left {};
struct Top {};
struct Right {};
struct Bottom {};
struct TopLeft {};
struct TopRight {};
struct BottomLeft {};
struct BottomRight {};


template<class WidthTrait, class HeightTrait, class Position>
class ClipFrame;


class _ClipFrame_Base : public ViewFrame {
protected:
	_ClipFrame_Base(view_ptr<> child) : ViewFrame(std::move(child)) {}

	// layout
protected:
	Size size;
	Rect child_region;
protected:
	Vector GetChildOffset() const { return child_region.point - point_zero; }
	Rect GetChildRegion() const { return child_region; }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override { return GetChildOffset(); }

	// paint
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region + GetChildOffset()); }
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { return DrawChild(child, point_zero + GetChildOffset(), canvas, draw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point -= GetChildOffset(); return ViewFrame::HitTest(event); }
};


template<>
class ClipFrame<Fixed, Fixed, TopLeft> : public _ClipFrame_Base, public SizeTrait<Fixed, Fixed> {
public:
	ClipFrame(view_ptr<> child) : _ClipFrame_Base(std::move(child)) { child_region.point = point_zero; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		child_region.size = child_size;
		Redraw(region_infinite);
	}
};

template<>
class ClipFrame<Fixed, Fixed, TopRight> : public _ClipFrame_Base, public SizeTrait<Fixed, Fixed> {
public:
	ClipFrame(view_ptr<> child) : _ClipFrame_Base(std::move(child)) { child_region.point.y = 0; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point.x = size.width - child_region.size.width;
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		child_region.size = child_size;
		child_region.point.x = size.width - child_region.size.width;
		Redraw(region_infinite);
	}
};

template<>
class ClipFrame<Fixed, Fixed, BottomLeft> : public _ClipFrame_Base, public SizeTrait<Fixed, Fixed> {
public:
	ClipFrame(view_ptr<> child) : _ClipFrame_Base(std::move(child)) { child_region.point.x = 0; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point.y = size.height - child_region.size.height;
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		child_region.size = child_size;
		child_region.point.y = size.height - child_region.size.height;
		Redraw(region_infinite);
	}
};

template<>
class ClipFrame<Fixed, Fixed, BottomRight> : public _ClipFrame_Base, public SizeTrait<Fixed, Fixed> {
public:
	ClipFrame(view_ptr<> child) : _ClipFrame_Base(std::move(child)) {}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point = Point(size.width - child_region.size.width, size.height - child_region.size.height);
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		child_region.size = child_size;
		child_region.point = Point(size.width - child_region.size.width, size.height - child_region.size.height);
		Redraw(region_infinite);
	}
};


template<class HeightTrait>
class ClipFrame<Fixed, HeightTrait, Left> : public _ClipFrame_Base, public SizeTrait<Fixed, HeightTrait> {
public:
	ClipFrame(view_ptr<Relative, HeightTrait> child) : _ClipFrame_Base(std::move(child)) { child_region.point = point_zero; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		size.height = child_region.size.height;
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		child_region.size = child_size;
		if (size.height != child_size.height) {
			size.height = child_size.height;
			SizeUpdated(size);
		} else {
			Redraw(region_infinite);
		}
	}
};

template<class HeightTrait>
class ClipFrame<Fixed, HeightTrait, Right> : public _ClipFrame_Base, public SizeTrait<Fixed, HeightTrait> {
public:
	ClipFrame(view_ptr<Relative, HeightTrait> child) : _ClipFrame_Base(std::move(child)) { child_region.point.y = 0; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point.x = size.width - child_region.size.width;
		size.height = child_region.size.height;
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		child_region.size = child_size;
		child_region.point.x = size.width - child_region.size.width;
		if (size.height != child_size.height) {
			size.height = child_size.height;
			SizeUpdated(size);
		} else {
			Redraw(region_infinite);
		}
	}
};


template<class WidthTrait>
class ClipFrame<WidthTrait, Fixed, Top> : public _ClipFrame_Base, public SizeTrait<WidthTrait, Fixed> {
public:
	ClipFrame(view_ptr<WidthTrait, Relative> child) : _ClipFrame_Base(std::move(child)) {}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		size.width = child_region.size.width;
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		child_region.size = child_size;
		if (size.width != child_size.width) {
			size.width = child_size.width;
			SizeUpdated(size);
		} else {
			Redraw(region_infinite);
		}
	}
};

template<class WidthTrait>
class ClipFrame<WidthTrait, Fixed, Bottom> : public _ClipFrame_Base, public SizeTrait<WidthTrait, Fixed> {
public:
	ClipFrame(view_ptr<WidthTrait, Relative> child) : _ClipFrame_Base(std::move(child)) { child_region.point.x = 0; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point.y = size.height - child_region.size.height;
		size.width = child_region.size.width;
		return size;
	}
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {
		child_region.size = child_size;
		child_region.point.y = size.height - child_region.size.height;
		if (size.width != child_size.width) {
			size.width = child_size.width;
			SizeUpdated(size);
		} else {
			Redraw(region_infinite);
		}
	}
};


} // namespace ViewDesign
