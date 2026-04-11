#include "WndDesign/window/Global.h"
#include "WndDesign/layout/OverlapLayout.h"
#include "WndDesign/frame/BorderFrame.h"
#include "WndDesign/widget/TitleBarFrame.h"
#include "WndDesign/message/mouse_tracker.h"
#include "WndDesign/message/key_tracker.h"
#include "WndDesign/wrapper/Background.h"
#include "WndDesign/wrapper/HitTestHelper.h"


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
	virtual void OnMouseMsg(MouseMsg msg) override {
		switch (msg.type) {
		case MouseMsg::LeftDown: AddChild(new Frame(msg.point)); break;
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

		// message
	private:
		MouseTracker mouse_tracker;
		KeyTracker key_tracker;
	private:
		virtual void OnMouseMsg(MouseMsg msg) override {
			switch (msg.type) {
			case MouseMsg::LeftDown: SetCapture(); break;
			case MouseMsg::LeftUp: ReleaseCapture(); break;
			}
			switch (mouse_tracker.Track(msg)) {
			case MouseTrackMsg::LeftClick:
				SetFocus();
				wnd->SetColor(Color(Color::Indigo, 128));
				break;
			case MouseTrackMsg::LeftDrag:
				region.point += msg.point - mouse_tracker.mouse_down_position;
				OverlapFrameRegionUpdated(region);
				break;
			case MouseTrackMsg::LeftDoubleClick:
				GetParent().RemoveChild(*this);
				break;
			}
		}
		virtual void OnKeyMsg(KeyMsg msg) override {
			key_tracker.Track(msg);
			if (msg.type == KeyMsg::KeyDown) {
				switch (msg.key) {
				case CharKey('F'): key_tracker.ctrl ? BringToFront() : BringForward(); break;
				case CharKey('B'): key_tracker.ctrl ? SendToBack() : SendBackward(); break;
				}
			}
		}
		virtual void OnNotifyMsg(NotifyMsg msg) override {
			if (msg == NotifyMsg::Blur) {
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
	global.MessageLoop();
}
