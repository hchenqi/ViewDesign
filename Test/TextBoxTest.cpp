#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/control/TextBox.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"


using namespace ViewDesign;


struct MainWindowStyle : TitleBarWindow::Style {
	MainWindowStyle() {
		title.text.assign(L"TextBoxTest");
	}
};


struct TextBoxStyle : TextBox::Style {
	TextBoxStyle() {
		font.size(75).color(Color::Black);
		// paragraph.flow_direction(FlowDirection::RightToLeft).read_direction(ReadDirection::TopToBottom);
	}
};


int main() {
	desktop.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new CenterFrame<Fixed, Fixed>(
				new TextBox(TextBoxStyle(), L"Hello World!\n你好，世界！")
			)
		)
	);
	desktop.EventLoop();
}
