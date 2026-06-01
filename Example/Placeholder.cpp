#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/control/Placeholder.h>
#include <ViewDesign/view/wrapper/Background.h>

using namespace ViewDesign;

void App() {
	desktop.AddWindow(
		create<DefaultBackground<DefaultWindow>>(
			DefaultWindow::Style(),
			u"Placeholder",
			create<Placeholder<Fixed, Fixed>>()
		)
	);
	desktop.EventLoop();
}
