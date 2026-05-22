#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/frame/BackgroundFrame.h"
#include "ViewDesign/view/control/Placeholder.h"

using namespace ViewDesign;

void App() {
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Background",
			new BackgroundFrame( // or use: create<BackgroundFrame<Fixed, Fixed>>. create<BackgroundFrame> doesn't work here because BackgroundFrame is a template
				Color::LightPink,
				new Placeholder<Fixed, Fixed>()
			)
		)
	);
	desktop.EventLoop();
}
