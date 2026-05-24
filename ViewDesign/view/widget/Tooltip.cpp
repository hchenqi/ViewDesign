#include "ViewDesign/view/widget/Tooltip.h"
#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/frame/BorderFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/control/TextBox.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/geometry/helper.h"
#include "ViewDesign/event/timer.h"
#include "ViewDesign/system/window.h"

#if defined(VIEWDESIGN_BACKEND_WIN32)
#include <ViewDesign/platform/win32/window.h>
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW)
#include <ViewDesign/platform/glfw/window.h>
#endif


namespace ViewDesign {

namespace {

class Tooltip : public Window {
private:
	Tooltip() : Window(
		u"",
		new MaxFrame(
			Size(200.0f, 200.0f),
			new BorderFrame(
				Border(1.5f, 0x767676),
				new PaddingFrame(
					Padding(5.0f, 2.0f),
					text_box = new SolidColorBackground<0xF1F2F7, TextBox>(TextStyle(), u"")
				)
			)
		)
	) {
#if defined(VIEWDESIGN_BACKEND_WIN32)
		Win32::SetWndUndecorated(Win32::AsHWND(GetHandle()));
		Win32::SetWndExStyle(Win32::AsHWND(GetHandle()), WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
		Win32::SetWndTopMost(Win32::AsHWND(GetHandle()));
#endif
#if defined(VIEWDESIGN_BACKEND_GLFW)
		glfwSetWindowAttrib(GLFW::AsGLFWWindow(GetHandle()), GLFW_DECORATED, GLFW_FALSE);
		glfwSetWindowAttrib(GLFW::AsGLFWWindow(GetHandle()), GLFW_FLOATING, GLFW_TRUE);
		glfwSetWindowAttrib(GLFW::AsGLFWWindow(GetHandle()), GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
		glfwSetWindowAttrib(GLFW::AsGLFWWindow(GetHandle()), GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
#endif
		region.size = UpdateChildSizeRef(*child, size_empty);
	}

private:
	struct TextStyle : TextBox::Style {
		TextStyle() {
			font.size(13).color(0x737374);
		}
	};

private:
	ref_ptr<TextBox> text_box;

private:
	Rect region;
private:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) override { return { region.size, region.size }; }
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) override { return Clamp(region, Rect(point_zero, size_ref)); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { region.size = child_size; }

private:
	inline static ref_ptr<Tooltip> ref = nullptr;
	inline static ref_ptr<std::unique_ptr<Tooltip>> instance_ref = nullptr;
public:
	static Tooltip& Get() {
		static std::unique_ptr<Tooltip> instance = nullptr;
		if (ref == nullptr) {
			instance.reset(new Tooltip());
			ref = instance.get();
			instance_ref = &instance;
		}
		return *ref;
	}
private:
	void ShowSelf() { desktop.AddWindow(std::move(*instance_ref)); }
	void HideSelf() { instance_ref->reset(static_cast<owner_ptr<Tooltip>>(desktop.RemoveWindow(*this).release())); }
	void SetOpacity(float opacity) { SetWindowOpacity(GetHandle(), opacity); }

private:
	static constexpr uint32 wait_time = 1 * 1000;  // 1 s
	static constexpr uint32 show_time = 5 * 1000;  // 5 s
	static constexpr uint32 switch_interval = 200;  // 200 ms
	static constexpr uint32 animation_interval = 20;  // 20 ms
	static constexpr uint32 animation_time = 160;  // 160 ms
	static constexpr uint32 animation_step_max = animation_time / animation_interval;  // 8
private:
	Timer timer = Timer(std::bind(&Tooltip::Callback, this));
private:
	enum class State { Hidden, Waiting, Showing, Shown, Hiding } state = State::Hidden;
	uint32 animation_step = 0;
private:
	void Callback() {
		switch (state) {
		case State::Hidden:
			break;
		case State::Waiting:
			state = State::Showing;
			animation_step = 0;
			timer.Set(animation_interval);
			region.point = GetCursorPosition() / GetScale() + Vector(0, 10.0f);
			SetOpacity(0.0f);
			ShowSelf();
			break;
		case State::Showing:
			if (animation_step < animation_step_max) {
				animation_step++;
				SetOpacity((float)animation_step / animation_step_max);
			} else {
				state = State::Shown;
				timer.Set(show_time);
			}
			break;
		case State::Shown:
			state = State::Hiding;
			animation_step = animation_step_max;
			timer.Set(animation_interval);
			break;
		case State::Hiding:
			if (animation_step > 0) {
				animation_step--;
				SetOpacity((float)animation_step / animation_step_max);
			} else {
				timer.Stop();
				state = State::Hidden;
				HideSelf();
			}
			break;
		}
	}

private:
	ref_ptr<ViewBase> view;
public:
	void Show(ViewBase& view, const u16string& text) {
		this->view = &view;
		text_box->Assign(text);
		switch (state) {
		case State::Hidden:
			state = State::Waiting;
			[[fallthrough]];
		case State::Waiting:
			timer.Set(wait_time);
			break;
		case State::Showing:
		case State::Shown:
		case State::Hiding:
			HideSelf();
			state = State::Waiting;
			timer.Set(switch_interval);
			break;
		}
	}
	void Hide(ViewBase& view) {
		switch (state) {
		case State::Hidden:
			break;
		case State::Waiting:
			timer.Stop();
			state = State::Hidden;
			break;
		case State::Showing:
			state = State::Hiding;
			break;
		case State::Shown:
			Callback();
			break;
		case State::Hiding:
			break;
		}
	}
	void Destroy(ViewBase& view) {
		if (this->view != &view) {
			return;
		}
		switch (state) {
		case State::Hidden:
			break;
		case State::Waiting:
			timer.Stop();
			state = State::Hidden;
			break;
		case State::Showing:
		case State::Shown:
		case State::Hiding:
			timer.Stop();
			state = State::Hidden;
			HideSelf();
			break;
		}
	}
};

} // namespace


void ShowTooltip(ViewBase& view, const u16string& text) {
	Tooltip::Get().Show(view, text);
}

void HideTooltip(ViewBase& view) {
	Tooltip::Get().Hide(view);
}

void DestroyTooltip(ViewBase& view) {
	Tooltip::Get().Destroy(view);
}


} // namespace ViewDesign
