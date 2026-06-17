#pragma once

#include "ViewDesign/view/Window.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>


namespace ViewDesign {

class Window;


class Desktop : public ViewBase {
private:
	friend class ViewBase;
	friend class Window;
	friend struct DesktopPrivateAccess;

private:
	Desktop();
	~Desktop();

public:
	static Desktop& Get();

	// window
private:
	std::vector<std::unique_ptr<Window>> window_list;
private:
	Window& AsWindow(ViewBase& child) const { return static_cast<Window&>(child); }
public:
	Window& GetWindow(ViewBase& view) { return AsWindow(GetDirectChild(view)); }
public:
	Window& AddWindow(std::unique_ptr<Window> window);
	std::unique_ptr<Window> RemoveWindow(Window& window);
public:
	bool WindowListEmpty() { return window_list.empty(); }
	void CloseAllWindows();

	// layout
public:
	SizeU GetSize() const;
private:
	std::pair<SizeU, RectI> GetWindowMinMaxRegion(Window& window) {
		VerifyChild(window);
		auto [size_min, size_max] = window.CalculateMinMaxSize(GetSize() / window.scale);
		return { Round(size_min * window.scale), RectI(point_i_zero, Round(size_max * window.scale)) };
	}
	void UpdateWindowSizeRef(Window& window) {
		VerifyChild(window);
		window.RegionUpdated(window.OnWindowSizeRefUpdate(GetSize() / window.scale));
	}
private:
	virtual Point ConvertChildPoint(ViewBase& child, Point point) const { return point + (AsWindow(child).point - point_zero); }
	virtual Point ConvertChildPoint(Point point, ViewBase& child) const { return point - (AsWindow(child).point - point_zero); }

	// context
private:
	void ReleaseView(ViewBase& view);
	void ReleaseWindow(Window& window);

	// mouse event
private:
	std::vector<ref_ptr<ViewBase>> view_track_stack;
	std::unordered_map<ref_ptr<ViewBase>, size_t> view_track_map;
	ref_ptr<Window> window_capture = nullptr;
	ref_ptr<ViewBase> view_capture = nullptr;
private:
	void SetWindowCapture(Window& window);
	void ReleaseWindowCapture(Window& window);
private:
	void SetTrack(ViewBase& view);
	void PopTrack(size_t index);
	void PushTrack(std::vector<ref_ptr<ViewBase>> trace);
	void LoseTrack() { PopTrack(0); }
private:
	void SetCapture(ViewBase& view);
	void ReleaseCapture(ViewBase& view);
	void LoseCapture();
private:
	void DispatchMouseEvent(Window& window, MouseEvent event);

	// key event
private:
	std::vector<ref_ptr<ViewBase>> view_focus_stack;
	std::unordered_map<ref_ptr<ViewBase>, size_t> view_focus_map;
	ref_ptr<Window> window_focus = nullptr;
private:
	void SetWindowFocus(Window& window_focus);
private:
	void SetFocus(ViewBase& view);
	void ReleaseFocus(ViewBase& view);
	void LoseFocus();
private:
	void DispatchKeyEvent(KeyEvent event);

	// ime event
private:
	std::unordered_set<ref_ptr<ViewBase>> ime_enabled_view;
private:
	void ImeWindowEnable();
	void ImeWindowDisable();
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
	bool WindowListEmpty() { return Get().WindowListEmpty(); }
	void CloseAllWindows() { return Get().CloseAllWindows(); }
public:
	Size GetSize() const { return Get().GetSize(); }
public:
	void EventLoop() { return Get().EventLoop(); }
	void PollEvents() { return Get().PollEvents(); }
} desktop;


} // namespace ViewDesign
