#pragma once

#include "../frame/DesktopFrame.h"
#include "../frame/WndFrameMutable.h"
#include "../frame/PaddingFrame.h"
#include "../frame/BorderFrame.h"
#include "../frame/CenterFrame.h"
#include "../frame/ClipFrame.h"
#include "../frame/MaxFrame.h"
#include "../frame/FixedFrame.h"
#include "../frame/BackgroundFrame.h"
#include "../layout/SplitLayout.h"
#include "../layout/StackLayout.h"
#include "../layout/ListLayout.h"
#include "../control/TextBox.h"
#include "../control/Button.h"
#include "../wrapper/Background.h"
#include "../wrapper/Cursor.h"
#include "../wrapper/HitTestHelper.h"
#include "../style/length_style_helper.h"
#include "../message/mouse_tracker.h"
#include "../message/context.h"
#include "../system/cursor.h"
#include "../system/win32_aero_snap.h"
#include "../widget/Tooltip.h"


namespace ViewDesign {


class TitleBarFrame : public DesktopFrame, ContextProvider {
public:
	using child_type = child_ptr<Assigned, Assigned>;
	using child_type_menu = child_ptr<Auto, Assigned>;

public:
	struct TitleBarStyle : TextBlockStyle {
		struct BarStyle {
		public:
			float _height = 30px;
			float _max_title_length = 300px;
			Color _background_color = Color::DarkGray;
			Color _foreground_color = Color::White;
		public:
			constexpr BarStyle& height(float height) { _height = height; return *this; }
			constexpr BarStyle& max_title_length(float max_title_length) { _max_title_length = max_title_length; return *this; }
			constexpr BarStyle& background_color(Color background_color) { _background_color = background_color; return *this; }
			constexpr BarStyle& foreground_color(Color foreground_color) { _foreground_color = foreground_color; return *this; }
		}bar;

		std::wstring text;
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
			width.normal(800px).min(200px).max(100pct);
			height.normal(500px).min(200px).max(100pct);
			position.setHorizontalCenter().setVerticalCenter();
			padding = Padding(2px);
			border.width(2px).radius(2px).color(Color::CadetBlue);
			background_color = Color::White;
			title.bar.background_color(Color::CadetBlue);
			title.text.assign(L"TitleBarFrame");
			title.font.color(Color::White);
			title.paragraph.line_spacing(100pct);
		}
	};

protected:
	class ResizeBorder : public CustomizedCursor<HitSelfFallback<BorderFrame<Assigned, Assigned>>, Cursor::Default> {
	public:
		using Base::Base;
	protected:
		virtual void OnMouseMsg(MouseMsg msg) override {
			if (msg.type == MouseMsg::Move || msg.type == MouseMsg::LeftDown) {
				BorderPosition border_position = HitTestBorderPosition(size, border._width + border._radius, msg.point);
				if (msg.type == MouseMsg::Move) {
					SetCursor(GetBorderPositionCursor(border_position));
				} else {
					AeroSnapBorderResizingEffect(*this, border_position);
				}
			}
		}
	};

	class TitleBar : public HitSelfFallback<SolidColorBackground<FixedFrame<Assigned, Auto>>>, Context, ContextProvider {
	public:
		class Title : public TextBox {
		public:
			Title(const TitleBarStyle& style) : TextBox(style, style.text) {}
		};

	protected:
		class ButtonBase : public Button<Auto, Assigned>, protected Context {
		public:
			ButtonBase(Color background, Color foreground, const std::wstring& tooltip_text) : Button<Auto, Assigned>(50.0f), Context(AsWndObject()), foreground(foreground), tooltip_text(tooltip_text) {
				this->background = this->background_normal = background;
			}
		protected:
			Color foreground;
			std::wstring tooltip_text;
		protected:
			virtual void OnNotifyMsg(NotifyMsg msg) override {
				Button::OnNotifyMsg(msg);
				switch (msg) {
				case NotifyMsg::MouseEnter: TooltipShow(tooltip_text); break;
				case NotifyMsg::MouseLeave: TooltipHide(); break;
				}
			}
		};

		class MinimizeButton : public ButtonBase {
		public:
			using ButtonBase::ButtonBase;
		protected:
			virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
				ButtonBase::OnDraw(figure_queue, draw_region);
				figure_queue.add(Point(20.0f, 15.0f), new Rectangle(Size(10.0f, 1.0f), 1.0f, foreground));
			}
		protected:
			virtual void OnClick() override { Context::Get<TitleBarFrame>().Minimize(); }
		};

		class MaximizeButton : public ButtonBase {
		public:
			using ButtonBase::ButtonBase;
		protected:
			virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
				ButtonBase::OnDraw(figure_queue, draw_region);
				if (Context::Get<TitleBarFrame>().IsMaximized()) {
					figure_queue.add(Point(22.0f, 10.0f), new Rectangle(Size(8.0f, 8.0f), 1.0f, foreground));
					figure_queue.add(Point(20.0f, 12.0f), new Rectangle(Size(8.0f, 8.0f), background, 1.0f, foreground));
				} else {
					figure_queue.add(Point(20.0f, 10.0f), new Rectangle(Size(10.0f, 10.0f), 1.0f, foreground));
				}
			}
		protected:
			virtual void OnClick() override { Context::Get<TitleBarFrame>().MaximizeOrRestore(); }
		};

		class CloseButton : public ButtonBase {
		public:
			using ButtonBase::ButtonBase;
		protected:
			virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
				ButtonBase::OnDraw(figure_queue, draw_region);
				figure_queue.add(Point(20.0f, 10.0f), new Line(Vector(10.0f, 10.0f), 1.0f, foreground));
				figure_queue.add(Point(20.0f, 20.0f), new Line(Vector(10.0f, -10.0f), 1.0f, foreground));
			}
		protected:
			virtual void OnClick() override { Context::Get<TitleBarFrame>().Destroy(); }
		};

	public:
		TitleBar(const TitleBarStyle::BarStyle& style, child_type_menu menu, alloc_ptr<Title> title) : Base(
			style._height,
			new StackLayoutMultiple(
				new HitThrough<CenterFrame<Assigned, Assigned>>(
					new MaxFrame(
						Size(style._max_title_length, length_max),
						std::move(title)
					)
				),
				new HitThroughMargin<ClipFrame<Assigned, Assigned, TopLeft>>(
					std::move(menu)
				),
				new HitThroughMargin<ClipFrame<Assigned, Assigned, TopRight>>(
					new ListLayout<Horizontal>(
						0.0f,
						new MinimizeButton(style._background_color, style._foreground_color, L"minimize"),
						new MaximizeButton(style._background_color, style._foreground_color, L"maximize"),
						new CloseButton(style._background_color, style._foreground_color, L"close")
					)
				)
			)
		), Context(AsWndObject()), ContextProvider(AsWndObject()) {
			background = style._background_color;
		}

	protected:
		MouseTracker mouse_tracker;
	protected:
		virtual void OnMouseMsg(MouseMsg msg) override {
			switch (mouse_tracker.Track(msg)) {
			case MouseTrackMsg::LeftDown: AeroSnapDraggingEffect(*this); break;
			case MouseTrackMsg::LeftDoubleClick: Context::Get<TitleBarFrame>().MaximizeOrRestore(); break;
			}
		}
	};

public:
	TitleBarFrame(Style style, child_type child, child_type_menu menu = new Placeholder<Auto, Assigned>(0.0f)) : DesktopFrame(
		style.title.text,
		outer_frame = new WndFrameMutable(
			new PaddingFrame(
				style.padding,
				border = new ResizeBorder(
					style.border,
					child_ptr<Assigned, Assigned>() = inner_frame = new WndFrameMutable(
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
	), ContextProvider(AsWndObject()), style(style) {}

	// style
protected:
	Style style;
protected:
	ref_ptr<ResizeBorder> border;
	ref_ptr<TitleBar::Title> title;
public:
	void SetTitle(std::wstring str) { title->Assign(str); DesktopFrame::SetTitle(str); }

	// layout
protected:
	virtual std::pair<Size, Size> CalculateMinMaxSize(Size size_ref) override {
		return LengthStyleHelper::CalculateMinMaxSize(style.width, style.height, size_ref);
	}
	virtual Rect OnDesktopFrameSizeRefUpdate(Size size_ref) override {
		Rect region = LengthStyleHelper::CalculateRegion(style.width, style.height, style.position, size_ref);
		UpdateChildSizeRef(child, region.size);
		return region;
	}

	// state
protected:
	ref_ptr<WndFrameMutable> outer_frame;
	ref_ptr<WndFrameMutable> inner_frame;
	child_ptr<Assigned, Assigned> inner_frame_placeholder = new Placeholder<Assigned, Assigned>;
	child_ptr<Assigned, Assigned> outer_frame_placeholder = child_ptr<Assigned, Assigned>();
protected:
	bool IsMaximized() { return GetState() == State::Maximized; }
	void MaximizeOrRestore() { if (GetState() == State::Normal) { Maximize(); } else if (GetState() == State::Maximized) { Restore(); } }
protected:
	virtual void OnStateChange(State state) {
		switch (state) {
		case State::Normal: if (outer_frame_placeholder) { WndFrameMutable::Rotate(*inner_frame, *outer_frame, outer_frame_placeholder, inner_frame_placeholder); } break;
		case State::Maximized: if (inner_frame_placeholder) { WndFrameMutable::Rotate(*inner_frame, inner_frame_placeholder, outer_frame_placeholder, *outer_frame); } break;
		}
	}
};


} // namespace ViewDesign
