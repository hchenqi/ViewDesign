#include "ViewDesign/window/Global.h"
#include "ViewDesign/frame/DesktopFrame.h"
#include "ViewDesign/control/Placeholder.h"
#include "ViewDesign/wrapper/Background.h"
#include "ViewDesign/wrapper/Cursor.h"
#include "ViewDesign/system/win32_aero_snap.h"


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
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::RightDown:
			SetFocus();
			background = background == Color::Gray ? Color::White : Color::Gray;
			Redraw(region_infinite);
			break;
		case MouseEvent::LeftDown: AeroSnapDraggingEffect(*this); break;
		}
	}
	virtual void OnKeyEvent(KeyEvent event) override {
		if (event.key == Key::Escape) {
			global.Terminate();
		}
	}
};


int main() {
	global.AddWnd(new MainFrame(L"DesktopFrameTest", new EmptyWindow()));
	global.EventLoop();

	global.AddWnd(new MainFrame(L"DesktopFrameTest", new WndFrame(new EmptyWindow())));
	global.EventLoop();

	std::unique_ptr<EmptyWindow> window(new EmptyWindow());
	global.AddWnd(new MainFrame(L"DesktopFrameTest", new WndFrameRef(*window)));
	global.EventLoop();
}
