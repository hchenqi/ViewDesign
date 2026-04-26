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
	Window(const u16string& title, view_ptr<> child);
	~Window();

public:
	using Handle = void*;
private:
	Handle handle;
public:
	Handle GetPlatformHandle() const { return handle; }

	// style
public:
	void SetTitle(const u16string& title);

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
	void Minimize();
	void Maximize();
	void Restore();
	void Close();

	// drawing
private:
	WindowLayer layer;
	Region invalid_region;
private:
	void ResizeLayer();
	void RecreateLayer();
protected:
	void Redraw(Rect redraw_region);
private:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override final { Redraw(child_redraw_region); }
	void OnDraw();

	// event
public:
	Point GetCursorPosition() const;
private:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override final { event.point *= scale.Invert(); return ViewFrame::HitTest(event); }
};


} // namespace ViewDesign
