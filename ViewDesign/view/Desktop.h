#pragma once

#include "ViewBase.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>


namespace ViewDesign {

class DesktopFrame;


class Desktop : public ViewBase {
private:
	friend class DesktopFrame;
	friend class ViewBase;
	friend struct Ime;
	friend struct DesktopApi;
	friend struct Global;

private:
	Desktop();
	~Desktop();

public:
	static Desktop& Get();

	// frame
private:
	std::vector<std::unique_ptr<DesktopFrame>> frame_list;
private:
	DesktopFrame& AddChild(std::unique_ptr<DesktopFrame> frame);
	std::unique_ptr<DesktopFrame> RemoveChild(DesktopFrame& frame);
private:
	DesktopFrame& GetDesktopFrame(ViewBase& view);
	DesktopFrame& GetDesktopFramePoint(ViewBase& view, Point& point);

	// paint
private:
	void RecreateFrameLayer();
private:
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override;

	// mouse event
private:
	std::vector<ref_ptr<ViewBase>> view_track_stack;
	std::unordered_map<ref_ptr<ViewBase>, size_t> view_track_map;
	ref_ptr<DesktopFrame> frame_capture = nullptr;
	ref_ptr<ViewBase> view_capture = nullptr;
private:
	void SetTrack(ViewBase& view);
	void LoseTrack();
	void SetCapture(ViewBase& view);
	void ReleaseCapture(ViewBase& view);
	void LoseCapture();
private:
	void DispatchMouseEvent(DesktopFrame& frame, MouseEvent event);

	// key event
private:
	std::vector<ref_ptr<ViewBase>> view_focus_stack;
	std::unordered_map<ref_ptr<ViewBase>, size_t> view_focus_map;
	ref_ptr<DesktopFrame> frame_focus = nullptr;
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
	void ImeEnable(ViewBase& view) { ime_enabled_view.emplace(&view); }
	void ImeDisable(ViewBase& view) { ime_enabled_view.erase(&view); }
	void ImeSetPosition(ViewBase& view, Point point);

	// event
private:
	void ReleaseWindow(ViewBase& view);

	// global
private:
	void EventLoop();
	void Terminate();
};

extern Desktop& desktop;


} // namespace ViewDesign
