#pragma once

#include "ViewDesign/view/layout/StackLayout.h"
#include "ViewDesign/view/layout/SplitLayout.h"
#include "ViewDesign/view/layout/ListLayout.h"
#include "ViewDesign/view/frame/MutableFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/FixedFrame.h"
#include "ViewDesign/view/frame/ScrollFrame.h"
#include "ViewDesign/view/control/TextView.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/wrapper/HitTestHelper.h"
#include "ViewDesign/view/wrapper/Button.h"
#include "ViewDesign/view/wrapper/Background.h"
#include "ViewDesign/view/figure/shape.h"
#include "ViewDesign/messaging/context.h"


namespace ViewDesign {


class TabView : public ViewFrame, public SizeTrait<Fixed, Fixed>, private ContextProvider {
public:
	using header_ptr = view_ptr<Auto, Fixed>;
	using content_ptr = view_ptr<Relative, Relative>;

public:
	struct Style {};

public:
	struct Tab {
	public:
		header_ptr header;
		content_ptr content;
	public:
		Tab(header_ptr header, content_ptr content) : header(std::move(header)), content(std::move(content)) {}
	};

public:
	class HeaderFrame : public HitSelfFallbackRecursive<Button<ViewFrame>>, public SizeTrait<Auto, Fixed>, private Context<TabView>, private ContextProvider {
	private:
		friend class TabView;
	private:
		HeaderFrame(Tab tab) : Base(std::move(tab.header)), Context(AsViewBase()), ContextProvider(AsViewBase()), content(std::move(tab.content)) {}
	private:
		content_ptr content;
	private:
		bool hovered = false;
		bool focused = false;
	private:
		static constexpr Color background_normal = color_transparent;
		static constexpr Color background_hovered_focused = ColorCode::LightGray;
	private:
		virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
			canvas.draw(draw_region.point, new Rectangle(draw_region.size, hovered || focused ? background_hovered_focused : background_normal));
			Base::OnDraw(canvas, draw_region);
		}
	private:
		void OnFocus() { focused = true; Redraw(rect_infinite); }
		void OnBlur() { focused = false; Redraw(rect_infinite); }
	public:
		void Focus() { Context::Get().OnTabFocus(*this); }
		void Close() { Context::Get().OnTabClose(*this); }
	private:
		virtual void OnHover() override { hovered = true; Redraw(rect_infinite); }
		virtual void OnLeave() override { hovered = false; Redraw(rect_infinite); }
		virtual void OnPress() override { Focus(); }
	};

	using ContentFrame = MutableFrame<Relative, Relative>;

public:
	class DefaultHeaderFixed : public HitThrough<PaddingFrame<Auto, Fixed>> {
	public:
		DefaultHeaderFixed(Padding padding, u16string text) : Base(
			padding,
			new CenterFrame<Auto, Fixed>(
				new MaxFrame<Auto, Relative>(
					200.0f,
					new TextView(TextView::Style(), std::move(text))
				)
			)
		) {}
		DefaultHeaderFixed(u16string text) : DefaultHeaderFixed(Padding(10.0f, 0.0f), std::move(text)) {}
	};

	class DefaultHeaderClosable : public StackLayout<Auto, Fixed, Fixed, Fixed> {
	public:
		DefaultHeaderClosable(u16string text) : StackLayout(
			new DefaultHeaderFixed(Padding(10.0f, 0.0f, 27.0f, 0.0f), std::move(text)),
			new CenterFrame<Fixed, Fixed>(
				new HitThroughMargin<ClipFrame<Fixed, Auto, Right>>(
					new PaddingFrame(
						Padding(10.0f),
						new CloseButton()
					)
				)
			)
		) {}

	private:
		class CloseButton : public Button<Placeholder<Auto, Auto>>, private Context<HeaderFrame> {
		private:
			static constexpr float length = 7.0f;
		public:
			CloseButton() : Base(Size(length, length)), Context(AsViewBase()) {}
		private:
			static constexpr Color foreground = ColorCode::White;
			static constexpr Color background_normal = color_transparent;
			static constexpr Color background_hovered = ColorCode::Gray;
			static constexpr Color background_pressed = ColorCode::DimGray;
		private:
			Color background = background_normal;
		private:
			void SetBackground(Color background) {
				if (this->background != background) {
					this->background = background;
					Redraw(rect_infinite);
				}
			}
		private:
			virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
				canvas.draw(Point(length / 2, length / 2), new Circle(length, background));
				canvas.draw(point_zero, new Line(Vector(length, length), 1.0f, foreground));
				canvas.draw(Point(length, 0.0f), new Line(Vector(-length, length), 1.0f, foreground));
			}
		private:
			virtual void OnHover() override { SetBackground(background_hovered); }
			virtual void OnPress() override { SetBackground(background_pressed); }
			virtual void OnLeave() override { SetBackground(background_normal); }
		private:
			virtual void OnClick() override { Context::Get().Close(); }
		};
	};

public:
	TabView(Style style, auto... tab) : ViewFrame(
		new SplitLayoutVertical(
			new FixedFrame<Fixed, Auto>(
				30.0f,
				new ScrollFrame<Horizontal>(
					tab_list = new ListLayoutHorizontal<Fixed>(
						1.0f,
						(view_ptr<Auto, Fixed>(new HeaderFrame(std::move(tab))))...
					)
				)
			),
			new ScrollFrame<Bidirectional>(
				content_frame = new ContentFrame(
					new Placeholder<Fixed, Fixed>()
				)
			)
		)
	), ContextProvider(AsViewBase()) {}

private:
	ref_ptr<ListLayoutHorizontal<Fixed>> tab_list;
	ref_ptr<ContentFrame> content_frame;
	ref_ptr<HeaderFrame> tab_focus = nullptr;

private:
	void OnTabFocus(HeaderFrame& tab) {
		if (tab_focus == &tab) {
			return;
		}
		if (tab_focus == nullptr) {
			content_frame->RotateWith(tab.content);
		} else {
			tab_focus->OnBlur();
			content_frame->RotateWith(tab.content, tab_focus->content);
		}
		tab_focus = &tab;
		tab_focus->OnFocus();
	}
	void OnTabClose(HeaderFrame& tab) {
		if (tab_focus == &tab) {
			tab_focus->OnBlur();
			content_frame->RotateWith(tab.content);
			tab_focus = nullptr;
		}
		tab_list->Erase(tab);
	}

public:
	HeaderFrame& Append(Tab tab) { tab_list->Append(new HeaderFrame(std::move(tab))); }
};


} // namespace ViewDesign
