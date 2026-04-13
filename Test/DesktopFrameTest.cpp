#include "ViewDesign/view/Global.h"
#include "ViewDesign/view/DesktopFrame.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/view/wrapper/Cursor.h"
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


class EmptyView : public CustomizedCursor<SolidColorBackground<Placeholder<Fixed, Fixed>, Color::Gray>, Cursor::Hand> {
public:
	EmptyView() {}
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
	global.Add(new MainFrame(L"DesktopFrameTest", new EmptyView()));
	global.EventLoop();

	global.Add(new MainFrame(L"DesktopFrameTest", new ViewFrame(new EmptyView())));
	global.EventLoop();

	std::unique_ptr<EmptyView> view(new EmptyView());
	global.Add(new MainFrame(L"DesktopFrameTest", new ViewFrameRef(*view)));
	global.EventLoop();
}
