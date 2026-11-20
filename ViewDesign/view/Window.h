#pragma once

#include "ViewDesign/view/ViewBase.h"
#include "ViewDesign/drawing/surface.h"


namespace ViewDesign {


class Window : public ViewBase {
private:
	friend class Desktop;
	friend struct WindowPrivateAccess;

public:
	Window(Handle window, view_ptr_any child);
	Window(const u16string& title, view_ptr_any child);
	virtual ~Window() override;

	// style
public:
	void SetTitle(const u16string& title);
	void SetIcon(const void* buffer, size_t size);
	void ClearIcon();

	// surface
private:
	Surface surface;
public:
	Handle GetHandle() const { return surface.GetWindow(); }

	// metrics
private:
	PointI point;
	Scale scale;
protected:
	PointI GetPixelPoint() const { return point; }
	SizeU GetPixelSize() const { return surface.GetSize(); }
	RectI GetPixelRegion() const { return RectI(GetPixelPoint(), GetPixelSize()); }
	Scale GetScale() const { return scale; }
private:
	void SetPixelPoint(PointI point) { this->point = point; }
	void SetPixelSize(SizeU size) { if (GetPixelSize() != size) { surface.Resize(size); UpdateChildSizeRef(child, size / scale); Redraw(rect_infinite); } }
	void SetScale(Scale scale) { this->scale = scale; }

	// state
public:
	enum class State { Normal, Minimized, Maximized };
private:
	State state = State::Normal;
private:
	void SetState(State state) { if (this->state != state) { this->state = state; OnStateChange(state); } }
public:
	State GetState() { return state; }
protected:
	virtual void OnStateChange(State state) {}
public:
	void Show();
	void Hide();
	void Minimize();
	void Maximize();
	void Restore();
	void Close();

	// child
protected:
	view_ptr_any child;

	// layout
protected:
	Point GetPoint() const { return GetPixelPoint() / GetScale(); }
protected:
	void RegionUpdated(Rect region);
protected:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) { return { size_empty, size_ref }; }
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) { return Rect(point_zero, UpdateChildSizeRef(child, size_ref)); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {}

	// drawing
protected:
	void Draw();
	void Redraw(Rect redraw_region);
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { DrawChild(child, point_zero, canvas, draw_region); }
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return HitTestChild(child, event); }
};


} // namespace ViewDesign
