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
	Handle GetHandle() const { return handle; }

	// style
public:
	void SetTitle(const u16string& title);

	// layout
private:
	Scale scale;
	Rect region;
protected:
	Scale GetScale() const { return scale; }
	Rect GetRegion() const { return region; }
private:
	std::pair<Size, Rect> GetMinMaxRegion(Size size_ref);
	void InitializeRegion(Size size_ref);
	void SetSize(Size size);
	void SetPoint(Point point) { region.point = point; }
	void SetScale(float value) { scale = Scale(value); }
protected:
	void WindowRegionUpdated(Rect region);
private:
	virtual Transform GetChildTransform(ViewBase& child) const override { return scale; }
protected:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) { return { size_empty, size_ref }; }
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) { UpdateChildSizeRef(child, size_ref); return Rect(point_zero, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {}

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
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	void OnDraw();

	// event
private:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point *= scale.Invert(); return ViewFrame::HitTest(event); }
};


} // namespace ViewDesign
