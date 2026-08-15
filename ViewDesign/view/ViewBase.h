#pragma once

#include "ViewDesign/common/uncopyable.h"
#include "ViewDesign/system/cursor.h"
#include "ViewDesign/drawing/canvas.h"
#include "ViewDesign/event/event.h"


namespace ViewDesign {


class ViewBase : Uncopyable {
private:
	friend class Desktop;

protected:
	ViewBase() {}
public:
	virtual ~ViewBase();
public:
	ViewBase& AsViewBase() { return *this; }
	const ViewBase& AsViewBase() const { return *this; }

	// style
protected:
	std::reference_wrapper<Cursor> cursor = GetCursor(CursorStyle::Arrow);

	// parent
private:
	ref_ptr<ViewBase> parent = nullptr;
public:
	bool HasParent() const { return parent != nullptr; }
	ViewBase& GetParent() const { if (!HasParent()) { throw std::logic_error("view has no parent"); } return *parent; }

	// child
protected:
	void VerifyChild(ViewBase& child) const { if (child.parent != this) { throw std::invalid_argument("invalid child view"); } }
	void RegisterChild(ViewBase& child);
	void UnregisterChild(ViewBase& child);
public:
	ViewBase& GetDirectChild(ViewBase& descendent) const;

	// layout
protected:
	Size UpdateChildSizeRef(ViewBase& child, Size size_ref) { VerifyChild(child); return child.OnSizeRefUpdate(size_ref); }
	void SizeUpdated(Size size) { if (HasParent()) { GetParent().OnChildSizeUpdate(*this, size); } }
protected:
	virtual Point ConvertChildPoint(ViewBase& child, Point point) const { return point; }
	virtual Point ConvertChildPoint(Point point, ViewBase& child) const { return point; }
public:
	Point ConvertDescendentPoint(ViewBase& descendent, Point point) const;
	Point ConvertDescendentPoint(Point point, ViewBase& descendent) const;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) { return size_ref; }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) {}

	// drawing
protected:
	void DrawChild(ViewBase& child, Point child_offset, Canvas& canvas, Rect draw_region);
	void DrawChild(ViewBase& child, Rect child_region, Canvas& canvas, Rect draw_region);
	void Redraw(Rect redraw_region) { if (HasParent() && !redraw_region.IsEmpty()) { GetParent().OnChildRedraw(*this, redraw_region); } }
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) {}
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) {}

	// event
public:
	void SetCapture();
	void ReleaseCapture();
	void SetFocus();
protected:
	ref_ptr<ViewBase> HitTestChild(ViewBase& child, MouseEvent& event) { return child.HitTest(event); }
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) { return this; }
protected:
	virtual void OnMouseEvent(MouseEvent event) {}
	virtual void OnKeyEvent(KeyEvent event) {}
	virtual void OnFocusEvent(FocusEvent event) {}
};


template<class View>
concept view_type = std::derived_from<View, ViewBase>;


class view_ref_any : public std::reference_wrapper<ViewBase> {
public:
	template<view_type View>
	view_ref_any(View& ref) : std::reference_wrapper<ViewBase>(ref) {}
public:
	operator ViewBase& () const { return get(); }
	operator ref_ptr<ViewBase>() const { return &get(); }
	ref_ptr<ViewBase> operator->() const { return &get(); }
};

class view_ptr_any : public std::unique_ptr<ViewBase> {
public:
	view_ptr_any() : std::unique_ptr<ViewBase>(nullptr) {}
	template<view_type View>
	view_ptr_any(owner_ptr<View> ptr) : std::unique_ptr<ViewBase>(ptr) {}
	template<view_type View>
	view_ptr_any(std::unique_ptr<View> ptr) : std::unique_ptr<ViewBase>(std::move(ptr)) {}
public:
	operator ViewBase& () const { return *get(); }
	operator ref_ptr<ViewBase>() const { return get(); }
	ref_ptr<ViewBase> operator->() const { return get(); }
};


template<class T>
concept unique_ptr_type = std::derived_from<T, std::unique_ptr<typename T::element_type, typename T::deleter_type>>;

template<class T>
auto create(auto&&... args) { return std::make_unique<T>(std::forward<decltype(args)>(args)...); }


} // namespace ViewDesign
