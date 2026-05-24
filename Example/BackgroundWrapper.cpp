// BackgroundFrame / SolidColorBackground wrapper:
// - BackgroundFrame only draws background within the extent of its child view
// - SolidColorBackground wrapper draws background according to the draw region that might exceed the extent of the view

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/frame/PaddingFrame.h>
#include <ViewDesign/view/control/Placeholder.h>
#include <ViewDesign/view/wrapper/Background.h>

using namespace ViewDesign;

void App() {
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"BackgroundWrapper",
			// PaddingFrame doesn't clip the child view's content within the padding and therefore the draw region for its child view exceeds the extent
			new PaddingFrame(
				Padding(50.0f),
				new SolidColorBackground<Color::LightPink, BackgroundFrame<Fixed, Fixed>>(
					Color::Ivory,
					new Placeholder<Fixed, Fixed>()
				)
			)
		)
	);
	desktop.EventLoop();
}
