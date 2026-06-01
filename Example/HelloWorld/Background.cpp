#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/wrapper/Background.h>

using namespace ViewDesign;

struct TextViewStyle : TextView::Style {
	TextViewStyle() {
		font.size(75.0f).color(ColorCode::Black);
	}
};

void App() {
	desktop.AddWindow(
		create<DefaultBackground<DefaultWindow>>(
			DefaultWindow::Style(),
			u"HelloWorld",
			create<CenterFrame<Fixed, Fixed>>(
				create<BackgroundFrame<Relative, Relative>>(
					ColorCode::LightPink,
					create<TextView>(TextViewStyle(), u"Hello World!")
				)
			)
		)
	);
	desktop.EventLoop();
}
