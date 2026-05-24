#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/BackgroundFrame.h>
#include <ViewDesign/view/control/Placeholder.h>

using namespace ViewDesign;

void App() {
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Background",
			// create<BackgroundFrame> doesn't work here directly, because BackgroundFrame is a class template
			// the template arguments must be explicitly given, as create<BackgroundFrame<Fixed, Fixed>>
			// with new we don't need to provide the template arguments thanks to template argument deduction guide
			// template argument deduction guide doesn't work everywhere and in certain cases we still need to explicitly provide the arguments
			new BackgroundFrame(
				Color::LightPink,
				new Placeholder<Fixed, Fixed>()
			)
		)
	);
	desktop.EventLoop();
}
