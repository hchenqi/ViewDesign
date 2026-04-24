#include "ViewDesign/view/Desktop.h"
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
		title.text.assign(u"OverlapLayoutTest");
		background_color = color_transparent;
	}
};


class MainView : public OverlapLayout {
private:
	class Window : public HitSelf<OverlapLayout::Window> {
	private:
		class View : public SolidColorBackground<Placeholder<Fixed, Fixed>> {
		public:
			View() { background = background_color; }
		private:
			constexpr static Color background_color = Color(Color::Orange, 0x40);
			constexpr static Color background_color_focus = Color(Color::Orange, 0x80);
		public:
			void Focus() { background = background_color_focus; Redraw(region_infinite); }
			void Blur() { background = background_color; Redraw(region_infinite); }
		};

	public:
		Window(Point point) : Base(
			new BorderFrame(
				Border(3.0f, 0.0f, Color::Yellow),
				view = new View()
			)
		), region(point, Size(300.0f, 300.0f)) {
			UpdateChildSizeRef(child, region.size);
		}

		// child
	private:
		ref_ptr<View> view;

		// layout
	private:
		Rect region;
	private:
		virtual Rect OnWindowSizeRefUpdate(Size size_ref) override { return region; }

		// event
	private:
		MouseTracker mouse_tracker;
		KeyTracker key_tracker;
	private:
		virtual void OnMouseEvent(MouseEvent event) override {
			switch (event.type) {
			case MouseEvent::LeftDown: SetFocus(); SetCapture(); break;
			case MouseEvent::LeftUp: ReleaseCapture(); break;
			}
			switch (mouse_tracker.Track(event)) {
			case MouseTrackEvent::LeftDrag:
				region.point += event.point - mouse_tracker.mouse_down_position;
				RegionUpdated(region);
				break;
			case MouseTrackEvent::LeftDoubleClick:
				GetParent().RemoveWindow(*this);
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
			switch (event) {
			case FocusEvent::Focus: view->Focus(); break;
			case FocusEvent::Blur: view->Blur(); break;
			}
		}
	};

public:
	MainView() {}

private:
	virtual void OnMouseEvent(MouseEvent event) override {
		switch (event.type) {
		case MouseEvent::LeftDown: AddWindow(new Window(event.point)); break;
		}
	}
};


void App() {
	desktop.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new MainView
		)
	);
	desktop.EventLoop();
}
