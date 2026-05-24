#pragma once

#include "ViewDesign/view/ViewBase.h"
#include "ViewDesign/geometry/scale.h"
#include "ViewDesign/drawing/surface.h"

#include <memory>


namespace ViewDesign {


class Window : public ViewBase {
private:
	friend class Desktop;
	friend struct WindowPrivateAccess;

public:
	Window(Handle window, std::unique_ptr<ViewBase> child);
	Window(Handle window, owner_ptr<ViewBase> child) : Window(window, std::unique_ptr<ViewBase>(child)) {}
	Window(const u16string& title, std::unique_ptr<ViewBase> child);
	Window(const u16string& title, owner_ptr<ViewBase> child) : Window(title, std::unique_ptr<ViewBase>(child)) {}
	virtual ~Window() override;

	// surface
private:
	Surface surface;
public:
	Handle GetHandle() const { return surface.GetWindow(); }

	// style
public:
	void SetTitle(const u16string& title);

	// child
protected:
	std::unique_ptr<ViewBase> child;

	// layout
private:
	PointI point;
	Scale scale;
protected:
	SizeU GetSize() const { return surface.GetSize(); }
	RectI GetRegion() const { return RectI(point, GetSize()); }
	Scale GetScale() const { return scale; }
private:
	void SetSize(SizeU size) { if (GetSize() != size) { surface.Resize(size); UpdateChildSizeRef(*child, size / scale); Redraw(rect_infinite); } }
	void SetPoint(PointI point) { this->point = point; }
	void SetScale(Scale scale) { this->scale = Scale(scale); }
protected:
	void RegionUpdated(Rect region);
private:
	virtual Transform GetChildTransform(ViewBase& child) const override { return scale; }
protected:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) { return { size_empty, size_ref }; }
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) { return Rect(point_zero, UpdateChildSizeRef(*child, size_ref)); }
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
protected:
	void Redraw(Rect redraw_region);
private:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	void OnDraw();

	// event
private:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { event.point /= scale; return HitTestChild(*child, event); }
};


} // namespace ViewDesign
