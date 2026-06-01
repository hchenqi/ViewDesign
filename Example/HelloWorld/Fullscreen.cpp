#include <ViewDesign/view/widget/UndecoratedWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/wrapper/Background.h>

using namespace ViewDesign;

struct FullscreenWindowStyle : UndecoratedWindow::Style {
	FullscreenWindowStyle() {
		width.fix(pct(100));
		height.fix(pct(100));
	}
};

struct TextViewStyle : TextView::Style {
	TextViewStyle() {
		font.size(75.0f).color(ColorCode::Black);
	}
};

void App() {
	desktop.AddWindow(
		create<DefaultBackground<UndecoratedWindow>>(
			FullscreenWindowStyle(),
			u"HelloWorld",
			create<CenterFrame<Fixed, Fixed>>(
				create<TextView>(TextViewStyle(), u"Hello World!")
			)
		)
	);
	desktop.EventLoop();
}
