#pragma once

#include "../window/wnd_traits.h"

#include <vector>


namespace ViewDesign {


template<class Direction>
class ListLayoutAuto;


class _ListLayoutAuto_Base : public WndType<Auto, Auto> {
public:
	using child_type = child_ptr<Auto, Auto>;

protected:
	_ListLayoutAuto_Base(float gap) : child_list(), gap(gap) {}

	// child
protected:
	struct ChildInfo {
	public:
		child_type child;
		Rect region;
	public:
		ChildInfo(child_type child) : child(std::move(child)) {}
	};
	std::vector<ChildInfo> child_list;
public:
	bool Empty() const { return child_list.empty(); }
	size_t Length() const { return child_list.size(); }
	WndObject& GetChild(size_t index) const { return child_list[index].child; }
	size_t GetChildIndex(WndObject& child) const { return WndObject::GetChildData<size_t>(child); }
private:
	void SetChildIndex(WndObject& child, size_t index) { WndObject::SetChildData<size_t>(child, index); }
	void UpdateIndex(size_t begin);
protected:
	size_t InsertChild(size_t index, child_type child);
	size_t InsertChild(size_t begin, std::vector<child_type> children);
	void EraseChild(size_t begin, size_t count);
	child_type ExtractChild(size_t index);
	std::vector<child_type> ExtractChild(size_t begin, size_t count);

	// layout
protected:
	Size size;
	float gap = 0.0f;
protected:
	Rect GetChildRegion(size_t index) const { return child_list[index].region; }
	Rect GetChildRegion(WndObject& child) const { return GetChildRegion(GetChildIndex(child)); }
protected:
	virtual Transform GetChildTransform(WndObject& child) const override;
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override;

	// paint
protected:
	virtual void OnChildRedraw(WndObject& child, Rect child_redraw_region) override;
};


template<>
class ListLayoutAuto<Vertical> : public _ListLayoutAuto_Base {
public:
	template<class... Ts>
	ListLayoutAuto(float gap, Ts... child_args) : _ListLayoutAuto_Base(gap) {
		if constexpr (sizeof...(Ts) > 0) {
			child_type list[] = { std::move(child_args)... };
			InsertChild(0, std::vector<child_ptr>(std::make_move_iterator(std::begin(list)), std::make_move_iterator(std::end(list))));
		}
	}

	// child
private:
	_ListLayoutAuto_Base::ChildInfo;
	_ListLayoutAuto_Base::child_list;
public:
	void InsertChild(size_t index, child_type child);
	void InsertChild(size_t begin, std::vector<child_type> children);
	void AppendChild(child_type child) { InsertChild(-1, std::move(child)); }
	void EraseChild(size_t begin, size_t count = 1);
	child_type ExtractChild(size_t index);
	std::vector<child_type> ExtractChild(size_t begin, size_t count);

	// layout
private:
	void UpdateLayout(size_t index);
protected:
	size_t HitTestIndex(Point point);
protected:
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override;

	// paint
protected:
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override;

	// message
protected:
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override;
};


template<>
class ListLayoutAuto<Horizontal> : public _ListLayoutAuto_Base {
public:
	template<class... Ts>
	ListLayoutAuto(float gap, Ts... child_args) : _ListLayoutAuto_Base(gap) {
		if constexpr (sizeof...(Ts) > 0) {
			child_type list[] = { std::move(child_args)... };
			InsertChild(0, std::vector<child_type>(std::make_move_iterator(std::begin(list)), std::make_move_iterator(std::end(list))));
		}
	}

	// child
private:
	_ListLayoutAuto_Base::ChildInfo;
	_ListLayoutAuto_Base::child_list;
public:
	void InsertChild(size_t index, child_type child);
	void InsertChild(size_t begin, std::vector<child_type> children);
	void AppendChild(child_type child) { InsertChild(-1, std::move(child)); }
	void EraseChild(size_t begin, size_t count = 1);
	child_type ExtractChild(size_t index);
	std::vector<child_type> ExtractChild(size_t begin, size_t count);

	// layout
private:
	void UpdateLayout(size_t index);
protected:
	size_t HitTestIndex(Point point);
protected:
	virtual void OnChildSizeUpdate(WndObject& child, Size child_size) override;

	// paint
protected:
	virtual void OnDraw(FigureQueue& figure_queue, Rect draw_region) override;

	// message
protected:
	virtual ref_ptr<WndObject> HitTest(MouseMsg& msg) override;
};


} // namespace ViewDesign
