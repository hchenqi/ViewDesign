#pragma once

#include "ViewDesign/view/Window.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>


namespace ViewDesign {

class Window;


class Desktop : public ViewBase {
private:
	friend class ViewBase;
	friend class Window;
	friend struct DesktopApi;

private:
	Desktop();
	~Desktop();

public:
	static Desktop& Get();

	// window
private:
	std::vector<std::unique_ptr<Window>> window_list;
public:
	Window& AddWindow(std::unique_ptr<Window> window);
	Window& AddWindow(owner_ptr<Window> window) { return AddWindow(std::unique_ptr<Window>(window)); }
	std::unique_ptr<Window> RemoveWindow(Window& window);
public:
	Window& GetWindow(ViewBase& view);
	Window& GetWindowPoint(ViewBase& view, Point& point);

	// layout
public:
	Size GetSize() const;

	// drawing
private:
	void RecreateWindowLayer();

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
	void LoseTrack();
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
	void SetWindowFocus(ref_ptr<Window> window_focus);
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
private:
	void ReleaseView(ViewBase& view);

	// system
public:
	void EventLoop();
	void Terminate();
};

extern Desktop& desktop;


} // namespace ViewDesign
