#include "ViewDesign/view/widget/Tooltip.h"
#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/frame/BorderFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/control/TextBox.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/geometry/clamp.h"
#include "ViewDesign/event/timer.h"
#include "ViewDesign/platform/win32/win32_api.h"


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
					text_box = new SolidColorBackground<TextBox, 0xF1F2F7>(TextStyle(), u"")
				)
			)
		)
	) {
		Win32::SetWndStyleTool(GetHWND());
		Win32::SetWndNoActivate(GetHWND());
		Win32::SetWndTopMost(GetHWND());
		Win32::SetWndMousePenetrate(GetHWND());
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
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) override { return clamp(region, Rect(point_zero, size_ref)); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { region.size = child_size; }

private:
	std::unique_ptr<Tooltip> self;
public:
	static Tooltip& Get() {
		static Tooltip& tooltip = []() -> Tooltip& {
			std::unique_ptr<Tooltip> tooltip(new Tooltip);
			Tooltip& ref = *tooltip; ref.self = std::move(tooltip);
			return ref;
		}();
		return tooltip;
	}
private:
	void ShowSelf() { desktop.AddWindow(std::move(self)); }
	void HideSelf() { self.reset(static_cast<owner_ptr<Tooltip>>(desktop.RemoveWindow(*this).release())); }
	void SetOpacity(uchar opacity) { Win32::SetWndOpacity(GetHWND(), opacity); }

private:
	static constexpr uint wait_time = 1 * 1000;  // 1 s
	static constexpr uint show_time = 5 * 1000;  // 5 s
	static constexpr uint switch_interval = 200;  // 200 ms
	static constexpr uint animation_interval = 20;  // 20 ms
	static constexpr uint animation_time = 160;  // 160 ms
	static constexpr uint animation_step_max = animation_time / animation_interval;  // 8
private:
	Timer timer = Timer(std::bind(&Tooltip::Callback, this));
private:
	enum class State { Hidden, Waiting, Showing, Shown, Hiding } state = State::Hidden;
	uint animation_step = 0;
private:
	void Callback() {
		switch (state) {
		case State::Hidden:
			break;
		case State::Waiting:
			state = State::Showing;
			animation_step = 0;
			timer.Set(animation_interval);
			region.point = GetDesktopCursorPosition() + Vector(0, 10);
			SetOpacity(0);
			ShowSelf();
			break;
		case State::Showing:
			if (animation_step < animation_step_max) {
				animation_step++;
				SetOpacity(0xFF * animation_step / animation_step_max);
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
				SetOpacity(0xFF * animation_step / animation_step_max);
			} else {
				this->view = nullptr;
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
		if (this->view != &view) {
			return;
		}
		this->view = nullptr;
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
		this->view = nullptr;
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
