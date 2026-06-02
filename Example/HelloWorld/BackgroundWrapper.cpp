// BackgroundFrame / SolidColorBackground wrapper:
// - BackgroundFrame only draws background within the extent of its child view
// - SolidColorBackground wrapper draws background according to the draw region that might exceed the extent of the view

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
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
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			u"HelloWorld",
			new CenterFrame<Fixed, Fixed>(
				new BackgroundFrame(
					ColorCode::LightPink,
					new PaddingFrame(
						Padding(20.0f),
						// `CenterFrame`, `BackgroundFrame` and `PaddingFrame` don't clip the child view's content within its extent
						// thus, the draw region for the child view exceeds its extent, and the inner background covers the outer
						new SolidColorBackground<ColorCode::LightYellow, TextView>(TextViewStyle(), u"Hello World!")
					)
				)
			)
		)
	);
	desktop.EventLoop();
}
