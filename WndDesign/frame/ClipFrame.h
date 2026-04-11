#pragma once

#include "WndFrame.h"


namespace ViewDesign {


struct Left {};
struct Top {};
struct Right {};
struct Bottom {};
struct TopLeft {};
struct TopRight {};
struct BottomLeft {};
struct BottomRight {};


template<class WidthType, class HeightType, class Position>
class ClipFrame;


class _ClipFrame_Base : public WndFrame {
protected:
	_ClipFrame_Base(child_ptr<> child) : WndFrame(std::move(child)) {}

	// layout
protected:
	Size size;
	Rect child_region;
protected:
	Vector GetChildOffset() const { return child_region.point - point_zero; }
	Rect GetChildRegion() const { return child_region; }
protected:
	virtual Transform GetChildTransform(WndObject& child) const override { return GetChildOffset(); }

	// paint
protected:
	virtual void OnChildRedraw(WndObject& child, Rect child_redraw_region) override { Redraw(child_redraw_region + GetChildOffset()); }
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override { return DrawChild(child, point_zero + GetChildOffset(), figure_queue, draw_region); }

	// message
protected:
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override { msg.point -= GetChildOffset(); return WndFrame::HitTest(msg); }
};


template<>
class ClipFrame<Assigned, Assigned, TopLeft> : public _ClipFrame_Base, public LayoutType<Assigned, Assigned> {
public:
	ClipFrame(child_ptr<> child) : _ClipFrame_Base(std::move(child)) { child_region.point = point_zero; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		child_region.size = child_size;
		Redraw(region_infinite);
	}
};

template<>
class ClipFrame<Assigned, Assigned, TopRight> : public _ClipFrame_Base, public LayoutType<Assigned, Assigned> {
public:
	ClipFrame(child_ptr<> child) : _ClipFrame_Base(std::move(child)) { child_region.point.y = 0; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point.x = size.width - child_region.size.width;
		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		child_region.size = child_size;
		child_region.point.x = size.width - child_region.size.width;
		Redraw(region_infinite);
	}
};

template<>
class ClipFrame<Assigned, Assigned, BottomLeft> : public _ClipFrame_Base, public LayoutType<Assigned, Assigned> {
public:
	ClipFrame(child_ptr<> child) : _ClipFrame_Base(std::move(child)) { child_region.point.x = 0; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point.y = size.height - child_region.size.height;
		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		child_region.size = child_size;
		child_region.point.y = size.height - child_region.size.height;
		Redraw(region_infinite);
	}
};

template<>
class ClipFrame<Assigned, Assigned, BottomRight> : public _ClipFrame_Base, public LayoutType<Assigned, Assigned> {
public:
	ClipFrame(child_ptr<> child) : _ClipFrame_Base(std::move(child)) {}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point = Point(size.width - child_region.size.width, size.height - child_region.size.height);
		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		child_region.size = child_size;
		child_region.point = Point(size.width - child_region.size.width, size.height - child_region.size.height);
		Redraw(region_infinite);
	}
};


template<>
class ClipFrame<Assigned, Auto, Left> : public _ClipFrame_Base, public LayoutType<Assigned, Auto> {
public:
	ClipFrame(child_ptr<Relative, Auto> child) : _ClipFrame_Base(std::move(child)) { child_region.point = point_zero; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		size.height = child_region.size.height;
		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		child_region.size = child_size;
		if (size.height != child_size.height) {
			size.height = child_size.height;
			SizeUpdated(size);
		} else {
			Redraw(region_infinite);
		}
	}
};

template<>
class ClipFrame<Assigned, Auto, Right> : public _ClipFrame_Base, public LayoutType<Assigned, Auto> {
public:
	ClipFrame(child_ptr<Relative, Auto> child) : _ClipFrame_Base(std::move(child)) { child_region.point.y = 0; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point.x = size.width - child_region.size.width;
		size.height = child_region.size.height;
		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
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


template<>
class ClipFrame<Auto, Assigned, Top> : public _ClipFrame_Base, public LayoutType<Auto, Assigned> {
public:
	ClipFrame(child_ptr<Auto, Relative> child) : _ClipFrame_Base(std::move(child)) {}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		size.width = child_region.size.width;
		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
		child_region.size = child_size;
		if (size.width != child_size.width) {
			size.width = child_size.width;
			SizeUpdated(size);
		} else {
			Redraw(region_infinite);
		}
	}
};

template<>
class ClipFrame<Auto, Assigned, Bottom> : public _ClipFrame_Base, public LayoutType<Auto, Assigned> {
public:
	ClipFrame(child_ptr<Auto, Relative> child) : _ClipFrame_Base(std::move(child)) { child_region.point.x = 0; }
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		child_region.size = UpdateChildSizeRef(child, size = size_ref);
		child_region.point.y = size.height - child_region.size.height;
		size.width = child_region.size.width;
		return size;
	}
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override {
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
