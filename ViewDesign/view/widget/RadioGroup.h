#pragma once

#include "../frame/PaddingFrame.h"
#include "../layout/ListLayout.h"
#include "../../style/value_tag.h"
#include "../../figure/shape.h"
#include "../../common/function_traits.h"


namespace ViewDesign {


template<class T>
struct RadioItem {
	T value;
	view_ptr<Assigned, Auto> child;
	bool selected = false;
};


template<class T>
class RadioGroup : public ListLayout<Vertical> {
public:
	template<class... Ts>
	RadioGroup(std::function<void(const T&)> callback, Ts... child_args) :
		ListLayout(0px, (new ItemFrame(*this, std::move(child_args)))...),
		callback(callback),
		selected_item(selected_item ? selected_item : nullptr) {}

private:
	class ItemFrameBase : public PaddingFrame<Assigned, Auto> {
	protected:
		ItemFrameBase(RadioGroup& group, bool selected, view_ptr child) : PaddingFrame(
			Padding(20px, 0px, 0px, 0px),
			std::move(child)
		), selected(selected) {
			if (selected) { group.selected_item = this; }
		}
	protected:
		RadioGroup& GetParent() { return static_cast<RadioGroup&>(ViewBase::GetParent()); }
	private:
		bool selected;
	public:
		void Select() { selected = true; Redraw(GetPaddingRegion()); }
		void Unselect() { selected = false; Redraw(GetPaddingRegion()); }
	private:
		Rect GetPaddingRegion() const { return Rect(Point(0, 0), Size(padding.left, size.height)); }
	private:
		virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override {
			PaddingFrame::OnDraw(figure_queue, draw_region);
			figure_queue.add(GetPaddingRegion().Center(), new Circle(6px, selected ? Color::Gray : color_transparent, 1.0f, Color::Gray));
		}
	private:
		virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { return selected ? PaddingFrame::HitTest(event) : this; }
	};

	template<class T>
	class ItemFrame : public ItemFrameBase {
	public:
		ItemFrame(RadioGroup& group, RadioItem<T> item) : ItemFrameBase(group, item.selected, std::move(item.child)), value(std::move(item.value)) {}
	private:
		T value;
	private:
		void Select() { ItemFrameBase::GetParent().SelectItem(*this, value); }
	private:
		virtual void OnMouseEvent(MouseEvent event) override { if (event.type == MouseEvent::LeftDown) { Select(); } }
	};

private:
	ref_ptr<ItemFrameBase> selected_item;
	std::function<void(const T&)> callback;
private:
	void SelectItem(ItemFrameBase& item, const T& value) {
		if (selected_item != nullptr) { selected_item->Unselect(); }
		selected_item = &item; item.Select();
		callback(value);
	}
};


template<class Func, class... Ts>
RadioGroup(Func, Ts...) -> RadioGroup<typename function_traits<Func>::template argument_type<0>>;


} // namespace ViewDesign
