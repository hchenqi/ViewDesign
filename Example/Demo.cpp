#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/widget/UndecoratedWindow.h>
#include <ViewDesign/view/control/Placeholder.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/wrapper/Background.h>


using namespace ViewDesign;


struct BackgroundWindowStyle : UndecoratedWindow::Style {
	BackgroundWindowStyle() {
		width.normal(pct(100)).min(pct(100)).max(pct(100));
		height.normal(pct(100)).min(pct(100)).max(pct(100));
	}
};

struct TextViewStyle : TextView::Style {
	TextViewStyle() {
		font.size(75.0f).color(ColorCode::Black);
	}
};


void App() {
	desktop.AddWindow(
		new UndecoratedWindow(
			BackgroundWindowStyle(),
			u"Background",
			new DefaultBackground<Placeholder<Fixed, Fixed>>()
		)
	);
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Placeholder",
			new Placeholder<Fixed, Fixed>()
		)
	);
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Example",
			new CenterFrame<Fixed, Fixed>(
				new TextView(TextViewStyle(), u"Hello World!")
			)
		)
	);
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Example",
			new ClipFrame<Fixed, Fixed, TopLeft>(
				new TextView(TextViewStyle(), u"Hello World!")
			)
		)
	);
	desktop.EventLoop();
}
