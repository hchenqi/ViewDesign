#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/MutableFrame.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/view/wrapper/Cursor.h"

#if defined(_WIN32)
#include "ViewDesign/platform/win32/aero_snap.h"
#endif


using namespace ViewDesign;


class MainWindow : public Window {
public:
	using Window::Window;
private:
	Size size = Size(800.0f, 500.0f);
private:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) override {
		return { Size(100.0f, 100.0f), size_ref };
	}
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) override {
		Rect region;
		region.size = UpdateChildSizeRef(child, size);
		region.point.x = (size_ref.width - region.size.width) / 2;
		region.point.y = (size_ref.height - region.size.height) / 2;
		return region;
	}
};


class EmptyView : public CustomizedCursor<SolidColorBackground<Placeholder<Fixed, Fixed>, Color(Color::Yellow, 0x80)>, CursorStyle::Hand> {
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown:
			SetFocus();
			Win32::AeroSnapDraggingEffect(*this);
			break;
		case MouseEvent::WheelVertical:
			background.alpha = std::max(0x00, std::min(0xFF, background.alpha + event.wheel_delta / 4));
			Redraw(region_infinite);
			break;
		}
	}
	virtual void OnKeyEvent(KeyEvent event) override {
		if (event.key == Key::Escape) {
			desktop.Terminate();
		}
	}
};


void App() {
	desktop.AddWindow(new MainWindow(u"WindowTest", new EmptyView()));
	desktop.EventLoop();

	desktop.AddWindow(new MainWindow(u"WindowTest", new ViewFrame(new EmptyView())));
	desktop.EventLoop();

	desktop.AddWindow(new MainWindow(u"WindowTest", new MutableFrame(new EmptyView())));
	desktop.EventLoop();

	std::unique_ptr<EmptyView> view(new EmptyView());

	desktop.AddWindow(new MainWindow(u"WindowTest", new ViewFrameRef(*view)));
	desktop.EventLoop();
}
