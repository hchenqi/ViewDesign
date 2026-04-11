#pragma once

#include "../ViewFrame.h"
#include "../frame/BackgroundFrame.h"
#include "../frame/ScrollFrame.h"
#include "../frame/FixedFrame.h"
#include "../frame/MaxFrame.h"
#include "../frame/CenterFrame.h"
#include "../frame/ClipFrame.h"
#include "../frame/PaddingFrame.h"
#include "../layout/ListLayout.h"
#include "../layout/SplitLayout.h"
#include "../layout/StackLayout.h"
#include "../control/Placeholder.h"
#include "../control/TextBox.h"
#include "../wrapper/HitTestHelper.h"
#include "../wrapper/Cursor.h"
#include "../../message/context.h"
#include "../../event/mouse_tracker.h"
#include "../../figure/shape.h"
#include "../../system/cursor.h"

#include <unordered_set>


namespace ViewDesign {


class TabView : public ViewFrame, public SizeTrait<Fixed, Fixed>, protected ContextProvider {
public:
	class Tab : public HitSelfFallback<ViewFrame>, public SizeTrait<Fixed, Auto>, protected Context, protected ContextProvider {
	public:
		class Title : public CenterFrame<Auto, Fixed> {
		protected:
			class Text : public TextBox {
			protected:
				struct Style : TextBox::Style {
					Style() {}
				};
			public:
				Text(std::wstring text) : TextBox(Style(), std::move(text)) {}
			};
		public:
			Title(std::wstring title) : CenterFrame(
				new PaddingFrame(
					Padding(5px, 0px),
					new MaxFrame<Auto, Auto>(
						size_max,
						text = new Text(std::move(title))
					)
				)
			) {}
		protected:
			ref_ptr<TextBox> text;
		public:
			const std::wstring& GetText() const {
				return text->GetText();
			}
			void SetText(std::wstring text) {
				this->text->Assign(std::move(text));
			}
		};

		class CloseButton : public Placeholder<Auto, Fixed>, protected Context {
		public:
			CloseButton() : Placeholder(30px), Context(AsViewBase()) {}
		protected:
			static constexpr Point center = Point(15px, 15px);
			static constexpr Color background_hover = Color(Color::Black, 32);
			static constexpr Color background_press = Color(Color::Black, 64);
			static constexpr Color foreground = Color::DimGray;
		protected:
			enum class State { Normal, Hover, Press } state = State::Normal;
			MouseTracker mouse_tracker;
		protected:
			virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
				if (auto& tab = Context::Get<Tab>(); !(tab.hover || tab.active)) { return; }
				switch (state) {
				case State::Hover: figure_queue.add(center, new Circle(7px, background_hover)); break;
				case State::Press: figure_queue.add(center, new Circle(7px, background_press)); break;
				}
				figure_queue.add(center, new Line(Vector(-3px, -3px), Vector(3px, 3px), 1.0f, foreground));
				figure_queue.add(center, new Line(Vector(3px, -3px), Vector(-3px, 3px), 1.0f, foreground));
			}
		protected:
			virtual void OnMouseEvent(MouseEvent event) override {
				switch (mouse_tracker.Track(event)) {
				case MouseTrackEvent::LeftDown: state = State::Press; Redraw(region_infinite); break;
				case MouseTrackEvent::LeftClick: Context::Get<Tab>().Close(); break;
				}
			}
			virtual void OnFocusEvent(FocusEvent event) override {
				switch (event) {
				case FocusEvent::MouseEnter: state = State::Hover; Redraw(region_infinite); break;
				case FocusEvent::MouseLeave: state = State::Normal; Redraw(region_infinite); break;
				}
			}
		};

		class Bar : public FixedFrame<Fixed, Auto> {
		public:
			Bar(view_ptr<Auto, Fixed> title, view_ptr<Auto, Fixed> close_button) : FixedFrame(
				30px,
				new StackLayout(
					new HitThrough<ClipFrame<Fixed, Fixed, TopLeft>>(std::move(title)),
					new HitThroughMargin<ClipFrame<Fixed, Fixed, TopRight>>(std::move(close_button))
				)
			) {}
		};

	public:
		Tab(view_ptr<Fixed, Auto> tab) : Base(std::move(tab)), Context(AsViewBase()), ContextProvider(AsViewBase()) {}
		Tab(view_ptr<Auto, Fixed> title, view_ptr<Auto, Fixed> close_button = new CloseButton()) : Tab(new Bar(std::move(title), std::move(close_button))) {}

		// state
	private:
		friend class TabView;
	protected:
		bool hover = false;
		bool active = false;
		bool selected = false;
	protected:
		void Show() { active = true; Redraw(region_infinite); Context::Get<TabView>().ShowTab(*this, OnShow()); }
		void Hide(view_ptr<Fixed, Fixed> content) { active = false; Redraw(region_infinite); OnHide(std::move(content)); }
		void Close() { Context::Get<TabView>().CloseTab(*this); }
		void Select() { if (!selected) { selected = true; Redraw(region_infinite); Context::Get<TabView>().SelectTab(*this); } }
		void Unselect() { if (selected) { selected = false; Redraw(region_infinite); Context::Get<TabView>().UnselectTab(*this); } }
	protected:
		virtual view_ptr<Fixed, Fixed> OnShow() { return new Placeholder<Fixed, Fixed>(); }
		virtual void OnHide(view_ptr<Fixed, Fixed> content) {}

		// paint
	protected:
		static constexpr Color background_normal = color_transparent;
		static constexpr Color background_hover = Color(Color::Black, 16);
		static constexpr Color background_active = Color(Color::Black, 48);
		static constexpr Color background_selected = Color(Color::Black, 80);
	protected:
		virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
			figure_queue.add(draw_region.point, new Rectangle(draw_region.size, selected ? background_selected : active ? background_active : hover ? background_hover : background_normal));
			ViewFrame::OnDraw(figure_queue, draw_region);
		}

		// event
	private:
		MouseTracker mouse_tracker;
	protected:
		virtual void OnMouseEvent(MouseEvent event) override {
			switch (mouse_tracker.Track(event)) {
			case MouseTrackEvent::LeftDown: event.ctrl ? void() : (SetFocus(), active ? void() : Show()); break;
			case MouseTrackEvent::LeftClick: event.ctrl ? (selected ? Unselect() : Select()) : void(); break;
			case MouseTrackEvent::LeftDrag: event.ctrl ? Context::Get<TabView>().BeginSelect(*this, event.point) : void(); break;
			}
		}
		virtual void OnFocusEvent(FocusEvent event) override {
			switch (event) {
			case FocusEvent::MouseEnter: hover = true; Redraw(region_infinite); break;
			case FocusEvent::MouseLeave: hover = false; Redraw(region_infinite); break;
			}
		}
	};

protected:
	class SideBar : public FixedFrame<Auto, Fixed>, protected ContextProvider {
	protected:
		class ResizeControl : public CustomizedCursor<Placeholder<Auto, Fixed>, Cursor::ResizeWE>, protected Context {
		public:
			ResizeControl() : Base(5px), Context(AsViewBase()) {}
		protected:
			static constexpr Color background_normal = Color::WhiteSmoke;
			static constexpr Color background_hover = Color::LightGray;
			static constexpr Color foreground = Color::LightGray;
		protected:
			bool hover = false;
		protected:
			virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
				if (hover) {
					figure_queue.add(draw_region.point, new Rectangle(draw_region.size, background_hover));
				} else {
					figure_queue.add(draw_region.point, new Rectangle(draw_region.size, background_normal));
					Point center = Rect(point_zero, size).Center();
					figure_queue.add(center - Vector(0px, 8px), new Circle(1.5px, foreground));
					figure_queue.add(center + Vector(0px, 0px), new Circle(1.5px, foreground));
					figure_queue.add(center + Vector(0px, 8px), new Circle(1.5px, foreground));
				}
			}
		protected:
			virtual void OnMouseEvent(MouseEvent event) override {
				switch (event.type) {
				case MouseEvent::LeftDown: Context::Get<SideBar>().BeginDrag(event.point.x - size.width); break;
				}
			}
			virtual void OnFocusEvent(FocusEvent event) override {
				switch (event) {
				case FocusEvent::MouseEnter: hover = true; Redraw(region_infinite); break;
				case FocusEvent::MouseLeave: hover = false; Redraw(region_infinite); break;
				}
			}
		};
	protected:
		static constexpr Color background = Color::LightYellow;

	public:
		SideBar(view_ptr<Fixed, Fixed> child) : FixedFrame(
			default_width,
			new SplitLayoutHorizontal(
				new BackgroundFrame(
					background,
					std::move(child)
				),
				new ResizeControl()
			)
		), ContextProvider(AsViewBase()) {}

	protected:
		static constexpr float default_width = 200px;
	protected:
		float drag_offset = 0;
	protected:
		void BeginDrag(float offset) {
			drag_offset = offset;
			SetCapture();
		}
	protected:
		virtual void OnMouseEvent(MouseEvent event) override {
			switch (event.type) {
			case MouseEvent::Move: SetWidth(event.point.x - drag_offset); break;
			case MouseEvent::LeftUp: ReleaseCapture(); break;
			}
		}
	};

	class TabList : public ListLayout<Vertical> {
	private:
		friend class TabView;

	public:
		TabList() : ListLayout(10px) {}

		// selection
	protected:
		std::unordered_set<size_t> selection_index_set;
	protected:
		bool HasSelection() const { return !selection_index_set.empty(); }
		void ClearSelection() {
			for (size_t index : std::vector<size_t>(selection_index_set.begin(), selection_index_set.end())) {
				static_cast<Tab&>(GetChild(index)).Unselect();
			}
		}
		void DeleteSelection() {
			if (!HasSelection()) { return; }
			size_t index = 0, begin = -1;
			child_list.erase(std::remove_if(child_list.begin(), child_list.end(), [&](auto& child) {
				return selection_index_set.contains(index) ? (begin = std::min(begin, index), index++, true) : (index++, false);
			}), child_list.end());
			selection_index_set.clear();
			UpdateIndex(begin); UpdateLayout(begin);
		}
	protected:
		void SelectChild(ViewBase& child) { selection_index_set.insert(GetChildIndex(child)); SetFocus(); }
		void UnselectChild(ViewBase& child) { selection_index_set.erase(GetChildIndex(child)); SetFocus(); }

		// active selection
	protected:
		bool is_selecting = false;
		Point selecting_begin_point;
		Rect selecting_region;
		IndexRange selecting_index_range;
	protected:
		void BeginSelect(Point point) {
			is_selecting = true;
			selecting_begin_point = point;
			selecting_region = region_empty;
			selecting_index_range = index_range_empty;
			SetFocus(); SetCapture();
		}
		void BeginSelect(ViewBase& child, Point point) {
			BeginSelect(ConvertDescendentPoint(child, point));
		}
		void DoSelect(Point point) {
			Rect new_selecting_region = Rect(selecting_begin_point, Size(1px, 1px)).Union(Rect(point, Size(1px, 1px)));
			Redraw(Extend(selecting_region.Union(new_selecting_region), Margin(1px)));
			selecting_region = new_selecting_region;
			IndexRange new_selecting_index_range = [=]() mutable {
				new_selecting_region = new_selecting_region.Intersect(Rect(point_zero, size));
				if (new_selecting_region.IsEmpty()) { return index_range_empty; }
				size_t index_begin = HitTestIndex(new_selecting_region.LeftTop()), index_end = HitTestIndex(new_selecting_region.RightBottom());
				if (!GetChildRegion(index_begin).Contains(new_selecting_region.LeftTop())) { index_begin++; }
				if (index_begin > index_end) { return index_range_empty; }
				return IndexRange(index_begin, index_end - index_begin + 1);
			}();
			if (new_selecting_index_range == selecting_index_range) { return; }
			IndexRange i = new_selecting_index_range.Intersect(selecting_index_range), u = new_selecting_index_range.Union(selecting_index_range);
			for (size_t index = u.begin(); (index = index == i.begin() ? i.end() : index), index < u.end(); ++index) {
				Tab& tab = static_cast<Tab&>(GetChild(index));
				!selection_index_set.contains(index) ? tab.Select() : tab.Unselect();
			}
			selecting_index_range = new_selecting_index_range;
		}
		void EndSelect() {
			is_selecting = false;
			Redraw(selecting_region);
			ReleaseCapture();
		}

		// paint
	private:
		virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
			ListLayout::OnDraw(figure_queue, draw_region);
			if (is_selecting) {
				figure_queue.add(selecting_region.point, new Rectangle(selecting_region.size, Color(Color::Black, 32), 1px, Color::Gray));
			}
		}

		// event
	private:
		MouseTracker mouse_tracker;
	private:
		virtual void OnMouseEvent(MouseEvent event) override {
			switch (mouse_tracker.Track(event)) {
			case MouseTrackEvent::LeftClick: is_selecting ? void() : ClearSelection(); break;
			case MouseTrackEvent::LeftDrag: is_selecting ? void() : BeginSelect(event.point); break;
			}
			switch (event.type) {
			case MouseEvent::Move: is_selecting ? DoSelect(event.point) : void(); break;
			case MouseEvent::LeftUp: is_selecting ? EndSelect() : void(); break;
			}
		}
		virtual void OnKeyEvent(KeyEvent event) override {
			switch (event.type) {
			case KeyEvent::KeyDown:
				switch (event.key) {
				case Key::Delete: DeleteSelection(); break;
				}
			}
		}
		virtual void OnFocusEvent(FocusEvent event) override {
			switch (event) {
			case FocusEvent::Blur: ClearSelection(); break;
			}
		}
	};

public:
	TabView() : ViewFrame(
		new SplitLayoutHorizontal(
			new SideBar(
				new ScrollFrame<Vertical>(
					tab_list = new TabList()
				)
			),
			view_ptr<Fixed, Fixed>() = content_frame = new ViewFrameMutable(new Placeholder<Fixed, Fixed>)
		)
	), ContextProvider(AsViewBase()) {}

private:
	ref_ptr<TabList> tab_list;
	ref_ptr<ViewFrameMutable> content_frame;
	ref_ptr<Tab> active_tab = nullptr;

private:
	void ShowTab(Tab& tab, view_ptr<Fixed, Fixed> content) {
		view_ptr<> previous_content = content_frame->Reset(std::move(content));
		if (active_tab != nullptr) {
			active_tab->Hide(view_ptr<Fixed, Fixed>() = std::move(previous_content));
		}
		active_tab = &tab;
	}
	void CloseTab(Tab& tab) {
		if (active_tab == &tab) {
			tab.Hide(view_ptr<Fixed, Fixed>() = content_frame->Reset(new Placeholder<Fixed, Fixed>));
			active_tab = nullptr;
		}
		tab_list->ClearSelection();
		tab_list->EraseChild(tab_list->GetChildIndex(tab));
	}

private:
	void SelectTab(Tab& tab) {
		tab_list->SelectChild(tab);
	}
	void UnselectTab(Tab& tab) {
		tab_list->UnselectChild(tab);
	}
	void BeginSelect(Tab& tab, Point point) {
		tab_list->BeginSelect(tab, point);
	}

public:
	void AddTab(std::unique_ptr<Tab> tab) {
		tab_list->AppendChild(std::move(tab));
	}
};


} // namespace ViewDesign
