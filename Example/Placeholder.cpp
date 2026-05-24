#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/control/Placeholder.h>

using namespace ViewDesign;

void App() {
	desktop.AddWindow(
		create<DefaultWindow>(
			DefaultWindow::Style(),
			u"Placeholder",
			create<Placeholder<Fixed, Fixed>>()
		)
	);
	desktop.EventLoop();
}
