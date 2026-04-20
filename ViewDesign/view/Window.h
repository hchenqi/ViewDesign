#pragma once

#include "ViewDesign/view/ViewFrame.h"
#include "ViewDesign/geometry/scale.h"
#include "ViewDesign/geometry/region.h"
#include "ViewDesign/drawing/window_layer.h"


namespace ViewDesign {


class Window : public ViewFrame {
private:
	friend class Desktop;
	friend struct WindowApi;

public:
	Window(std::wstring title, view_ptr<> child);
	~Window();

	// style
public:
	void SetTitle(std::wstring title);

	// layout
private:
	Scale scale;
	Rect region;
private:
	Rect GetRegion() { return region; }
	void SetScale(float value) { scale = Scale(value); }
	void SetSize(Size size);
	void SetPoint(Point point) { region.point = point; }
	std::pair<Size, Rect> GetMinMaxRegion(Size size_ref);
	void InitializeRegion(Size size_ref);
protected:
	void WindowRegionUpdated(Rect region);
private:
	virtual Transform GetChildTransform(ViewBase& child) const override final { return scale; }
protected:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) { return { size_empty, size_ref }; }
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) { UpdateChildSizeRef(child, size_ref); return Rect(point_zero, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {}
private:
	void SizeUpdated() {}  // never used
	virtual Size OnSizeRefUpdate(Size size_ref) override final { return size_ref; }  // never used

	// state
protected:
	enum class State { Normal, Minimized, Maximized };
private:
	State state = State::Normal;
private:
	void SetState(State state) { if (this->state != state) { this->state = state; OnStateChange(state); } }
protected:
	State GetState() { return state; }
protected:
	virtual void OnStateChange(State state) {}
protected:
	void Show();
	void Hide();
	void SetForeground();
	void Minimize();
	void Maximize();
	void Restore();
protected:
	void Destroy();

	// drawing
public:
	using HANDLE = void*;
private:
	HANDLE hwnd = nullptr;
	WindowLayer layer;
	Region invalid_region;
public:
	HANDLE GetHWND() const { return hwnd; }
private:
	void ResizeLayer();
	void RecreateLayer();
protected:
	void Redraw(Rect redraw_region);
	ViewFrame::OnDraw;
private:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override final { Redraw(child_redraw_region); }
	void OnDraw();

	// event
public:
	Point GetDesktopCursorPosition() const;
private:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override final { event.point *= scale.Invert(); return ViewFrame::HitTest(event); }
};


} // namespace ViewDesign
