#pragma once

#include "../ViewFrame.h"
#include "../../common/reversion_wrapper.h"

#include <list>


namespace ViewDesign {


class OverlapLayout : public ViewType<Fixed, Fixed> {
public:
	class Window : public ViewFrame {
	private:
		friend class OverlapLayout;
	public:
		Window(view_ptr<> child) : ViewFrame(std::move(child)) {}
		~Window() {}
	protected:
		OverlapLayout& GetParent() const { return static_cast<OverlapLayout&>(ViewBase::GetParent()); }
	private:
		Rect region;
	public:
		void BringForward() { GetParent().BringForward(*this); }
		void BringToFront() { GetParent().BringToFront(*this); }
		void SendBackward() { GetParent().SendBackward(*this); }
		void SendToBack() { GetParent().SendToBack(*this); }
	private:
		ViewFrame::SizeUpdated;
		ViewFrame::OnSizeRefUpdate;
	protected:
		void RegionUpdated(Rect region) { if (HasParent()) { GetParent().OnWindowRegionUpdate(*this, region); } }
	protected:
		virtual Rect OnWindowSizeRefUpdate(Size size_ref) { UpdateChildSizeRef(child, size_ref); return Rect(point_zero, size_ref); }
		virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override {}
	protected:
		void Redraw(Rect redraw_region) { if (HasParent()) { GetParent().OnWindowRedraw(*this, redraw_region); } }
	protected:
		virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	};

public:
	using window_ptr = std::unique_ptr<Window>;

public:
	OverlapLayout() {}
	~OverlapLayout() {}

	// window
protected:
	std::list<window_ptr> window_list;
protected:
	static Window& AsWindow(ViewBase& window) { return static_cast<Window&>(window); }
public:
	void AddWindow(window_ptr window) {
		RegisterChild(*window);
		UpdateWindowSizeRef(*window, size);
		Redraw(window_list.emplace_back(std::move(window))->region);
	}
	void AddWindow(owner_ptr<Window> window) { AddWindow(window_ptr(window)); }
	window_ptr RemoveWindow(Window& window) {
		auto it = std::find_if(window_list.begin(), window_list.end(), [&](const window_ptr& ptr) { return ptr.get() == &window; });
		if (it == window_list.end()) { throw std::invalid_argument("invalid window"); }
		window_ptr ptr = std::move(*it); window_list.erase(it);
		Redraw(ptr->region);
		UnregisterChild(*ptr);
		return ptr;
	}
public:
	void BringForward(Window& window) {
		auto it = std::find_if(window_list.begin(), window_list.end(), [&](const window_ptr& ptr) { return ptr.get() == &window; });
		if (it == window_list.end()) { throw std::invalid_argument("invalid window"); }
		auto next = std::next(it);
		if (next == window_list.end()) { return; }
		std::swap(*it, *next);
		Redraw((*it)->region.Intersect((*next)->region));
	}
	void BringToFront(Window& window) {
		auto it = std::find_if(window_list.begin(), window_list.end(), [&](const window_ptr& ptr) { return ptr.get() == &window; });
		if (it == window_list.end()) { throw std::invalid_argument("invalid window"); }
		if (std::next(it) == window_list.end()) { return; }
		window_list.splice(window_list.end(), window_list, it);
		Redraw((*it)->region);
	}
	void SendBackward(Window& window) {
		auto it = std::find_if(window_list.begin(), window_list.end(), [&](const window_ptr& ptr) { return ptr.get() == &window; });
		if (it == window_list.end()) { throw std::invalid_argument("invalid window"); }
		if (it == window_list.begin()) { return; }
		auto prev = std::prev(it);
		std::swap(*it, *prev);
		Redraw((*it)->region.Intersect((*prev)->region));
	}
	void SendToBack(Window& window) {
		auto it = std::find_if(window_list.begin(), window_list.end(), [&](const window_ptr& ptr) { return ptr.get() == &window; });
		if (it == window_list.end()) { throw std::invalid_argument("invalid window"); }
		if (it == window_list.begin()) { return; }
		window_list.splice(window_list.begin(), window_list, it);
		Redraw((*it)->region);
	}

	// layout
protected:
	Size size = size_empty;
private:
	ViewBase::UpdateChildSizeRef;
	ViewBase::OnChildSizeUpdate;
protected:
	void UpdateWindowSizeRef(Window& window, Size size_ref) { VerifyChild(window); window.region = window.OnWindowSizeRefUpdate(size_ref); }
protected:
	virtual Transform GetChildTransform(ViewBase& child) const override {
		return AsWindow(child).region.point - point_zero;
	}
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		size = size_ref;
		for (auto& window : window_list) {
			UpdateWindowSizeRef(*window, size);
		}
		return size;
	}
	virtual void OnWindowRegionUpdate(Window& window, Rect region) {
		Rect redraw_region = window.region.Union(region);
		window.region = region;
		Redraw(redraw_region);
	}

	// drawing
private:
	ViewBase::OnChildRedraw;
protected:
	virtual void OnWindowRedraw(Window& window, Rect redraw_region) {
		Redraw(redraw_region + (window.region.point - point_zero));
	}
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		for (auto& window : window_list) {
			DrawChild(*window, window->region, canvas, draw_region);
		}
	}

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		MouseEvent event_copy = event;
		for (auto& window : reverse(window_list)) {
			if (window->region.Contains(event.point)) {
				event.point -= window->region.point - point_zero;
				if (ref_ptr<ViewBase> res = HitTestChild(*window.get(), event)) {
					return res;
				}
				event = event_copy;
			}
		}
		return this;
	}
};


} // namespace ViewDesign
