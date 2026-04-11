#pragma once

#include "../common/uncopyable.h"
#include "../style/cursor_style.h"
#include "../geometry/geometry.h"
#include "../figure/figure_queue.h"
#include "../event/event.h"


namespace ViewDesign {


class ViewBase : Uncopyable {
private:
	friend class Desktop;
	friend struct ViewBasePrivateAccess;

protected:
	ViewBase() {}
public:
	virtual ~ViewBase();
protected:
	ViewBase& AsViewBase() { return *this; }

	// style
protected:
	Cursor cursor = Cursor::Arrow;

	// parent
private:
	ref_ptr<ViewBase> parent = nullptr;
protected:
	bool HasParent() const { return parent != nullptr; }
	ViewBase& GetParent() const { if (!HasParent()) { throw std::invalid_argument("window has no parent"); } return *parent; }

	// child
protected:
	void VerifyChild(ViewBase& child) const {
		if (child.parent != this) { throw std::invalid_argument("invalid child window"); }
	}
	void RegisterChild(ViewBase& child) {
		if (child.HasParent()) { throw std::invalid_argument("window already has a parent"); } child.parent = this;
	}
	void UnregisterChild(ViewBase& child) {
		VerifyChild(child); child.parent = nullptr;
	}
protected:
	ViewBase& GetDirectChild(ViewBase& descendent) const;

	// parent data
private:
	uint64 parent_data = 0;
protected:
	template<class T, class = std::enable_if_t<sizeof(T) <= sizeof(uint64)>>
	void SetChildData(ViewBase& child, T data) {
		VerifyChild(child); memcpy(&child.parent_data, &data, sizeof(T));
	}
	template<class T, class = std::enable_if_t<sizeof(T) <= sizeof(uint64)>>
	T GetChildData(ViewBase& child) const {
		VerifyChild(child); T data; memcpy(&data, &child.parent_data, sizeof(T)); return data;
	}

	// layout
protected:
	void SizeUpdated(Size size) { if (HasParent()) { GetParent().OnChildSizeUpdate(*this, size); } }
	Size UpdateChildSizeRef(ViewBase& child, Size size_ref) { VerifyChild(child); return child.OnSizeRefUpdate(size_ref); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const { return Transform::Identity(); }
protected:
	Transform GetDescendentTransform(ViewBase& descendent) const;
	Point ConvertDescendentPoint(ViewBase& descendent, Point point) const;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) { return size_ref; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) {}

	// paint
protected:
	void Redraw(Rect redraw_region) { if (HasParent() && !redraw_region.IsEmpty()) { GetParent().OnChildRedraw(*this, redraw_region); } }
	void DrawChild(ViewBase& child, Point child_offset, FigureQueue& figure_queue, Rect draw_region);
	void DrawChild(ViewBase& child, Rect child_region, FigureQueue& figure_queue, Rect draw_region);
protected:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) {}
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) {}

	// event
protected:
	void SetCapture();
	void ReleaseCapture();
	void SetFocus();
	void ReleaseFocus();
protected:
	ref_ptr<ViewBase> HitTestChild(ViewBase& child, MouseEvent& event) { return child.HitTest(event); }
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) { return this; }
protected:
	virtual void OnMouseEvent(MouseEvent event) {}
	virtual void OnKeyEvent(KeyEvent event) {}
	virtual void OnFocusEvent(FocusEvent event) {}
};


} // namespace ViewDesign
