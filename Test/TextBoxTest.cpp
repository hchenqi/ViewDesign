#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/widget/UndecoratedWindow.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/control/TextBox.h"


using namespace ViewDesign;


struct TextBoxStyle : TextBox::Style {
	TextBoxStyle() {
		font.size(75).color(Color::Black);
		// paragraph.flow_direction(FlowDirection::RightToLeft).read_direction(ReadDirection::TopToBottom);
	}
};

static const u16string title = u"TextBoxTest";
static const u16string text = u"Hello World!\n你好，世界！";


void App() {
	desktop.AddWindow(
		new DefaultWindow(
			title, DefaultWindow::Style(),
			new CenterFrame<Fixed, Fixed>(
				new TextBox(TextBoxStyle(), text)
			)
		)
	);
	desktop.EventLoop();

	desktop.AddWindow(
		new DefaultWindow(
			title, DefaultWindow::Style(),
			new ClipFrame<Fixed, Fixed, TopLeft>(
				new TextBox(TextBoxStyle(), text)
			)
		)
	);
	desktop.EventLoop();

	desktop.AddWindow(
		new DefaultWindow(
			title, DefaultWindow::Style(),
			new ClipFrame<Fixed, Fixed, BottomRight>(
				new TextBox(TextBoxStyle(), text)
			)
		)
	);
	desktop.EventLoop();

	desktop.AddWindow(
		new UndecoratedWindow(
			title, UndecoratedWindow::Style(),
			new CenterFrame<Fixed, Fixed>(
				new TextBox(TextBoxStyle(), text)
			)
		)
	);
	desktop.EventLoop();

	desktop.AddWindow(
		new TitleBarWindow(
			title, TitleBarWindow::Style(),
			new CenterFrame<Fixed, Fixed>(
				new TextBox(TextBoxStyle(), text)
			)
		)
	);
	desktop.EventLoop();
}
