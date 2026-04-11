#include "desktop.h"
#include "../frame/DesktopFrame.h"
#include "../system/win32_api.h"
#include "../system/win32_ime.h"
#include "../system/cursor.h"
#include "../common/reversion_wrapper.h"


namespace ViewDesign {

Desktop& desktop = Desktop::Get();


Desktop::Desktop() {}

Desktop::~Desktop() {}

Desktop& Desktop::Get() {
	static Desktop desktop;
	return desktop;
}

DesktopFrame& Desktop::AddChild(std::unique_ptr<DesktopFrame> frame) {
	RegisterChild(*frame);
	frame->InitializeRegion(Win32::GetDesktopSize());
	frame->Show();
	return *frame_list.emplace_back(std::move(frame));
}

std::unique_ptr<DesktopFrame> Desktop::RemoveChild(DesktopFrame& frame) {
	auto it = std::find_if(frame_list.begin(), frame_list.end(), [&](const std::unique_ptr<DesktopFrame>& ptr) { return ptr.get() == &frame; });
	if (it == frame_list.end()) { throw std::invalid_argument("invalid desktop frame"); }
	frame.Hide();
	UnregisterChild(frame);
	std::unique_ptr<DesktopFrame> ptr = std::move(*it);	frame_list.erase(it);
	if (frame_list.empty()) { Terminate(); }
	return ptr;
}

DesktopFrame& Desktop::GetDesktopFrame(WndObject& wnd) {
	ref_ptr<WndObject> child = &wnd;
	for (ref_ptr<WndObject> parent = child->parent; parent != &desktop; child = parent, parent = child->parent) {
		if (parent == nullptr) { throw std::invalid_argument("window not registered"); }
	}
	return static_cast<DesktopFrame&>(*child);
}

DesktopFrame& Desktop::GetDesktopFramePoint(WndObject& wnd, Point& point) {
	ref_ptr<WndObject> child = &wnd;
	for (ref_ptr<WndObject> parent = child->parent; parent != &desktop; child = parent, parent = child->parent) {
		if (parent == nullptr) { throw std::invalid_argument("window not registered"); }
		point *= parent->GetChildTransform(*child);
	}
	return static_cast<DesktopFrame&>(*child);
}

void Desktop::RecreateFrameLayer() {
	for (auto& frame : frame_list) {
		frame->RecreateLayer();
	}
}

void Desktop::OnChildRedraw(WndObject& child, Rect child_redraw_region) {
	static_cast<DesktopFrame&>(child).Redraw(child_redraw_region);
}

void Desktop::SetTrack(WndObject& wnd) {
	if (!wnd_track_stack.empty() && wnd_track_stack.back() == &wnd) { return; }
	std::vector<ref_ptr<WndObject>> trace;
	for (ref_ptr<WndObject> curr = &wnd;;) {
		trace.push_back(curr);
		curr = curr->parent;
		if (curr == nullptr || curr == &desktop) {
			LoseTrack();
			break;
		}
		if (wnd_track_map.contains(curr)) {
			wnd_track_stack.back()->OnFocusEvent(FocusEvent::MouseOut);
			for (size_t index = wnd_track_map[curr]; wnd_track_stack.size() > index; wnd_track_stack.pop_back()) {
				wnd_track_stack.back()->OnFocusEvent(FocusEvent::MouseLeave);
				wnd_track_map.erase(wnd_track_stack.back());
			}
			break;
		}
	}
	for (; !trace.empty(); trace.pop_back()) {
		trace.back()->OnFocusEvent(FocusEvent::MouseEnter);
		wnd_track_stack.push_back(trace.back());
		wnd_track_map.emplace(trace.back(), wnd_track_stack.size());
	}
	wnd.OnFocusEvent(FocusEvent::MouseOver);
	SetCursor(wnd.cursor);
}

void Desktop::LoseTrack() {
	if (wnd_track_stack.empty()) { return; }
	wnd_track_stack.back()->OnFocusEvent(FocusEvent::MouseOut);
	for (auto wnd : reverse(wnd_track_stack)) {
		wnd->OnFocusEvent(FocusEvent::MouseLeave);
	}
	wnd_track_stack.clear();
	wnd_track_map.clear();
}

void Desktop::SetCapture(WndObject& wnd) {
	if (wnd_capture == &wnd) { return; }
	DesktopFrame& frame = GetDesktopFrame(wnd);
	if (frame_capture != &frame) { Win32::SetCapture(frame.hwnd); }
	if (wnd_capture != &wnd) { LoseCapture(); }
	frame_capture = &frame; wnd_capture = &wnd;
}

void Desktop::ReleaseCapture(WndObject& wnd) {
	if (wnd_capture == &wnd) {
		Win32::ReleaseCapture();
	}
}

void Desktop::LoseCapture() {
	frame_capture = nullptr; wnd_capture = nullptr;
}

void Desktop::DispatchMouseEvent(DesktopFrame& frame, MouseEvent event) {
	if (wnd_capture != nullptr) {
		event.point *= frame.GetDescendentTransform(*wnd_capture).Invert();
		wnd_capture->OnMouseEvent(event);
	} else {
		for (ref_ptr<WndObject> curr = &frame;;) {
			ref_ptr<WndObject> next = curr->HitTest(event);
			if (next == nullptr) {
				return;
			} else if (next == curr) {
				SetTrack(*curr);
				curr->OnMouseEvent(event);
				return;
			} else {
				curr = next;
			}
		}
	}
}

void Desktop::SetFocus(WndObject& wnd) {
	if (!wnd_focus_stack.empty() && wnd_focus_stack.back() == &wnd) { return; }
	std::vector<ref_ptr<WndObject>> trace;
	for (ref_ptr<WndObject> curr = &wnd;;) {
		trace.push_back(curr);
		ref_ptr<WndObject> next = curr->parent;
		if (next == nullptr) {
			LoseFocus();
			break;
		}
		if (next == &desktop) {
			LoseFocus();
			frame_focus = static_cast<ref_ptr<DesktopFrame>>(curr);
			Win32::SetFocus(frame_focus->hwnd);
			break;
		}
		if (wnd_focus_map.contains(curr)) {
			wnd_focus_stack.back()->OnFocusEvent(FocusEvent::Blur);
			for (size_t index = wnd_focus_map[curr]; wnd_focus_stack.size() > index; wnd_focus_stack.pop_back()) {
				wnd_focus_stack.back()->OnFocusEvent(FocusEvent::FocusOut);
				wnd_focus_map.erase(wnd_focus_stack.back());
			}
			break;
		}
		curr = next;
	}
	for (; !trace.empty(); trace.pop_back()) {
		trace.back()->OnFocusEvent(FocusEvent::FocusIn);
		wnd_focus_stack.push_back(trace.back());
		wnd_focus_map.emplace(trace.back(), wnd_focus_stack.size());
	}
	if (frame_focus) {
		ime_enabled_wnd.contains(&wnd) ? ViewDesign::ImeEnable(frame_focus->hwnd) : ViewDesign::ImeDisable(frame_focus->hwnd);
	}
	wnd.OnFocusEvent(FocusEvent::Focus);
}

void Desktop::ReleaseFocus(WndObject& wnd) {
	if (!wnd_focus_stack.empty() && wnd_focus_stack.back() == &wnd) {
		Win32::SetFocus(nullptr);
	}
}

void Desktop::LoseFocus() {
	if (wnd_focus_stack.empty()) { return; }
	wnd_focus_stack.back()->OnFocusEvent(FocusEvent::Blur);
	for (auto wnd : reverse(wnd_focus_stack)) {
		wnd->OnFocusEvent(FocusEvent::FocusOut);
	}
	wnd_focus_stack.clear();
	wnd_focus_map.clear();
	frame_focus = nullptr;
}

void Desktop::DispatchKeyEvent(KeyEvent event) {
	if (!wnd_focus_stack.empty()) {
		wnd_focus_stack.back()->OnKeyEvent(event);
	}
}

void Desktop::ImeSetPosition(WndObject& wnd, Point point) {
	DesktopFrame& frame = GetDesktopFramePoint(wnd, point);
	ViewDesign::ImeSetPosition(frame.hwnd, point);
}

void Desktop::ReleaseWindow(WndObject& wnd) {
	if (&wnd == &desktop) { return; }
	if (wnd_track_map.contains(&wnd)) {
		wnd_track_stack.clear();
		wnd_track_map.clear();
	}
	ReleaseCapture(wnd);
	ReleaseFocus(wnd);
	ImeDisable(wnd);
}

void Desktop::EventLoop() { Win32::MessageLoop(); }

void Desktop::Terminate() { frame_list.clear(); Win32::Terminate(); }


} // namespace ViewDesign
