#include <ViewDesign/view/control/Placeholder.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/widget/Popup.h>
#include <ViewDesign/view/widget/Box.h>
#include <ViewDesign/view/widget/FilledButton.h>
#include <ViewDesign/view/wrapper/HitTestHelper.h>
#include <ViewDesign/view/wrapper/Background.h>


using namespace ViewDesign;


class MainView : public Placeholder<Fixed, Fixed> {
private:
	void ShowMenu(Point point) {
		ShowPopup(
			*this, point,
			new HitSelf<FilledButton<Box<Bounded, Bounded>>>(
				FilledButtonStyle(), [&] { desktop.CloseAllWindows(); },
				Margin(0.0f), Border(2.0f, 3.0f, ColorCode::Gray), color_transparent, Padding(3.0f),
				new TextView(TextView::Style(), u"exit")
			)
		);
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown: SetFocus(); break;
		case MouseEvent::RightDown: ShowMenu(event.point); break;
		}
	}
};


void App() {
	desktop.AddWindow(
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			u"Placeholder",
			new MainView()
		)
	);
	desktop.EventLoop();
}
