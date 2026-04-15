#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/ScaleFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/BorderFrame.h"
#include "ViewDesign/view/control/TextBox.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"

#include <cmath>


using namespace ViewDesign;


struct MainWindowStyle : TitleBarWindow::Style {
	MainWindowStyle() {
		title.text.assign(L"ScaleFrameTest");
	}
};


class ScaleView : public ScaleFrame<Fixed, Fixed> {
public:
	ScaleView(child_type child) : ScaleFrame(Scale(1.0), std::move(child)) {}
private:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (event.ctrl && event.type == MouseEvent::WheelVertical) {
			return this;
		}
		return ScaleFrame::HitTest(event);
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		SetScale(scale * Scale(powf(1.1f, event.wheel_delta / 120.0f)));
	}
};


struct TextBoxStyle : TextBlockStyle {
	TextBoxStyle() {
		font.size(75).color(Color::Lime);
	}
};


int main() {
	desktop.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new ScaleView(
				new ClipFrame<Fixed, Fixed, TopLeft>(
					new BorderFrame(
						Border(2px, Color::Red),
						new TextBox(TextBoxStyle(), L"Hello World!")
					)
				)
			)
		)
	);
	desktop.EventLoop();
}
