#include "ViewDesign/view/Global.h"
#include "ViewDesign/view/frame/BorderFrame.h"
#include "ViewDesign/view/layout/OverlapLayout.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/view/wrapper/HitTestHelper.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"
#include "ViewDesign/event/mouse_tracker.h"
#include "ViewDesign/event/key_tracker.h"


using namespace ViewDesign;


struct MainWindowStyle : TitleBarWindow::Style {
	MainWindowStyle() {
		title.text.assign(L"OverlapLayoutTest");
		background_color = color_transparent;
	}
};


class MainView : public OverlapLayout {
public:
	MainView() {}

private:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown: AddChild(new Frame(event.point)); break;
		}
	}

private:
	class View : public SolidColorBackground<Placeholder<Fixed, Fixed>> {
	public:
		View(Color background_color) { background = background_color; }
	public:
		void SetColor(Color color) { background = color; Redraw(region_infinite); }
	};

	class Frame : public HitSelf<OverlapFrame> {
	public:
		Frame(Point point) : Base(
			new BorderFrame(
				Border(5px, Color::Wheat),
				view = new View(Color(Color::Indigo, 64))
			)
		), region(point, Size(300, 300)) {
			UpdateChildSizeRef(child, region.size);
		}

		// child
	private:
		ref_ptr<View> view;

		// layout
	private:
		Rect region;
	private:
		virtual Rect OnOverlapFrameSizeRefUpdate(Size size_ref) override { return region; }

		// event
	private:
		MouseTracker mouse_tracker;
		KeyTracker key_tracker;
	private:
		virtual void OnMouseEvent(MouseEvent event) override {
			switch (event.type) {
			case MouseEvent::LeftDown: SetCapture(); break;
			case MouseEvent::LeftUp: ReleaseCapture(); break;
			}
			switch (mouse_tracker.Track(event)) {
			case MouseTrackEvent::LeftClick:
				SetFocus();
				view->SetColor(Color(Color::Indigo, 128));
				break;
			case MouseTrackEvent::LeftDrag:
				region.point += event.point - mouse_tracker.mouse_down_position;
				OverlapFrameRegionUpdated(region);
				break;
			case MouseTrackEvent::LeftDoubleClick:
				GetParent().RemoveChild(*this);
				break;
			}
		}
		virtual void OnKeyEvent(KeyEvent event) override {
			key_tracker.Track(event);
			if (event.type == KeyEvent::KeyDown) {
				switch (event.key) {
				case CharKey('F'): key_tracker.ctrl ? BringToFront() : BringForward(); break;
				case CharKey('B'): key_tracker.ctrl ? SendToBack() : SendBackward(); break;
				}
			}
		}
		virtual void OnFocusEvent(FocusEvent event) override {
			if (event == FocusEvent::Blur) {
				view->SetColor(Color(Color::Indigo, 64));
			}
		}
	};
};


int main() {
	global.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new MainView
		)
	);
	global.EventLoop();
}
