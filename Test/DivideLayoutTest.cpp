#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/BorderFrame.h"
#include "ViewDesign/view/layout/DivideLayout.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"
#include "ViewDesign/figure/shape.h"


using namespace ViewDesign;


struct MainWindowStyle : TitleBarWindow::Style {
	MainWindowStyle() {
		title.text.assign(L"DivideLayoutTest");
	}
};


template<class View>
class Background : public View {
public:
	using View::View;
private:
	bool show_background = false;
	bool show_background_focus = false;
private:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		if (show_background || show_background_focus) {
			canvas.draw(draw_region.point, new Rectangle(draw_region.size, Color(Color::Orange, 0x40)));
		}
		View::OnDraw(canvas, draw_region);
	}
private:
	virtual void OnFocusEvent(FocusEvent event) override {
		switch (event) {
		case FocusEvent::MouseEnter: show_background = true; View::Redraw(region_infinite); break;
		case FocusEvent::MouseLeave: show_background = false; View::Redraw(region_infinite); break;
		case FocusEvent::FocusIn: show_background_focus = true; View::Redraw(region_infinite); break;
		case FocusEvent::FocusOut: show_background_focus = false; View::Redraw(region_infinite); break;
		}
	}
};


class Block : public Placeholder<Fixed, Fixed> {
public:
	using Placeholder::Placeholder;
private:
	Size size;
private:
	virtual Size OnSizeRefUpdate(Size size_ref) override { return size = size_ref; }
private:
	bool show_border = false;
	bool show_border_focus = false;
private:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
		if (show_border || show_border_focus) {
			canvas.draw(point_zero, new Rectangle(size, show_border + show_border_focus, Color::Orange));
		}
	}
private:
	virtual void OnMouseEvent(MouseEvent event) {
		if (event.type == MouseEvent::LeftDown) {
			SetFocus();
		}
	}
	virtual void OnFocusEvent(FocusEvent event) override {
		switch (event) {
		case FocusEvent::MouseOver: show_border = true; Redraw(region_infinite); break;
		case FocusEvent::MouseOut: show_border = false; Redraw(region_infinite); break;
		case FocusEvent::Focus: show_border_focus = true; Redraw(region_infinite); break;
		case FocusEvent::Blur: show_border_focus = false; Redraw(region_infinite); break;
		}
	}
};


int main() {
	desktop.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new Background<DivideLayout<Vertical>>(
				new Background<DivideLayout<Horizontal>>(
					new Block(),
					new Block(),
					new Block()
				),
				new Background<DivideLayout<Horizontal>>(
					new Block(),
					new Block(),
					new Block()
				),
				new Background<DivideLayout<Horizontal>>(
					new Block(),
					new Block(),
					new Block()
				)
			)
		)
	);
	desktop.EventLoop();
}
