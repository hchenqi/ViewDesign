#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/ScaleFrame.h>
#include <ViewDesign/view/frame/StretchFrame.h>
#include <ViewDesign/view/control/ImageView.h>

using namespace ViewDesign;

static const u16string file = uR"(C:\Windows\Web\Wallpaper\Windows\img0.jpg)";

void App() {
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"ImageView (centered) (hint: zoom with Ctrl + scroll)",
			new CenterFrame<Fixed, Fixed>(
				new ScaleFrame(
					new ImageView(file)
				)
			)
		)
	);
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"ImageView (stretched)",
			new StretchFrame<Fixed, Fixed>(
				new ImageView(file)
			)
		)
	);
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"ImageView (repeating) (hint: zoom with Ctrl + scroll)",
			new ScaleFrame(
				new ImageRepeatView(file, Point(500.0f, 300.0f))
			)
		)
	);
	desktop.EventLoop();
}
