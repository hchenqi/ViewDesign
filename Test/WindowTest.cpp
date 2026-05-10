#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/LayerFrame.h"
#include "ViewDesign/view/frame/MutableFrame.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/view/wrapper/Cursor.h"
#include "ViewDesign/drawing/bitmap.h"

#if defined(VIEWDESIGN_BACKEND_WIN32)
#include "ViewDesign/platform/win32/aero_snap.h"
#endif


using namespace ViewDesign;


class MainWindow : public Window {
public:
	using Window::Window;
private:
	Size size = Size(400.0f, 400.0f);
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


class MainView : public Placeholder<Fixed, Fixed> {
	// layout
private:
	Size size;
	Size patch_size;
private:
	virtual Size OnSizeRefUpdate(Size size_ref) override {
		size = size_ref;
		patch_size = Size(size.width / 2, size.height / 2);
		bitmap_11.Update([&](PixelBuffer& pixel_buffer) { pixel_buffer.Resize(size.width, size.height, background_11); });
		return size;
	}

	// drawing
private:
	static constexpr uint background_00 = 0xF2521C;
	static constexpr uint background_01 = 0x80CC29;
	static constexpr uint background_10 = 0x00ADF0;
	static constexpr uint background_11 = 0xFABD0A;
private:
	uchar alpha = 0x7F;
private:
	Bitmap bitmap_11;
private:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		canvas.draw(point_zero, new Rectangle(patch_size, Color(background_00, alpha)));
		canvas.Offset(Vector(patch_size.width, 0), [&]() {
			canvas.draw(point_zero, new Rectangle(patch_size, Color(background_01, alpha)));
		});
		canvas.Group(Vector(0, patch_size.height), Rect(0, 0, size.width, patch_size.height), [&]() {
			canvas.draw(point_zero, new Rectangle(patch_size, Color(background_10, alpha)));
			canvas.draw(Point(patch_size.width, 0), new BitmapFigure(bitmap_11, alpha / (float)0xFF));
		});
	}

	// event
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown:
			SetFocus();
#if defined(VIEWDESIGN_BACKEND_WIN32)
			Win32::AeroSnapDraggingEffect(desktop.GetWindow(*this).GetHandle());
#endif
			break;
		case MouseEvent::WheelVertical:
			alpha = std::max(0x00, std::min(0xFF, alpha + event.wheel_delta / 4));
			Redraw(region_infinite);
			break;
		}
	}
	virtual void OnKeyEvent(KeyEvent event) override {
		if (event.key == Key::Escape) {
			desktop.GetWindow(*this).Close();
		}
	}
};


void App() {
	desktop.AddWindow(new MainWindow(u"WindowTest", new MainView()));
	desktop.EventLoop();

	std::unique_ptr<MainView> view(new MainView());
	desktop.AddWindow(new MainWindow(u"WindowTest", new ViewFrame(new MainView())));
	desktop.AddWindow(new MainWindow(u"WindowTest", new ViewFrameRef(*view)));
	// desktop.AddWindow(new MainWindow(u"WindowTest", new LayerFrame(new MainView())));
	desktop.AddWindow(new MainWindow(u"WindowTest", new MutableFrame(new MainView())));
	desktop.EventLoop();
}
