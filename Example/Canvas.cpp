#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/control/Placeholder.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/view/figure/shape.h>


using namespace ViewDesign;


class CanvasView : public DefaultBackground<Placeholder<Fixed, Fixed>> {
	// layout
private:
	Size size;
private:
	// Placeholder<Fixed, Fixed> doesn't have a `size` member and just returns `size_ref` as the original `ViewBase::OnSizeRefUpdate()`
	// here we manually save the size
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size = size_ref; }

	// drawing
private:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		Base::OnDraw(canvas, draw_region);
		canvas.draw(point_zero, new Line(Vector(size.width - 30.0f, 20.0f), Vector(size.width * 2 / 3, size.height - 20.0f), 10.0f, ColorCode::Violet));
		canvas.draw(Point(10.0f, 10.0f), new Rectangle(size / Scale(3.0f), ColorCode::Orange, 2.0f, ColorCode::DarkOrange));
		canvas.draw(Point(size.width / 4, 50.0f), new RoundedRectangle(size / Scale(2.0f), 50.0f, Color(ColorCode::Cyan, 0x50), 5.0f, ColorCode::PaleGreen));
		canvas.draw(Point(size.width / 3, size.height * 2 / 3), new Ellipse(size.width / 4, size.height / 6, ColorCode::AliceBlue, 3.0f, ColorCode::Beige));
		canvas.draw(Point(size.width / 3, size.height * 2 / 3), new Circle(1.0f, ColorCode::Black));
	}
};


void App() {
	desktop.AddWindow(
		new DefaultWindow(
			DefaultWindow::Style(),
			u"Canvas",
			new CanvasView()
		)
	);
	desktop.EventLoop();
}
