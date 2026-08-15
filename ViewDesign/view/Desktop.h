#pragma once

#include "ViewDesign/view/Window.h"

#include <unordered_map>
#include <unordered_set>


namespace ViewDesign {


class Desktop : public ViewBase {
private:
	friend class ViewBase;
	friend class Window;
	friend struct DesktopPrivateAccess;

private:
	Desktop() {}
	~Desktop() {}
public:
	static Desktop& Get();

	// tree
private:
	void OnViewDetach(ViewBase& view);

	// window
private:
	std::vector<std::unique_ptr<Window>> window_list;
public:
	Window& AddWindow(std::unique_ptr<Window> window);
	std::unique_ptr<Window> RemoveWindow(Window& window);
public:
	bool WindowListEmpty() const { return window_list.empty(); }
	void CloseAllWindows();
private:
	static Window& AsWindow(ViewBase& child) { return static_cast<Window&>(child); }
public:
	Window& GetWindow(ViewBase& view) { return AsWindow(GetDirectChild(view)); }

	// layout
public:
	SizeU GetPixelSize() const;
private:
	std::pair<SizeU, RectI> GetWindowMinMaxRegion(Window& window);
	void UpdateWindowSizeRef(Window& window);
private:
	virtual Point ConvertChildPoint(ViewBase& child, Point point) const override { return point + (AsWindow(child).GetPoint() - point_zero); }
	virtual Point ConvertChildPoint(Point point, ViewBase& child) const override { return point - (AsWindow(child).GetPoint() - point_zero); }

	// mouse event
private:
	std::vector<ref_ptr<ViewBase>> view_track_stack;
	std::unordered_map<ref_ptr<ViewBase>, size_t> view_track_map;
private:
	void PushTrack(size_t index, std::vector<ref_ptr<ViewBase>> trace);
private:
	void LoseWindowTrack() { PushTrack(0, {}); }
private:
	void SetTrack(ViewBase& view);
private:
	ref_ptr<Window> window_capture = nullptr;
	ref_ptr<ViewBase> view_capture = nullptr;
private:
	void SetWindowCapture(Window& window);
	void ReleaseWindowCapture(Window& window);
	void LoseWindowCapture() { window_capture = nullptr; view_capture = nullptr; }
private:
	void SetCapture(ViewBase& view);
	void ReleaseCapture(ViewBase& view);
private:
	void DispatchMouseEvent(Window& window, MouseEvent event);

	// key event
private:
	std::vector<ref_ptr<ViewBase>> view_focus_stack;
	std::unordered_map<ref_ptr<ViewBase>, size_t> view_focus_map;
private:
	void PushFocus(size_t index, std::vector<ref_ptr<ViewBase>> trace);
private:
	void SetWindowFocus(Window& window);
	void LoseWindowFocus() { PushFocus(0, {}); }
private:
	void SetFocus(ViewBase& view);
private:
	void DispatchKeyEvent(KeyEvent event);

	// ime event
private:
	std::unordered_set<ref_ptr<ViewBase>> ime_enabled_view;
private:
	void ImeWindowEnable(Window& window);
	void ImeWindowDisable(Window& window);
	void ImeWindowSetPosition(Window& window, Point point);
private:
	void ImeEnable(ViewBase& view) { ime_enabled_view.emplace(&view); }
	void ImeDisable(ViewBase& view) { ime_enabled_view.erase(&view); }
	void ImeSetPosition(ViewBase& view, Point point);

	// event
public:
	void EventLoop();
	void PollEvents();
};


inline struct DesktopAPI {
public:
	Desktop& Get() { return Desktop::Get(); }
	const Desktop& Get() const { return Desktop::Get(); }
public:
	Window& GetWindow(ViewBase& view) { return Get().GetWindow(view); }
public:
	Window& AddWindow(std::unique_ptr<Window> window) { return Get().AddWindow(std::move(window)); }
	Window& AddWindow(owner_ptr<Window> window) { return AddWindow(std::unique_ptr<Window>(window)); }
	std::unique_ptr<Window> RemoveWindow(Window& window) { return Get().RemoveWindow(window); }
public:
	Point ConvertDescendentPoint(ViewBase& descendent, Point point) { return Get().ConvertDescendentPoint(descendent, point); }
	Point ConvertDescendentPoint(Point point, ViewBase& descendent) { return Get().ConvertDescendentPoint(point, descendent); }
public:
	bool WindowListEmpty() { return Get().WindowListEmpty(); }
	void CloseAllWindows() { return Get().CloseAllWindows(); }
public:
	Size GetPixelSize() const { return Get().GetPixelSize(); }
public:
	void EventLoop() { return Get().EventLoop(); }
	void PollEvents() { return Get().PollEvents(); }
} desktop;


} // namespace ViewDesign
