#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/widget/TabView.h>
#include <ViewDesign/view/wrapper/Background.h>

using namespace ViewDesign;

void App() {
	desktop.AddWindow(
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			u"TabView",
			new TabView(
				TabView::Style(),
				TabView::Tab(new TabView::DefaultHeaderFixed(u"fixed-1"), new SolidColorBackground<ColorCode::LightPink, Placeholder<Fixed, Fixed>>()),
				TabView::Tab(new TabView::DefaultHeaderFixed(u"fixed-2"), new SolidColorBackground<ColorCode::LightBlue, Placeholder<Fixed, Fixed>>()),
				TabView::Tab(new TabView::DefaultHeaderClosable(u"closable-1"), new SolidColorBackground<ColorCode::LightYellow, Placeholder<Fixed, Fixed>>()),
				TabView::Tab(new TabView::DefaultHeaderClosable(u"closable-2"), new SolidColorBackground<ColorCode::LightGreen, Placeholder<Fixed, Fixed>>())
			)
		)
	);
	desktop.EventLoop();
}
