#include "ViewDesign/window/Global.h"
#include "ViewDesign/layout/OverlapLayout.h"
#include "ViewDesign/frame/BorderFrame.h"
#include "ViewDesign/widget/TitleBarFrame.h"
#include "ViewDesign/message/mouse_tracker.h"
#include "ViewDesign/message/key_tracker.h"
#include "ViewDesign/wrapper/Background.h"
#include "ViewDesign/wrapper/HitTestHelper.h"


using namespace ViewDesign;


struct MainFrameStyle : TitleBarFrame::Style {
	MainFrameStyle() {
		title.text.assign(L"OverlapLayoutTest");
		background_color = color_transparent;
	}
};


class MainWindow : public OverlapLayout {
public:
	MainWindow() {}

private:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown: AddChild(new Frame(event.point)); break;
		}
	}

private:
	class Wnd : public SolidColorBackground<Placeholder<Assigned, Assigned>> {
	public:
		Wnd(Color background_color) { background = background_color; }
	public:
		void SetColor(Color color) { background = color; Redraw(region_infinite); }
	};

	class Frame : public HitSelf<OverlapFrame> {
	public:
		Frame(Point point) : Base(
			new BorderFrame(
				Border(5px, Color::Wheat),
				wnd = new Wnd(Color(Color::Indigo, 64))
			)
		), region(point, Size(300, 300)) {
			UpdateChildSizeRef(child, region.size);
		}

		// child
	private:
		ref_ptr<Wnd> wnd;

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
				wnd->SetColor(Color(Color::Indigo, 128));
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
				wnd->SetColor(Color(Color::Indigo, 64));
			}
		}
	};
};


int main() {
	global.AddWnd(
		new TitleBarFrame(
			MainFrameStyle(),
			new MainWindow
		)
	);
	global.EventLoop();
}
