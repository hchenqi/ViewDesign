#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/control/TextView.h>

using namespace ViewDesign;

struct TextViewStyle : TextView::Style {
	TextViewStyle() {
		font.size(75.0f).color(ColorCode::Black);
	}
};

void App() {
	desktop.AddWindow(
		create<DefaultWindow>(
			DefaultWindow::Style(),
			u"Example",
			create<CenterFrame<Fixed, Fixed>>(
				create<TextView>(TextViewStyle(), u"Hello World!\n你好，世界！")
			)
		)
	);
	desktop.EventLoop();
}
