#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/control/TextBox.h"

using namespace ViewDesign;

struct TextBoxStyle : TextBox::Style {
	TextBoxStyle() {
		font.size(75.0f).color(Color::Black);
	}
};

void App() {
	desktop.AddWindow(
		create<DefaultWindow>(
			DefaultWindow::Style(),
			u"Example",
			create<CenterFrame<Fixed, Fixed>>(
				create<TextBox>(TextBoxStyle(), u"Hello World!")
			)
		)
	);
	desktop.EventLoop();
}
