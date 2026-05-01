#pragma once

#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/frame/MutableFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/frame/BorderFrame.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/frame/FixedFrame.h"
#include "ViewDesign/view/frame/BackgroundFrame.h"
#include "ViewDesign/view/layout/SplitLayout.h"
#include "ViewDesign/view/layout/StackLayout.h"
#include "ViewDesign/view/layout/ListLayout.h"
#include "ViewDesign/view/control/TextBox.h"
#include "ViewDesign/view/control/Button.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/view/wrapper/Cursor.h"
#include "ViewDesign/view/wrapper/HitTestHelper.h"
#include "ViewDesign/view/widget/Tooltip.h"
#include "ViewDesign/style/length_style_helper.h"
#include "ViewDesign/event/mouse_tracker.h"
#include "ViewDesign/messaging/context.h"
#include "ViewDesign/geometry/border_helper.h"

#if defined(VIEWDESIGN_BACKEND_WIN32)
#include "ViewDesign/platform/win32/aero_snap.h"
#endif


namespace ViewDesign {


class TitleBarWindow : public Window, ContextProvider {
public:
	using child_type = view_ptr<Fixed, Fixed>;
	using menu_type = view_ptr<Auto, Fixed>;

public:
	struct TitleBarStyle : TextBlockStyle {
		struct BarStyle {
		public:
			float _height = 30.0f;
			float _max_title_length = 300.0f;
			Color _background_color = Color::DarkGray;
			Color _foreground_color = Color::White;
		public:
			constexpr BarStyle& height(float height) { _height = height; return *this; }
			constexpr BarStyle& max_title_length(float max_title_length) { _max_title_length = max_title_length; return *this; }
			constexpr BarStyle& background_color(Color background_color) { _background_color = background_color; return *this; }
			constexpr BarStyle& foreground_color(Color foreground_color) { _foreground_color = foreground_color; return *this; }
		}bar;

		u16string text;
	};

	struct Style {
		LengthStyle width;
		LengthStyle height;
		PositionStyle position;
		Padding padding;
		Border border;
		Color background_color;
		TitleBarStyle title;

		Style() {
			width.normal(px(800)).min(px(200)).max(pct(100));
			height.normal(px(500)).min(px(200)).max(pct(100));
			position.setHorizontalCenter().setVerticalCenter();
			padding = Padding(2.0f);
			border.width(2.0f).radius(2.0f).color(Color::CadetBlue);
			background_color = Color::White;
			title.bar.background_color(Color::CadetBlue);
			title.text.assign(u"TitleBarWindow");
			title.font.color(Color::White);
			title.paragraph.line_spacing(pct(100));
		}
	};

protected:
	class ResizeBorder : public CustomizedCursor<HitSelfFallback<BorderFrame<Fixed, Fixed>>, CursorStyle::NoChange> {
	public:
		using Base::Base;
	protected:
		virtual void OnMouseEvent(MouseEvent event) override {
			if (event.type == MouseEvent::Move || event.type == MouseEvent::LeftDown) {
				BorderPosition border_position = HitTestBorderPosition(size, border._width + border._radius, event.point);
				if (event.type == MouseEvent::Move) {
#if defined(VIEWDESIGN_BACKEND_WIN32)
					SetCursor(GetCursor(GetBorderPositionCursorStyle(border_position)));
#endif
				} else {
#if defined(VIEWDESIGN_BACKEND_WIN32)
					Win32::AeroSnapBorderResizingEffect(desktop.GetWindow(*this).GetHandle(), border_position);
#endif
				}
			}
		}
	};

	class TitleBar : public HitSelfFallback<SolidColorBackground<FixedFrame<Fixed, Auto>>>, Context, ContextProvider {
	public:
		class Title : public TextBox {
		public:
			Title(const TitleBarStyle& style) : TextBox(style, style.text) {}
		};

	protected:
		class ButtonBase : public Button<Auto, Fixed>, protected Context {
		public:
			ButtonBase(Color background, Color foreground, const u16string& tooltip_text) : Button<Auto, Fixed>(50.0f), Context(AsViewBase()), foreground(foreground), tooltip_text(tooltip_text) {
				this->background = this->background_normal = background;
			}
			~ButtonBase() {
				DestroyTooltip(*this);
			}
		protected:
			Color foreground;
			u16string tooltip_text;
		protected:
			virtual void OnFocusEvent(FocusEvent event) override {
				Button::OnFocusEvent(event);
				switch (event) {
				case FocusEvent::MouseEnter: ShowTooltip(*this, tooltip_text); break;
				case FocusEvent::MouseLeave: HideTooltip(*this); break;
				}
			}
		};

		class MinimizeButton : public ButtonBase {
		public:
			using ButtonBase::ButtonBase;
		protected:
			virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
				ButtonBase::OnDraw(canvas, draw_region);
				canvas.draw(Point(20.0f, 15.0f), new Rectangle(Size(10.0f, 1.0f), 1.0f, foreground));
			}
		protected:
			virtual void OnClick() override { Context::Get<TitleBarWindow>().Minimize(); }
		};

		class MaximizeButton : public ButtonBase {
		public:
			using ButtonBase::ButtonBase;
		protected:
			virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
				ButtonBase::OnDraw(canvas, draw_region);
				if (Context::Get<TitleBarWindow>().IsMaximized()) {
					canvas.draw(Point(22.0f, 10.0f), new Rectangle(Size(8.0f, 8.0f), 1.0f, foreground));
					canvas.draw(Point(20.0f, 12.0f), new Rectangle(Size(8.0f, 8.0f), background, 1.0f, foreground));
				} else {
					canvas.draw(Point(20.0f, 10.0f), new Rectangle(Size(10.0f, 10.0f), 1.0f, foreground));
				}
			}
		protected:
			virtual void OnClick() override { Context::Get<TitleBarWindow>().MaximizeOrRestore(); }
		protected:
			virtual void OnFocusEvent(FocusEvent event) override {
				Button::OnFocusEvent(event);
				switch (event) {
				case FocusEvent::MouseEnter: ShowTooltip(*this, Context::Get<TitleBarWindow>().IsMaximized() ? u"restore" : u"maximize"); break;
				case FocusEvent::MouseLeave: HideTooltip(*this); break;
				}
			}
		};

		class CloseButton : public ButtonBase {
		public:
			using ButtonBase::ButtonBase;
		protected:
			virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
				ButtonBase::OnDraw(canvas, draw_region);
				canvas.draw(Point(20.0f, 10.0f), new Line(Vector(10.0f, 10.0f), 1.0f, foreground));
				canvas.draw(Point(20.0f, 20.0f), new Line(Vector(10.0f, -10.0f), 1.0f, foreground));
			}
		protected:
			virtual void OnClick() override { Context::Get<TitleBarWindow>().Close(); }
		};

	public:
		TitleBar(const TitleBarStyle::BarStyle& style, menu_type menu, owner_ptr<Title> title) : Base(
			style._height,
			new StackLayoutMultiple(
				create<HitThrough<CenterFrame<Fixed, Fixed>>>(
					new MaxFrame(
						Size(style._max_title_length, length_max),
						std::move(title)
					)
				),
				create<HitThroughMargin<ClipFrame<Fixed, Fixed, Left>>>(
					std::move(menu)
				),
				create<HitThroughMargin<ClipFrame<Fixed, Fixed, Right>>>(
					new ListLayoutHorizontal(
						0.0f,
						create<MinimizeButton>(style._background_color, style._foreground_color, u"minimize"),
						create<MaximizeButton>(style._background_color, style._foreground_color, u""),
						create<CloseButton>(style._background_color, style._foreground_color, u"close")
					)
				)
			)
		), Context(AsViewBase()), ContextProvider(AsViewBase()) {
			background = style._background_color;
		}

	protected:
		MouseTracker mouse_tracker;
	protected:
		virtual void OnMouseEvent(MouseEvent event) override {
			switch (mouse_tracker.Track(event)) {
#if defined(VIEWDESIGN_BACKEND_WIN32)
			case MouseTrackEvent::LeftDown: Win32::AeroSnapDraggingEffect(desktop.GetWindow(*this).GetHandle()); break;
#endif
			case MouseTrackEvent::LeftDoubleClick: Context::Get<TitleBarWindow>().MaximizeOrRestore(); break;
			}
		}
	};

public:
	TitleBarWindow(Style style, child_type child, menu_type menu = new Placeholder<Auto, Fixed>(0.0f)) : Window(
		style.title.text,
		outer_frame = new MutableFrame(
			new PaddingFrame(
				style.padding,
				border = new ResizeBorder(
					style.border,
					inner_frame = new MutableFrame(
						new SplitLayoutVertical(
							new TitleBar(
								style.title.bar,
								std::move(menu),
								title = new TitleBar::Title(style.title)
							),
							new BackgroundFrame(
								style.background_color,
								std::move(child)
							)
						)
					)
				)
			)
		)
	), ContextProvider(AsViewBase()), style(style) {}

	// style
protected:
	Style style;
protected:
	ref_ptr<ResizeBorder> border;
	ref_ptr<TitleBar::Title> title;
public:
	void SetTitle(u16string str) { title->Assign(str); Window::SetTitle(str); }

	// layout
protected:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) override {
		return LengthStyleHelper::CalculateMinMaxSize(style.width, style.height, size_ref);
	}
	virtual Rect OnWindowSizeRefUpdate(Size size_ref) override {
		Rect region = LengthStyleHelper::CalculateRegion(style.width, style.height, style.position, size_ref);
		UpdateChildSizeRef(child, region.size);
		return region;
	}

	// state
protected:
	ref_ptr<MutableFrame<Fixed, Fixed>> outer_frame;
	ref_ptr<MutableFrame<Fixed, Fixed>> inner_frame;
	view_ptr<Fixed, Fixed> inner_frame_placeholder = new Placeholder<Fixed, Fixed>;
	view_ptr<Fixed, Fixed> outer_frame_placeholder = view_ptr<Fixed, Fixed>();
protected:
	bool IsMaximized() { return GetState() == State::Maximized; }
	void MaximizeOrRestore() { if (GetState() == State::Normal) { Maximize(); } else if (GetState() == State::Maximized) { Restore(); } }
protected:
	virtual void OnStateChange(State state) override {
		switch (state) {
		case State::Normal: if (outer_frame_placeholder) { inner_frame->RotateWith(*outer_frame, outer_frame_placeholder, inner_frame_placeholder); } break;
		case State::Maximized: if (inner_frame_placeholder) { inner_frame->RotateWith(inner_frame_placeholder, outer_frame_placeholder, *outer_frame); } break;
		}
	}
};


} // namespace ViewDesign
