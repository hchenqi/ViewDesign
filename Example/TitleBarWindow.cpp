#include "ViewDesign/view/widget/TitleBarWindow.h"
#include "ViewDesign/view/control/Placeholder.h"

using namespace ViewDesign;

void App() {
	desktop.AddWindow(
		new TitleBarWindow(
			TitleBarWindow::Style(),
			u"TitleBarWindow",
			new Placeholder<Fixed, Fixed>()
		)
	);
	desktop.EventLoop();
}
