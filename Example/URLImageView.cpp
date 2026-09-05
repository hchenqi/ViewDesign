#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/ScaleFrame.h>
#include <ViewDesign/view/control/ImageView.h>
#include <ViewDesign/view/wrapper/Background.h>

#include <string>
#include <stdexcept>

#ifdef _WIN32
#include <Windows.h>
#endif


using namespace ViewDesign;


namespace {

const std::string image_url = "https://raw.githubusercontent.com/coppersalts/HTML5b/ebd44046df040c76e70209d50bf79437d1d09dfd/visuals/premenubg.png";

} // namespace


void App() {
	try {
		Image image(image_url);

		desktop.AddWindow(
			new DefaultBackground<DefaultWindow>(
				DefaultWindow::Style(),
				u"URL Image",
				new CenterFrame<Fixed, Fixed>(
					new ScaleFrame(
						new Stateful::ImageView(image)
					)
				)
			)
		);
		desktop.EventLoop();
	}
	catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "URLImageView failed", MB_ICONERROR);
	}
}
