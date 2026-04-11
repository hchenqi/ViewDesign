#include "WndDesign/window/Global.h"
#include "WndDesign/frame/DesktopFrame.h"
#include "WndDesign/control/Placeholder.h"
#include "WndDesign/wrapper/Background.h"
#include "WndDesign/wrapper/Cursor.h"
#include "WndDesign/system/win32_aero_snap.h"


using namespace ViewDesign;


class MainFrame : public DesktopFrame {
public:
	using DesktopFrame::DesktopFrame;
private:
	Rect region = Rect(100, 100, 800, 500);
private:
	virtual Rect OnDesktopFrameSizeRefUpdate(Size size_ref) override {
		region.size = UpdateChildSizeRef(child, region.size);
		return region;
	}
};


class EmptyWindow : public CustomizedCursor<SolidColorBackground<Placeholder<Assigned, Assigned>, Color::Gray>, Cursor::Hand> {
public:
	EmptyWindow() {}
private:
	virtual void OnMouseMsg(MouseMsg msg) override {
		switch (msg.type) {
		case MouseMsg::RightDown:
			SetFocus();
			background = background == Color::Gray ? Color::White : Color::Gray;
			Redraw(region_infinite);
			break;
		case MouseMsg::LeftDown: AeroSnapDraggingEffect(*this); break;
		}
	}
	virtual void OnKeyMsg(KeyMsg msg) override {
		if (msg.key == Key::Escape) {
			global.Terminate();
		}
	}
};


int main() {
	global.AddWnd(new MainFrame(L"DesktopFrameTest", new EmptyWindow()));
	global.MessageLoop();

	global.AddWnd(new MainFrame(L"DesktopFrameTest", new WndFrame(new EmptyWindow())));
	global.MessageLoop();

	std::unique_ptr<EmptyWindow> window(new EmptyWindow());
	global.AddWnd(new MainFrame(L"DesktopFrameTest", new WndFrameRef(*window)));
	global.MessageLoop();
}
