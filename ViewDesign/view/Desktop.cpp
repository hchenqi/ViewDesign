#include "Desktop.h"
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

Window& Desktop::AddWindow(std::unique_ptr<Window> window) {
	RegisterChild(*window);
	window->InitializeRegion(Win32::GetDesktopSize());
	window->Show();
	return *window_list.emplace_back(std::move(window));
}

std::unique_ptr<Window> Desktop::RemoveWindow(Window& window) {
	auto it = std::find_if(window_list.begin(), window_list.end(), [&](const std::unique_ptr<Window>& ptr) { return ptr.get() == &window; });
	if (it == window_list.end()) { throw std::invalid_argument("invalid desktop window"); }
	window.Hide();
	UnregisterChild(window);
	std::unique_ptr<Window> ptr = std::move(*it);	window_list.erase(it);
	if (window_list.empty()) { Terminate(); }
	return ptr;
}

Window& Desktop::GetWindow(ViewBase& view) {
	ref_ptr<ViewBase> child = &view;
	for (ref_ptr<ViewBase> parent = child->parent; parent != &desktop; child = parent, parent = child->parent) {
		if (parent == nullptr) { throw std::invalid_argument("view not registered"); }
	}
	return static_cast<Window&>(*child);
}

Window& Desktop::GetWindowPoint(ViewBase& view, Point& point) {
	ref_ptr<ViewBase> child = &view;
	for (ref_ptr<ViewBase> parent = child->parent; parent != &desktop; child = parent, parent = child->parent) {
		if (parent == nullptr) { throw std::invalid_argument("view not registered"); }
		point *= parent->GetChildTransform(*child);
	}
	return static_cast<Window&>(*child);
}

void Desktop::RecreateFrameLayer() {
	for (auto& window : window_list) {
		window->RecreateLayer();
	}
}

void Desktop::OnChildRedraw(ViewBase& child, Rect child_redraw_region) {
	static_cast<Window&>(child).Redraw(child_redraw_region);
}

void Desktop::SetTrack(ViewBase& view) {
	if (!view_track_stack.empty() && view_track_stack.back() == &view) { return; }
	std::vector<ref_ptr<ViewBase>> trace;
	for (ref_ptr<ViewBase> curr = &view;;) {
		trace.push_back(curr);
		curr = curr->parent;
		if (curr == nullptr || curr == &desktop) {
			LoseTrack();
			break;
		}
		if (view_track_map.contains(curr)) {
			view_track_stack.back()->OnFocusEvent(FocusEvent::MouseOut);
			for (size_t index = view_track_map[curr]; view_track_stack.size() > index; view_track_stack.pop_back()) {
				view_track_stack.back()->OnFocusEvent(FocusEvent::MouseLeave);
				view_track_map.erase(view_track_stack.back());
			}
			break;
		}
	}
	for (; !trace.empty(); trace.pop_back()) {
		trace.back()->OnFocusEvent(FocusEvent::MouseEnter);
		view_track_stack.push_back(trace.back());
		view_track_map.emplace(trace.back(), view_track_stack.size());
	}
	view.OnFocusEvent(FocusEvent::MouseOver);
	SetCursor(view.cursor);
}

void Desktop::LoseTrack() {
	if (view_track_stack.empty()) { return; }
	view_track_stack.back()->OnFocusEvent(FocusEvent::MouseOut);
	for (auto view : reverse(view_track_stack)) {
		view->OnFocusEvent(FocusEvent::MouseLeave);
	}
	view_track_stack.clear();
	view_track_map.clear();
}

void Desktop::SetCapture(ViewBase& view) {
	if (view_capture == &view) { return; }
	Window& window = GetWindow(view);
	if (window_capture != &window) { Win32::SetCapture(window.hwnd); }
	if (view_capture != &view) { LoseCapture(); }
	window_capture = &window; view_capture = &view;
}

void Desktop::ReleaseCapture(ViewBase& view) {
	if (view_capture == &view) {
		Win32::ReleaseCapture();
	}
}

void Desktop::LoseCapture() {
	window_capture = nullptr; view_capture = nullptr;
}

void Desktop::DispatchMouseEvent(Window& window, MouseEvent event) {
	if (view_capture != nullptr) {
		event.point *= window.GetDescendentTransform(*view_capture).Invert();
		view_capture->OnMouseEvent(event);
	} else {
		for (ref_ptr<ViewBase> curr = &window;;) {
			ref_ptr<ViewBase> next = curr->HitTest(event);
			if (next == nullptr) {
				SetTrack(*curr);
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

void Desktop::SetFocus(ViewBase& view) {
	if (!view_focus_stack.empty() && view_focus_stack.back() == &view) { return; }
	std::vector<ref_ptr<ViewBase>> trace;
	for (ref_ptr<ViewBase> curr = &view;;) {
		trace.push_back(curr);
		ref_ptr<ViewBase> next = curr->parent;
		if (next == nullptr) {
			LoseFocus();
			break;
		}
		if (next == &desktop) {
			LoseFocus();
			window_focus = static_cast<ref_ptr<Window>>(curr);
			Win32::SetFocus(window_focus->hwnd);
			break;
		}
		if (view_focus_map.contains(curr)) {
			view_focus_stack.back()->OnFocusEvent(FocusEvent::Blur);
			for (size_t index = view_focus_map[curr]; view_focus_stack.size() > index; view_focus_stack.pop_back()) {
				view_focus_stack.back()->OnFocusEvent(FocusEvent::FocusOut);
				view_focus_map.erase(view_focus_stack.back());
			}
			break;
		}
		curr = next;
	}
	for (; !trace.empty(); trace.pop_back()) {
		trace.back()->OnFocusEvent(FocusEvent::FocusIn);
		view_focus_stack.push_back(trace.back());
		view_focus_map.emplace(trace.back(), view_focus_stack.size());
	}
	if (window_focus) {
		ime_enabled_view.contains(&view) ? ViewDesign::ImeEnable(window_focus->hwnd) : ViewDesign::ImeDisable(window_focus->hwnd);
	}
	view.OnFocusEvent(FocusEvent::Focus);
}

void Desktop::ReleaseFocus(ViewBase& view) {
	if (!view_focus_stack.empty() && view_focus_stack.back() == &view) {
		Win32::SetFocus(nullptr);
	}
}

void Desktop::LoseFocus() {
	if (view_focus_stack.empty()) { return; }
	view_focus_stack.back()->OnFocusEvent(FocusEvent::Blur);
	for (auto view : reverse(view_focus_stack)) {
		view->OnFocusEvent(FocusEvent::FocusOut);
	}
	view_focus_stack.clear();
	view_focus_map.clear();
	window_focus = nullptr;
}

void Desktop::DispatchKeyEvent(KeyEvent event) {
	if (!view_focus_stack.empty()) {
		view_focus_stack.back()->OnKeyEvent(event);
	}
}

void Desktop::ImeSetPosition(ViewBase& view, Point point) {
	Window& window = GetWindowPoint(view, point);
	ViewDesign::ImeSetPosition(window.hwnd, point);
}

void Desktop::ReleaseView(ViewBase& view) {
	if (&view == &desktop) { return; }
	if (view_track_map.contains(&view)) {
		view_track_stack.clear();
		view_track_map.clear();
	}
	ReleaseCapture(view);
	ReleaseFocus(view);
	ImeDisable(view);
}

void Desktop::EventLoop() { Win32::MessageLoop(); }

void Desktop::Terminate() { window_list.clear(); Win32::Terminate(); }


} // namespace ViewDesign
