#include "ViewDesign/view/widget/UndecoratedWindow.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/wrapper/Background.h"

#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/control/Placeholder.h"

#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/control/TextBox.h"

#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/control/TextBox.h"


using namespace ViewDesign;


struct BackgroundWindowStyle : UndecoratedWindow::Style {
	BackgroundWindowStyle() {
		width.normal(pct(100)).min(pct(100)).max(pct(100));
		height.normal(pct(100)).min(pct(100)).max(pct(100));
	}
};

struct TextBoxStyle : TextBox::Style {
	TextBoxStyle() {
		font.size(75.0f).color(Color::Black);
	}
};


void App() {
	desktop.AddWindow(
		new UndecoratedWindow(
			BackgroundWindowStyle(),
			u"Background",
			new SolidColorBackground<Placeholder<Fixed, Fixed>, Color::White>()
		)
	);
	desktop.AddWindow(
		create<DefaultWindow>(
			DefaultWindow::Style(),
			u"Placeholder",
			create<Placeholder<Fixed, Fixed>>()
		)
	);
	desktop.AddWindow(
		create<DefaultWindow>(
			DefaultWindow::Style(),
			u"Example",
			create<CenterFrame<Fixed, Fixed>>(
				create<TextBox>(TextBoxStyle(), u"Hello World!")
			)
		)
	);
	desktop.AddWindow(
		create<DefaultWindow>(
			DefaultWindow::Style(),
			u"Example",
			create<ClipFrame<Fixed, Fixed, TopLeft>>(
				create<TextBox>(TextBoxStyle(), u"Hello World!")
			)
		)
	);
	desktop.EventLoop();
}
