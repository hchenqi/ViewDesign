#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/view/wrapper/Cursor.h"
#include "ViewDesign/system/win32_aero_snap.h"


using namespace ViewDesign;


class MainWindow : public Window {
public:
	using Window::Window;
private:
	Size size = Size(800, 500);
private:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) {
		return { Size(100, 100), size_ref };
	}
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) override {
		Rect region;
		region.size = UpdateChildSizeRef(child, size);
		region.point.x = (size_ref.width - region.size.width) / 2;
		region.point.y = (size_ref.height - region.size.height) / 2;
		return region;
	}
};


class EmptyView : public CustomizedCursor<SolidColorBackground<Placeholder<Fixed, Fixed>, Color(Color::Yellow, 0x7F)>, Cursor::Hand> {
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown:
			SetFocus();
			AeroSnapDraggingEffect(*this);
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


int main() {
	desktop.AddWindow(new MainWindow(L"WindowTest", new EmptyView()));
	desktop.EventLoop();

	desktop.AddWindow(new MainWindow(L"WindowTest", new ViewFrame(new EmptyView())));
	desktop.EventLoop();

	desktop.AddWindow(new MainWindow(L"WindowTest", new ViewFrameMutable(new EmptyView())));
	desktop.EventLoop();

	std::unique_ptr<EmptyView> view(new EmptyView());

	desktop.AddWindow(new MainWindow(L"WindowTest", new ViewFrameRef(*view)));
	desktop.EventLoop();

	desktop.AddWindow(new MainWindow(L"WindowTest", new ViewFrameRefMutable(*view)));
	desktop.EventLoop();
}
