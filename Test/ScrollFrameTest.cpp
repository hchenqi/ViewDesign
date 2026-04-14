#include "ViewDesign/view/Global.h"
#include "ViewDesign/view/Window.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/ScaleFrame.h"
#include "ViewDesign/view/frame/ScrollFrame.h"
#include "ViewDesign/view/control/EditBox.h"
#include "ViewDesign/view/wrapper/Background.h"


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


class ScaleLayer : public ScaleFrame<Fixed, Auto> {
public:
	ScaleLayer(child_type child) : ScaleFrame(Scale(1.0), std::move(child)) {}
private:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override {
		if (event.ctrl && event.type == MouseEvent::WheelVertical) {
			return this;
		}
		return ScaleFrame::HitTest(event);
	}
private:
	virtual void OnMouseEvent(MouseEvent event) override {
		SetScale(scale * Scale(powf(1.1f, event.wheel_delta / 120.0f)));
	}
};


class MyEditBox : public SolidColorBackground<EditBox> {
private:
	struct Style : EditBox::Style {
		Style() {
			paragraph.line_spacing(25px);
			paragraph.baseline_spacing(20px);
			font.family(L"Calibri").size(20px);
		}
	};
protected:
	MyEditBox() : Base(Style(), L"Hello World!") {}
public:
	static auto Create() {
		return new ClipFrame<Fixed, Auto, Left>(new PaddingFrame(Padding(30px), new MyEditBox()));
	}
};


int main() {
	global.AddWindow(
		new MainWindow(
			L"ScrollFrameTest",
			new ScrollFrame<Vertical>(
				new ScaleLayer(
					MyEditBox::Create()
				)
			)
		)
	);
	global.EventLoop();
}
