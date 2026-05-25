#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/widget/UndecoratedWindow.h>
#include <ViewDesign/view/widget/TitleBarWindow.h>
#include <ViewDesign/view/frame/CenterFrame.h>
#include <ViewDesign/view/frame/ClipFrame.h>
#include <ViewDesign/view/control/TextView.h>


using namespace ViewDesign;


struct TextViewStyle : TextView::Style {
	TextViewStyle() {
		font.size(75).color(Color::Black);
		// paragraph.flow_direction(FlowDirection::RightToLeft).read_direction(ReadDirection::TopToBottom);
	}
};

static const u16string title = u"TextViewTest";
static const u16string text = u"Hello World!\n你好，世界！";


void App() {
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			title,
			new CenterFrame<Fixed, Fixed>(
				new TextView(TextViewStyle(), text)
			)
		)
	);
	desktop.EventLoop();

	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			title,
			new ClipFrame<Fixed, Fixed, TopLeft>(
				new TextView(TextViewStyle(), text)
			)
		)
	);
	desktop.EventLoop();

	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			title,
			new ClipFrame<Fixed, Fixed, BottomRight>(
				new TextView(TextViewStyle(), text)
			)
		)
	);
	desktop.EventLoop();

	desktop.AddWindow(
		new UndecoratedWindow(
			UndecoratedWindow::Style(),
			title,
			new CenterFrame<Fixed, Fixed>(
				new TextView(TextViewStyle(), text)
			)
		)
	);
	desktop.EventLoop();

	desktop.AddWindow(
		new TitleBarWindow(
			TitleBarWindow::Style(),
			title,
			new CenterFrame<Fixed, Fixed>(
				new TextView(TextViewStyle(), text)
			)
		)
	);
	desktop.EventLoop();
}
