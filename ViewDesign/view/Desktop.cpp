#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/system/desktop.h"
#include "ViewDesign/system/window.h"
#include "ViewDesign/system/event_loop.h"
#include "ViewDesign/common/reversion_wrapper.h"


namespace ViewDesign {


Desktop& Desktop::Get() {
	static Desktop desktop;
	return desktop;
}

void Desktop::OnViewDetach(ViewBase& view) {
	ime_enabled_view.erase(&view);
	if (auto it = view_focus_map.find(&view); it != view_focus_map.end()) { view_focus_stack[it->second] = nullptr; view_focus_map.erase(it); }
	if (view_capture == &view) { ReleaseWindowCapture(*window_capture); }
	if (auto it = view_track_map.find(&view); it != view_track_map.end()) { view_track_stack[it->second] = nullptr; view_track_map.erase(it); }
}

Window& Desktop::AddWindow(std::unique_ptr<Window> window) {
	RegisterChild(*window);
	UpdateWindowSizeRef(*window);
	window->Show();
	return *window_list.emplace_back(std::move(window));
}

std::unique_ptr<Window> Desktop::RemoveWindow(Window& window) {
	auto it = std::find_if(window_list.begin(), window_list.end(), [&](const std::unique_ptr<Window>& ptr) { return ptr.get() == &window; });
	if (it == window_list.end()) { return nullptr; }
	std::unique_ptr<Window> ptr = std::move(*it);
	window_list.erase(it);
	window.Hide();
	UnregisterChild(window);
	return ptr;
}

void Desktop::CloseAllWindows() {
	for (auto& window : window_list) {
		window->Close();
	}
}

SizeU Desktop::GetPixelSize() const {
	return GetDesktopPixelSize();
}

std::pair<SizeU, RectI> Desktop::GetWindowMinMaxRegion(Window& window) {
	auto [size_min, size_max] = window.CalculateMinMaxSize(GetPixelSize() / window.scale);
	return { Round(size_min * window.scale), RectI(point_i_zero, Round(size_max * window.scale)) };
}

void Desktop::UpdateWindowSizeRef(Window& window) {
	window.RegionUpdated(window.OnWindowSizeRefUpdate(GetPixelSize() / window.scale));
}

void Desktop::PushTrack(size_t index, std::vector<ref_ptr<ViewBase>> trace) {
	if (!view_track_stack.empty() && view_track_stack.back() != nullptr) {
		view_track_stack.back()->OnFocusEvent(FocusEvent::MouseOut);
	}
	for (; view_track_stack.size() > index; view_track_stack.pop_back()) {
		if (view_track_stack.back() != nullptr) {
			view_track_stack.back()->OnFocusEvent(FocusEvent::MouseLeave);
			view_track_map.erase(view_track_stack.back());
		}
	}
	for (; !trace.empty(); trace.pop_back()) {
		view_track_map.emplace(trace.back(), view_track_stack.size());
		view_track_stack.push_back(trace.back());
		view_track_stack.back()->OnFocusEvent(FocusEvent::MouseEnter);
	}
	if (!view_track_stack.empty()) {
		view_track_stack.back()->OnFocusEvent(FocusEvent::MouseOver);
		Window& window = static_cast<Window&>(*view_track_stack.front());
		SetWindowCursor(window.GetHandle(), view_track_stack.back()->cursor);
	}
}

void Desktop::SetTrack(ViewBase& view) {
	if (!view_track_stack.empty() && view_track_stack.back() == &view) { return; }
	std::vector<ref_ptr<ViewBase>> trace;
	for (ref_ptr<ViewBase> curr = &view;;) {
		trace.push_back(curr);
		curr = curr->parent;
		if (curr == nullptr) { PushTrack(0, {}); break; }
		if (curr == &desktop.Get()) { PushTrack(0, std::move(trace)); break; }
		if (auto it = view_track_map.find(curr); it != view_track_map.end()) { PushTrack(it->second + 1, std::move(trace)); break; }
	}
}

void Desktop::SetWindowCapture(Window& window) {
	window_capture = &window;
	ViewDesign::SetWindowCapture(window.GetHandle());
}

void Desktop::ReleaseWindowCapture(Window& window) {
	ViewDesign::ReleaseWindowCapture(window.GetHandle());
}

void Desktop::SetCapture(ViewBase& view) {
	if (view_capture == &view) { return; }
	Window& window = GetWindow(view);
	if (window_capture != &window) { SetWindowCapture(window); }
	view_capture = &view;
}

void Desktop::ReleaseCapture(ViewBase& view) {
	if (view_capture == &view) {
		ReleaseWindowCapture(*window_capture);
	}
}

void Desktop::DispatchMouseEvent(Window& window, MouseEvent event) {
	if (view_capture != nullptr) {
		event.point = window.ConvertDescendentPoint(event.point, *view_capture);
		return view_capture->OnMouseEvent(event);
	}
	for (ref_ptr<ViewBase> curr = &window, next;; curr = next) {
		next = curr->HitTest(event);
		if (next == nullptr) {
			break;
		}
		if (next == curr) {
			SetTrack(*curr);
			curr->OnMouseEvent(event);
			break;
		}
	}
}

void Desktop::PushFocus(size_t index, std::vector<ref_ptr<ViewBase>> trace) {
	if (!view_focus_stack.empty() && view_focus_stack.back() != nullptr) {
		view_focus_stack.back()->OnFocusEvent(FocusEvent::Blur);
	}
	for (; view_focus_stack.size() > index; view_focus_stack.pop_back()) {
		if (view_focus_stack.back() != nullptr) {
			view_focus_stack.back()->OnFocusEvent(FocusEvent::FocusOut);
			view_focus_map.erase(view_focus_stack.back());
		}
	}
	for (; !trace.empty(); trace.pop_back()) {
		view_focus_map.emplace(trace.back(), view_focus_stack.size());
		view_focus_stack.push_back(trace.back());
		view_focus_stack.back()->OnFocusEvent(FocusEvent::FocusIn);
	}
	if (!view_focus_stack.empty()) {
		view_focus_stack.back()->OnFocusEvent(FocusEvent::Focus);
		Window& window = static_cast<Window&>(*view_focus_stack.front());
		ime_enabled_view.contains(view_focus_stack.back()) ? ImeWindowEnable(window) : ImeWindowDisable(window);
	}
}

void Desktop::SetWindowFocus(Window& window) {
	ViewDesign::SetWindowFocus(window.GetHandle());
}

void Desktop::SetFocus(ViewBase& view) {
	if (!view_focus_stack.empty() && view_focus_stack.back() == &view) { return; }
	std::vector<ref_ptr<ViewBase>> trace;
	for (ref_ptr<ViewBase> curr = &view, next;; curr = next) {
		trace.push_back(curr);
		next = curr->parent;
		if (next == nullptr) { PushFocus(0, {}); break; }
		if (next == &desktop.Get()) { SetWindowFocus(*static_cast<ref_ptr<Window>>(curr)); PushFocus(0, std::move(trace)); break; }
		if (auto it = view_focus_map.find(next); it != view_focus_map.end()) { PushFocus(it->second + 1, std::move(trace)); break; }
	}
}

void Desktop::DispatchKeyEvent(KeyEvent event) {
	if (!view_focus_stack.empty() && view_focus_stack.back() != nullptr) {
		view_focus_stack.back()->OnKeyEvent(event);
	}
}

void Desktop::ImeWindowEnable(Window& window) {
	ViewDesign::ImeWindowEnable(window.GetHandle());
}

void Desktop::ImeWindowDisable(Window& window) {
	ViewDesign::ImeWindowDisable(window.GetHandle());
}

void Desktop::ImeWindowSetPosition(Window& window, Point point) {
	ViewDesign::ImeWindowSetPosition(window.GetHandle(), Round(point));
}

void Desktop::ImeSetPosition(ViewBase& view, Point point) {
	Window& window = GetWindow(view);
	point = window.ConvertDescendentPoint(view, point);
	ImeWindowSetPosition(window, point);
}

void Desktop::EventLoop() {
	ViewDesign::EventLoop();
}

void Desktop::PollEvents() {
	ViewDesign::PollEvents();
}


} // namespace ViewDesign
